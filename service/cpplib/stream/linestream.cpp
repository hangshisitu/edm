/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "stream/linestream.h"

RFC_NAMESPACE_BEGIN

size_t ReadLineStream::m_nReadLineBufferSize = 64 * 1024;
size_t WriteLineStream::m_nWriteLineBufferSize = 64 * 1024;

ReadLineStream::ReadLineStream(ReadStream & feedStream, size_t nBufferSize, size_t nNeedReadSize) : m_feedStream(feedStream)
{
	m_lpBuffer = new char[nBufferSize];
	m_nBufferSize = nBufferSize;
	m_nDataSize = 0;
	m_nCurPos = 0;
	m_nNeedReadSize = nNeedReadSize;
	m_bEndOfStream = false;
}

ReadLineStream::~ReadLineStream(void)
{
	delete []m_lpBuffer;
	m_lpBuffer = NULL;
	m_nBufferSize = 0;
	m_nDataSize = 0;
	m_nCurPos = 0;
	m_nNeedReadSize = 0;
	m_bEndOfStream = false;
}

bool ReadLineStream::readFromFeed(void)
{
	// Move current data block to beginning of m_lpBuffer
	if ( m_nCurPos != 0 )
	{
		memmove(m_lpBuffer, m_lpBuffer + m_nCurPos, m_nDataSize - m_nCurPos);
		m_nDataSize = m_nDataSize - m_nCurPos;
		m_nCurPos = 0;
	}

	if ( m_nDataSize >= m_nBufferSize )		//buffer已经满,但还没遇到回车符
		return false;

	size_t nReadCount = m_feedStream.read(m_lpBuffer + m_nDataSize, m_nBufferSize - m_nDataSize);
	if ( StreamBase::resultIsError(nReadCount) )
		return false;
	m_bEndOfStream = StreamBase::resultIsEndOfStream(nReadCount);
	m_nDataSize += nReadCount;
	return true;
}

bool ReadLineStream::readLineFromBuffer(const char ** pLine, size_t * pLineLen, const char ** pReturnSymbol, size_t * pReturnSymbolSize)
{
	for ( size_t nPos = m_nCurPos; nPos < m_nDataSize; ++nPos )
	{
		if ( m_lpBuffer[nPos] == '\r' || m_lpBuffer[nPos] == '\n' )
		{
			// return false to get '\n' on next byte
			if ( m_lpBuffer[nPos] == '\r' && nPos + 1 >= m_nDataSize )
				return false;
			*pLine = m_lpBuffer + m_nCurPos;
			*pLineLen = nPos - m_nCurPos;
			*pReturnSymbol = m_lpBuffer + nPos;
			m_nCurPos = nPos + 1;
			if ( m_lpBuffer[nPos] == '\r' && m_lpBuffer[m_nCurPos] == '\n' )
				++m_nCurPos;
			*pReturnSymbolSize = m_nCurPos - nPos;
			return true;
		}
	} // for ( size_t nPos = m_nCurPos; nPos < m_nDataSize; ++nPos )
	return false;
}

bool ReadLineStream::readLine(const char ** pLine, size_t * pLineLen,
							  const char ** pReturnSymbol, size_t * pReturnSymbolSize)
{
	if ( isEndOfStream() )
		return false;
	if ( readLineFromBuffer(pLine, pLineLen, pReturnSymbol, pReturnSymbolSize) )
		return true;

	while ( true )
	{
		if ( !readFromFeed() )
			return false;
		if ( isEndOfStream() )
		{
			size_t nEndPos = ( m_nDataSize > 0 ? m_nDataSize - 1 : m_nDataSize );
			if ( nEndPos < m_nDataSize && m_lpBuffer[nEndPos] != '\r' )
				++nEndPos;
			*pLine = m_lpBuffer + m_nCurPos;
			*pLineLen = nEndPos - m_nCurPos;
			*pReturnSymbol = m_lpBuffer + nEndPos;
			*pReturnSymbolSize = m_nDataSize - nEndPos;
			return ( m_nDataSize > 0 );
		}

		if ( readLineFromBuffer(pLine, pLineLen, pReturnSymbol, pReturnSymbolSize) )
			return true;
	} // while ( true )
	return false;
}

bool ReadLineStream::readLine(const char ** pLine, size_t * pLineLen)
{
	const char * pReturnSymbol = NULL;
	size_t nReturnSymbolSize = 0;
	return readLine(pLine, pLineLen, &pReturnSymbol, &nReturnSymbolSize);
}

bool ReadLineStream::readLine(stdstring & strLine, stdstring & strReturnSymbol)
{
	const char * pLine = NULL;
	const char * pReturnSymbol = NULL;
	size_t nLineLen = 0, nReturnSymbolSize = 0;
	if ( !readLine(&pLine, &nLineLen, &pReturnSymbol, &nReturnSymbolSize) )
		return false;
	strLine.assign(pLine, nLineLen);
	strReturnSymbol.assign(pReturnSymbol, nReturnSymbolSize);
	return true;
}

bool ReadLineStream::readLine(stdstring & strLine)
{
	stdstring strReturnSymbol;
	return readLine(strLine, strReturnSymbol);
}

RFC_NAMESPACE_END
