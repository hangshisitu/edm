/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_ATOMICINTEGER_H_201008
#define RFC_ATOMICINTEGER_H_201008

#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

template<typename typeInteger, typename typeLock = MutexLock>
class AtomicInteger
{
public:
	typedef AutoMutexLockProxy<typeLock>	typeAutoLock;
	AtomicInteger(void) : m_nVal(0) {}
	explicit AtomicInteger(typeInteger n) : m_nVal(n) {}
	AtomicInteger(const AtomicInteger & _Other) : m_nVal(_Other.getValue()) {}
	template<class typeOtherInt, typename typeOtherLock>
	AtomicInteger(const AtomicInteger<typeOtherInt, typeOtherLock> & _Other) : m_nVal(_Other.getValue()) {}

	AtomicInteger &	operator = (typeInteger n) { typeAutoLock au(m_lock); m_nVal = n; return *this; }
	AtomicInteger &	operator ++ (void) { typeAutoLock au(m_lock); ++m_nVal; return *this; }
	AtomicInteger		operator ++ (int) {typeAutoLock au(m_lock); typeInteger n = m_nVal++; return AtomicInteger(n); }
	AtomicInteger &	operator -- (void) {typeAutoLock au(m_lock); --m_nVal; return *this; }
	AtomicInteger		operator -- (int) {typeAutoLock au(m_lock); typeInteger n = m_nVal--; return AtomicInteger(n); }

	typeInteger			getAndSet(typeInteger n) { typeAutoLock au(m_lock); typeInteger nOld = m_nVal; m_nVal = n; return nOld; }
	inline typeInteger	swap(typeInteger n) { return getAndSet(n); }
	void				setValue(typeInteger n) { typeAutoLock au(m_lock); m_nVal = n; }
	typeInteger			getValue(void) const  { typeAutoLock au(m_lock); return m_nVal; }

	typeInteger			addAndGet(typeInteger n) { typeAutoLock au(m_lock); m_nVal += n; return m_nVal; }
	inline typeInteger	incAndGet(void) { return addAndGet(1); }
	typeInteger			getAndAdd(typeInteger n) { typeAutoLock au(m_lock); typeInteger nOld = m_nVal; m_nVal += n; return nOld; }
	inline typeInteger	getAndInc(void) { return getAndAdd(1); }

	typeInteger			subAndGet(typeInteger n) { typeAutoLock au(m_lock); m_nVal -= n; return m_nVal; }
	inline typeInteger	decAndGet(void) { return subAndGet(1); }
	typeInteger			getAndSub(typeInteger n) { typeAutoLock au(m_lock); typeInteger nOld = m_nVal; m_nVal -= n; return nOld; }
	inline typeInteger	getAndDec(void) { return getAndSub(1); }

	//if m_nVal == nCompare, set m_nVal = nNewVal and return true
	bool				updateOnEqual(typeInteger nCompare, typeInteger nNewVal)
	{
		 typeAutoLock au(m_lock);
		 if ( m_nVal != nCompare )
			 return false;
		 m_nVal = nNewVal;
		 return true;
	}

	void				swap(AtomicInteger & _Other)
	{
		typeAutoLock au1(m_lock);
		typeAutoLock au2(_Other.m_lock);
		std::swap(m_nVal, _Other.m_nVal);
	}

	bool				operator == (typeInteger n) const { return compare(n) == 0; }
	bool				operator != (typeInteger n) const { return compare(n) != 0; }
	bool				operator > (typeInteger n) const { return compare(n) > 0; }
	bool				operator < (typeInteger n) const { return compare(n) < 0; }
	bool				operator >= (typeInteger n) const { return compare(n) >= 0; }
	bool				operator <= (typeInteger n) const { return compare(n) <= 0; }

	bool				operator == (const AtomicInteger & _Other) const { return compare(_Other) == 0; }
	bool				operator != (const AtomicInteger & _Other) const { return compare(_Other) != 0; }
	bool				operator > (const AtomicInteger & _Other) const { return compare(_Other) > 0; }
	bool				operator < (const AtomicInteger & _Other) const { return compare(_Other) < 0; }
	bool				operator >= (const AtomicInteger & _Other) const { return compare(_Other) >= 0; }
	bool				operator <= (const AtomicInteger & _Other) const { return compare(_Other) <= 0; }

protected:
	int					compare(typeInteger n) const
	{
		typeInteger nVal = getValue();
		if ( nVal == n )
			return 0;
		return ( nVal > n ? 1 : -1 );
	}
	int					compare(const AtomicInteger & _Other) const { return compare(_Other.getValue()); }
private:
	typeInteger					m_nVal;
	typeLock					m_lock;
};

typedef	AtomicInteger<rfc_int_32, MutexLock>			AtomicInteger32;
typedef	AtomicInteger<rfc_int_64, MutexLock>			AtomicInteger64;

RFC_NAMESPACE_END

#endif	//RFC_ATOMICINTEGER_H_201008
