/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_CHECKSUM_H_201008
#define RFC_CHECKSUM_H_201008

#include "stream/streambase.h"

RFC_NAMESPACE_BEGIN

class Checksum : public WriteStream
{
public:
	virtual ~Checksum(void) {}

	virtual void			reset(void) = 0;
	virtual size_t			write(const void * pBuffer, size_t nSize) = 0;
	virtual size_t			flush(void) = 0;

	virtual const void *	getCheckSum(void) = 0;
	virtual const void *	getCheckSum(void) const = 0;
	virtual size_t			getCheckSumSize(void) const = 0;

	virtual off_t			tellp(void) const { return 0; }
	virtual off_t			seekp(off_t nOffset, seek_position nWhence = seek_begin) { return 0; }
};

RFC_NAMESPACE_END

#endif	//RFC_CHECKSUM_H_201008
