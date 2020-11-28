/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "charset/mimedecoder.h"
#include "charset/iconvencoder.h"

RFC_NAMESPACE_BEGIN

DecodeMailboxVisitor::DecodeMailboxVisitor(typeDecodeMailboxList & listDecodeMailbox)
	: m_listDecodeMailbox(listDecodeMailbox), m_strDestCharset(MimeDecoder::getDefaultCharset()) {}

DecodeMailboxVisitor::DecodeMailboxVisitor(typeDecodeMailboxList & listDecodeMailbox, const DwString & strDestCharset)
: m_listDecodeMailbox(listDecodeMailbox), m_strDestCharset(strDestCharset) {}

void DecodeMailboxVisitor::onVisitMailbox(const DwMailbox & aMailbox)
{
	DecodeMailbox aDecodeMailbox;
	if ( !aMailbox.FullName().empty() )
	{
		/*char chFirst = strFullName[0];
		size_t nFullNameLen = strFullName.length();
		if( (chFirst == '"' || chFirst == '\'') && nFullNameLen >= 2 && chFirst == strFullName[nFullNameLen - 1] )
			strFullName = strFullName.substr(1, nFullNameLen - 2); */
		MimeDecoder::decodeMimeString(aMailbox.FullName(), aDecodeMailbox.m_strFullName, false, m_strDestCharset);
	}

	aDecodeMailbox.m_strAddress = ( aMailbox.LocalPart() + '@' + aMailbox.Domain() );
	m_listDecodeMailbox.push_back(aDecodeMailbox);
}

void DecodeMailboxVisitor::convertListToString(const typeDecodeMailboxList & listDecodeMailbox, DwString & strDecodeList)
{
	typeDecodeMailboxList::const_iterator it, itEnd = listDecodeMailbox.end();
	for ( it = listDecodeMailbox.begin(); it != itEnd; ++it )
	{
		if ( !strDecodeList.empty() )
			strDecodeList += ", ";
		const DecodeMailbox & aDecodeMailbox = (*it);
		if ( !aDecodeMailbox.m_strFullName.empty() )
		{
			strDecodeList += aDecodeMailbox.m_strFullName;
			strDecodeList += ' ';
		}
		strDecodeList += '<';
		strDecodeList += aDecodeMailbox.m_strAddress;
		strDecodeList += '>';
	} // for ( it = listDecodeMailbox.begin(); it != itEnd; ++it )
}

//////////////////////////////////////////////////////////////////////////

DwString MimeDecoder::m_strDefaultCharset = "gbk";

