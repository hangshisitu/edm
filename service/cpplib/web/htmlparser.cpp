/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "web/htmlparser.h"
#include "web/urlhelper.h"
#include "base/stringtokenizer.h"

RFC_NAMESPACE_BEGIN

const stdstring HtmlParser::m_strPlainTextTag("plaintext");
const stdstring HtmlParser::m_strXmpTag("xmp");
const stdstring HtmlParser::m_strTextAreaTag("textarea");
const stdstring HtmlParser::m_strStyleTag("style");
const stdstring HtmlParser::m_strScriptTag("script");

const FindSubString HtmlParser::m_strClosedXmp("</xmp", 5);
const FindSubString HtmlParser::m_strClosedTextArea("</textarea", 10);
const FindSubString HtmlParser::m_strClosedStyle("</style", 7);
const FindSubString HtmlParser::m_strClosedScript("</script", 8);

bool HtmlParser::isLiteralTag(const stdstring & strTagName)
{
	return ( StringFunc::strCaseEqual(strTagName, m_strPlainTextTag)
			|| StringFunc::strCaseEqual(strTagName, m_strXmpTag)
			|| StringFunc::strCaseEqual(strTagName, m_strTextAreaTag)
			|| StringFunc::strCaseEqual(strTagName, m_strStyleTag)
			|| StringFunc::strCaseEqual(strTagName, m_strScriptTag) );
}

const char * HtmlParser::findLiteralClosedTag(const FindSubString & strPattern, const char * pBegin, const char * pHtmlEnd)
{
	if ( pBegin >= pHtmlEnd )
		return pHtmlEnd;
	size_t n = strPattern.findIgnoreCase(pBegin, pHtmlEnd - pBegin);
	while ( n != stdstring::npos )
	{
		const char * pClosedPos = pBegin + n + strPattern.getKeySize();
		if ( (pClosedPos < pHtmlEnd) && (::isspace(*pClosedPos) || *pClosedPos == '>') )
			return pBegin + n;
		pBegin = pClosedPos;
		n = strPattern.findIgnoreCase(pBegin, pHtmlEnd - pBegin);
	} // while ( n != stdstring::npos )
	return pHtmlEnd;
}

const char * HtmlParser::parseLiteralTag(const stdstring & strTagName, const char * pBegin, const char * pHtmlEnd)
{
	if ( pBegin >= pHtmlEnd )
		return pHtmlEnd;

	const char * pSave = pHtmlEnd;
	if ( StringFunc::strCaseEqual(strTagName, m_strPlainTextTag) ) // plaintext�����ȫ���Ǵ��ı�
	{
		onPlainContent(pBegin, pHtmlEnd);
	}
	else if ( StringFunc::strCaseEqual(strTagName, m_strXmpTag) )
	{
		pSave = findLiteralClosedTag(m_strClosedXmp, pBegin, pHtmlEnd);
		onPlainContent(StringFunc::g_strCRLF.data(), StringFunc::g_strCRLF.data() + StringFunc::g_strCRLF.size());
		onPlainContent(pBegin, pSave);
		onPlainContent(StringFunc::g_strCRLF.data(), StringFunc::g_strCRLF.data() + StringFunc::g_strCRLF.size());
	}
	else if ( StringFunc::strCaseEqual(strTagName, m_strTextAreaTag) )
	{
		pSave = findLiteralClosedTag(m_strClosedTextArea, pBegin, pHtmlEnd);
		onPlainContent(pBegin, pSave);
	}
	else if ( StringFunc::strCaseEqual(strTagName, m_strStyleTag) )
	{
		pSave = findLiteralClosedTag(m_strClosedStyle, pBegin, pHtmlEnd);
		onStyleContent(pBegin, pSave);
	}
	else if ( StringFunc::strCaseEqual(strTagName, m_strScriptTag) )
	{
		pSave = findLiteralClosedTag(m_strClosedScript, pBegin, pHtmlEnd);
		onScriptContent(pBegin, pSave);
	}
	else			//����ԭλ��
		return pBegin;
	return parseHtmlTag(pSave, pHtmlEnd);
}

