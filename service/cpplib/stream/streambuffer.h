/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_STREAMBUFFER_H_201008
#define RFC_STREAMBUFFER_H_201008

#include "stream/streambase.h"

RFC_NAMESPACE_BEGIN

class ReadStreamBuffer : private NonCopyAble
{
public:
	ReadStreamBuffer(ReadStream & feedStream, size_t nBufferSize = ReadStreamBuffer::m_nDefaultBufferSize);
	~ReadStreamBuffer(void);
	
	static size_t m_nDefaultBufferSize;
	static void				setReadLineBufferSize(size_t nSize) { m_nReadLineBufferSize = nSize; }
	static size_t			getReadLineBufferSize(void) { return m_nReadLineBufferSize; }
	

	bool					readLine(const char ** pLine, size_t * pLineLen,
							const char ** pReturnSymbol, size_t * pReturnSymbolSize);
	bool					readLine(const char ** pLine, size_t * pLineLen);
	bool					readLine(stdstring & strLine, stdstring & strReturnSymbol);
	bool					readLine(stdstring & strLine);

	bool					isEndofStream(void) const { return m_bEndofStream; }

protected:
	static size_t			m_nReadLineBufferSize;

	ReadStream	&			m_feedStream;
	char *					m_lpBuffer;
	size_t					m_nBufferSize;				// m_lpBuffer分配的大小
	size_t					m_nDataSize;				// m_lpBuffer保存数据的大小
	bool					m_bEndofStream;
};

class WriteStreamBuffer : private NonCopyAble
{
public:
	static void				setWriteLineBufferSize(size_t nSize) { m_nWriteLineBufferSize = nSize; }
	static size_t			getWriteLineBufferSize(void) { return m_nWriteLineBufferSize; }

protected:
	static size_t			m_nWriteLineBufferSize;
};

class StreamBuffer : virtual public ReadStreamBuffer, virtual public WriteStreamBuffer
{
public:
};

RFC_NAMESPACE_END

#endif	//RFC_STREAMBUFFER_H_201008
