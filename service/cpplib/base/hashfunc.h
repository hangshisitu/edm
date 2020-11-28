/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_HASHFUNC_H_201008
#define RFC_HASHFUNC_H_201008

#include "base/string.h"
#include <algorithm>
#include <functional>
#include <ext/hash_map>
#include <ext/hash_set>

RFC_NAMESPACE_BEGIN

typedef rfc_uint_32		typeHashInteger;
#define RFC_STD_HASH_NAMESPACE __gnu_cxx

class HashFunc
{
public:
	static const typeHashInteger		g_nMagicHashNumber;

	static typeHashInteger		hashIntegerValue(typeHashInteger nHashKey, typeHashInteger nHashValue)
	{
		return (nHashValue == 0) ? nHashKey : (g_nMagicHashNumber * nHashValue) ^ nHashKey;
	}

	template<typename typeChar>
	static typeHashInteger		hashStringValue(const typeChar * p, size_t nSize)
	{
		typeHashInteger nHashValue = 0;
		const typeChar * pEnd = p + nSize;
		for ( ; p < pEnd; ++p )
			nHashValue = hashIntegerValue(*p, nHashValue);
		return nHashValue;
	}

	template<typename typeChar>
	static typeHashInteger		hashNocaseStringValue(const typeChar * p, size_t nSize)
	{
		typeHashInteger nHashValue = 0;
		const typeChar * pEnd = p + nSize;
		for ( ; p < pEnd; ++p )
			nHashValue = hashIntegerValue(StringFunc::toupper(*p), nHashValue);
		return nHashValue;
	}

	static typeHashInteger		hashValue(const stdstring & str)
	{
		return hashStringValue(str.data(), str.size());
	}

	template<typename typeInteger>
	static typeHashInteger		hashValue(const typeInteger & n)
	{
		return hashIntegerValue(static_cast<typeHashInteger>(n), 0);
	}

	static size_t				getNextPrimeNumber(size_t nValue);
	static inline size_t		getHashBucketCount(size_t nBucketCount) { return getNextPrimeNumber(nBucketCount); }
};

template<typename typeKey>
struct HashKeyCompare
{
	int	operator()(const typeKey & a, const typeKey & b) const
	{
		if ( a == b )
			return 0;
		return ( a > b ? 1 : -1 );
	}
};

template<>
struct HashKeyCompare<stdstring>
{
	int operator()(const stdstring & a, const stdstring & b) const {
		return a.compare(b);
	}
};

template<typename typeKey>
struct HashFuncPred
{
	size_t operator()(const typeKey & v) const {
		return HashFunc::hashValue(v);
	}
};

struct IgnoreCaseStringHashFuncPred
{
	size_t operator()(const stdstring & str) const {
		return HashFunc::hashNocaseStringValue(str.data(), str.size());
	}
};

template<typename typeKey, typename typeHashKeyCompare = HashKeyCompare<typeKey> >
class KeyEqualPred
{
public:
	KeyEqualPred() {}
	KeyEqualPred(typeHashKeyCompare _HashKeyCompare) : m_hashKeyCompare(_HashKeyCompare) {}
	bool operator()(const typeKey & _Keyval1, const typeKey & _Keyval2) const
	{	return ( m_hashKeyCompare(_Keyval1, _Keyval2) == 0 );	}
protected:
	typeHashKeyCompare				m_hashKeyCompare;
};

template<typename typeKey, typename typeValue, typename typeHashFuncPred = HashFuncPred<typeKey>, typename typeHashKeyCompare = HashKeyCompare<typeKey> >
class STDHashMap : public RFC_STD_HASH_NAMESPACE::hash_map<typeKey, typeValue, typeHashFuncPred, KeyEqualPred<typeKey, typeHashKeyCompare> > {};

template<typename typeKey, typename typeHashFuncPred = HashFuncPred<typeKey>, typename typeHashKeyCompare = HashKeyCompare<typeKey> >
class STDHashSet : public RFC_STD_HASH_NAMESPACE::hash_set<typeKey, typeHashFuncPred, KeyEqualPred<typeKey, typeHashKeyCompare> > {};

template<typename typeKey, typename typeValue, typename typeHashFuncPred = HashFuncPred<typeKey>, typename typeHashKeyCompare = HashKeyCompare<typeKey> >
class STDHashMultiMap : public RFC_STD_HASH_NAMESPACE::hash_multimap<typeKey, typeValue,typeHashFuncPred, KeyEqualPred<typeKey, typeHashKeyCompare> > {};

template<typename typeKey, typename typeHashFuncPred = HashFuncPred<typeKey>, typename typeHashKeyCompare = HashKeyCompare<typeKey> >
class STDHashMultiSet : public RFC_STD_HASH_NAMESPACE::hash_multiset<typeKey, typeHashFuncPred, KeyEqualPred<typeKey, typeHashKeyCompare> > {};

RFC_NAMESPACE_END

#endif	//RFC_HASHFUNC_H_201008
