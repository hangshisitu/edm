/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_HASHTABLE_H_201008
#define RFC_HASHTABLE_H_201008

#include "base/hashfunc.h"
#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

//返回数据使用引用计数保证线程安全和高效加锁

template<typename typeKey, typename typeValue, typename typeLock = NullLock, typename typeHashFuncPred = HashFuncPred<typeKey>,
typename typeHashKeyCompare = HashKeyCompare<typeKey> >
class HashTable : private NonCopyAble
{
public:
	typedef HashTable<typeKey, typeValue, typeLock, typeHashFuncPred, typeHashKeyCompare>				typeSelfHashTable;
	typedef STDHashMap<typeKey, typeValue, typeHashFuncPred, typeHashKeyCompare>						typeSubMap;
	typedef typeKey																						key_type;
	typedef typename typeSubMap::value_type																value_type;
	typedef typeLock																					lock_type;
	typedef typeHashFuncPred																			hash_func_pred;
	typedef typeHashKeyCompare																			key_compare;

	typedef typename typeSubMap::iterator																typeSubMapIterator;
	typedef typename typeSubMap::const_iterator															typeSubMapConstIterator;
	typedef typeValue																					referent_type;
	typedef typename typeSubMap::allocator_type															allocator_type;
	typedef typename typeSubMap::size_type																size_type;
	typedef typename typeSubMap::difference_type														difference_type;
	typedef typename typeSubMap::reference																reference;
	typedef typename typeSubMap::const_reference														const_reference;
	typedef typename typeSubMap::mapped_type															mapped_type;

	template<typename typeHashTable, typename typeSubIterator>
	class HashTableIterator
	{
	public:
		typedef typename typeSubIterator::value_type					value_type;
		typedef typename typeSubIterator::difference_type				difference_type;
		typedef typename typeSubIterator::reference						reference;
		typedef typename typeSubIterator::pointer						pointer;

		HashTableIterator(void) : m_nCurIndex(0), m_pHashTable(NULL), m_subItCur(), m_subItEnd() {}
		HashTableIterator(size_t nIndex, typeSelfHashTable * pHashTable, typeSubIterator itCur, typeSubIterator itEnd)
			: m_nCurIndex(nIndex), m_pHashTable(pHashTable), m_subItCur(itCur), m_subItEnd(itEnd) {}
		HashTableIterator(const HashTableIterator & it)
			: m_nCurIndex(it.m_nCurIndex), m_pHashTable(it.m_pHashTable), m_subItCur(it.m_subItCur),m_subItEnd(it.m_subItEnd) {}

		reference operator*() const { return *m_subItCur; }
		pointer operator->() const { return &(operator*()); }

		HashTableIterator& operator++()
		{
			moveNext();
			return *this;
		}

		HashTableIterator operator++(int)
		{
			HashTableIterator __tmp = *this;
			moveNext();
			return __tmp;
		}

		bool operator==(const HashTableIterator& it) const
		{ return (m_pHashTable == it.m_pHashTable) && (m_pHashTable == NULL || (m_nCurIndex == it.m_nCurIndex && m_subItCur == it.m_subItCur)); }

		bool operator!=(const HashTableIterator& it) const	{ return !(*this == it); }

	protected:
		void	moveNext(void)
		{
			if ( m_pHashTable == NULL || ++m_subItCur != m_subItEnd )
				return;

			for ( ++m_nCurIndex; m_nCurIndex < m_pHashTable->getBlockCount(); ++m_nCurIndex )
			{
				if ( m_pHashTable->getMap(m_nCurIndex).size() )
				{
					m_subItCur = m_pHashTable->getMap(m_nCurIndex).begin();
					m_subItEnd = m_pHashTable->getMap(m_nCurIndex).end();
					return;
				}
			} //for ( ++m_nCurIndex; m_nCurIndex < m_pHashTable->getBlockCount(); ++m_nCurIndex ) //move to next sub map
			m_pHashTable = NULL;
		}

	protected:
		friend class								HashTable;
		size_t										m_nCurIndex;
		typeSelfHashTable	*						m_pHashTable;
		typeSubIterator								m_subItCur, m_subItEnd;
	};

	class const_iterator;
	class iterator : public HashTableIterator<HashTable, typeSubMapIterator>
	{
	private:
		friend class								typeSelfHashTable::const_iterator;
	};

	class const_iterator : public HashTableIterator<const HashTable, typeSubMapConstIterator>
	{
	public:
		typedef	HashTableIterator<const HashTable, typeSubMapConstIterator>	typeParentClass;
		const_iterator(const iterator & it) : typeParentClass()
		{
			typeParentClass::m_nCurIndex = it.m_nCurIndex;
			typeParentClass::m_pHashTable = it.m_pHashTable;
			typeParentClass::m_subItCur = it.m_subItCur;
			typeParentClass::m_subItEnd = it.m_subItEnd;
		}
	};
	friend class								iterator;
	friend class								const_iterator;