const char * HtmlParser::parseHtmlTag(const char * pTagBegin, const char * pHtmlEnd)
{
	bool bAttributeFormatError = false;
	HtmlTag htmlTag;
	const char * pCurPos = pTagBegin + 1;					// ����'<'
	htmlTag.m_bClosedTag = ( *pCurPos == '/' );
	if ( htmlTag.m_bClosedTag )
		++pCurPos;

	const char * pSave = pCurPos;
	while( pCurPos < pHtmlEnd && ::isalnum(*pCurPos) )		// ��ȡ��ǩ����
		++pCurPos;
	htmlTag.m_strTagName.assign(pSave, pCurPos - pSave);

	typeTagAttributePair tagAttribute;
	IsSpacePred spacePred;
	while ( pCurPos < pHtmlEnd && *pCurPos != '>' )
	{
		pCurPos = std::find_if(pCurPos, pHtmlEnd, std::not1(spacePred));	//����������ǰ��Ŀո�
		pSave = pCurPos;
		while ( pCurPos < pHtmlEnd && !::isspace(*pCurPos) && *pCurPos != '>' && *pCurPos != '=' ) //����������
			++pCurPos;
		tagAttribute.first.assign(pSave, pCurPos - pSave);

		pCurPos = std::find_if(pCurPos, pHtmlEnd, std::not1(spacePred));	//�����Ⱥ�ǰ��Ŀո�
		if ( *pCurPos != '=' )
			continue;

		pSave = std::find_if(pCurPos + 1, pHtmlEnd, std::not1(spacePred));	//�����Ⱥź���Ŀո�
		char chQuote = *pSave;
		if ( chQuote == '\'' || chQuote == '\"' )
		{
			for ( pCurPos = pSave + 1; pCurPos < pHtmlEnd && *pCurPos != chQuote; ++pCurPos )		//����һ������
			{
				if ( *pCurPos == '>' && !StringFunc::strCaseEqual(htmlTag.m_strTagName, m_strPlainTextTag)
					&& !StringFunc::strCaseEqual(htmlTag.m_strTagName, m_strXmpTag)
					&& !StringFunc::strCaseEqual(htmlTag.m_strTagName, m_strTextAreaTag) )		//<plaintext> <xmp> <textarea> Ҫ�ҵ�ƥ�������
					break;
			} // for ( pCurPos = pSave + 1; pCurPos < pHtmlEnd && *pCurPos != chQuote; ++pCurPos )

			if ( pCurPos < pHtmlEnd && *pCurPos == chQuote )
			{
				tagAttribute.second.assign(pSave + 1, pCurPos - pSave - 1);
				++pCurPos;		//��������
			}
			else
			{
				bAttributeFormatError = true;
			}
		}
		else
		{
			for ( pCurPos = pSave; pCurPos < pHtmlEnd && !::isspace(*pCurPos) && *pCurPos != '>'; )		//����һ������
				++pCurPos;
			tagAttribute.second.assign(pSave, pCurPos - pSave);
		}

		htmlTag.m_listAttribute.push_back( tagAttribute );
	} // while ( pCurPos < pHtmlEnd && *pCurPos != '>' )

	bool bMatchBrachket = ( pCurPos < pHtmlEnd && *pCurPos == '>' );
	if ( bMatchBrachket )
		++pCurPos;
	bool bLiteralTag = isLiteralTag(htmlTag.m_strTagName);
	if ( !bMatchBrachket || (!bLiteralTag && bAttributeFormatError) )
		onHtmlContent(pTagBegin, pCurPos);
	else
		onHtmlTag(htmlTag);

	if ( bAttributeFormatError )
		onTagAttributeFormatError();

	if ( bLiteralTag && !htmlTag.m_bClosedTag && pCurPos < pHtmlEnd )
		return parseLiteralTag(htmlTag.m_strTagName, pCurPos, pHtmlEnd);
	return pCurPos;
}

const char * HtmlParser::parseComment(const char * pTagBegin, const char * pHtmlEnd)
{
	// �����"<!--"��ʼ,�����"-->"����; ���û��"-->",����"<!--"��ĵ�һ��'>'����; ���û��'>',�������ݴ���
	if ( pTagBegin + 4 < pHtmlEnd && pTagBegin[0] == '<' && pTagBegin[1] == '!' && pTagBegin[2] == '-' && pTagBegin[3] == '-' )
	{
		for ( const char * pCommentEnd = pTagBegin + 4; pCommentEnd + 2 < pHtmlEnd; ++pCommentEnd )
		{
			if ( pCommentEnd[0] == '-' && pCommentEnd[1] == '-' && pCommentEnd[2] == '>' )		//�ҵ�ע�͵Ľ���λ��
			{
				onHtmlComment(pTagBegin, pCommentEnd + 3);
				return pCommentEnd + 3;	// ����"-->"����һ��λ��
			}
		} // for ( const char * pCommentEnd = pTagBegin + 4; pCommentEnd + 2 < pHtmlEnd; ++pCommentEnd )
	}

	const char * pCommentEnd = std::find(pTagBegin, pHtmlEnd, '>');	//��λ'>'��λ��
	if ( pCommentEnd < pHtmlEnd && *pCommentEnd == '>' )
	{
		++pCommentEnd;
		onHtmlComment(pTagBegin, pCommentEnd);
		return pCommentEnd;		//����">"����һ��λ��
	}

	onHtmlContent(pTagBegin, pHtmlEnd);
	return pHtmlEnd;				// û���ҵ�ע�ͽ�����λ��,����pHtmlEnd
}

