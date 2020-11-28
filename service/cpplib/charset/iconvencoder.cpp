/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "charset/iconvencoder.h"

#if (SUPPORT_ICONV_ENCODER != 0)

#include "base/formatstream.h"
#include "base/byteorderfunc.h"

#include "iconv/iconv.h"
// typedef   void*   iconv_t; 

RFC_NAMESPACE_BEGIN

bool IConvEncoder::isSupportedCharset(const char * lpszCharset)
{
	return ( //StringFunc::strCasecmp(lpszCharset, "utf16") != 0
	//	&& StringFunc::strCasecmp(lpszCharset, "utf-16") != 0
	//	&& StringFunc::strCasecmp(lpszCharset, "utf-16be") != 0
	//	&& StringFunc::strCasecmp(lpszCharset, "utf-16le") != 0
		StringFunc::strCasecmp(lpszCharset, "utf32") != 0
		&& StringFunc::strCasecmp(lpszCharset, "utf-32") != 0
		&& StringFunc::strCasecmp(lpszCharset, "utf-32be") != 0
		&& StringFunc::strCasecmp(lpszCharset, "utf-32le") != 0 );
}

const char * IConvEncoder::getCharsetName(const char * lpszCharset)
{
	if ( StringFunc::strCasecmp(lpszCharset, "unicode-1-1-utf-7") == 0 )
		return "utf-7";
	else if ( StringFunc::strCasecmp(lpszCharset, "x-gbk") == 0 || StringFunc::strCasecmp(lpszCharset, "gb2312") == 0 )
		return "gbk";
	return lpszCharset;
}

void IConvEncoder::setCharset(const char * lpszSrcCharset, const char * lpszDestCharset, bool bDiscardUnconvertible)
{
	close();
	lpszSrcCharset = getCharsetName(lpszSrcCharset);
	lpszDestCharset = getCharsetName(lpszDestCharset);
	if ( !isSupportedCharset(lpszSrcCharset) )
		return;
	if ( (m_pHandler = static_cast<void *>(iconv_open(lpszDestCharset, lpszSrcCharset))) == (iconv_t)(-1) )
		return;	//throw IConvException( FormatString("iconv_open % -> % fail").arg(lpszSrcCharset).arg(lpszDestCharset).str() );
	m_bDiscardUnconvertible = bDiscardUnconvertible;
}

void IConvEncoder::close(void)
{
	if ( m_pHandler != NULL && m_pHandler != (iconv_t)(-1) )
	{
		iconv_close(static_cast<iconv_t>(m_pHandler));
		m_pHandler = NULL;
	}
}

void IConvEncoder::flush(void)
{
	if ( m_pHandler != NULL && m_pHandler != (iconv_t)(-1) )
		iconv(static_cast<iconv_t>(m_pHandler), NULL, NULL, NULL, NULL);
}

bool IConvEncoder::convert(const char * lpszSrc, size_t nSrcLen, char * pDestContent, size_t & nDestSize)
{
	if ( m_pHandler == NULL || m_pHandler == (iconv_t)(-1) )
		return false;

	if ( iconv(static_cast<iconv_t>(m_pHandler), (char ** )&lpszSrc, &nSrcLen, &pDestContent, &nDestSize) == (size_t)(-1) )
	{
		// An invalid multibyte sequence is encountered in the input.
		// In this case it sets errno to EILSEQ and returns (size_t)(-1).
		// *inbuf is left pointing to the beginning of the invalid multibyte sequence.
		if ( errno == EILSEQ && m_bDiscardUnconvertible )
		{
			int one = 1;
			iconvctl(static_cast<iconv_t>(m_pHandler), ICONV_SET_DISCARD_ILSEQ, &one);
			if ( m_pHandler != NULL && m_pHandler != (iconv_t)(-1) )
				iconv(static_cast<iconv_t>(m_pHandler), (char ** )&lpszSrc, &nSrcLen, &pDestContent, &nDestSize);
			m_bDiscardUnconvertible = false;
		}
	}

	if ( m_pHandler != NULL && m_pHandler != (iconv_t)(-1) )
	{
		if ( iconv(static_cast<iconv_t>(m_pHandler), NULL, NULL, &pDestContent, &nDestSize) == (size_t)(-1) )
			return false;
	}
	flush();
	return true;
}

