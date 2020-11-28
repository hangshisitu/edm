/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_REFERENCECOUNT_H_201008
#define RFC_REFERENCECOUNT_H_201008

//#include "base/exception.h"
#include "base/globalfunc.h"

RFC_NAMESPACE_BEGIN

//DECLARE_EXCEPTION(AutoArrayException, Exception)

//like std::auto_ptr, and AutoArray constructed by new element_type [], and delete [] in destructor
template<class typeObject>
class AutoArray
{
public:
	typedef typeObject element_type;
	AutoArray(void) : m_pAryPtr(NULL) {}
	explicit AutoArray(element_type * Ptr) : m_pAryPtr(Ptr) {}
	AutoArray(AutoArray<element_type> & _Right) : m_pAryPtr(_Right.release()) {}

	template<class typeChild>
	explicit AutoArray(typeChild * Ptr) : m_pAryPtr(Ptr) {}

	template<class typeChild>
	AutoArray(AutoArray<typeChild> & _Right) : m_pAryPtr(_Right.release()) {}

	~AutoArray(void) { delete []m_pAryPtr; }

	AutoArray<element_type>& operator=(AutoArray<element_type>& _Right)
	{	
		reset(_Right.release());
		return (*this);
	}

	template<class typeChild>
	AutoArray<element_type>& operator=(AutoArray<typeChild>& _Right)
	{	
		reset(_Right.release());
		return (*this);
	}

	inline element_type& operator*() const
	{	
		return (*get());
	}

	inline element_type * operator->() const
	{	
		return get();
	}

	//! return wrapped pointer
	inline element_type * get() const
	{	
		return m_pAryPtr;
	}

	inline element_type * release()
	{	
		element_type * pTmp = m_pAryPtr;
		m_pAryPtr = 0;
		return pTmp;
	}

	inline void reset(element_type* Ptr = 0)
	{	
		if ( Ptr != m_pAryPtr )
			delete []m_pAryPtr;
		m_pAryPtr = Ptr;
	}

	inline element_type & operator[] (size_t nIndex)
	{
		return m_pAryPtr[nIndex];
	}

	inline const element_type & operator[] (size_t nIndex) const
	{
		return m_pAryPtr[nIndex];
	}

private:
	element_type * m_pAryPtr;	
};

RFC_NAMESPACE_END

#endif	//RFC_REFERENCECOUNT_H_201008