void HtmlParser::parseHtml(const char * pHtml, size_t nHtmlLen)
{
	onStart();
	if ( pHtml == NULL || nHtmlLen == 0 )
		return;

	const char * const pHtmlEnd = pHtml + nHtmlLen;
	const char * pCurPos = pHtml;
	const char * pTagBegin = std::find(pCurPos, pHtmlEnd, '<');		//��λ'<'��λ��
	while ( pTagBegin < pHtmlEnd && !mustExitParser() )
	{
		if ( pCurPos < pTagBegin )
			onHtmlContent(pCurPos, pTagBegin);

		char chTagFirstChar = '\0';
		if ( pTagBegin + 1 < pHtmlEnd )
			chTagFirstChar = pTagBegin[1];

		if ( chTagFirstChar == '/' || ::isalpha(chTagFirstChar) )	//'<'���һ����ĸ���Ǵ򿪱�ǩ
			pCurPos = parseHtmlTag(pTagBegin, pHtmlEnd);
		else if ( chTagFirstChar == '!' || chTagFirstChar == '?' )
			pCurPos = parseComment(pTagBegin, pHtmlEnd);
		else	//�������ݴ���
		{
			pCurPos = std::find(pTagBegin + 1, pHtmlEnd, '<');
			onHtmlContent(pTagBegin, pCurPos);
		}

		pTagBegin = std::find(pCurPos, pHtmlEnd, '<');		
	} // while ( pTagBegin < pHtmlEnd && !mustExitParser() )

	if ( pCurPos < pHtmlEnd && !mustExitParser() )
		onHtmlContent(pCurPos, pHtmlEnd);
	onFinish();
}

//////////////////////////////////////////////////////////////////////////

class HtmlParserURLVistor : public URLVisitor
{
public:
	HtmlParserURLVistor(typeURLSet & setURL) : m_setURL(setURL) {}
	void				onURL(const char * pURL, const char * pURLEnd);
	void				insertURL(const stdstring & strURL);
protected:
	typeURLSet	&		m_setURL;
};

void HtmlParserURLVistor::onURL(const char * pURL, const char * pURLEnd)
{
	if ( pURLEnd > pURL )
		insertURL( stdstring(pURL, pURLEnd - pURL) );
}

void HtmlParserURLVistor::insertURL(const stdstring & strURL)
{
	if ( strURL.empty() || m_setURL.size() > 300 )
		return;

	if ( strURL.find('\r') == stdstring::npos && strURL.find('\n') == stdstring::npos )
	{
		if ( StringFunc::strnCasecmp(strURL, "mailto:", 7) != 0 )	// ����ȡ��mailto:��ͷ���ַ���
			m_setURL.insert(strURL);
		return;
	}

	stdstring strRemoveCRLF(strURL);
	GlobalFunc::removeSpace(strRemoveCRLF);
	if ( StringFunc::strnCasecmp(strRemoveCRLF, "mailto:", 7) != 0 )	// ����ȡ��mailto:��ͷ���ַ���
		m_setURL.insert(strRemoveCRLF);
}

