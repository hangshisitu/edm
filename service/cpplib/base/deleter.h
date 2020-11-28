/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_DELETER_H_201008
#define RFC_DELETER_H_201008

#include "rfcdefine.h"
#include <stdlib.h>

RFC_NAMESPACE_BEGIN

struct NullDeleter
{
	template<typename element_type>
	void operator()(element_type * pt) const {}
};

struct Deleter
{
	template<typename element_type>
	void operator()(element_type * pt) const
	{
		delete pt;
	}
};

struct FirstDeleter
{
	template<typename element_type>
	void operator()(element_type & pt) const
	{
		delete pt.first;
		pt.first = NULL;
	}
};

struct SecondDeleter
{
	template<typename element_type>
	void operator()(element_type & pt) const
	{
		delete pt.second;
		pt.second = NULL;
	}
};

struct FirstSecondDeleter
{
	template<typename element_type>
	void operator()(element_type & pt) const
	{
		delete pt.first;
		pt.first = NULL;
		delete pt.second;
		pt.second = NULL;
	}
};

struct ArrayDeleter
{
	template<typename element_type>
	void operator()(element_type * pt) const
	{
		//typedef char type_must_be_complete[sizeof(element_type)];
		delete []pt;
	}
};

struct AllocFreer
{
	template<typename element_type>
	void operator()(element_type * pt) const
	{
		if ( pt != NULL )
			free(pt);
	}
};

//////////////////////////////////////////////////////////////////////////

template<typename typeInstance>
class AutoDeleter
{
public:
	AutoDeleter(void) : m_pInstance(NULL), m_bAutoDeleteInstance(false) {}
	~AutoDeleter(void) { clear(); }
	AutoDeleter(typeInstance * pInstance, bool bAutoDeleteInstance) : m_pInstance(pInstance), m_bAutoDeleteInstance(bAutoDeleteInstance) {}
	AutoDeleter(AutoDeleter & _Right) : m_pInstance(NULL), m_bAutoDeleteInstance(false) { *this = _Right; }

	void						clear();
	void						attach(typeInstance * pInstance, bool bAutoDeleteInstance);
	typeInstance *				detach(void);
	const AutoDeleter &			operator = (AutoDeleter & _Right);

	//typeInstance *			getInstance(void) { return m_pInstance; }
	bool						isNULL(void) { return m_pInstance == NULL; }
	const typeInstance *		operator -> (void) const { return m_pInstance; }
	typeInstance *				operator -> (void) { return m_pInstance; }

protected:
private:
	typeInstance *									m_pInstance;
	bool											m_bAutoDeleteInstance;
};

template<typename typeInstance>
void AutoDeleter<typeInstance>::clear()
{
	if ( m_bAutoDeleteInstance )
		delete m_pInstance;
	m_pInstance = NULL;
	m_bAutoDeleteInstance = false;
}

template<typename typeInstance>
void AutoDeleter<typeInstance>::attach(typeInstance * pInstance, bool bAutoDeleteInstance)
{
	if ( m_pInstance == pInstance )
		return;

	clear();
	m_pInstance = pInstance;
	m_bAutoDeleteInstance = bAutoDeleteInstance;
}

template<typename typeInstance>
typeInstance * AutoDeleter<typeInstance>::detach(void)
{
	typeInstance * p = m_pInstance;
	m_pInstance = NULL;
	m_bAutoDeleteInstance = false;
	return p;
}

template<typename typeInstance>
const AutoDeleter<typeInstance> & AutoDeleter<typeInstance>::operator = (AutoDeleter & _Right)
{
	attach(_Right.m_pInstance, _Right.m_bAutoDeleteInstance);
	_Right.detach();
	return *this;
}

RFC_NAMESPACE_END

#endif	//RFC_DELETER_H_201008
