/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "system/filehandle.h"
#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

void FileHandle::close(void)
{
	if ( m_nFileHandle == INVALID_HANDLE )
		return;

	processUnLock();
	::close(m_nFileHandle);
	m_nFileHandle = INVALID_HANDLE;
}

int FileHandle::open(const char * lpszFilePath, int nFlags, mode_t nMode)
{
#ifdef O_LARGEFILE
	nFlags |= O_LARGEFILE;
#endif
	close();
	m_nFileHandle = ::open(lpszFilePath, nFlags, nMode);
	return m_nFileHandle;
}

size_t FileHandle::read(void * pBuffer, size_t nSize)
{
	return StreamBase::readHandle(m_nFileHandle, pBuffer, nSize);
}

size_t FileHandle::write(const void * pBuffer, size_t nSize)
{
	return StreamBase::writeHandle(m_nFileHandle, pBuffer, nSize);
}

off_t FileHandle::tellg(void) const
{
	return ::lseek(m_nFileHandle, 0, seek_cur);
}

off_t FileHandle::seekg(off_t nOffset, seek_position nWhence)
{
	return ::lseek(m_nFileHandle, nOffset, nWhence);
}

bool FileHandle::truncate(off_t nSize)
{
	return ( ::ftruncate(m_nFileHandle,nSize) == 0 );
}

bool FileHandle::processWriteLock(const TimeValue & tvTimeWait)
{
	return waitProcessLock(false, tvTimeWait);
}

bool FileHandle::processReadLock(const TimeValue & tvTimeWait)
{
	return waitProcessLock(true, tvTimeWait);
}

bool FileHandle::processUnLock(void)
{
	if ( !m_nInternalFalgs.check(LOCK_PROCESS) )
		return true;

	struct flock fProcessLock;
	memset(&fProcessLock, 0, sizeof(fProcessLock));
	fProcessLock.l_type = F_UNLCK;
	int nRet = fcntl(m_nFileHandle, F_SETLKW, &fProcessLock);
	if ( nRet == 0 )
		m_nInternalFalgs.clearFlag(LOCK_PROCESS);
	return ( nRet == 0 );
}

bool FileHandle::waitProcessLock(bool bReadLock, const TimeValue & tvTimeWait)
{
	if ( m_nInternalFalgs.check(LOCK_PROCESS) )
		return false;

	struct flock fProcessLock;
	memset(&fProcessLock, 0, sizeof(fProcessLock));
	fProcessLock.l_type = bReadLock ? F_RDLCK : F_WRLCK;
	int nFcntlCmd = ( tvTimeWait == TimeValue::g_tvInfiniteTime ) ? F_SETLKW : F_SETLK;
	int nRet = fcntl(m_nFileHandle, nFcntlCmd, &fProcessLock);

	if ( nRet != 0 )
	{
		if ( errno == EINTR || errno == ENOLCK || errno == EDEADLK || tvTimeWait == TimeValue::g_tvInfiniteTime )
			return false;
		delay(tvTimeWait);
		nRet = fcntl(m_nFileHandle, nFcntlCmd, &fProcessLock);
	} // if ( nRet != 0 )

	if ( nRet == 0 )
		m_nInternalFalgs.setFlag(LOCK_PROCESS);
	return ( nRet == 0 );
}

RFC_NAMESPACE_END
