/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_STRING_H_201008
#define RFC_STRING_H_201008

#include "rfcdefine.h"
#include <string>

RFC_NAMESPACE_BEGIN

typedef std::string						stdstring;
typedef std::wstring					stdwstring;

class StringFunc
{
public:
	static const stdstring				g_strSpaceString;
	static const stdstring				g_strCRLF;
	static const stdstring				g_strEmptyString;
	static const stdwstring				g_wstrEmptyWideString;

	static const stdstring				g_strUpperHexChar;
	static const stdstring				g_strLowerHexChar;
	static const unsigned char			g_UpcaseCharsetMap[];			// 大写格式字符集
	static const unsigned char			g_LowcaseCharsetMap[];

	//在GB2312区位码表中,0xb0a1－0xf7fe是汉字,每个汉字由两个字节组成
	static inline bool isGBKHanZi(rfc_uint_8 chFirstByte)
	{
		static const rfc_uint_8 g_chGBKHanZiBegin = static_cast<rfc_uint_8>(0xb0);
		static const rfc_uint_8 g_chGBKHanZiEnd = static_cast<rfc_uint_8>(0xf7);
		return ( g_chGBKHanZiBegin <= chFirstByte && chFirstByte <= g_chGBKHanZiEnd );
	}

	static inline int toupper(int ch)
	{
		unsigned int nIndex = static_cast<unsigned int>(ch);
		if ( nIndex >= 256 )
			return ch;
		return static_cast<int>(g_UpcaseCharsetMap[nIndex]);
	}

	static inline int tolower(int ch)
	{
		unsigned int nIndex = static_cast<unsigned int>(ch);
		if ( nIndex >= 256 )
			return ch;
		return static_cast<int>(g_LowcaseCharsetMap[nIndex]);
	}

	template<class typeString>
	static typeString & toUpper(typeString & strVal)
	{ 
		std::transform(strVal.begin(), strVal.end(), strVal.begin(), StringFunc::toupper);
		return strVal;
	}

	template<class typeString>
	static typeString & toLower(typeString & strVal)
	{ 
		std::transform(strVal.begin(), strVal.end(), strVal.begin(), StringFunc::tolower);
		return strVal;
	}

	static inline char getUpperHexChar(int n)
	{
		if ( n < 0 || n >= 16 )
			return RFC_CHAR_NULL;
		return g_strUpperHexChar[n];
	}

	static inline char getLowerHexChar(int n)
	{
		if ( n < 0 || n >= 16 )
			return RFC_CHAR_NULL;
		return g_strLowerHexChar[n];
	}

	static inline int convertHexNumberToDec(int nHexNumber)
	{
		if ( ::isdigit(nHexNumber) )				// '0' - '9'
			return nHexNumber - 48;					// '0' = 48
		else if ( ::isalnum(nHexNumber) )
			return StringFunc::toupper(nHexNumber + 10) - 65;		// 'A' = 65
		return -1;		//is not hex number
	}

	template<typename typeChar>
	static size_t length(const typeChar * p)
	{
		if ( p == NULL )
			return 0;
		typeChar chNULL = typeChar();
		const typeChar * pScan = p;
		for ( ; *pScan != chNULL; ++pScan )
			;
		return static_cast<size_t>( pScan - p );
	}

	// param str1, str2 Null-terminated strings to compare
	// return code = 0: str1 identical to str2
	// return code > 0:	str1 greater than str2
	// return code < 0:	str1 less than str2
	template<typename typeChar>
	static int			strCasecmp(const typeChar * str1, const typeChar * str2)
	{
		typeChar chNULL = typeChar();
		while ( tolower(*str1) == tolower(*str2) )
		{
			if ( *str1 == chNULL )
				return 0;
			++str1;
			++str2;
		} // while ( tolower(*str1) == tolower(*str2) )
		return ( tolower(*str1) - tolower(*str2) );
	}

