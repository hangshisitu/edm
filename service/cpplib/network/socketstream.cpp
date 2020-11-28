/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "network/socketstream.h"
#include "network/socketselector.h"
#include "network/networkfunc.h"
#include "base/byteorderfunc.h"
#include "base/formatstream.h"
#include <sys/uio.h>			// for writev()

RFC_NAMESPACE_BEGIN

SocketStream::SocketStream(typeSocketHandle nSocketID) : m_nSocketID(nSocketID), m_tvTimeout(5),
	m_nMaxTransferSize(GlobalConstant::g_uMaxSize)
{
}

SocketStream::SocketStream(const SocketStream & s) : m_nSocketID(s.m_nSocketID), m_tvTimeout(s.m_tvTimeout),
	m_nMaxTransferSize(s.m_nMaxTransferSize)
{
}

size_t SocketStream::readWithoutTimed(void * pBuffer, size_t nSize)
{
	nSize = StreamBase::readHandle(m_nSocketID, pBuffer, nSize);
	if ( !StreamBase::resultIsError(nSize) && nSize > m_nMaxTransferSize )
		throw SocketStreamException(E2BIG, FormatString("SocketStream::readWithoutTimed size limit:%(%)").arg(nSize).arg(m_nMaxTransferSize).str());
	return nSize;
}

size_t SocketStream::writeWithoutTimed(const void * pBuffer, size_t nSize)
{
	if ( nSize > m_nMaxTransferSize )
		throw SocketStreamException(E2BIG, FormatString("SocketStream::writeWithoutTimed size limit:%(%)").arg(nSize).arg(m_nMaxTransferSize).str());

	size_t nWriteCount = StreamBase::writeHandle(m_nSocketID, pBuffer, nSize);
	int nRetryCount = 5;
	while ( StreamBase::resultIsError(nWriteCount) && nRetryCount-- > 0 ) // 被中断时要重试
	{
		if ( errno != EINTR && errno != EAGAIN )
			break;
		nWriteCount = StreamBase::writeHandle(m_nSocketID, pBuffer, nSize);
	} // while
	return nWriteCount;
}

size_t SocketStream::read(void * pBuffer, size_t nSize)
{
	if ( m_tvTimeout == TimeValue::g_tvInfiniteTime )
		return readWithoutTimed(pBuffer, nSize);

	for ( int i = 5; i > 0; --i )
	{
		int nSocketMonitorRet = SocketSyncMonitor::socketReadCheckTimeout(m_nSocketID, m_tvTimeout);
		if ( nSocketMonitorRet == ETIMEDOUT )
			return StreamBase::g_nEndOfStream;
		else if ( static_cast<size_t>(nSocketMonitorRet) == StreamBase::g_nErrorOccur )
			return StreamBase::g_nErrorOccur;

		size_t nReadCount = readWithoutTimed(pBuffer, nSize);
		if ( StreamBase::resultIsEndOfStream(nReadCount) )
			return StreamBase::g_nErrorOccur;
		else if ( StreamBase::resultIsError(nReadCount) && errno != EINTR && errno != EAGAIN )
			return StreamBase::g_nErrorOccur;
		else if ( !StreamBase::resultIsError(nReadCount) )
			return nReadCount;
	} // for ( int i = 5; i > 0; --i )

	return StreamBase::g_nErrorOccur;
}

size_t SocketStream::write(const void * pBuffer, size_t nSize)
{
	if ( m_tvTimeout != TimeValue::g_tvInfiniteTime )
	{
		int nSocketMonitorRet = SocketSyncMonitor::socketWriteCheckTimeout(m_nSocketID, m_tvTimeout);
		if ( nSocketMonitorRet == ETIMEDOUT )
			return StreamBase::g_nEndOfStream;
		else if ( static_cast<size_t>(nSocketMonitorRet) == StreamBase::g_nErrorOccur )
			return StreamBase::g_nErrorOccur;
	} //if ( m_tvTimeout != TimeValue::g_tvInfiniteTime )

	return writeWithoutTimed(pBuffer, nSize);
}

size_t SocketStream::readWithSize(BinaryStream & binData, bool bOnlyBodySize)
{
	rfc_uint_32 nSize = 0;
	size_t nReadCount = readBlock(&nSize, sizeof(nSize));
	if ( nReadCount != sizeof(nSize) )
		return StreamBase::g_nErrorOccur;

	nSize = ByteOrderFunc::ntoh(nSize);
	// 指定长度包括包头长度+包体长度,即4+包体长度,再读取nSize-4即可
	if ( !bOnlyBodySize )
	{
		if ( nSize < sizeof(rfc_uint_32) )
			throw SocketStreamException(E2BIG, FormatString("SocketStream::readWithSize size smaller than 4:%").arg(nSize).str());
		nSize -= sizeof(rfc_uint_32);
	}

	if ( nSize > m_nMaxTransferSize )
		throw SocketStreamException(E2BIG, FormatString("SocketStream::readWithSize size limit:%(%)").arg(nSize).arg(m_nMaxTransferSize).str());
	binData.clearAndReserve(nSize);
	binData.resize(nSize);
	if ( nSize > 0 && readBlock(binData.data(), nSize) != nSize )
		return StreamBase::g_nErrorOccur;
	return nSize;
}

size_t SocketStream::writeWithSize(const void * pBuffer, size_t nSize, bool bOnlyBodySize)
{
	if ( nSize > m_nMaxTransferSize )
		throw SocketStreamException(E2BIG, FormatString("SocketStream::writeWithSize size limit:%(%)").arg(nSize).arg(m_nMaxTransferSize).str());

	rfc_uint_32 nNetByteOrderSize = ByteOrderFunc::hton(static_cast<rfc_uint_32>(nSize));
	// 指定长度包括包头长度+包体长度,即4+包体长度
	if ( !bOnlyBodySize )
		nNetByteOrderSize = ByteOrderFunc::hton(static_cast<rfc_uint_32>(nSize + sizeof(rfc_uint_32)) );
	struct iovec iov[2];
	iov[0].iov_base = (char *)&nNetByteOrderSize;
	iov[0].iov_len = sizeof(nNetByteOrderSize);
	iov[1].iov_base = (char *)pBuffer;
	iov[1].iov_len = nSize;

	if ( m_tvTimeout != TimeValue::g_tvInfiniteTime )
	{
		int nSocketMonitorRet = SocketSyncMonitor::socketWriteCheckTimeout(m_nSocketID, m_tvTimeout);
		if ( nSocketMonitorRet == ETIMEDOUT )
			return StreamBase::g_nEndOfStream;
		else if ( static_cast<size_t>(nSocketMonitorRet) == StreamBase::g_nErrorOccur )
			return StreamBase::g_nErrorOccur;
	} //if ( m_tvTimeout != TimeValue::g_tvInfiniteTime )

	int nWriteCount = ::writev(m_nSocketID, iov, (nSize == 0) ? 1 : 2);
	nWriteCount -= sizeof(nNetByteOrderSize);
	if ( nWriteCount < 0 )
		return StreamBase::g_nErrorOccur;

	if ( nWriteCount < static_cast<int>(nSize) )
	{
		size_t nRestSize = nSize - static_cast<size_t>(nWriteCount);
		nRestSize = writeBlock(static_cast<const rfc_uint_8 *>(pBuffer) + nWriteCount, nRestSize);
		//if ( StreamBase::resultIsEndOfStream(nRestSize) || StreamBase::resultIsError(nRestSize) )
		if ( nRestSize != nSize - static_cast<size_t>(nWriteCount) )
			return StreamBase::g_nErrorOccur;
	}
	return nSize;
}

RFC_NAMESPACE_END
