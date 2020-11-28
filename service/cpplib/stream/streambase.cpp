/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "stream/streambase.h"
#include "base/autoarray.h"

RFC_NAMESPACE_BEGIN

size_t ReadStream::readBlock(void * pBuffer, size_t nSize)
{
	size_t nReadCount = read(pBuffer, nSize);
	if ( nReadCount == nSize || StreamBase::resultIsEndOfStream(nReadCount) || StreamBase::resultIsError(nReadCount) )
		return nReadCount;

	size_t nRest = nSize - nReadCount;
	rfc_uint_8 * pCurPos = static_cast<rfc_uint_8*>(pBuffer) + nReadCount;
	while ( nRest > 0 )
	{
		nReadCount = read(pCurPos, nRest);
		if ( StreamBase::resultIsEndOfStream(nReadCount) || StreamBase::resultIsError(nReadCount) )
			break;
		nRest -= nReadCount;
		pCurPos += nReadCount;
	} //while ( nRest > 0 )
	return nSize - nRest;
}

void ReadStream::readBlockException(void * pBuffer, size_t nSize)
{
	if ( readBlock(pBuffer, nSize) != nSize )
		throw StreamException("ReadStream::readBlockException");
}

size_t WriteStream::writeBlock(const void * pBuffer, size_t nSize)
{
	size_t nWriteCount = write(pBuffer, nSize);
	if ( nWriteCount == nSize || StreamBase::resultIsEndOfStream(nWriteCount) || StreamBase::resultIsError(nWriteCount) )
		return nWriteCount;

	size_t nRest = nSize - nWriteCount;
	const rfc_uint_8 * pCurPos = static_cast<const rfc_uint_8*>(pBuffer) + nWriteCount;
	while ( nRest > 0 )
	{
		nWriteCount = write(pCurPos, nRest);
		if ( StreamBase::resultIsEndOfStream(nWriteCount) || StreamBase::resultIsError(nWriteCount)  )
			break;
		nRest -= nWriteCount;
		pCurPos += nWriteCount;
	} //while ( nRest > 0 )
	return nSize - nRest;
}

void WriteStream::writeBlockException(const void * pBuffer, size_t nSize)
{
	if ( writeBlock(pBuffer, nSize) != nSize )
		throw StreamException("ReadStream::writeBlockException");
}

//////////////////////////////////////////////////////////////////////////

const size_t StreamBase::g_nEndOfStream = 0;
const size_t StreamBase::g_nErrorOccur = static_cast<size_t>(-1);
const off_t StreamBase::g_nSeekOutOfRange = static_cast<off_t>(-1);
const size_t StreamBase::g_nBufferDefaultSize = 8 * 1024;

size_t StreamBase::readHandle(int nHandle, void * pBuffer, size_t nSize)
{
	if ( nHandle < 0 )
		return g_nErrorOccur;
	return static_cast<size_t>( ::read(nHandle, pBuffer, nSize) );
}

size_t StreamBase::writeHandle(int nHandle, const void * pBuffer, size_t nSize)
{
	if ( nHandle < 0 )
		return g_nErrorOccur;
	return static_cast<size_t>( ::write(nHandle, pBuffer, nSize) );
}

off_t StreamBase::getNewSeekPosition(off_t nCurPos, off_t nTotalSize, off_t nOffset, seek_position nWhence)
{
	off_t nNewPos = nOffset;
	if ( nWhence == seek_cur )
		nNewPos += nCurPos;
	else if ( nWhence == seek_end )
		nNewPos += nTotalSize;
	return nNewPos;
}

size_t StreamBase::copyStreamToStream(ReadStream & srcStream, WriteStream & destStream, size_t nCopySize, size_t nCachSize)
{
	if ( nCopySize == 0 )
		return 0;

	if ( nCachSize > nCopySize )
		nCachSize = nCopySize;
	AutoArray<rfc_uint_8> auCache(new rfc_uint_8[nCachSize]);
	rfc_uint_8 * pBuffer = auCache.get();
	size_t nRestSize = nCopySize;
	while( nRestSize > 0 )
	{
		size_t nReadCount = srcStream.read(pBuffer, GlobalFunc::getMin(nCachSize, nRestSize));
		if ( StreamBase::resultIsEndOfStream(nReadCount) )
			break;
		else if ( StreamBase::resultIsError(nReadCount) )
			return nReadCount;
		else if ( destStream.writeBlock(pBuffer, nReadCount) != nReadCount )
			return g_nErrorOccur;
		nRestSize -= nReadCount;
	} // while( nRestSize > 0 )
	return nCopySize - nRestSize;
}

RFC_NAMESPACE_END
