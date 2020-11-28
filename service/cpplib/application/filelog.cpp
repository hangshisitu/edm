/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-08      */

#include "application/filelog.h"
#include "thread/thread.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

FileLog::FileLog(void) : m_nFileSize(0), m_nFileMaxSize(GlobalConstant::g_uMaxSize), m_nFileSaveDays(-1)
{
}

bool FileLog::init(const stdstring & strLogPath, size_t nFileMaxSize, int nFileSaveDays)
{
	m_nFileSize = 0;
	m_nFileMaxSize = nFileMaxSize;
	m_nFileSaveDays = nFileSaveDays;
	m_strLogPath = strLogPath;
	m_dtLastLogTime.setNULL();
	m_File.close();

	openFile(DateTime(time(NULL)), 0);
	return m_File.isOpened();
}

void FileLog::openFile(const DateTime & dt, size_t nLastLogSize)
{
	bool bOnTheSameDay = m_dtLastLogTime.isSameDay(dt);
	bool bFileSizeExceed = ( m_nFileSize + nLastLogSize > m_nFileMaxSize );
	if ( m_File.isOpened() && bOnTheSameDay && !bFileSizeExceed )
		return;

	stdstring strParentPath, strFileName;
	FileSystem::splitPath(m_strLogPath, strParentPath, strFileName);
	FileSystem::makeDirRecursive(strParentPath.c_str());

	// 同一天 由于文件过大切换日志文件 原文件名加上时间是新文件名
	if ( m_File.isOpened() && bOnTheSameDay && bFileSizeExceed )
	{
		m_File.close();
		int nHour = 0, nMinute = 0, nSecond = 0;
		dt.getTime(nHour, nMinute, nSecond);
		stdstring strCurrentFullPath = m_strLogPath + '.' + m_dtLastLogTime.formatDate();
		stdstring strRenameFullPath = strCurrentFullPath + '.';
		StringFunc::addIntToString(strRenameFullPath, nHour, 10, true, 2, '0');
		StringFunc::addIntToString(strRenameFullPath, nMinute, 10, true, 2, '0');
		StringFunc::addIntToString(strRenameFullPath, nSecond, 10, true, 2, '0');

		// 如果切换的新文件存在,在文件后缀加上毫秒微秒信息区分文件名
		if ( FileSystem::pathExist(strRenameFullPath.c_str()) )
		{
			DateTime dtNow;
			dtNow.getPresentTime();
			strRenameFullPath += '.';
			StringFunc::addIntToString(strRenameFullPath, dtNow.getFractionMilli(), 10, true, 3, '0');
			StringFunc::addIntToString(strRenameFullPath, dtNow.getFractionMicro(), 10, true, 3, '0');
		}
		FileSystem::renameFile(strCurrentFullPath.c_str(), strRenameFullPath.c_str());
	} // if ( m_File.isOpened() && bOnTheSameDay && bFileSizeExceed )

	m_nFileSize = 0;
	m_dtLastLogTime = dt;
	stdstring strNewFileName = strFileName + '.' + m_dtLastLogTime.formatDate();
	stdstring strFullPath = strParentPath + '/' + strNewFileName;
	m_File.open(strFullPath.c_str(), O_WRONLY | O_CREAT | O_APPEND);
	FileSystem::makeSymbolLink(strNewFileName.c_str(), m_strLogPath.c_str());
	if ( m_File.isOpened() )
		m_nFileSize = static_cast<size_t>( m_File.seekp(0, seek_end) );
	if ( !bOnTheSameDay )
		deleteExpiredFile();
}

void FileLog::deleteExpiredFile(void) const
{
	if ( m_nFileSaveDays <= 0 )
		return;

	stdstring strParentPath, strFileName;
	FileSystem::splitPath(m_strLogPath, strParentPath, strFileName);
	DateTime dtExpiredDate = m_dtLastLogTime;
	dtExpiredDate.stepDateTime(0, 0, -1 * m_nFileSaveDays);
	stdstring strExpiredFile = strFileName + '.' + dtExpiredDate.formatDate();
	typeFilePathList listFileName;
	FileSystem::getFileNameList(strParentPath, false, listFileName, strFileName);
	for ( typeFilePathList::const_iterator it = listFileName.begin(); it != listFileName.end(); ++it )
	{
		const stdstring & strExistFile = (*it);
		if ( strExistFile < strExpiredFile && strExistFile != strFileName )
		{
			stdstring strFullPath = strParentPath + '/' + strExistFile;
			FileSystem::unlinkFile(strFullPath.c_str());
			std::cout << FormatString("[%] Delete expired log file:%, save days:%, expired file:%")
				.arg(m_dtLastLogTime.formatDateTime()).arg(strFullPath).arg(m_nFileSaveDays).arg(strExpiredFile).str() << std::endl;
		}
	} // for ( typeFilePathList::const_iterator it = listFileName.begin(); it != listFileName.end(); ++it )
}

void FileLog::writeLog(const stdstring & strLog)
{
	DateTime dt(time(NULL));
	stdstring strData = FormatString("[%] %").arg(dt.formatTime()).arg(strLog).str();
	AutoMutexLock auLock(m_lockFile);
	openFile(dt, strData.size());
	if ( m_File.isOpened() )
	{
		m_nFileSize += strData.size();
		m_File.write(strData.data(), strData.size());
	}
}

void FileLog::writeLogLine(const stdstring & strLine)
{
	DateTime dt(time(NULL));
	stdstring strData = FormatString("[%] %").arg(dt.formatTime()).arg(strLine).str();
	GlobalFunc::replace(strData, "\r", "\\r");
	GlobalFunc::replace(strData, "\n", "\\n");
	strData += StringFunc::g_strCRLF;
	AutoMutexLock auLock(m_lockFile);
	openFile(dt, strData.size());
	if ( m_File.isOpened() )
	{
		m_nFileSize += strData.size();
		m_File.write(strData.data(), strData.size());
	}
}

void FileLog::writeLogWithTID(const stdstring & strLog)
{
	DateTime dt(time(NULL));
	stdstring strData = FormatString("T:%[%] %").arg(ThreadFunc::getSelfThreadID()).arg(dt.formatTime()).arg(strLog).str();
	AutoMutexLock auLock(m_lockFile);
	openFile(dt, strData.size());
	if ( m_File.isOpened() )
	{
		m_nFileSize += strData.size();
		m_File.write(strData.data(), strData.size());
	}
}

void FileLog::writeLogLineWithTID(const stdstring & strLine)
{
	DateTime dt(time(NULL));
	stdstring strData = FormatString("T:%[%] %").arg(ThreadFunc::getSelfThreadID()).arg(dt.formatTime()).arg(strLine).str();
	GlobalFunc::replace(strData, "\r", "\\r");
	GlobalFunc::replace(strData, "\n", "\\n");
	strData += StringFunc::g_strCRLF;
	AutoMutexLock auLock(m_lockFile);
	openFile(dt, strData.size());
	if ( m_File.isOpened() )
	{
		m_nFileSize += strData.size();
		m_File.write(strData.data(), strData.size());
	}
}

RFC_NAMESPACE_END
