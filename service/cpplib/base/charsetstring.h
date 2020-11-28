/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_CHARSETSTRING_H_201008
#define RFC_CHARSETSTRING_H_201008

#include "base/string.h"

RFC_NAMESPACE_BEGIN

class CharsetString
{
public:
	typedef char			value_type;
	CharsetString(void) : m_pBuffer(""), m_nSize(0) {}
	CharsetString(const CharsetString & str) {}
	CharsetString & operator = (const CharsetString & str) { return *this; }
	void		swap(CharsetString & str) {}
	
	const value_type *		data(void) const { return m_pBuffer; }
	size_t					size(void) const { return m_nSize; }

private:
	value_type	*			m_pBuffer;
	size_t					m_nSize;
};


RFC_NAMESPACE_END

#endif	//RFC_CHARSETSTRING_H_201008
