/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_RESOURCECOUNTER_H_201008
#define RFC_RESOURCECOUNTER_H_201008

#include "system/hashtable.h"

RFC_NAMESPACE_BEGIN

// 资源计数器,例如当前IP同时连接数
template<typename typeKey, typename typeLock = MutexLock, typename typeHashFuncPred = HashFuncPred<typeKey>,
typename typeHashKeyCompare = HashKeyCompare<typeKey> >
class ResourceCounter : protected HashTable< typeKey, size_t, typeLock, typeHashFuncPred, typeHashKeyCompare >
{
public:
	typedef AutoMutexLockProxy<typeLock> typeAutoLock;
	typedef HashTable< typeKey, size_t, typeLock, typeHashFuncPred, typeHashKeyCompare > typeHashTable;

	size_t			getCounter(const typeKey & strKey) const;
	void			setCounter(const typeKey & strKey, size_t nCounter);

	// 返回加上nAddCounter后的结果
	size_t			addAndGetCounter(const typeKey & strKey, size_t nAddCounter);
	inline size_t	incAndGetCounter(const typeKey & strKey) { return addAndGetCounter(strKey, 1); }

	// 返回减去nSubCounter后的结果, 如果计数是0,则删除strKey对应的记录
	size_t			subAndGetCounter(const typeKey & strKey, size_t nSubCounter);
	inline size_t	decAndGetCounter(const typeKey & strKey) { return subAndGetCounter(strKey, 1); }
};

template<typename typeKey, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare >
size_t ResourceCounter<typeKey, typeLock, typeHashFuncPred, typeHashKeyCompare>::getCounter(const typeKey & strKey) const
{
	size_t nHashIndex = typeHashTable::getHashIndex(strKey);
	typeAutoLock auLock( typeHashTable::getLock(nHashIndex) );
	const typename typeHashTable::typeSubMap & subMap = typeHashTable::getMap(nHashIndex);
	typename typeHashTable::typeSubMap::const_iterator it = subMap.find(strKey);
	if ( it == subMap.end() )
		return 0;
	return it->second;
}

template<typename typeKey, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare >
void ResourceCounter<typeKey, typeLock, typeHashFuncPred, typeHashKeyCompare>::setCounter(const typeKey & strKey, size_t nCounter)
{
	size_t nHashIndex = typeHashTable::getHashIndex(strKey);
	typeAutoLock auLock( typeHashTable::getLock(nHashIndex) );
	typename typeHashTable::typeSubMap & subMap = typeHashTable::getMap(nHashIndex);
	subMap[strKey] = nCounter;
}

template<typename typeKey, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare >
size_t ResourceCounter<typeKey, typeLock, typeHashFuncPred, typeHashKeyCompare>::addAndGetCounter(const typeKey & strKey, size_t nAddCounter)
{
	size_t nHashIndex = typeHashTable::getHashIndex(strKey);
	typeAutoLock auLock( typeHashTable::getLock(nHashIndex) );
	typename typeHashTable::typeSubMap & subMap = typeHashTable::getMap(nHashIndex);
	size_t & nCounter = subMap[strKey];
	nCounter += nAddCounter;
	return nCounter;
}

template<typename typeKey, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare >
size_t ResourceCounter<typeKey, typeLock, typeHashFuncPred, typeHashKeyCompare>::subAndGetCounter(const typeKey & strKey, size_t nSubCounter)
{
	size_t nHashIndex = typeHashTable::getHashIndex(strKey);
	typeAutoLock auLock( typeHashTable::getLock(nHashIndex) );
	typename typeHashTable::typeSubMap & subMap = typeHashTable::getMap(nHashIndex);
	typename typeHashTable::typeSubMap::iterator it = subMap.find(strKey);
	if ( it == subMap.end() )
		return 0;

	size_t n = ( it->second > nSubCounter ) ? ( it->second - nSubCounter ) : 0;
	if ( n == 0 )
		subMap.erase(it);
	else
		it->second = n;
	return n;
}

RFC_NAMESPACE_END

#endif	//RFC_RESOURCECOUNTER_H_201008