	enum { DEFAULT_BUCKET_COUNT = 512 };
	HashTable(size_t nBucketCount = DEFAULT_BUCKET_COUNT, const typeHashFuncPred & hashfunObj = typeHashFuncPred()) : m_hashFunc(hashfunObj)
	{ init(nBucketCount); }

	template <class typeInputIterator>
	HashTable(typeInputIterator _First, typeInputIterator _Last, size_t nBucketCount = DEFAULT_BUCKET_COUNT,
		const typeHashFuncPred & hashfunObj = typeHashFuncPred()) : m_hashFunc(hashfunObj)
	{
		init(nBucketCount);
		insert(_First, _Last);
	}

	~HashTable(void) { clearAll(); }

	bool operator == (const HashTable & _Right) const { return m_pHashBlock == _Right.m_pHashBlock; }
	void					clear(void);
	void					swap(HashTable & _Right);
	void					swapByBlock(HashTable & _Right);
	size_t					size(void) const;
	bool					empty(void) const;

	void					clearAndResize(size_t nBucketCount);
	size_t					getBlockCount(void) const { return m_nBucketCount; }

	iterator				begin(void)
	{
		for ( size_t i = 0; i < m_nBucketCount; ++i )
		{
			typeSubMap & subMap = m_pHashBlock[i].m_subMap;
			if ( !subMap.empty() )
				return iterator(i, this, subMap.begin(), subMap.end());
		}
		return iterator();
	}

	inline iterator			end(void) { return iterator(); }
	const_iterator			begin(void) const
	{
		for ( size_t i = 0; i < m_nBucketCount; ++i )
		{
			const typeSubMap & subMap = m_pHashBlock[i].m_subMap;
			if ( !subMap.empty() )
				return iterator(i, this, subMap.begin(), subMap.end());
		}
		return iterator();
	}
	inline const_iterator	end(void) const { return const_iterator(); }

	iterator				find(const typeKey & key)
	{
		size_t nIndex = getHashIndex(key);
		AutoMutexLockProxy<typeLock> auLock(m_pHashBlock[nIndex].m_lockMap);
		typeSubMap & subMap = m_pHashBlock[nIndex].m_subMap;
		typename typeSubMap::iterator it = subMap.find(key);
		if ( it == subMap.end() )
			return end();
		return iterator(nIndex, this, it, subMap.end());
	}

	const_iterator			find(const typeKey & key) const
	{
		size_t nIndex = getHashIndex(key);
		const typeSubMap & subMap = m_pHashBlock[nIndex].m_subMap;
		typename typeSubMap::const_iterator it = subMap.find(key);
		if ( it == subMap.end() )
			return end();
		return const_iterator(nIndex, this, it, subMap.end());
	}

	size_t					count(const typeKey & key) const
	{
		return ( find(key) == end() ) ? 0 : 1;
	}

	mapped_type & operator[](const key_type & key)
	{
		size_t nIndex = getHashIndex(key);
		AutoMutexLockProxy<typeLock> auLock(m_pHashBlock[nIndex].m_lockMap);
		typeSubMap & subMap = m_pHashBlock[nIndex].m_subMap;
		return subMap[key];
	}

	const mapped_type & operator[](const key_type & key) const
	{
		size_t nIndex = getHashIndex(key);
		const typeSubMap & subMap = m_pHashBlock[nIndex].m_subMap;
		return subMap[key];
	}

	std::pair<iterator,bool> insert(const value_type & obj)
	{
		size_t nIndex = getHashIndex(obj.first);
		AutoMutexLockProxy<typeLock> auLock(m_pHashBlock[nIndex].m_lockMap);
		typeSubMap & subMap = m_pHashBlock[nIndex].m_subMap;
		std::pair<typename typeSubMap::iterator, bool> itResult = subMap.insert(obj);
		return std::pair<iterator, bool>(iterator(nIndex, this, itResult.first, subMap.end()), itResult.second);
	}

	iterator				insert(iterator position, const value_type& obj)
	{
		return insert(obj).first;
	}

	template <class typeInputIterator>
	void insert(typeInputIterator _First, typeInputIterator _Last)
	{
		for ( ; _First != _Last; ++_First )
			insert(*_First);
	}

	size_t erase(const key_type & key)
	{
		size_t nIndex = getHashIndex(key);
		AutoMutexLockProxy<typeLock> auLock(m_pHashBlock[nIndex].m_lockMap);
		return m_pHashBlock[nIndex].m_subMap.erase(key);
	}
	void erase(iterator it) { erase(it.m_subItCur->first); }
	void erase(iterator _First, iterator _Last)
	{
		for ( ; _First != _Last; ++_First )
			erase(_First);
	}

	size_t					getHashVal(const typeKey & key) const { return m_hashFunc(key); }
	size_t					getHashIndex(const typeKey & key) const { safeCheck(0, "getHashIndex"); return getHashVal(key) % m_nBucketCount; }

