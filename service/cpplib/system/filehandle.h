/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_FILEHANDLE_H_201008
#define RFC_FILEHANDLE_H_201008

#include "stream/streambase.h"
#include "system/filesystem.h"
#include "base/datetime.h"

RFC_NAMESPACE_BEGIN

DECLARE_EXCEPTION(FileHandleException, Exception)

class FileHandle : public StreamBase
{
public:
	FileHandle(void) : m_nFileHandle(INVALID_HANDLE) {}
	~FileHandle(void) { close(); }

	void			close(void);
	int				getHandle(void) const { return m_nFileHandle; }
	bool			isOpened(void) const { return ( m_nFileHandle != INVALID_HANDLE ); }

	// @nFlags: O_RDONLY O_WRONLY O_RDWR O_CREAT O_TRUNC O_APPEND etc.
	// @nMode: S_IRUSR etc. more information see 'man open'
	int				open(const char * lpszFilePath, int nFlags, mode_t nMode = DEFAULT_OPEN_FILE_MODE);

	size_t			read(void * pBuffer, size_t nSize);
	size_t			write(const void * pBuffer, size_t nSize);

	off_t			tellg(void) const;
	off_t			seekg(off_t nOffset, seek_position nWhence = seek_begin);
	bool			truncate(off_t nSize);

	bool			processWriteLock(const TimeValue & tvTimeWait = TimeValue::g_tvInfiniteTime);
	bool			processReadLock(const TimeValue & tvTimeWait = TimeValue::g_tvInfiniteTime);
	bool			processUnLock(void);

protected:
	bool			waitProcessLock(bool bReadLock, const TimeValue & tvTimeWait);

private:
	enum	InternalFlags
	{
		FLAGS_NONE					= 0,
		LOCK_PROCESS				= 1,
	};

	int				m_nFileHandle;
	BitFlags<rfc_uint_8> m_nInternalFalgs;
};

RFC_NAMESPACE_END

#endif	//RFC_FILEHANDLE_H_201008
