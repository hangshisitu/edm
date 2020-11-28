/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_RANGESTORAGE_H_201008
#define RFC_RANGESTORAGE_H_201008

#include "base/globalfunc.h"
#include <map>

RFC_NAMESPACE_BEGIN

/*class SizeRange
{
public:
	SizeRange(void);
	SizeRange(size_t nMinSize, size_t nMaxSize);

	void		clear(void);
	size_t		getMinSize(void) const { return m_nMinSize; }
	void		setMinSize(size_t nMinSize) { m_nMinSize = nMinSize; }
	size_t		getMaxSize(void) const { return m_nMaxSize; }
	void		setMaxSize(size_t nMaxSize) { m_nMaxSize = nMaxSize; }
	bool		checkInRange(size_t nSize) const { return ( m_nMinSize <= nSize && nSize <= m_nMaxSize ); }

	void		parseSizeRange(const stdstring & strSizeRange)
	{
		clear();
		if ( strSizeRange.empty() )
			return;

		size_t nPos = strSizeRange.find('-');
		if ( nPos == stdstring::npos )
		{
			rfc_int_64 n = StringFunc::convertSizeUnitToByte(strSizeRange.data(), strSizeRange.size());
			setMinSize(static_cast<size_t>(n));
			setMaxSize(static_cast<size_t>(n));
		}
		else
		{
			rfc_int_64 n = StringFunc::convertSizeUnitToByte(strSizeRange.data(), strSizeRange.size());
			if ( n > 0 )
				setMinSize(static_cast<size_t>(n));
			if ( nPos + 1 < strSizeRange.size() )
			{
				n = StringFunc::convertSizeUnitToByte(strSizeRange.data() + nPos + 1, strSizeRange.size() - nPos - 1);
				setMaxSize( ( n < 0 ) ? 0 : static_cast<size_t>(n) );
			}
		}
	}

protected:
	size_t		m_nMinSize, m_nMaxSize;
};*/

// typeKey必须支持+(int) -(int)操作符
// 不比较nBegin和nLast的大小
template<typename typeKey, typename typeData>
class RangeDataMap
{
public:
	typedef	typeKey									key_type;
	typedef typeData								data_type;
	typedef std::pair<typeKey, typeData>			value_type;
	typedef std::map<typeKey, value_type>			typeRangeMap;

	RangeDataMap(void) : m_bNegative(false) {}
	void			setNegative(bool bNegative) { m_bNegative = bNegative; }
	bool			isNegative(void) const { return m_bNegative; }
	const typeRangeMap &	getRangeMap(void) const { return m_mapRange; }

	bool		empty(void) const { return m_mapRange.empty(); }
	void		clear(void) { m_bNegative = false; m_mapRange.clear(); }
	void		swap(RangeDataMap & mapRange) { std::swap(m_bNegative, mapRange.m_bNegative); m_mapRange.swap(mapRange.m_mapRange); }
	size_t		size(void) const { return m_mapRange.size(); }

	void		updateOrInsert(const typeKey & nBegin, const typeKey & nLast, const typeData & nData);
	void		updateOrInsert(const typeKey & nKey, const typeData & nData) { return updateOrInsert(nKey, nKey, nData); }

	void		erase(const typeKey & nBegin, const typeKey & nLast);
	void		erase(const typeKey & nKey) { return erase(nKey, nKey); }

	bool		findInRange(const typeKey & nKey) const;
	bool		findInRange(const typeKey & nKey, typeData & nData) const;
	bool		findInRange(const typeKey & nKey, typeKey & nBegin, typeKey & nLast, typeData & nData) const;

	void		compact(void);

protected:
	bool			m_bNegative;			//是否按取反搜索
	typeRangeMap	m_mapRange;

	// bOnlyForPos 只获取nKey所在的位置,不比较nKey是否在[nBegin,nLast]范围
	typename typeRangeMap::const_iterator	find(const typeKey & nKey, bool bOnlyForPos) const;
	typename typeRangeMap::iterator			find(const typeKey & nKey, bool bOnlyForPos);
};

template<typename typeKey, typename typeData>
typename RangeDataMap<typeKey, typeData>::typeRangeMap::const_iterator RangeDataMap<typeKey, typeData>::find(const typeKey & nKey, bool bOnlyForPos) const
{
	if ( m_mapRange.empty() )
		return m_mapRange.end();

	typename typeRangeMap::const_iterator it = m_mapRange.lower_bound(nKey);		// find the first iterator which it->first >= nKey
	if ( (it == m_mapRange.end()) || (it != m_mapRange.begin() && it->first > nKey) )
		--it;
	if ( bOnlyForPos || (it->first <= nKey && nKey <= it->second.first) )
		return it;
	return m_mapRange.end();
}

