/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_POOL_H_201008
#define RFC_POOL_H_201008

#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

class ResourcePool : private NonCopyAble
{
public:
	static const size_t npos;
	ResourcePool(size_t nSize = 0);
	virtual ~ResourcePool(void) { clearAll(); }

	size_t						getFreeCount(void) const { return m_nFreeCount; }
	size_t						getTotalCount(void) const { return m_nTotalCount; }

	virtual bool				resize(size_t nSize);
	virtual size_t				apply(void);		//wait until resource available
	virtual size_t				apply(const TimeValue & tvTimeWait);
	virtual void				release(size_t nIndex, bool bResult = true);

	bool						clearAll(void) { return resize(0); }
	void						lockAll(void);
	void						lockAllAndResize(size_t nSize);
	void						releaseAll(void);

protected:
	size_t						getIndexWithoutLock(void);
	void						clearWithoutLock(bool bLockAll);
	bool						resizeWithoutLock(bool bLockAll, size_t nSize);

	bool						m_bHasLockedAll;
	size_t	*					m_lpnResourcePool;
	size_t						m_nFreeCount, m_nTotalCount, m_nNextAvailableIndex;
	MutexLock					m_lockForPool;
	ConditionVariant			m_condRelease;
};

template<typename typePool>
class SafePoolAgent
{
public:
	SafePoolAgent(typePool & aPool) : m_nIndex(ResourcePool::npos), m_Pool(aPool) {}
	~SafePoolAgent(void) { release(); }

	void				release(bool bResult = true)
	{
		if ( m_nIndex != ResourcePool::npos )
			m_Pool.release(m_nIndex, bResult);
		m_nIndex = ResourcePool::npos;
	}

	size_t				apply(const TimeValue & tvTimeWait)
	{
		release(true);
		m_nIndex = m_Pool.apply(tvTimeWait);
		return m_nIndex;
	}

protected:
	size_t					m_nIndex;
	typePool &				m_Pool;
};

RFC_NAMESPACE_END

#endif	//RFC_POOL_H_201008
