/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_SHAREPTR_H_201008
#define RFC_SHAREPTR_H_201008

#include "system/atomicinteger.h"
#include "base/exception.h"

RFC_NAMESPACE_BEGIN

DECLARE_EXCEPTION(SharePtrException, Exception)

// 支持析构时是否需要回收内存, 重载release();需要做的事情

// 引用计数, 同一块内存,只有最后一个对象析构时才释放内存
template<typename typeObject>
class SharePtr
{
public:
	typedef typeObject			element_type;
	SharePtr(void) : m_pSrcObject(NULL), m_pCounter(NULL) {}
	explicit SharePtr(element_type * pObject) : m_pSrcObject(NULL), m_pCounter(NULL) { reset(pObject); }
	SharePtr(const SharePtr<element_type> & _Right) : m_pSrcObject(NULL), m_pCounter(NULL) { reset(_Right); }

	template<class typeChild>
	explicit SharePtr(typeChild * pObject) : m_pSrcObject(NULL), m_pCounter(NULL) { reset(pObject); }

	template<class typeChild>
	SharePtr(const SharePtr<typeChild> & _Right) : m_pSrcObject(NULL), m_pCounter(NULL) { reset(_Right); }

	~SharePtr(void) { release(); }

	bool					isNULL(void) const { return (m_pSrcObject == NULL); }

	element_type &			getRef(void) throw(SharePtrException)
	{
		if ( isNULL() )
			throw SharePtrException("SharePtr::m_pSrcObject is NULL on getRef");
		return *m_pSrcObject;
	}

	const element_type &	getConstRef(void) const throw(SharePtrException)
	{
		if ( isNULL() )
			throw SharePtrException("SharePtr::m_pSrcObject is NULL on getConstRef");
		return *m_pSrcObject;
	}

	element_type *			getPtr(void) { return m_pSrcObject; }
	const element_type *	getConstPtr(void) const { return m_pSrcObject; }

	element_type *			operator->(void)
	{
		if ( isNULL() )
			throw SharePtrException("SharePtr::m_pSrcObject is NULL on operator->");
		return m_pSrcObject;
	}

	const element_type *	operator->(void) const
	{
		if ( isNULL() )
			throw SharePtrException("SharePtr::m_pSrcObject is NULL on operator->");
		return m_pSrcObject;
	}

	void					release(void)
	{
		if ( isNULL() )
			return;
		if ( m_pCounter->decAndGet() == 0 )
		{
			delete m_pCounter;
			delete m_pSrcObject;
		}
		m_pCounter = NULL;
		m_pSrcObject = NULL;
	}

	void					reset(element_type * pObject)
	{
		if ( m_pSrcObject == pObject )
			return;
		release();
		m_pSrcObject = pObject;
		m_pCounter = new typeRefCounter(1);
	}

	void					reset(const SharePtr<element_type> & _Right)
	{
		if ( _Right.m_pSrcObject == m_pSrcObject )
			return;
		release();
		m_pSrcObject = _Right.m_pSrcObject;
		m_pCounter = _Right.m_pCounter;
		m_pCounter->incAndGet();
	}

	template<class typeChild>
	void					reset(const SharePtr<typeChild> & _Right)
	{	
		if ( _Right.m_pSrcObject == m_pSrcObject )
			return;
		release();
		m_pSrcObject = _Right.m_pSrcObject;
		m_pCounter = _Right.m_pCounter;
		m_pCounter->incAndGet();
	}

	SharePtr<element_type> & operator=(const SharePtr<element_type> & _Right)
	{	
		reset(_Right);
		return (*this);
	}

	template<class typeChild>
	SharePtr<element_type> & operator=(const SharePtr<typeChild> & _Right)
	{	
		reset(_Right);
		return (*this);
	}

	rfc_uint_32					getRefCount(void) const { return m_pCounter->getValue(); }

private:
	typedef AtomicInteger<rfc_uint_32, MutexLock>	typeRefCounter;
	typeObject			*		m_pSrcObject;	
	typeRefCounter		*		m_pCounter;
};


RFC_NAMESPACE_END

#endif	//RFC_SHAREPTR_H_201008


