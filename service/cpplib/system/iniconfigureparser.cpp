/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "system/iniconfigureparser.h"
#include "base/stringtokenizer.h"

RFC_NAMESPACE_BEGIN

const stdstring	IniConfigureParser::m_strCommentSymbol = "#;!";

void IniConfigureParser::setReplaceEnvVar(const stdstring & strKey, const stdstring & strValue)
{
	m_mapReplaceEnvVar[strKey] = strValue;
}

bool IniConfigureParser::getReplaceEnvVar(const stdstring & strKey, stdstring & strValue) const
{
	typeReplaceEnvVarMap::const_iterator it = m_mapReplaceEnvVar.find(strKey);
	if ( it != m_mapReplaceEnvVar.end() )
		strValue = it->second;
	else
	{
		const char * lpszEnv = getenv(strKey.c_str());
		if ( lpszEnv == NULL )
			return false;
		strValue = lpszEnv;
	}
	return true;
}

const char * IniConfigureParser::findTheEnd(const char * pBegin, const char * pEnd, char chEnd)
{
	for ( ; pBegin < pEnd; ++pBegin )
	{
		if ( *pBegin == '\\' && pBegin + 1 < pEnd )
			++pBegin;
		else if ( *pBegin == chEnd )
			break;
	} // for ( ; pBegin < pEnd; ++pBegin )
	return pBegin;
}

bool IniConfigureParser::needNextLine(const stdstring & strCurrentLine)
{
	if ( strCurrentLine.empty() || *strCurrentLine.rbegin() != '\\' )
		return false;

	if ( strCurrentLine.size() == 1 )
		return true;
	if ( strCurrentLine[strCurrentLine.size() - 2] != '\\' )
		return true;

	// 行结尾出现多个反斜杆,计算反斜杆数量,基数个则为合并连接下一行
	size_t nPos = strCurrentLine.find_last_not_of('\\') + 1;
	return ( ((strCurrentLine.size() - nPos) % 2 ) != 0 );
}

void IniConfigureParser::decodeValue(const char * pBegin, const char * pEnd, stdstring & strValue) const
{
	stdstring strSwap;
	for ( ; pBegin < pEnd; ++pBegin )
	{
		char ch = *pBegin;
		if ( ch == '\\' && pBegin + 1 < pEnd )
		{
			++pBegin;
			ch = *pBegin;
			if( ch == 'r' )
				ch = '\r';
			else if( ch == 'n' )
				ch = '\n';
			else if( ch == 't' )
				ch = '\t';
			else if ( ch == 'f' )
				ch = '\f';
			else if ( ch == 'v' )
				ch = '\v';
			else if ( ::isdigit(ch) )
				ch -= '0';
			else if ( ch == 'x' && pBegin + 2 < pEnd )			// Hex encode
			{
				int n1 = StringFunc::convertHexNumberToDec( *(pBegin + 1) );
				int n2 = StringFunc::convertHexNumberToDec( *(pBegin + 2) );
				if ( n1 >= 0 && n2 >= 0 )
				{
					ch = static_cast<char>( (n1 << 4) + n2 );
					pBegin += 2;
				}
			}
		} // if ( ch == '\\' && pBegin + 1 < pEnd )
		else if ( m_bReplaceEnvVar && ch == '$' && pBegin + 1 < pEnd && *(pBegin + 1) == '(' )
		{
			const char * pEnvVarEnd = findTheEnd(pBegin + 2, pEnd, ')');
			if ( pEnvVarEnd < pEnd )		// match ')' for environment variant
			{
				stdstring strEnvVarValue;
				if ( getReplaceEnvVar(stdstring(pBegin + 2, pEnvVarEnd - pBegin - 2), strEnvVarValue) )
					strSwap += strEnvVarValue;
				else
					strSwap.append(pBegin, pEnvVarEnd + 1 - pBegin);

				pBegin = pEnvVarEnd;
				continue;
			}
		} // else if ( m_bReplaceEnvVar && ch == '$' && pBegin + 1 < pEnd && *(pBegin + 1) == '(' )

		strSwap += ch;
	} // for ( ; pBegin < pEnd; ++pBegin )

	strSwap.swap(strValue);
}

bool IniConfigureParser::getIniLine(ReadLineStream & lineStream, stdstring & strLine, bool bTrimSpace) const
{
	stdstring strSwap, strReadSingleLine;
	while ( lineStream.readLine(strReadSingleLine) )
	{
		if ( strSwap.empty() )
			StringFunc::trimLeft(strReadSingleLine, StringFunc::g_strSpaceString);
		strSwap += strReadSingleLine;
		if ( !strSwap.empty() && !needNextLine(strReadSingleLine) )
			break;
		if ( !strSwap.empty() )
			strSwap.erase(strSwap.size() - 1);			// erase next line symbol '\' at end of line
	} //while ( lineStream.readLine(strReadSingleLine) )

	strSwap.swap(strLine);
	StringFunc::trimRight(strLine, StringFunc::g_strSpaceString);
	return ( !strLine.empty() );
}