	template<typename typeChar>
	static int			strCasecmp(const typeChar * str1, size_t l1, const typeChar * str2, size_t l2)
	{
		typeChar chNULL = typeChar();
		size_t n = ( l1 < l2 ? l1 : l2 );
		while ( n != 0 && tolower(*str1) == tolower(*str2) )
		{
			if ( *str1 == chNULL )
				break;
			++str1;
			++str2;
			--n;
		} // while ( n != 0 && tolower(*str1) == tolower(*str2) )
		return (n == 0) ? static_cast<int>(l1 - l2) : ( tolower(*str1) - tolower(*str2) );
	}

	template<typename typeString>
	static inline int	strCasecmp(const typeString & str1, const typename typeString::value_type * str2)
	{
		return strCasecmp(str1.data(), str1.size(), str2, StringFunc::length(str2));
	}

	template<typename typeString>
	static inline int	strCasecmp(const typename typeString::value_type * str1, const typeString & str2)
	{
		return strCasecmp(str1, StringFunc::length(str1), str2.data(), str2.size());
	}

	template<typename typeString>
	static inline int	strCasecmp(const typeString & str1, const typeString & str2)
	{
		return strCasecmp(str1.data(), str1.size(), str2.data(), str2.size());
	}

	template<typename typeString>
	static inline int	strCasecmp(const typeString & str1, size_t offset1, size_t l1, const typeString & str2, size_t offset2, size_t l2)
	{
		return strCasecmp(str1.data() + offset1, l1, str2.data() + offset2, l2);
	}

	template<typename typeChar>
	static inline bool	strCaseEqual(const typeChar * str1, size_t l1, const typeChar * str2, size_t l2)
	{
		if ( l1 != l2 )
			return false;
		return ( strCasecmp(str1, l1, str2, l2) == 0 );
	}

	template<typename typeString>
	static inline bool	strCaseEqual(const typeString & str1, const typename typeString::value_type * str2)
	{
		return strCaseEqual(str1.data(), str1.size(), str2, StringFunc::length(str2));
	}

	template<typename typeString>
	static inline bool	strCaseEqual(const typename typeString::value_type * str1, const typeString & str2)
	{
		return strCaseEqual(str1, StringFunc::length(str1), str2.data(), str2.size());
	}

	template<typename typeString>
	static inline bool	strCaseEqual(const typeString & str1, const typeString & str2)
	{
		return strCaseEqual(str1.data(), str1.size(), str2.data(), str2.size());
	}

	template<typename typeChar>
	static inline int	strnCasecmp(const typeChar * str1, const typeChar * str2, size_t n)
	{
		return strCasecmp(str1, n, str2, n);
	}

	template<typename typeString>
	static inline int	strnCasecmp(const typeString & str1, const typename typeString::value_type * str2, size_t n)
	{
		return strCasecmp(str1.data(), (str1.size() < n ? str1.size() : n), str2, n);
	}

	template<typename typeString>
	static inline int	strnCasecmp(const typename typeString::value_type * str1, const typeString & str2, size_t n)
	{
		return strCasecmp(str1, n, str2.data(), n);
	}

	template<typename typeString>
	static inline int	strnCasecmp(const typeString & str1, const typeString & str2, size_t n)
	{
		return strCasecmp(str1.data(), (str1.size() < n ? str1.size() : n), str2.data(), n);
	}

	template<typename typeChar>
	static size_t		strnCpy(typeChar * pDest, const typeChar * pSrc, size_t nCount)
	{
		typeChar chNULL = typeChar();
		size_t nCopySrcLen = 0;
		for ( ; nCopySrcLen < nCount && pSrc[nCopySrcLen] != chNULL; ++nCopySrcLen )
			pDest[nCopySrcLen] = pSrc[nCopySrcLen];

		pDest[nCopySrcLen] = chNULL;			//set NULL at end of pDest
		return nCopySrcLen;
	}

	template<typename typeString>
	static typeString & reverseAppend(typeString & strDest, const typename typeString::value_type * pSrc, size_t nLen)
	{
		if ( pSrc == NULL || nLen == 0 )
			return strDest;
		strDest.reserve( strDest.size() + nLen );
		for ( --nLen; nLen > 0; --nLen )
			strDest += pSrc[nLen];
		strDest += pSrc[0];
		return strDest;
	}

