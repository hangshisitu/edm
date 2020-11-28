/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "rfcdefine.h"

#if (SUPPORT_POSIX_SPINLOCK != 0)

#ifndef RFC_SPINLOCK_H_201108
#define RFC_SPINLOCK_H_201108

#include "mutex/mutexbase.h"

RFC_NAMESPACE_BEGIN

//SpinLock ��һ����������ͬ��������һ�ַ����������� lock ����������ʱ�������ǰ��Լ��ҵ�һ���ȴ����У�������ѭ�� CPU ��ת�ȴ������߳��ͷ�����
//Ҳ����˵���ȴ�������spin lock��������cpu��Դ���ʺ��ٽ�������ǳ��򵥵ĳ���

class SpinLock : public MutexBase
{
public:
	SpinLock(void);
	~SpinLock(void);

	void			lock(void) const throw(MutexException);
	bool			tryLock(void) const throw(MutexException);
	void			release(void) const throw(MutexException);

protected:
	mutable pthread_spinlock_t	m_lock;
};

RFC_NAMESPACE_END

#endif	//RFC_SPINLOCK_H_201108

#endif	// #if (SUPPORT_POSIX_SPINLOCK != 0)