/* RFC 2047
encoded-word = "=?" charset "?" encoding "?" encoded-text "?="
charset = token    ; see section 3
encoding = token   ; see section 4
token = 1*<Any CHAR except SPACE, CTLs, and especials>
especials = "(" / ")" / "<" / ">" / "@" / "," / ";" / ":" / "<"> / "/" / "[" / "]" / "?" / "." / "="
encoded-text = 1*<Any printable ASCII character other than "?" or SPACE> */
//如果只有一个encoded-word,没有其他数据,不做任何修改;其他格式添加空格以支持mimepp解码
void MimeDecoder::trimMimeString(const DwString & strEncodeSrc, DwString & strTrimEncodeDest)
{
	enum EncodedWordBlock
	{
		BLOCK_NOT_ENCODE, //不是encoded-word
		BLOCK_CHARSET,
		BLOCK_ENCODING,
		BLOCK_ENCODED_TEXT,
	};
	static const DwString g_strEspecials = "()<>@,;:/[]?.=";
	static const char * const g_pBeginEncode = "=?";
	static const char g_chDelimiter = '?';

	EncodedWordBlock nBlock = BLOCK_NOT_ENCODE;
	strTrimEncodeDest.clear();
	size_t nNotEncodeBegin = 0, nCurPos = 0, nSrcLen = strEncodeSrc.length();
	size_t nBeginEncode = strEncodeSrc.find(g_pBeginEncode, nNotEncodeBegin);
	while ( nBeginEncode != DwString::npos && nCurPos < nSrcLen )
	{
		nBlock = BLOCK_CHARSET;
		for ( nCurPos = nBeginEncode + 2; nCurPos < nSrcLen; ++nCurPos ) //skip "=?"
		{
			char ch = strEncodeSrc[nCurPos];
			if ( nBlock == BLOCK_CHARSET || nBlock == BLOCK_ENCODING )
			{
				if ( ch == g_chDelimiter ) //go to next block
					nBlock = (nBlock == BLOCK_CHARSET) ? BLOCK_ENCODING : BLOCK_ENCODED_TEXT;
				else if ( ::isspace(ch) || ::iscntrl(ch) || g_strEspecials.find(ch) != DwString::npos )
					nBlock = BLOCK_NOT_ENCODE;
			} //if ( nBlock == BLOCK_CHARSET || nBlock == BLOCK_ENCODING )
			else if ( nBlock == BLOCK_ENCODED_TEXT )
			{
				if ( ch == g_chDelimiter && nCurPos + 1 < nSrcLen && strEncodeSrc[nCurPos+1] == '=' ) //it is end of encoded-word
				{
					if ( nNotEncodeBegin != nBeginEncode )
					{
						strTrimEncodeDest += strEncodeSrc.substr(nNotEncodeBegin, nBeginEncode - nNotEncodeBegin);
						strTrimEncodeDest += ' ';
					}
					nNotEncodeBegin = nCurPos + 2; //go to end of "?="
					strTrimEncodeDest += strEncodeSrc.substr(nBeginEncode, nNotEncodeBegin - nBeginEncode);
					strTrimEncodeDest += ' ';
					nBeginEncode = strEncodeSrc.find(g_pBeginEncode, nCurPos + 2);
					break;
				}
				else if ( ch == g_chDelimiter || ::isspace(ch) )
					nBlock = BLOCK_NOT_ENCODE;
			} //else if ( nBlock == BLOCK_ENCODED_TEXT )
			if ( nBlock == BLOCK_NOT_ENCODE )
			{
				strTrimEncodeDest += strEncodeSrc.substr(nNotEncodeBegin, nCurPos - nNotEncodeBegin);
				nNotEncodeBegin = nCurPos;
				nBeginEncode = strEncodeSrc.find(g_pBeginEncode, nNotEncodeBegin);
				break;
			} //if ( nBlock == BLOCK_NOT_ENCODE )
		} //for ( nCurPos = nBeginEncode + 2; nCurPos < nSrcLen; ++nCurPos ) //skip "=?"
	} //while ( nBeginEncode != DwString::npos )
	if ( nNotEncodeBegin != 0 && nNotEncodeBegin < nSrcLen )
		strTrimEncodeDest += strEncodeSrc.substr(nNotEncodeBegin);
	if ( strTrimEncodeDest.empty() )
		strTrimEncodeDest = strEncodeSrc;
}

void MimeDecoder::decodeOneWord(const DwEncodedWord & dwWord, const DwString & strDestCharset, DwString & strPreCharset,
								DwString & strPreEncodedData, DwString & strDestContent)
{
	const DwString & strWordCharset = dwWord.Charset();
	if ( DwStrcasecmp(strPreCharset, strWordCharset) != 0 && !strPreEncodedData.empty() ) // 编码不同,对前一部分解码
	{
		DwString strPreDecodeDest;
		convertCharset(strPreEncodedData, strPreCharset, strPreDecodeDest, strDestCharset);
		//if ( !strDestContent.empty() ) // 普通字符前加空格
		//	strDestContent += ' ';
		strDestContent += strPreDecodeDest;
		strPreEncodedData.clear();
	} // if // 编码不同,对前一部分解码

	strPreEncodedData += dwWord.DecodedText();
	strPreCharset = strWordCharset;
}