void HtmlTextParser::fetchURL(const HtmlTag & htmlTag)
{
	if ( htmlTag.m_listAttribute.empty() || htmlTag.m_bClosedTag )
		return;

	// 1) ��ȡ src,href,background ���Ե�url
	// 2) ��ȡstyle���Ե�url
	// 3) ��ȡform tag��url
	// 4) ��ȡmeta tag��url
	HtmlParserURLVistor urlCollector(m_setURL);
	typeTagAttributeList::const_iterator it, itEnd = htmlTag.m_listAttribute.end();
	for ( it = htmlTag.m_listAttribute.begin(); it != itEnd; ++it )
	{
		if ( StringFunc::strCaseEqual(it->first, "src")
			|| StringFunc::strCaseEqual(it->first, "href")
			|| StringFunc::strCaseEqual(it->first, "background") )
		{
			urlCollector.insertURL(it->second);
		}
		else if ( StringFunc::strCaseEqual(it->first, "style") )
		{
			URLHelper::getURLWithPrefix(it->second.data(), it->second.size(), "http://", 7, urlCollector);
			URLHelper::getURLWithPrefix(it->second.data(), it->second.size(), "https://", 8, urlCollector);
			URLHelper::getURLWithPrefix(it->second.data(), it->second.size(), "ftp://", 6, urlCollector);
			URLHelper::getURLWithPrefix(it->second.data(), it->second.size(), "www.", 4, urlCollector);

		}
		else if ( StringFunc::strCaseEqual(htmlTag.m_strTagName, "form")
				&& StringFunc::strCaseEqual(it->first, "action")
				&& URLHelper::checkURLPrefix(it->second) )
		{
			urlCollector.insertURL(it->second);
		}
		else if ( StringFunc::strCaseEqual(htmlTag.m_strTagName, "meta")
				&& StringFunc::strCaseEqual(it->first, "content") )
		{
			StringTokenizer<stdstring> tokenMetaContent(it->second.data(), it->second.size());
			stdstring strURLPair, strMetaKey, strMetaValue;
			while ( tokenMetaContent.getNext(strURLPair, ';') )
			{
				size_t nEqualPos = strURLPair.find('=');
				if ( nEqualPos == stdstring::npos )
					continue;
				strMetaKey = strURLPair.substr(0, nEqualPos);
				StringFunc::trimSpace(strMetaKey);
				if ( StringFunc::strCaseEqual(strMetaKey, "url")
					|| StringFunc::strCaseEqual(strMetaKey, "src")
					|| StringFunc::strCaseEqual(strMetaKey, "href") )
				{
					strMetaValue = strURLPair.substr(nEqualPos + 1);
					StringFunc::trimSpace(strMetaValue);
					urlCollector.insertURL(strMetaValue);
				}
			} // while ( tokenMetaContent.getNext(strURLPair, ';') )
		}
	} // for ( it = htmlTag.m_listAttribute.begin(); it != itEnd; ++it )
}

void HtmlTextParser::onHtmlTag(const HtmlTag & htmlTag)
{
	static const stdstring		g_strHrTag("hr");
	static const stdstring		g_strBrTag("br");
	static const stdstring		g_strDivTag("div");
	static const stdstring		g_strPTag("p");
	static const stdstring		g_strHrText("\r\n\r\n"); //("\r\n--------------------------------------------------------------------------------\r\n");
	static const stdstring		g_strPText("\r\n\r\n");
	if ( !htmlTag.m_bClosedTag && StringFunc::strCaseEqual(htmlTag.m_strTagName, g_strHrTag) )
		onPlainContent(g_strHrText.data(), g_strHrText.data() + g_strHrText.size());
	else if ( StringFunc::strCaseEqual(htmlTag.m_strTagName, g_strBrTag) || StringFunc::strCaseEqual(htmlTag.m_strTagName, g_strDivTag) )
		onPlainContent(StringFunc::g_strCRLF.data(), StringFunc::g_strCRLF.data() + StringFunc::g_strCRLF.size());
	else if ( StringFunc::strCaseEqual(htmlTag.m_strTagName, g_strPTag) )
		onPlainContent(g_strPText.data(), g_strPText.data() + g_strPText.size());
	fetchURL(htmlTag);
}

void HtmlTextParser::onHtmlContent(const char * pContentBegin, const char * pContentEnd)
{
	stdstring strSegmentText;
	strSegmentText.reserve(pContentEnd - pContentBegin);
	bool bGotSpace = false;
	for ( ; pContentBegin < pContentEnd; ++pContentBegin )
	{
		if ( ::isspace(*pContentBegin) )
		{
			bGotSpace = true;
			continue;
		}

		if( bGotSpace && !strSegmentText.empty() )
			strSegmentText += ' ';
		bGotSpace = false;
		strSegmentText += (*pContentBegin);
	} // for ( ; pContentBegin < pContentEnd; ++pContentBegin )

	if ( bGotSpace && !strSegmentText.empty() )
		strSegmentText += ' ';
	if ( !strSegmentText.empty() )
		onPlainContent(strSegmentText.data(), strSegmentText.data() + strSegmentText.size());
}

void HtmlTextParser::onPlainContent(const char * pContentBegin, const char * pContentEnd)
{
	m_strFullText.append(pContentBegin, pContentEnd-pContentBegin);
}

void HtmlTextParser::onFinish(void)
{
	GlobalFunc::replace(m_strFullText, "&nbsp;", " ");
	GlobalFunc::replace(m_strFullText, "&quot;", "\"");
	GlobalFunc::replace(m_strFullText, "&amp;", "&");
	GlobalFunc::replace(m_strFullText, "&lt;", "<");
	GlobalFunc::replace(m_strFullText, "&gt;", ">");
}

RFC_NAMESPACE_END