bool IniConfigureParser::parseLine(const stdstring & strLine, stdstring & strKey, stdstring & strValue, stdstring & strComment) const
{
	if ( strLine.empty() )
		return false;

	// 查找key/value分隔符,优先使用'=',如果没有'='则使用第一个空格
	const char * pKeyValueSplitPos = NULL;
	const char * pSpacePos = NULL;
	const char * pCommentPos = NULL;

	const char * const pBegin = strLine.data();
	const char * const pEnd = pBegin + strLine.size();
	for ( const char * p = pBegin; p < pEnd; ++p )
	{
		char ch = *p;
		if ( ch == '\'' || ch == '\"' )
		{
			const char * pQuoteEnd = findTheEnd(p +1, pEnd, ch);
			if ( pQuoteEnd < pEnd )
				p = pQuoteEnd;
		}
		else if ( ch == '=' && pKeyValueSplitPos == NULL )
			pKeyValueSplitPos = p;
		else if ( (ch == ' ' || ch == '\t') && pSpacePos == NULL )
			pSpacePos = p;
		else if ( pCommentPos == NULL && m_strCommentSymbol.find(ch) != stdstring::npos )		// it is comment
			pCommentPos = p;
		else if ( ch == '\\' && p + 1 < pEnd )		// escape
			++p;
	} // for ( const char * p = pBegin;; p < pEnd; ++p )

	if ( pCommentPos == NULL )
		pCommentPos = pEnd;
	if ( pCommentPos + 1 < pEnd )
		strComment.assign(pCommentPos + 1, pEnd - pCommentPos - 1);
	else
		strComment.clear();

	if ( pCommentPos == pBegin )
		return false;

	if ( *pBegin == '[' )		// it is section name
	{
		strKey.assign(pBegin, pCommentPos - pBegin);
		strValue.clear();
	}
	else						// configure key value pair
	{
		if ( pKeyValueSplitPos == NULL )
			pKeyValueSplitPos = pSpacePos;
		if ( pKeyValueSplitPos == NULL )
			pKeyValueSplitPos = pCommentPos;

		strKey.assign(pBegin, pKeyValueSplitPos - pBegin);
		StringFunc::trim(strKey, StringFunc::g_strSpaceString);
		if ( strKey.empty() )
			return false;
		if ( pKeyValueSplitPos >= pCommentPos && !m_bSkipError )
			throw IniParserException("INI configure key/value line not found split symbol: " + strLine);

		if ( pKeyValueSplitPos + 1 < pCommentPos )
			strValue.assign(pKeyValueSplitPos + 1, pCommentPos - pKeyValueSplitPos - 1);
		else
			strValue.clear();
		StringFunc::trim(strValue, StringFunc::g_strSpaceString);
		if ( (strValue.size() >= 2) && (*strValue.begin() == '\'' || *strValue.begin() == '\"') )
		{
			const char * pValueBegin = strValue.data() + 1;
			const char * pValueEnd = findTheEnd(pValueBegin, strValue.data() + strValue.size(), *strValue.begin());
			if ( pValueEnd < strValue.data() + strValue.size() )
				strValue = strValue.assign(pValueBegin, pValueEnd - pValueBegin);
		}

		stdstring strDecodeValue;
		decodeValue(strValue.data(), strValue.data() + strValue.size(), strDecodeValue);
		strDecodeValue.swap(strValue);
	}

	return true;
}

void IniConfigureParser::parseContent(const stdstring & strContent, PVariant & varIniConfigure)
{
	BinaryStream binStream;
	binStream.attach(const_cast<char *>(strContent.data()), strContent.size(), false);
	ReadLineStream lineStream(binStream);
	return loadFromLineStream(lineStream, varIniConfigure);
}

void IniConfigureParser::loadFromStream(ReadStream & inputStream, PVariant & varIniConfigure)
{
	ReadLineStream lineStream(inputStream);
	return loadFromLineStream(lineStream, varIniConfigure);
}

void IniConfigureParser::loadFromLineStream(ReadLineStream & lineStream, PVariant & varIniConfigure)
{
	typedef std::vector<PVariant *>			typeSectionStack;
	stdstring strLine, strKey, strValue, strComment;
	typeSectionStack vecCurrentSectionStack;
	vecCurrentSectionStack.push_back( &varIniConfigure );

	while ( getIniLine(lineStream, strLine, true) )
	{
		if ( !parseLine(strLine, strKey, strValue, strComment) )
			continue;

		if ( strKey.empty() )
			continue;
		else if ( *strKey.begin() == '[' )		// it is section name
		{
			const char * pSectionEnd = findTheEnd(strKey.data() + 1, strKey.data() + strKey.size(), ']');
			if ( pSectionEnd >= strKey.data() + strKey.size() && !m_bSkipError )		// not match ']'
				throw IniParserException("INI configure section line not match end symbol ']': " + strLine);

			// remove all last section 
			while ( vecCurrentSectionStack.size() > 1 )
				vecCurrentSectionStack.pop_back();

			stdstring strSectionName;
			StringTokenizer<stdstring> tokenSection(strKey.data() + 1, pSectionEnd - strKey.data() - 1);
			while ( tokenSection.getNext(strSectionName, '/') )
			{
				PVariant & varLastSection = *(vecCurrentSectionStack.back());
				PVariant * pSubSection = varLastSection.find(strSectionName);
				if ( pSubSection == NULL )		// not found sub section, should insert
				{
					if ( !varLastSection.isNocaseStringMap() && !varLastSection.isStringMap() )
						varLastSection.asNocaseStringMap();
					pSubSection = &(varLastSection.add(strSectionName));
				}
				vecCurrentSectionStack.push_back(pSubSection);
			}
		}
		else		// configure key value pair
		{
			PVariant & varLastSection = *(vecCurrentSectionStack.back());
			if ( !varLastSection.isNocaseStringMap() && !varLastSection.isStringMap() )
				varLastSection.asNocaseStringMap();
			varLastSection.add(strKey, PVariant(strValue));
		}
	}
}

RFC_NAMESPACE_END
