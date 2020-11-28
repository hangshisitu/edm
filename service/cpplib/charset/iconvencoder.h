/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */
#include "rfcdefine.h"

#if (SUPPORT_ICONV_ENCODER != 0)

#ifndef RFC_ICONVENCODER_H_201008
#define RFC_ICONVENCODER_H_201008

#include "base/exception.h"
#include "base/globalfunc.h"

RFC_NAMESPACE_BEGIN

DECLARE_EXCEPTION(IConvException, Exception)

class IConvEncoder : private NonCopyAble
{
public:
	IConvEncoder(void) : m_bDiscardUnconvertible(true), m_pHandler(NULL) {}
	~IConvEncoder(void) { close(); }

	static bool		isSupportedCharset(const char * lpszCharset);
	static const char *	getCharsetName(const char * lpszCharset);
	void			setCharset(const char * lpszSrcCharset, const char * lpszDestCharset, bool bDiscardUnconvertible = true);

	template<typename typeString>
	bool			convert(const char * pSrcContent, size_t nSrcLen, typeString & strDestContent)
	{
		flush();
		if ( nSrcLen == 0 )
			return true;

		stdstring strSwap(nSrcLen * 4, RFC_CHAR_NULL);
		size_t nDestSize = strSwap.size();
		if ( !convert(pSrcContent, nSrcLen, const_cast<char *>(strSwap.data()), nDestSize) )
			return false;

		*strSwap.rbegin() = RFC_CHAR_NULL;
		nDestSize = GlobalFunc::getMin(nDestSize, StringFunc::length(strSwap.c_str()));
		strDestContent.append(strSwap.data(), nDestSize);
		return true;
	}

	template<typename typeSrcString, typename typeDestString>
	bool			convert(const typeSrcString & strSrcContent, typeDestString & strDestContent)
	{
		return convert(strSrcContent.data(), strSrcContent.size(), strDestContent);
	}

protected:
	void			close(void);
	void			flush(void);
	bool			convert(const char * pSrcContent, size_t nSrcLen, char * pDestContent, size_t & nDestSize);

private:
	bool			m_bDiscardUnconvertible;
	void *			m_pHandler;
};

class IConvProxy
{
public:
	//static void	convert(const stdstring & strSrcCharset, const void * pSrcContent, size_t nLength, CharsetString & strDestContent);

	static bool		isCharsetEqual(const char * lpszSrcCharset, const char * lpszDestCharset);

	template<typename typeString>
	static void		convert(const char * lpszSrcCharset, const char * lpszDestCharset, const char * pSrcContent, size_t nSrcLen, typeString & strDestContent)
	{
		if ( isCharsetEqual(lpszSrcCharset, lpszDestCharset) )
		{
			strDestContent.append(pSrcContent, nSrcLen);
			return;
		}

		IConvEncoder iconvEncoder;
		iconvEncoder.setCharset(lpszSrcCharset, lpszDestCharset);
		if ( !iconvEncoder.convert(pSrcContent, nSrcLen, strDestContent) )
			strDestContent.append(pSrcContent, nSrcLen);
	}
};

class DecodeHtmlHelper
{
public:
	// "&#22914;" 转成 "如", 返回是否做了decode
	static bool			decodeHtml(stdstring & strHtml, const char * lpszOutputCharset = "GBK");
	static stdstring &	decodeHtml(stdstring & strResult, const char * pHtml, size_t nSize, const char * lpszOutputCharset = "GBK");
	static stdstring	decodeHtml(const char * pHtml, size_t nSize, const char * lpszOutputCharset = "GBK");
	static stdstring &	decodeHtml(stdstring & strResult, const stdstring & strHtml, const char * lpszOutputCharset = "GBK");
};

RFC_NAMESPACE_END

#endif	//RFC_ICONVENCODER_H_201008

#endif	// #if (SUPPORT_ICONV_ENCODER != 0)