void MimeDecoder::decodeMimeString(const DwString & strSrcContent, DwString & strDestContent, bool bSubjectFiled, const DwString & strDestCharset)
{
	if ( strSrcContent.empty() )
		return;
	DwString strTrimEncodeDest;
	trimMimeString(strSrcContent, strTrimEncodeDest);
	DwText dwText(strTrimEncodeDest);
	dwText.SetEncodeTextSymbol(bSubjectFiled);
	dwText.Parse();

	DwString strPreCharset, strPreEncodedData;
	int nWordCnt = dwText.NumEncodedWords();
	for ( int i = 0; i < nWordCnt; i++ )
	{
		decodeOneWord(dwText.EncodedWordAt(i), strDestCharset, strPreCharset, strPreEncodedData, strDestContent);
	} //for ( int i = 0; i < nWordCnt; i++ )

	if ( !strPreEncodedData.empty() ) // 对剩余部分解码
	{
		DwString strPreDecodeDest;
		convertCharset(strPreEncodedData, strPreCharset, strPreDecodeDest, strDestCharset);
		//if ( !strDestContent.empty() ) // 普通字符前加空格
		//	strDestContent += ' ';
		strDestContent += strPreDecodeDest;
	}
}

void MimeDecoder::encodeMimeString(const DwString & strSrcContent, const DwString & strSrcCharset, DwString & strDestContent,
								   const DwString & strDestCharset, char chEncoding)
{
	if ( strSrcContent.empty() )
		return;

	// if all characters are 7-bit, then don't encode
	size_t n8BitChars = 0;
	const char * pEnd = strSrcContent.data() + strSrcContent.size();
	for ( const char * p = strSrcContent.data(); p < pEnd; ++p )
	{
		int ch = static_cast<int>( *p );
		if ( ch < 32 || ch > 127 )
			++n8BitChars;
	} // for ( const char * p = strSrcContent.data(); p < pEnd; ++p )
	if ( n8BitChars == 0 )
	{
		strDestContent += strSrcContent;
		return;
	}

	DwString strConvertCharsetDest;
	IConvEncoder iconvEncoder;
	iconvEncoder.setCharset(strSrcCharset.c_str(), strDestCharset.c_str());
	if ( !iconvEncoder.convert(strSrcContent.data(), strSrcContent.size(), strDestContent) )
	{
		strDestContent += strSrcContent;
		return;
	}

	if ( chEncoding != 'Q' && chEncoding != 'q' && chEncoding != 'B' && chEncoding != 'b' )
		chEncoding = (5 * n8BitChars < strSrcContent.size()) ? 'Q' : 'B';
	DwEncodedWord dwEncodeWord;
	dwEncodeWord.SetCharset(strDestCharset.c_str());
	dwEncodeWord.SetEncodingType(chEncoding);
	dwEncodeWord.SetDecodedText(strConvertCharsetDest);
	dwEncodeWord.Assemble();
	strDestContent = dwEncodeWord.AsString();
}

void MimeDecoder::convertCharset(const char * pSrcContent, size_t nSrcLen, const DwString & strSrcCharset, DwString & strDestContent,
								const DwString & strDestCharset)
{
	DwString strFromCharset = ( (strSrcCharset.empty() || strSrcCharset == "US-ASCII") ? m_strDefaultCharset : strSrcCharset );
	DwString strToCharset = ( (strDestCharset.empty() || strDestCharset == "US-ASCII") ? m_strDefaultCharset : strDestCharset );
	IConvProxy::convert(strFromCharset.c_str(), strToCharset.c_str(), pSrcContent, nSrcLen, strDestContent);
}

void MimeDecoder::convertCharset(const DwString & strSrcContent, const DwString & strSrcCharset, DwString & strDestContent,
								 const DwString & strDestCharset)
{
	convertCharset(strSrcContent.data(), strSrcContent.size(), strSrcCharset, strDestContent, strDestCharset);
}