	template<typename typeStringSrc, typename typeStringDest>
	static typeStringDest & reverseAppend(typeStringDest & strDest, const typeStringSrc & strSrc)
	{
		return reverseAppend(strDest, strSrc.data(), strSrc.size());
	}

	template<typename typeString>
	static bool			checkPrefix(const typeString & strSrc, const typeString & strPrefix, bool bCaseSensitive = true)
	{
		if ( bCaseSensitive )		//大小写敏刚
			return strSrc.size() >= strPrefix.size() && strSrc.compare(0, strPrefix.size(), strPrefix) == 0;
		return strnCasecmp(strSrc, strPrefix, strPrefix.size()) == 0;
	}

	template<typename typeString>
	static bool			checkPostfix(const typeString & strSrc, const typeString & strPostfix, bool bCaseSensitive = true)
	{
		if ( strSrc.size() < strPostfix.size() )
			return false;
		if ( bCaseSensitive )		//大小写敏刚
			return strSrc.compare(strSrc.size() - strPostfix.size(), strPostfix.size(), strPostfix) == 0;
		return strCasecmp(strSrc, strSrc.size() - strPostfix.size(), strPostfix.size(), strPostfix, 0, strPostfix.size()) == 0;
	}

	template<class typeString>
	static typeString & trimLeft(typeString & strVal, const typeString & strSpace)
	{
		strVal.erase(0, strVal.find_first_not_of(strSpace));
		return strVal;
	}

	template<class typeString>
	static typeString & trimRight(typeString & strVal, const typeString & strSpace)
	{
		strVal.erase( strVal.find_last_not_of(strSpace) + 1 );
		return strVal;
	}

	template<class typeString>
	static typeString & trim(typeString & strVal, const typeString & strSpace)
	{
		return trimLeft( trimRight(strVal, strSpace), strSpace );
	}

	static stdstring & trimSpace(stdstring & strVal)
	{
		return trim(strVal, g_strSpaceString);
	}

	template<typename typeString>
	static void			encodeEscapeString(const typeString & strSrc, typeString & strResult)
	{
		typeString strSwap;
		const typename typeString::value_type * tpEnd = strSrc.data() + strSrc.size();
		for ( const typename typeString::value_type * tp = strSrc.data(); tp < tpEnd; ++tp )
		{
			typename typeString::value_type ch = (*tp);
			if ( ch == '\r' )
				strSwap.append("\\r", 2);
			else if ( ch == '\n' )
				strSwap.append("\\n", 2);
			else if ( ch == '\t' )
				strSwap.append("\\t", 2);
			else if ( ch == '\f' )
				strSwap.append("\\f", 2);
			else if ( ch == '\v' )
				strSwap.append("\\v", 2);
			else
				strSwap += ch;
		} // for ( const typename typeString::value_type * tp = strSrc.data(); tp < tpEnd; ++tp )
		strResult.swap(strSwap);
	}

	template<typename typeString>
	static bool			decodeEscapeString(const typename typeString::value_type * & tp, const typename typeString::value_type * tpEnd,
									const typeString & strBreakChars, typeString & strValue)
	{
		if ( tp == NULL || tp >= tpEnd )
			return false;

		for ( ; tp < tpEnd && strBreakChars.find(*tp) == typeString::npos; ++tp )
		{
			typename typeString::value_type ch = (*tp);
			if( ch == '\\' && tp + 1 < tpEnd )
			{
				ch = *(++tp);
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
				else if ( ch == 'x' && tp + 2 < tpEnd )			// Hex encode
				{
					int n1 = convertHexNumberToDec( *(tp + 1) );
					int n2 = convertHexNumberToDec( *(tp + 2) );
					if ( n1 >= 0 && n2 >= 0 )
					{
						ch = static_cast<typename typeString::value_type>( (n1 << 4) + n2 );
						tp += 2;
					}
				}
			} // if( ch == '\\' && tp + 1 < tpEnd )
			strValue += ch;
		} // for( ; tp < tpEnd && strBreakChars.find(*tp) == typeString::npos; ++tp )

		if ( tp < tpEnd && strBreakChars.find(*tp) != typeString::npos )
			++tp;
		return true;
	}

