/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_LINESTREAM_H_201008
#define RFC_LINESTREAM_H_201008

#include "stream/streambase.h"
#include "stream/streambuffer.h"

RFC_NAMESPACE_BEGIN

class ReadLineStream : private NonCopyAble
{
public:
	ReadLineStream(ReadStream & feedStream, size_t nBufferSize = ReadLineStream::m_nReadLineBufferSize, size_t nNeedReadSize = GlobalConstant::g_uMaxSize);
	~ReadLineStream(void);
	static size_t			m_nReadLineBufferSize;

	static void				setReadLineBufferSize(size_t nSize) { m_nReadLineBufferSize = nSize; }
	static size_t			getReadLineBufferSize(void) { return m_nReadLineBufferSize; }

	bool					readLine(const char ** pLine, size_t * pLineLen, const char ** pReturnSymbol, size_t * pReturnSymbolSize);
	bool					readLine(const char ** pLine, size_t * pLineLen);
	bool					readLine(stdstring & strLine, stdstring & strReturnSymbol);
	bool					readLine(stdstring & strLine);

	bool					isEndOfStream(void) const { return m_bEndOfStream; }

protected:
	bool					readFromFeed(void);
	bool					readLineFromBuffer(const char ** pLine, size_t * pLineLen,
							const char ** pReturnSymbol, size_t * pReturnSymbolSize);

	ReadStream	&			m_feedStream;
	char *					m_lpBuffer;
	size_t					m_nBufferSize;				// m_lpBuffer分配的大小
	size_t					m_nDataSize;				// m_lpBuffer保存数据的大小
	size_t					m_nCurPos;
	size_t					m_nNeedReadSize;			// 指定只需读取多少字节
	bool					m_bEndOfStream;
};

class WriteLineStream : private NonCopyAble
{
public:
	static size_t			m_nWriteLineBufferSize;
	static void				setWriteLineBufferSize(size_t nSize) { m_nWriteLineBufferSize = nSize; }
	static size_t			getWriteLineBufferSize(void) { return m_nWriteLineBufferSize; }
};

class LineStream : virtual public ReadLineStream, virtual public WriteLineStream
{
public:

private:
	
	
};


RFC_NAMESPACE_END

#endif	//RFC_LINESTREAM_H_201008
