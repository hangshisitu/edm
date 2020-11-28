/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "stream/binarystream.h"

RFC_NAMESPACE_BEGIN

BinaryStream::BinaryStream(int nAutoGrowUpSize) : m_lpBuffer(NULL), m_nBufferSize(0), m_nDataSize(0),
m_nCurPos(0), m_nInternalFalgs(FLAGS_AUTODEL), m_nAutoGrowUpSize(nAutoGrowUpSize)
{
}

BinaryStream::BinaryStream(void * lpBuffer, size_t nBufLen, bool bAutoDelete)
: m_lpBuffer(reinterpret_cast<rfc_uint_8 *>(lpBuffer)), m_nBufferSize(nBufLen), m_nDataSize(nBufLen),
m_nCurPos(0), m_nInternalFalgs(FLAGS_NONE), m_nAutoGrowUpSize(0)
{
	if ( bAutoDelete )
		m_nInternalFalgs = FLAGS_AUTODEL;
}

BinaryStream::~BinaryStream(void)
{
	clear();
}

void BinaryStream::clear(void)
{
	if ( m_lpBuffer != NULL && !isAttach() )
		delete []m_lpBuffer;
	m_lpBuffer = NULL;
	m_nBufferSize = 0;
	m_nDataSize = 0;
	m_nCurPos = 0;
	m_nInternalFalgs = FLAGS_AUTODEL;
}

void BinaryStream::clearAndReserve(size_t nSize)
{
	if ( isAttach() )
		throw BinaryStreamException("BinaryStream::clearAndReserve memory is attach");

	if ( nSize <= m_nBufferSize )
	{
		m_nDataSize = 0;
		m_nCurPos = 0;
	}
	else
	{
		clear();
		reserve(nSize);
	}
}

void BinaryStream::reserve(size_t nSize)
{
	if ( nSize > m_nBufferSize )
	{
		if ( isAttach() )
			throw BinaryStreamException("BinaryStream::reserve memory is attach");

		rfc_uint_8 * pBuffer = new rfc_uint_8[nSize];
		if ( m_lpBuffer != NULL )
		{
			memcpy(pBuffer, m_lpBuffer, m_nDataSize);
			delete []m_lpBuffer;
		}
		m_nBufferSize = nSize;
		m_lpBuffer = pBuffer;
	}
}

void BinaryStream::resize(size_t nSize)
{
	reserve(nSize);
	m_nDataSize = nSize;
}

void BinaryStream::resize(size_t nSize, rfc_uint_8 bFillter)
{
	reserve(nSize);
	if ( nSize > m_nDataSize )
		memset(m_lpBuffer + m_nDataSize, bFillter, nSize - m_nDataSize);
	m_nDataSize = nSize;
}

off_t BinaryStream::seekg(off_t nOffset, seek_position nWhence)
{
	off_t nNewPos = StreamBase::getNewSeekPosition(m_nCurPos, static_cast<off_t>( m_nDataSize ), nOffset, nWhence);
	if ( nNewPos < 0 || nNewPos > static_cast<off_t>( m_nDataSize ) )
		return static_cast<off_t>(-1);
	m_nCurPos = nNewPos;
	return m_nCurPos;
}

size_t BinaryStream::getBuffer(void * lpBuffer, size_t nSize) const
{
	if ( m_nDataSize <= static_cast<size_t>(m_nCurPos) )
		return 0;

	size_t nRestSize = m_nDataSize - static_cast<size_t>(m_nCurPos);
	if ( nRestSize < nSize )
		nSize = nRestSize;
	memcpy(lpBuffer, m_lpBuffer + m_nCurPos, nSize);
	return nSize;
}

size_t BinaryStream::read(void * lpBuffer, size_t nSize)
{
	nSize = getBuffer(lpBuffer, nSize);
	m_nCurPos += static_cast<off_t>(nSize);
	return	nSize;
}

size_t BinaryStream::write(const void * lpBuffer, size_t nSize)
{
	if ( nSize == 0 )
		return 0;

	if ( m_nBufferSize < static_cast<size_t>(m_nCurPos) + nSize )
	{
		size_t nTotalSize = static_cast<size_t>(m_nCurPos) + nSize;
		if ( (m_nAutoGrowUpSize == -1) || (m_nAutoGrowUpSize > 0 && nTotalSize > static_cast<size_t>(m_nAutoGrowUpSize)) )
			nTotalSize <<= 1;
		else if ( m_nAutoGrowUpSize > 0 )
			nTotalSize += static_cast<size_t>( m_nAutoGrowUpSize );

		reserve(nTotalSize);
	}

	memcpy(m_lpBuffer + m_nCurPos, lpBuffer, nSize);
	m_nCurPos += static_cast<off_t>(nSize);
	if (static_cast<off_t>(m_nDataSize) < m_nCurPos)
		m_nDataSize = m_nCurPos;
	return nSize;
}

bool BinaryStream::truncate(off_t nSize)
{
	resize(nSize);
	if ( m_nCurPos > nSize )
		m_nCurPos = nSize;
	return true;
}

void BinaryStream::attach(void * lpBuffer, size_t nBufLen, bool bAutoDelete)
{
	clear();
	m_lpBuffer = reinterpret_cast<rfc_uint_8 *>(lpBuffer);
	m_nBufferSize = m_nDataSize = nBufLen;
	m_nCurPos = 0;
	m_nInternalFalgs = ( bAutoDelete ? FLAGS_AUTODEL : FLAGS_NONE );
}

rfc_uint_8 * BinaryStream::detach(void)
{
	rfc_uint_8 * res = m_lpBuffer;
	m_lpBuffer = NULL;
	clear();
	return res;
}

rfc_uint_8 & BinaryStream::operator[](size_t nIndex)
{
	if ( m_lpBuffer == NULL || nIndex >= m_nDataSize )
		throw BinaryStreamException("Out of BinaryStream bound");
	return m_lpBuffer[nIndex];
}

rfc_uint_8 BinaryStream::operator [](size_t nIndex) const
{
	if ( m_lpBuffer == NULL || nIndex >= m_nDataSize )
		throw BinaryStreamException("Out of BinaryStream bound");
	return m_lpBuffer[nIndex];
}

RFC_NAMESPACE_END
