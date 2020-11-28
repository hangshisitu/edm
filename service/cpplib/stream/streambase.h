/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_STREAMBASE_H_201008
#define RFC_STREAMBASE_H_201008

#include "base/exception.h"
#include "base/globalfunc.h"

RFC_NAMESPACE_BEGIN

DECLARE_EXCEPTION(StreamException, Exception)

class ReadStream : private NonCopyAble
{
public:
	virtual ~ReadStream(void) {}

	virtual off_t			tellg(void) const = 0;
	virtual off_t			seekg(off_t nOffset, seek_position nWhence = seek_begin) = 0;

	virtual size_t			read(void * pBuffer, size_t nSize) = 0;
	size_t					readBlock(void * pBuffer, size_t nSize);
	void					readBlockException(void * pBuffer, size_t nSize);// throw (StreamException);
};

class WriteStream : private NonCopyAble
{
public:
	virtual ~WriteStream(void) {}

	virtual off_t			tellp(void) const = 0;
	virtual off_t			seekp(off_t nOffset, seek_position nWhence = seek_begin) = 0;
	virtual size_t			flush(void) { return 0; }
	virtual bool			truncate(off_t nSize) { return false; }

	virtual size_t			write(const void * pBuffer, size_t nSize) = 0;
	size_t					writeBlock(const void * pBuffer, size_t nSize);
	void					writeBlockException(const void * pBuffer, size_t nSize);// throw (StreamException);
};

class StreamBase : virtual public ReadStream, virtual public WriteStream
{
public:
	virtual ~StreamBase(void) {}

	static const size_t		g_nEndOfStream;
	static const size_t		g_nErrorOccur;
	static const off_t		g_nSeekOutOfRange;
	static const size_t		g_nBufferDefaultSize;

	static inline bool		resultIsEndOfStream(size_t nResult) { return ( nResult == g_nEndOfStream); }
	static inline bool		resultIsError(size_t nResult) { return ( nResult == g_nErrorOccur); }

	static size_t			readHandle(int nHandle, void * pBuffer, size_t nSize);
	static size_t			writeHandle(int nHandle, const void * pBuffer, size_t nSize);

	static off_t			getNewSeekPosition(off_t nCurPos, off_t nTotalSize, off_t nOffset, seek_position nWhence);

	static size_t		copyStreamToStream(ReadStream & srcStream, WriteStream & destStream, size_t nCopySize, size_t nCachSize = g_nBufferDefaultSize);

	virtual off_t			tellg(void) const { return g_nSeekOutOfRange; };
	virtual off_t			tellp(void) const { return tellg(); }
	virtual off_t			seekg(off_t nOffset, seek_position nWhence = seek_begin) { return g_nSeekOutOfRange; }
	virtual off_t			seekp(off_t nOffset, seek_position nWhence = seek_begin) { return seekg(nOffset, nWhence); }
};

RFC_NAMESPACE_END

#endif	//RFC_STREAMBASE_H_201008
