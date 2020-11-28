/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "stream/linestream.h"

RFC_NAMESPACE_BEGIN

size_t ReadStreamBuffer::m_nDefaultBufferSize = 64 * 1024;
size_t WriteStreamBuffer::m_nWriteLineBufferSize = 64 * 1024;

ReadStreamBuffer::ReadStreamBuffer(ReadStream & feedStream, size_t nBufferSize) : m_feedStream(feedStream)
{
	m_lpBuffer = new char[nBufferSize];
	m_nBufferSize = nBufferSize;
	m_nDataSize = 0;
	m_bEndofStream = false;
}

ReadStreamBuffer::~ReadStreamBuffer(void)
{
	delete []m_lpBuffer;
	m_lpBuffer = NULL;
	m_nBufferSize = 0;
	m_nDataSize = 0;
	m_bEndofStream = false;
}

RFC_NAMESPACE_END