bool IConvProxy::isCharsetEqual(const char * lpszSrcCharset, const char * lpszDestCharset)
{
	if ( StringFunc::strCasecmp(lpszSrcCharset, lpszDestCharset) == 0 )
		return true;
	if ( StringFunc::strnCasecmp(lpszSrcCharset, "US-ASCII", 8) == 0 || StringFunc::strnCasecmp(lpszDestCharset, "US-ASCII", 8) == 0 )
		return true;

	bool bSrcGB = ( StringFunc::strCasecmp(lpszSrcCharset, "gb2312") == 0 ||
					StringFunc::strCasecmp(lpszSrcCharset, "gbk") == 0 ||
					StringFunc::strCasecmp(lpszSrcCharset, "gb18030") == 0 );
	bool bDestGB = ( StringFunc::strCasecmp(lpszDestCharset, "gb2312") == 0 ||
					StringFunc::strCasecmp(lpszDestCharset, "gbk") == 0 ||
					StringFunc::strCasecmp(lpszDestCharset, "gb18030") == 0 );
	return ( bSrcGB && bDestGB );
}


bool DecodeHtmlHelper::decodeHtml(stdstring & strHtml, const char * lpszOutputCharset)
{
	if ( strHtml.find("&#") == stdstring::npos )
		return false;

	stdstring strResult;
	const char * pUTF16Charset = ByteOrderFunc::getUTF16Charset();
	const char * const pHtmlBegin = strHtml.data();
	const char * const pHtmlEnd = pHtmlBegin + strHtml.size();
	const char * pPlainBegin = pHtmlBegin;
	for ( const char * pCurPos = pHtmlBegin; pCurPos < pHtmlEnd; ++pCurPos )
	{
		bool bDecEncode = ( (pCurPos + 3 < pHtmlEnd) && pCurPos[0] == '&' && pCurPos[1] == '#' && ::isdigit(pCurPos[2]) );
		bool bHexEncode = ( (pCurPos + 4 < pHtmlEnd) && pCurPos[0] == '&' && pCurPos[1] == '#'
			&& StringFunc::tolower(pCurPos[2]) == 'x' && ::isxdigit(pCurPos[3]) );
		if ( bDecEncode || bHexEncode )
		{
			// "&#22914;" dec, min=1, max=7
			// "&#x5982;" hex, min=1, max=6
			int nEncodePrefixLen = ( bDecEncode ? 2 : 3 );
			const char * const pEncodeBegin = pCurPos + nEncodePrefixLen;
			for ( pCurPos = pEncodeBegin; pCurPos < pHtmlEnd; ++pCurPos )
			{
				if ( (bDecEncode && !::isdigit(*pCurPos)) || (bHexEncode && !::isxdigit(*pCurPos)) )
					break;
			} // for ( pCurPos = pEncodeBegin; pCurPos < pHtmlEnd; ++pCurPos )

			if ( *pCurPos == ';' && pEncodeBegin != pCurPos && pEncodeBegin + 7 >= pCurPos )
			{
				strResult.reserve(strHtml.size());
				if ( pPlainBegin + nEncodePrefixLen < pEncodeBegin )
					strResult.append(pPlainBegin, pEncodeBegin - nEncodePrefixLen - pPlainBegin);
				rfc_uint_16 nValue = StringFunc::stringToInt<rfc_uint_16>(pEncodeBegin, pCurPos - pEncodeBegin, bDecEncode ? 10 : 16);
				IConvProxy::convert(pUTF16Charset, lpszOutputCharset, reinterpret_cast<const char *>(&nValue), sizeof(nValue), strResult);
				pPlainBegin = pCurPos + 1;
			} //if ( *pCurPos == ';' && pEncodeBegin != pCurPos && pEncodeBegin + 7 >= pCurPos )
		} // if ( bDecEncode || bHexEncode )
	} // for ( const char * pCurPos = pHtmlBegin; ; ++pCurPos )

	if ( pHtmlBegin == pPlainBegin )		// Ã»ÓÐ±àÂë
		return false;
	strResult.append(pPlainBegin, pHtmlEnd - pPlainBegin);
	strHtml.swap(strResult);
	return true;
}

stdstring & DecodeHtmlHelper::decodeHtml(stdstring & strResult, const char * pHtml, size_t nSize, const char * lpszOutputCharset)
{
	strResult.assign(pHtml, nSize);
	decodeHtml(strResult, lpszOutputCharset);
	return strResult;
}

stdstring DecodeHtmlHelper::decodeHtml(const char * pHtml, size_t nSize, const char * lpszOutputCharset)
{
	stdstring strResult;
	return decodeHtml(strResult, pHtml, nSize, lpszOutputCharset);
}

stdstring & DecodeHtmlHelper::decodeHtml(stdstring & strResult, const stdstring & strHtml, const char * lpszOutputCharset)
{
	return decodeHtml(strResult, strHtml.data(), strHtml.size(), lpszOutputCharset);
}

RFC_NAMESPACE_END

#endif	// #if (SUPPORT_ICONV_ENCODER != 0)
