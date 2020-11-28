/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_SINGLETON_H_201008
#define RFC_SINGLETON_H_201008

#include "mutex/mutex.h"
#include "base/deleter.h"

RFC_NAMESPACE_BEGIN

template<typename typeInstance, typename typeLock = MutexLock>
class SingletonProxy : public typeInstance
{
public:
	static typeInstance &		getInstance(void);
	static typeInstance &		resetInstance(std::auto_ptr<typeInstance> & auNewInstance);

private:
	SingletonProxy(void) {}

	static std::auto_ptr<typeInstance>				m_auSingleton;
	static typeLock									m_lockSingleton;
};

template<typename typeInstance, typename typeLock>
std::auto_ptr< typeInstance > SingletonProxy<typeInstance, typeLock>::m_auSingleton;

template<typename typeInstance, typename typeLock>
typeLock SingletonProxy<typeInstance, typeLock>::m_lockSingleton;


template<typename typeInstance, typename typeLock>
typeInstance & SingletonProxy<typeInstance, typeLock>::getInstance(void)
{
	if ( m_auSingleton.get() == NULL )
	{
		AutoMutexLockProxy<typeLock> auLock( m_lockSingleton );
		if ( m_auSingleton.get() == NULL )
			m_auSingleton.reset( new SingletonProxy<typeInstance, typeLock>() );
	} //if ( m_auSingleton.get() == NULL )

	return ( *m_auSingleton.get() );
}

template<typename typeInstance, typename typeLock>
typeInstance & SingletonProxy<typeInstance, typeLock>::resetInstance(std::auto_ptr<typeInstance> & auNewInstance)
{
	AutoMutexLockProxy<typeLock> auLock( m_lockSingleton );
	m_auSingleton = auNewInstance;	//TODO: 多线程注意保护m_auSingleton的数据可能正在被使用
	return ( *m_auSingleton.get() );
}

//////////////////////////////////////////////////////////////////////////
/*template<typename typeInstance, typename typeLock = MutexLock>
class Singleton : private NonCopyAble
{
public:
	static typeInstance &		getInstance(void);
	static typeInstance &		resetInstance(typeInstance * pNewInstance, bool bAutoDelete);

protected:
	Singleton(void) : m_pInstance(NULL), m_bAutoDeleteInstance(false) {}
	~Singleton(void) { clear(); }
	void						clear(void);
	void						reset(typeInstance * pNewInstance, bool bAutoDelete);

private:
	typedef Singleton<typeInstance, typeLock>		typeSingleton;
	static std::auto_ptr<typeSingleton>				m_auSingleton;
	static typeLock									m_lockSingleton;

	typeInstance *									m_pInstance;
	bool											m_bAutoDeleteInstance;
};

template<typename typeInstance, typename typeLock>
std::auto_ptr< Singleton<typeInstance, typeLock> > Singleton<typeInstance, typeLock>::m_auSingleton;
								
template<typename typeInstance, typename typeLock>
typeLock Singleton<typeInstance, typeLock>::m_lockSingleton;

template<typename typeInstance, typename typeLock>
void Singleton<typeInstance, typeLock>::clear(void)
{
	if ( m_bAutoDeleteInstance )
		delete m_pInstance;
	m_pInstance = NULL;
	m_bAutoDeleteInstance = false;
}

template<typename typeInstance, typename typeLock>
void Singleton<typeInstance, typeLock>::reset(typeInstance * pNewInstance, bool bAutoDelete)
{
	clear();
	m_pInstance = pNewInstance;
	m_bAutoDeleteInstance = bAutoDelete;
}

template<typename typeInstance, typename typeLock>
typeInstance & Singleton<typeInstance, typeLock>::getInstance(void)
{
	if ( m_auSingleton.get() == NULL )
	{
		AutoMutexLockProxy<typeLock> auLock( m_lockSingleton );
		if ( m_auSingleton.get() == NULL )
			m_auSingleton.reset( new typeSingleton() );
	} //if ( m_auSingleton.get() == NULL )

	if ( m_auSingleton->m_pInstance == NULL )
	{
		AutoMutexLockProxy<typeLock> auLock( m_lockSingleton );
		if ( m_auSingleton->m_pInstance == NULL )
			m_auSingleton->reset(new typeInstance(), true);
	} //if ( m_auSingleton->m_pInstance == NULL )	
	return ( *m_auSingleton->m_pInstance );
}

template<typename typeInstance, typename typeLock>
typeInstance & Singleton<typeInstance, typeLock>::resetInstance(typeInstance * pNewInstance, bool bAutoDelete)
{
	AutoMutexLockProxy<typeLock> auLock( m_lockSingleton );
	if ( m_auSingleton.get() == NULL )
		m_auSingleton.reset( new typeSingleton() );

	m_auSingleton->reset(pNewInstance, bAutoDelete);		
	return ( *m_auSingleton->m_pInstance );
}*/

RFC_NAMESPACE_END

#endif	//RFC_SINGLETON_H_201008
