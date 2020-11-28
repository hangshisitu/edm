/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-08      */

#ifndef RFC_FILELOG_H_201108
#define RFC_FILELOG_H_201108

#include "mutex/mutex.h"
#include "system/filehandle.h"

RFC_NAMESPACE_BEGIN

class FileLog : private NonCopyAble
{
public:
	FileLog(void);
	// strLogPath may like /home/iedm/logs/deliverengine.log
	bool				init(const stdstring & strLogPath, size_t nFileMaxSize = GlobalConstant::g_uMaxSize, int nFileSaveDays = -1);

	void				writeLog(const stdstring & strLog);
	void				writeLogLine(const stdstring & strLine);

	void				writeLogWithTID(const stdstring & strLog);
	void				writeLogLineWithTID(const stdstring & strLine);

protected:
	void				openFile(const DateTime & dt, size_t nLastLogSize);
	void				deleteExpiredFile(void) const;

	MutexLock			m_lockFile;
	size_t				m_nFileSize, m_nFileMaxSize;
	int					m_nFileSaveDays;
	stdstring			m_strLogPath;
	DateTime			m_dtLastLogTime;
	FileHandle			m_File;
};

RFC_NAMESPACE_END

#endif	//RFC_FILELOG_H_201108
