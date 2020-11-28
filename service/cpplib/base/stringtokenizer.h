/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_STRINGTOKENIZER_H_201008
#define RFC_STRINGTOKENIZER_H_201008

#include "base/string.h"

RFC_NAMESPACE_BEGIN

template<class typeString>
class StringTokenizer
{
public:
	typedef typename typeString::value_type typeChar;
	typedef typename typeString::size_type	typeSize;

	StringTokenizer(void) : m_lpszSrc(NULL), m_lpszSrcEnd(NULL), m_lpszDelimiter(NULL), m_bIncludeDelimiter(false) {}

	StringTokenizer(const typeChar * lpszSrc, typeSize nSize, bool bIncludeDelimiter = false)
		: m_lpszSrc(lpszSrc), m_lpszSrcEnd(lpszSrc + nSize), m_lpszDelimiter(NULL), m_bIncludeDelimiter(bIncludeDelimiter) {}

	void	setSplitString(const typeChar * lpszSrc, typeSize nSize)
	{
		m_lpszSrc = lpszSrc;
		m_lpszSrcEnd = lpszSrc + nSize;
	}

	void	setDelimiter(const typeChar * lpszDelimiter, bool bIncludeDelimiter = false)
	{
		m_lpszDelimiter = lpszDelimiter;
		m_bIncludeDelimiter = bIncludeDelimiter;
	}

	static bool getTokenBlock(const typeChar * pSrc, const typeChar * pSrcEnd, typeString & strToken,
		const typeChar * lpszDelimiter, size_t nSkipBlockCount, bool bIncludeDelimiter = false)
	{
		const typeChar * p = find_next(pSrc, pSrcEnd, strToken, lpszDelimiter, nSkipBlockCount, bIncludeDelimiter);
		return ( p != NULL );
	}

	static bool getTokenBlock(const typeChar * pSrc, const typeChar * pSrcEnd, typeString & strToken,
		typeChar chDelimiter, size_t nSkipBlockCount, bool bIncludeDelimiter = false)
	{
		const typeChar * p = find_next(pSrc, pSrcEnd, strToken, chDelimiter, nSkipBlockCount, bIncludeDelimiter);
		return ( p != NULL );
	}

	bool getNext(typeString & strToken, const typeChar * lpszDelimiter = NULL)
	{
		m_lpszSrc = find_next(m_lpszSrc, m_lpszSrcEnd, strToken, (lpszDelimiter == NULL) ? m_lpszDelimiter : lpszDelimiter, 0, m_bIncludeDelimiter);
		return ( m_lpszSrc != NULL );
	}

	bool getNext(typeString & strToken, typeChar chDelimiter)	//only one char delimiter
	{
		m_lpszSrc = find_next(m_lpszSrc, m_lpszSrcEnd, strToken, chDelimiter, 0, m_bIncludeDelimiter);
		return ( m_lpszSrc != NULL );
	}

protected:
	static const typeChar * find_next(const typeChar * pSrc, const typeChar * pSrcEnd, typeString & strToken,
		const typeChar * lpszDelimiter, size_t nSkipBlockCount, bool bIncludeDelimiter)		
	{
		if ( pSrc == NULL || pSrcEnd == NULL || pSrc >= pSrcEnd )
			return NULL;

		strToken.clear();
		if ( lpszDelimiter == NULL && nSkipBlockCount == 0 )
			strToken.assign(pSrc, pSrcEnd);
		if ( lpszDelimiter == NULL )
			return ( nSkipBlockCount == 0 ) ? pSrcEnd : NULL;

		while ( pSrc < pSrcEnd && nSkipBlockCount > 0 )
		{
			while ( pSrc < pSrcEnd && strchr(lpszDelimiter, *pSrc) != NULL )	//skip delimiter
				++pSrc;
			while ( pSrc < pSrcEnd && strchr(lpszDelimiter, *pSrc) == NULL )
				++pSrc;
			--nSkipBlockCount;
		}
		if ( pSrc >= pSrcEnd || nSkipBlockCount > 0 )
			return NULL;

		const typeChar * pTokenBegin = pSrc;
		while ( pSrc < pSrcEnd && strchr(lpszDelimiter, *pSrc) != NULL )		//skip delimiter
			++pSrc;
		if ( !bIncludeDelimiter )		
			pTokenBegin = pSrc;

		while ( pSrc < pSrcEnd && strchr(lpszDelimiter, *pSrc) == NULL )
			++pSrc;
		if ( pTokenBegin == pSrc )
			return NULL;
		strToken.assign(pTokenBegin, pSrc);
		return pSrc;
	}

	static const typeChar * find_next(const typeChar * pSrc, const typeChar * pSrcEnd, typeString & strToken,
		typeChar chDelimiter, size_t nSkipBlockCount, bool bIncludeDelimiter)		
	{
		if ( pSrc == NULL || pSrcEnd == NULL || pSrc >= pSrcEnd )
			return NULL;

		strToken.clear();
		while ( pSrc < pSrcEnd && nSkipBlockCount > 0 )
		{
			while ( pSrc < pSrcEnd && *pSrc == chDelimiter )	//skip delimiter
				++pSrc;
			while ( pSrc < pSrcEnd && *pSrc != chDelimiter )
				++pSrc;
			--nSkipBlockCount;
		}
		if ( pSrc >= pSrcEnd || nSkipBlockCount > 0 )
			return NULL;

		const typeChar * pTokenBegin = pSrc;
		while ( pSrc < pSrcEnd && *pSrc == chDelimiter )		//skip delimiter
			++pSrc;
		if ( !bIncludeDelimiter )		
			pTokenBegin = pSrc;

		while ( pSrc < pSrcEnd && *pSrc != chDelimiter )
			++pSrc;
		if ( pTokenBegin == pSrc )
			return NULL;
		strToken.assign(pTokenBegin, pSrc);
		return pSrc;
	}

private:
	const typeChar *	m_lpszSrc;
	const typeChar *	m_lpszSrcEnd;
	const typeChar *	m_lpszDelimiter;
	bool				m_bIncludeDelimiter;
};

RFC_NAMESPACE_END

#endif	//RFC_STRINGTOKENIZER_H_201008