	template<typename typeString>
	static bool			decodeEscapeString(const typename typeString::value_type * & tp, const typename typeString::value_type * tpEnd,
									typename typeString::value_type chEnd, typeString & strValue)
	{
		return decodeEscapeString(tp, tpEnd, typeString(1, chEnd), strValue);
	}

	template<typename typeInt, typename typeChar>
	static typeInt		stringToInt(const typeChar * pStr, size_t nLen = std::basic_string<typeChar>::npos, rfc_uint_8 nBase = 10)
	{
		typeInt nResult(0);
		if ( nBase < 2 || nBase > 16 )
			return nResult;

		const typeChar * tpEnd = pStr + (nLen == std::basic_string<typeChar>::npos ? StringFunc::length(pStr) : nLen );
		for ( const typeChar * tp = skipSign(pStr, tpEnd); tp < tpEnd; ++tp )
		{
			int nDecNumber = convertHexNumberToDec(*tp);
			if ( nDecNumber < 0 || static_cast<rfc_uint_8>(nDecNumber) >= nBase )
				break;
			nResult *= nBase;
			nResult += static_cast<rfc_uint_8>(nDecNumber);
		} // for ( const typeChar * tp = skipSign(pStr, tpEnd); tp < tpEnd; ++tp )
		return getSignedResult(nResult, pStr, tpEnd);
	}

	template<typename typeInt, typename typeChar>
	static typeInt		smartConvertDigit(const typeChar * & lpszStr, size_t nDigitCount, rfc_uint_8 nBase = 10)
	{
		while ( *lpszStr != RFC_CHAR_NULL && ::isspace(*lpszStr) )
			++lpszStr;

		typeInt nResult(0);
		if ( nBase < 2 || nBase > 16 )
			return nResult;
		const typeChar * pSaveBegin = lpszStr;
		const typeChar * pEnd = lpszStr + nDigitCount;
		for ( lpszStr = skipSign(lpszStr, pEnd); lpszStr < pEnd && *lpszStr != RFC_CHAR_NULL; ++lpszStr )
		{
			int nDecNumber = convertHexNumberToDec(*lpszStr);
			if ( nDecNumber < 0 || static_cast<rfc_uint_8>(nDecNumber) >= nBase )
				break;
			nResult *= nBase;
			nResult += static_cast<rfc_uint_8>(nDecNumber);
		} // for ( lpszStr = skipSign(lpszStr, pEnd);; lpszStr < pEnd && *lpszStr != RFC_CHAR_NULL; ++lpszStr )
		return getSignedResult(nResult, pSaveBegin, pEnd);
	}

	template<typename typeInt, typename typeChar>
	static typeInt		smartConvertDigit2(const typeChar * lpszStr, size_t nDigitCount, rfc_uint_8 nBase = 10)
	{
		const typeChar * p = lpszStr;
		return smartConvertDigit(p, nDigitCount, nBase);
	}

	template<typename typeString, typename typeInt>
	static void	addIntToString(typeString & strResult, typeInt nValue, rfc_uint_8 nBase = 10, bool bUpcase = true, int nWidth = -1, int nFill = 0)
	{
		const stdstring & strHexCharset = ( bUpcase ? g_strUpperHexChar : g_strLowerHexChar );
		if ( nValue == 0 || nBase < 2 || nBase > 16 )
		{
			for ( --nWidth; nFill != 0 && nWidth > 0; --nWidth )
				strResult += static_cast<typename typeString::value_type>(nFill);
			strResult += static_cast<typename typeString::value_type>( strHexCharset[0] );
			return;
		}

		bool bSigned = skipSign(nValue);
		typeString strBuffer;
		strBuffer.reserve( (sizeof(typeInt) << 3) + 1 );
		for ( ; nValue > 0; nValue /= nBase )
			strBuffer += static_cast<typename typeString::value_type>( strHexCharset[nValue % nBase] );

		if ( bSigned )
			strResult += static_cast<typename typeString::value_type>('-');
		for ( nWidth -= static_cast<int>(strBuffer.size()); nFill != 0 && nWidth > 0; --nWidth )
			strResult += static_cast<typename typeString::value_type>(nFill);
		reverseAppend(strResult, strBuffer);
	}