template<typename typeKey, typename typeData>
typename RangeDataMap<typeKey, typeData>::typeRangeMap::iterator RangeDataMap<typeKey, typeData>::find(const typeKey & nKey, bool bOnlyForPos)
{
	if ( m_mapRange.empty() )
		return m_mapRange.end();

	typename typeRangeMap::iterator it = m_mapRange.lower_bound(nKey);		// find the first iterator which it->first >= nKey
	if ( (it == m_mapRange.end()) || (it != m_mapRange.begin() && it->first > nKey) )
		--it;
	if ( bOnlyForPos || (it->first <= nKey && nKey <= it->second.first) )
		return it;
	return m_mapRange.end();
}

template<typename typeKey, typename typeData>
void RangeDataMap<typeKey, typeData>::updateOrInsert(const typeKey & nBegin, const typeKey & nLast, const typeData & nData)
{
	typename typeRangeMap::iterator it = find(nBegin, false);
	if ( (it == m_mapRange.end()) || it->second.first < nBegin ) // not between it->first and it->second.first
		it = m_mapRange.insert( std::make_pair(nBegin, value_type(nLast, nData)) ).first;
	else if ( it->first == nBegin )			// exactly match it->first
	{
		if ( it->second.first > nLast )
			m_mapRange.insert( std::make_pair(nLast + 1, it->second) );
		it->second.first = nLast;
	}
	else	//between it->first and it->second.first
	{
		if ( it->second.first > nLast )
			m_mapRange.insert( std::make_pair(nLast + 1, it->second) );
		it->second.first = nBegin - 1;
		it = m_mapRange.insert( std::make_pair(nBegin, value_type(nLast, nData)) ).first;
	}

	while ( true )
	{
		typename typeRangeMap::iterator itNext = it;
		++itNext;
		if ( (itNext == m_mapRange.end()) || itNext->first > nLast )
			break;

		if ( itNext->second.first <= nLast )
			m_mapRange.erase(itNext);
		else
		{
			m_mapRange.insert( std::make_pair(nLast + 1, itNext->second) );
			m_mapRange.erase(itNext);
			break;
		}
	} // while ( true )
	it->second.second = nData;
}

template<typename typeKey, typename typeData>
void RangeDataMap<typeKey, typeData>::erase(const typeKey & nBegin, const typeKey & nLast)
{
	typename typeRangeMap::iterator it = find(nBegin, true);
	if ( it == m_mapRange.end() )
		return;

	while ( true )
	{
		typename typeRangeMap::iterator itNext = it;
		++itNext;
		if ( (itNext == m_mapRange.end()) || itNext->first > nLast )
			break;

		if ( itNext->second.first <= nLast )
			m_mapRange.erase(itNext);
		else
		{
			m_mapRange.insert( std::make_pair(nLast + 1, itNext->second) );
			m_mapRange.erase(itNext);
			break;
		}
	} // while ( true )

	if ( it->first == nBegin )			// exactly match it->first
	{
		if ( it->second.first > nLast )
			m_mapRange.insert( std::make_pair(nLast + 1, it->second) );
		m_mapRange.erase(it);
	}
	else if ( it->second.first > nBegin )
	{
		if ( it->second.first > nLast )
			m_mapRange.insert( std::make_pair(nLast + 1, it->second) );
		it->second.first = nBegin - 1;
	}
}

template<typename typeKey, typename typeData>
bool RangeDataMap<typeKey, typeData>::findInRange(const typeKey & nKey) const
{
	typename typeRangeMap::const_iterator it = find(nKey, false);
	return ( m_bNegative ? it == m_mapRange.end() : it != m_mapRange.end() );
}

template<typename typeKey, typename typeData>
bool RangeDataMap<typeKey, typeData>::findInRange(const typeKey & nKey, typeData & nData) const
{
	typename typeRangeMap::const_iterator it = find(nKey, false);
	if ( it != m_mapRange.end() )
		nData = it->second.second;
	return ( m_bNegative ? it == m_mapRange.end() : it != m_mapRange.end() );
}

template<typename typeKey, typename typeData>
bool RangeDataMap<typeKey, typeData>::findInRange(const typeKey & nKey, typeKey & nBegin, typeKey & nLast, typeData & nData) const
{
	typename typeRangeMap::const_iterator it = find(nKey, false);
	if ( it != m_mapRange.end() )
	{
		nBegin = it->first;
		nLast = it->second.first;
		nData = it->second.second;
	}
	return ( m_bNegative ? it == m_mapRange.end() : it != m_mapRange.end() );
}

template<typename typeKey, typename typeData>
void RangeDataMap<typeKey, typeData>::compact(void)
{
	typename typeRangeMap::iterator it = m_mapRange.begin();
	typename typeRangeMap::iterator itEnd = m_mapRange.end();
	while ( it != itEnd )
	{
		while ( true )
		{
			typename typeRangeMap::iterator itNext = it;
			++itNext;
			if ( (itNext == m_mapRange.end()) || itNext->first > it->second.first + 1 || itNext->second.second != it->second.second )
				break;
			it->second.first = itNext->second.first;
			m_mapRange.erase(itNext);
		}
		++it;
	} // while ( it != itEnd )
}

RFC_NAMESPACE_END

#endif	//RFC_RANGESTORAGE_H_201008