	typeLock &				getLock(size_t nIndex) const
	{
		safeCheck(nIndex, "getLock");
		return m_pHashBlock[nIndex].m_lockMap;
	}

	typeSubMap &			getMap(size_t nIndex)
	{
		safeCheck(nIndex, "getMap");
		return m_pHashBlock[nIndex].m_subMap;
	}

	const typeSubMap &		getMap(size_t nIndex) const
	{
		safeCheck(nIndex, "const getMap");
		return m_pHashBlock[nIndex].m_subMap;
	}

protected:
	void					init(size_t nBucketCount);
	void					clearAll(void);
	void					safeCheck(size_t nIndex, const stdstring & strCaller) const throw (Exception)
	{
		if ( nIndex >= m_nBucketCount )
			throw Exception(ERANGE, strCaller + "out of bounds");
	}

private:
	struct typeHashBlock
	{
		typeLock					m_lockMap;
		typeSubMap					m_subMap;
	};
	size_t							m_nBucketCount;
	typeHashBlock	*				m_pHashBlock;
	typeHashFuncPred				m_hashFunc;
};

//////////////////////////////////////////////////////////////////////////

template<typename typeKey, typename typeValue, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare>
void HashTable<typeKey, typeValue, typeLock, typeHashFuncPred, typeHashKeyCompare>::init(size_t nBucketCount)
{
	m_nBucketCount = 0;
	m_pHashBlock = NULL;
	clearAndResize(nBucketCount);
}

template<typename typeKey, typename typeValue, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare>
void HashTable<typeKey, typeValue, typeLock, typeHashFuncPred, typeHashKeyCompare>::clear(void)
{
	for ( size_t i = 0; i < m_nBucketCount; ++i )
	{
		AutoMutexLockProxy<typeLock> auLock(m_pHashBlock[i].m_lockMap);
		m_pHashBlock[i].m_subMap.clear();
	}
}

template<typename typeKey, typename typeValue, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare>
void HashTable<typeKey, typeValue, typeLock, typeHashFuncPred, typeHashKeyCompare>::clearAll(void)
{
	clear();
	m_nBucketCount = 0;
	delete []m_pHashBlock;
	m_pHashBlock = NULL;
}

template<typename typeKey, typename typeValue, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare>
void HashTable<typeKey, typeValue, typeLock, typeHashFuncPred, typeHashKeyCompare>::clearAndResize(size_t nBucketCount)
{
	clearAll();
	nBucketCount = HashFunc::getHashBucketCount(nBucketCount);
	m_pHashBlock = new typeHashBlock[nBucketCount];
	m_nBucketCount = nBucketCount;
}

template<typename typeKey, typename typeValue, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare>
void HashTable<typeKey, typeValue, typeLock, typeHashFuncPred, typeHashKeyCompare>::swap(HashTable & _Right)
{
	std::swap(m_nBucketCount, _Right.m_nBucketCount);
	std::swap(m_pHashBlock, _Right.m_pHashBlock);
	std::swap(m_hashFunc, _Right.m_hashFunc);
}

template<typename typeKey, typename typeValue, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare>
void HashTable<typeKey, typeValue, typeLock, typeHashFuncPred, typeHashKeyCompare>::swapByBlock(HashTable & _Right)
{
	if ( m_nBucketCount != _Right.m_nBucketCount )
		throw Exception("swapByBlock block size is not equal");

	for ( size_t i = 0; i < m_nBucketCount; ++i )
	{
		AutoMutexLockProxy<typeLock> auLock(m_pHashBlock[i].m_lockMap);
		m_pHashBlock[i].m_subMap.swap(_Right.m_pHashBlock[i].m_subMap);
	}
}

template<typename typeKey, typename typeValue, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare>
size_t HashTable<typeKey, typeValue, typeLock, typeHashFuncPred, typeHashKeyCompare>::size(void) const
{
	size_t nTotalCount = 0;
	for ( size_t i = 0; i < m_nBucketCount; ++i )
	{
		AutoMutexLockProxy<typeLock> auLock(m_pHashBlock[i].m_lockMap);
		nTotalCount += m_pHashBlock[i].m_subMap.size();
	}
	return nTotalCount;
}

template<typename typeKey, typename typeValue, typename typeLock, typename typeHashFuncPred, typename typeHashKeyCompare>
bool HashTable<typeKey, typeValue, typeLock, typeHashFuncPred, typeHashKeyCompare>::empty(void) const
{
	for ( size_t i = 0; i < m_nBucketCount; ++i )
	{
		AutoMutexLockProxy<typeLock> auLock(m_pHashBlock[i].m_lockMap);
		if ( !m_pHashBlock[i].m_subMap.empty() )
			return false;
	}
	return true;
}

RFC_NAMESPACE_END

#endif	//RFC_HASHTABLE_H_201008