	template<typename typeInt>
	static stdstring	intToString(typeInt nValue, rfc_uint_8 nBase = 10, bool bUpcase = true, int nWidth = -1, int nFill = 0)
	{
		stdstring strResult;
		addIntToString(strResult, nValue, nBase, bUpcase, nWidth, nFill);
		return strResult;
	}

	// 0x 开头是16进制; 0开头是8进制; 其他是10进制. 成功解码返回true;否则返回false; 只支持正数
	static bool			decodeNumberToDec(const char * pEncodeNumber, size_t nLen, rfc_uint_32 & nDecimalResult);

	static rfc_int_64	convertSizeUnitToByte(const char * pStr, size_t nLen = std::basic_string<char>::npos);
	static rfc_int_64	convertPeriodUnitToMilliSecond(const char * pStr, size_t nLen = std::basic_string<char>::npos);	//转为毫秒

	// 检查strLine是否以'\n'结尾,如果不是在后面加"\r\n"
	static void			appendCRLF(stdstring & strLine);
	static void			removeCRLF(stdstring & strLine);

	//! A lightweight and simple pattern matching function
	//! match the pSrcStr with '*' & '?'
	//! @auth By Jack Handy
	//! Copy From www.codeproject.com
	template<typename typeChar>
	static bool			wildcardFullMatch(const typeChar * pWildcard, const typeChar * pSrcStr,
								typeChar chWildMatch = typeChar('*'), typeChar chSingleMatch = typeChar('?') )
	{
		const typeChar * cp = NULL;
		const typeChar * mp = NULL;
		while ( (*pSrcStr) && (*pWildcard != chWildMatch) ) 
		{
			if ( (*pWildcard != *pSrcStr) && (*pWildcard != chSingleMatch) ) 
				return false;
			++pWildcard;
			++pSrcStr;
		}

		while ( *pSrcStr ) 
		{
			if ( *pWildcard == chWildMatch ) 
			{
				if ( !*++pWildcard )
					return true;
				mp = pWildcard;
				cp = pSrcStr + 1;
			} 
			else if ( (*pWildcard == *pSrcStr) || (*pWildcard == chSingleMatch) ) 
			{
				++pWildcard;
				++pSrcStr;
			}
			else 
			{
				pWildcard = mp;
				pSrcStr = cp++;
			}
		}

		while ( *pWildcard == chWildMatch ) 
			++pWildcard;
		return !*pWildcard;
	}

	template<class typeString>
	static bool checkFullMatch(const typeString & strValue, const typeString & strFilterKey, bool bCaseSensitive)
	{
		return ( (bCaseSensitive && strValue == strFilterKey) ||
			(!bCaseSensitive && StringFunc::strCaseEqual(strValue, strFilterKey)) );
	}

	template<class typeString>
	static bool checkSubKeyMatch(const typeString & strValue, const typeString & strFilterKey, const typeString & strDelimiter, bool bCaseSensitive)
	{
		if( checkFullMatch(strValue, strFilterKey, bCaseSensitive) )
			return true;
		if( strValue.size() <= strFilterKey.size() )
			return false;

		typeString strLowerValue(strDelimiter + strValue + strDelimiter);
		typeString strLowerKey(strDelimiter + strFilterKey + strDelimiter);
		if( !bCaseSensitive )
		{
			StringFunc::toLower(strLowerValue);
			StringFunc::toLower(strLowerKey);
		}
		return ( strLowerValue.find(strLowerKey) != typeString::npos );
	}