void MimeDecoder::decodeMailbox(const DwMailbox& aMailbox, typeDecodeMailboxList & listDecodeMailbox, const DwString & strDestCharset)
{
	DecodeMailboxVisitor decodeMailboxVisitor(listDecodeMailbox, strDestCharset);
	decodeMailboxVisitor.onVisitMailbox(aMailbox);
}

void MimeDecoder::decodeAddress(const DwAddress & dwAddress, typeDecodeMailboxList & listDecodeMailbox, const DwString & strDestCharset)
{
	DecodeMailboxVisitor decodeMailboxVisitor(listDecodeMailbox, strDestCharset);
	MimeParser::parseAddress(dwAddress, decodeMailboxVisitor);
}

void MimeDecoder::decodeAddressField(const DwField & dwField, typeDecodeMailboxList & listDecodeMailbox, const DwString & strDestCharset)
{
	DecodeMailboxVisitor decodeMailboxVisitor(listDecodeMailbox, strDestCharset);
	MimeParser::parseAddressField(dwField, decodeMailboxVisitor);
}

void MimeDecoder::decodeMailboxList(const DwMailboxList & dwMailBoxList, typeDecodeMailboxList & listDecodeMailbox, const DwString & strDestCharset)
{
	DecodeMailboxVisitor decodeMailboxVisitor(listDecodeMailbox, strDestCharset);
	MimeParser::parseMailBoxList(dwMailBoxList, decodeMailboxVisitor);
}

void MimeDecoder::decodeMailboxList(const DwString & strMailBoxList, typeDecodeMailboxList & listDecodeMailbox, const DwString & strDestCharset)
{
	DecodeMailboxVisitor decodeMailboxVisitor(listDecodeMailbox, strDestCharset);
	MimeParser::parseMailBoxList(strMailBoxList, decodeMailboxVisitor);
}

void MimeDecoder::decodeMailboxList(const DwMailboxList & dwMailBoxList, DwString & strDecodeMailboxList, const DwString & strDestCharset)
{
	typeDecodeMailboxList listDecodeMailbox;
	DecodeMailboxVisitor decodeMailboxVisitor(listDecodeMailbox, strDestCharset);
	MimeParser::parseMailBoxList(dwMailBoxList, decodeMailboxVisitor);
	DecodeMailboxVisitor::convertListToString(listDecodeMailbox, strDecodeMailboxList);
}

void MimeDecoder::decodeAddressList(const DwAddressList & dwAddressList, typeDecodeMailboxList & listDecodeMailbox, const DwString & strDestCharset)
{
	DecodeMailboxVisitor decodeMailboxVisitor(listDecodeMailbox, strDestCharset);
	MimeParser::parseAddressList(dwAddressList, decodeMailboxVisitor);
}

void MimeDecoder::decodeAddressList(const DwString & strAddressList, typeDecodeMailboxList & listDecodeMailbox, const DwString & strDestCharset)
{
	DecodeMailboxVisitor decodeMailboxVisitor(listDecodeMailbox, strDestCharset);
	MimeParser::parseAddressList(strAddressList, decodeMailboxVisitor);
}

void MimeDecoder::decodeAddressList(const DwAddressList & dwAddressList, DwString & strDecodeMailboxList, const DwString & strDestCharset)
{
	typeDecodeMailboxList listDecodeMailbox;
	DecodeMailboxVisitor decodeMailboxVisitor(listDecodeMailbox, strDestCharset);
	MimeParser::parseAddressList(dwAddressList, decodeMailboxVisitor);
	DecodeMailboxVisitor::convertListToString(listDecodeMailbox, strDecodeMailboxList);
}

void MimeDecoder::decodeSubject(const DwHeaders & dwHeader, DwString & strSubject, const DwString & strDestCharset)
{
	const DwField * pSubjectField = dwHeader.FindField("Subject");
	if ( pSubjectField == NULL || pSubjectField->FieldBody() == NULL  )
		return;
	decodeMimeString(pSubjectField->FieldBody()->AsString(), strSubject, true, strDestCharset);
}

RFC_NAMESPACE_END