	// 检查第一个字符和最后一个字符是否一致
	template<typename typeChar>
	static inline bool checkBeginEndPair(const typeChar * pBegin, const typeChar * pEnd, typeChar chBegin, typeChar chEnd)
	{
		if ( pBegin == NULL || pEnd == NULL || pBegin + 2 > pEnd )
			return false;
		return ( *pBegin == chBegin && *pEnd == chEnd );
	}

protected:
	template<typename typeChar>
	static inline const typeChar * skipSign(const typeChar * tp, const typeChar * tpEnd)
	{
		if ( (tp < tpEnd) && (*tp == typeChar('-') || *tp == typeChar('+')) )
			return tp + 1;
		return tp;
	}

	template<typename typeInt>
	static inline bool skipSign(typeInt & nVal)
	{
		if ( nVal >= 0 )
			return false;
		nVal = -nVal;
		return true;
	}

	template<typename typeInt, typename typeChar>
	static inline typeInt getSignedResult(const typeInt & nResult, const typeChar * tp, const typeChar * tpEnd)
	{
		if ( tp < tpEnd && *tp == typeChar('-') && nResult != 0 )
			return -nResult;
		return nResult;
	}
};

struct IgnoreCaseStringCompare {
	template<typename typeString>
	bool operator()(const typeString & a, const typeString & b) const {
		return StringFunc::strCasecmp(a, b) < 0;
	}
};

struct IgnoreCaseHashStringCompare {
	template<typename typeString>
	int operator()(const typeString & a, const typeString & b) const {
		return StringFunc::strCasecmp(a, b);
	}
};

class SimpleString
{
public:
	typedef char		typeChar;
	SimpleString(void) : m_pStorage(NULL) {}
	SimpleString(const typeChar * pStorage) : m_pStorage(NULL) { reset(pStorage); }
	SimpleString(const SimpleString & p) : m_pStorage(NULL) { reset(p.m_pStorage); }
	~SimpleString(void) { clear(); }

	SimpleString & operator = (const SimpleString & p) { reset(p.m_pStorage); return *this; }
	SimpleString & operator = (const typeChar * pStorage) { reset(pStorage); return *this; }

	bool				empty(void) const	{ return (m_pStorage == NULL); }
	void				clear(void);
	void				reset(const typeChar * pSrc);
	const typeChar *	get(void) const;		//如果m_pStorage==NULL, return StringFunc::g_strEmptyString

private:
	typeChar *			m_pStorage;
};

class FindSubString
{
public:
	typedef char		typeChar;
	typedef rfc_uint_8	typeUnsignedChar;
	FindSubString(void) : m_nKeySize(0), m_pKeyBuffer(NULL),  m_pIndex(NULL) {}
	FindSubString(const typeChar * pKey, size_t nKeySize) : m_nKeySize(0), m_pKeyBuffer(NULL), m_pIndex(NULL)
	{ compileKeyWord(pKey, nKeySize); }
	FindSubString(const FindSubString & s) : m_nKeySize(0), m_pKeyBuffer(NULL), m_pIndex(NULL) { *this = s; }
	~FindSubString(void) { clear(); }

	FindSubString & operator = (const FindSubString & s);

	void				compileKeyWord(const typeChar * pKey, size_t nKeySize);
	void				clear(void);

	size_t				findCaseSensitive(const typeChar * pText, size_t nTextLen) const;
	bool				matchCaseSensitive(const typeChar * pText, size_t nTextLen) const;

	size_t				findIgnoreCase(const typeChar * pText, size_t nTextLen) const;
	bool				matchIgnoreCase(const typeChar * pText, size_t nTextLen) const;

	size_t				getKeySize(void) const { return m_nKeySize; }
	const typeChar *	getKey(void) const { return m_pKeyBuffer; }

protected:
	size_t				m_nKeySize;
	typeChar *			m_pKeyBuffer;
	size_t *			m_pIndex;
};

class StringScaner
{
public:
	virtual				~StringScaner(void) {}
	void				scanString(const char * p, size_t n);

protected:
	virtual void		beforeScan(const char * p, const char * pEnd) = 0;
	virtual bool		onScan(const char * pCurPos) = 0;
	virtual void		afterScan(const char * p, const char * pEnd) = 0;
};

RFC_NAMESPACE_END

#endif	//RFC_STRING_H_201008
