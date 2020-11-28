/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "system/filesystem.h"
#include "system/filehandle.h"
#include "system/scandir.h"
#include "base/autoarray.h"
#include "base/stringtokenizer.h"
#include "base/formatstream.h"

#include <sys/statvfs.h>
#include <unistd.h>
#include <sys/sendfile.h>

RFC_NAMESPACE_BEGIN

bool FileAttr::statFile(int nFileHandle, const stdstring& strFullPath)
{
	struct stat info;
	if ( nFileHandle == INVALID_HANDLE || fstat(nFileHandle, &info) != 0 )
		return statFile(strFullPath);
	return statFile(info, strFullPath);
}

bool FileAttr::statFile(const stdstring& strFullPath)
{
	struct stat info;
	if ( stat(strFullPath.c_str(), &info) != 0 )
		return false;
	return statFile(info, strFullPath);
}

bool FileAttr::statFile(const struct stat& info, const stdstring& strFullPath)
{
	stdstring strParentPath;
	FileSystem::splitPath(strFullPath, strParentPath, m_strFileName);
	m_strFullPath = strFullPath;

	m_nFileFlags.clearAll();
	m_nFileFlags.setFlagToggle(info.st_mode & S_IFREG, STAT_TYPE_FILE);
	m_nFileFlags.setFlagToggle(info.st_mode & S_IFDIR, STAT_TYPE_DIR);
	m_nFileFlags.setFlagToggle(info.st_mode & S_IFLNK, STAT_TYPE_LINK);
	m_nFileFlags.setFlagToggle(m_strFileName == "." || m_strFileName == "..", STAT_TYPE_SYS_DIR);

	m_nFileFlags.setFlagToggle(info.st_mode & S_IRUSR, FILE_MODE_READ);
	m_nFileFlags.setFlagToggle(info.st_mode & S_IWUSR, FILE_MODE_WRITE);
	m_nFileFlags.setFlagToggle((info.st_mode & S_IXUSR) && (info.st_mode & S_IXGRP) && (info.st_mode & S_IXOTH), FILE_MODE_EXEC);

	m_nSize = info.st_size; 
	m_tCreate = info.st_ctime; 
	m_tModify = info.st_mtime; 
	m_tRead = info.st_atime;
	return true;
}

//////////////////////////////////////////////////////////////////////////

int FileSystem::unlinkFile(const char * lpszFileName)
{
	return unlink(lpszFileName);
}

int FileSystem::removeDir(const char * lpszPath)
{
	return rmdir(lpszPath);
}

int FileSystem::removeDirRecursive(const char * lpszPath)
{
	return removePathRecursive(lpszPath, 256, true);
}

int FileSystem::renameFile(const char * lpszOldName, const char * lpszNewName)
{
	return rename(lpszOldName, lpszNewName);
}

int FileSystem::makeDirRecursive(const char * lpszPath, int nOwnerID, int nMode)
{
	if ( pathExist(lpszPath) )
		return 0;
	stdstring strPath(lpszPath);
	if ( strPath.empty() || strPath.size() > RFC_MAX_PATH_LEN )
		return -1;
	if ( *strPath.rbegin() != RFC_CHAR_PATH_SLASH )
		strPath += RFC_CHAR_PATH_SLASH;

	int nRetCode = 0;
	size_t nSlashPos = strPath.find(RFC_CHAR_PATH_SLASH, 1);
	for ( ; nSlashPos != stdstring::npos && nRetCode == 0; nSlashPos = strPath.find(RFC_CHAR_PATH_SLASH, nSlashPos + 1) )
	{
		strPath[nSlashPos] = RFC_CHAR_NULL;
		if ( !pathExist(strPath.c_str()) )
			nRetCode = mkdir(strPath.c_str(), nMode);
		if ( nRetCode == 0 && nOwnerID != -1 )
			nRetCode = lchown(strPath.c_str(), nOwnerID, static_cast<gid_t>(-1));
		strPath[nSlashPos] = RFC_CHAR_PATH_SLASH;
	} // for
	return nRetCode;
}

bool FileSystem::pathExist(const char * lpszPath)
{
	return ( access(lpszPath, F_OK) ==0 );
}

bool FileSystem::makeSymbolLink(const char * lpszTargetPath, const char * lpszLinkName, int nOwnerID)
{
	struct stat statInfo;
	if ( lstat(lpszLinkName, &statInfo) != 0 || (statInfo.st_mode & S_IFLNK) != 0 )
		unlinkFile(lpszLinkName);

	if ( ::symlink(lpszTargetPath, lpszLinkName) != 0 )
		return false;

	if ( (nOwnerID != -1) && changeOwner(lpszLinkName, nOwnerID, -1) != 0 )
		return false;

	return true;
}

bool FileSystem::accessPath(const stdstring& strPath, FileAccessMode nAccessMode)
{
	BitFlags<int>	nAccessModeFlags;
	nAccessModeFlags.setFlagToggle(nAccessMode & FILE_ACCESS_MODE_R, R_OK);
	nAccessModeFlags.setFlagToggle(nAccessMode & FILE_ACCESS_MODE_W, W_OK);
	nAccessModeFlags.setFlagToggle(nAccessMode & FILE_ACCESS_MODE_X, X_OK);
	nAccessModeFlags.setFlagToggle(nAccessMode & FILE_ACCESS_MODE_F, F_OK);
	return ( access(strPath.c_str(), nAccessModeFlags.getFlags()) == 0 ); 
}

stdstring FileSystem::getCurrentDirectory(void) throw(FileSystemException)
{
	AutoArray<char> auPath( new char[RFC_MAX_PATH_LEN] );
	memset(auPath.get(), 0, RFC_MAX_PATH_LEN);
	if ( getcwd(auPath.get(), RFC_MAX_PATH_LEN) == NULL )
		throw FileSystemException( stdstring("Get Current Directory Error:%") + GlobalFunc::getSystemErrorInfo() );
	return auPath.get();
}

void FileSystem::changeCurrentDirectory(const stdstring & strPath) throw(FileSystemException)
{
	if ( chdir(strPath.c_str()) != 0 )
		throw FileSystemException( FormatString("Change Current Directory to % Error:%").arg(strPath).arg(GlobalFunc::getSystemErrorInfo()).str() );
}

int FileSystem::changeOwner(const char * lpszFileName, int nOwnerID, int nGroupID)
{
	return lchown(lpszFileName, nOwnerID, nGroupID);
}

int FileSystem::changeOwner(int nFileHandle, int nOwnerID, int nGroupID)
{
	return fchown(nFileHandle, nOwnerID, nGroupID);
}

int FileSystem::changeMode(const char * lpszFileName, int nMode)
{
	return int( chmod( lpszFileName, nMode ) );
}

int FileSystem::changeMode(int nFileHandle, int nMode)
{
	return fchmod( nFileHandle, nMode );
}

int FileSystem::sync(int nFileHandle)
{
	return fsync( nFileHandle );
}

stdstring &	FileSystem::fixPath(const stdstring & strSrcPath, stdstring & strPath)
{
	std::list<stdstring> listPart;
	splitPath(strSrcPath, listPart);
	return joinPath(listPart, strPath);
}

void FileSystem::splitPath(const stdstring & strPath, std::list<stdstring> & listPart)
{
	listPart.clear();
	StringTokenizer<stdstring> tokenPath(strPath.data(), strPath.size());
	stdstring strPart;
	while ( tokenPath.getNext(strPart, "/\\") )
	{
		if ( strPart == "." )
			continue;

		if ( strPart == ".." && listPart.size() >= 2 )
		{
			listPart.pop_back();
			listPart.pop_back();
		}
		else
		{
			listPart.push_back(strPart);
			listPart.push_back(stdstring(1, RFC_CHAR_PATH_SLASH));
		}
	} // while ( tokenPath.getNext(strPart, "/\\") )

	if ( !strPath.empty() && strPath[0] == RFC_CHAR_PATH_SLASH )	// it is a absolute path
		listPart.push_front(stdstring(1, RFC_CHAR_PATH_SLASH));
	if ( !strPath.empty() && *strPath.rbegin() != RFC_CHAR_PATH_SLASH )	// it is a file
		listPart.pop_back();
}

stdstring &	FileSystem::joinPath(const std::list<stdstring> & listPart, stdstring & strPath)
{
	strPath.clear();
	for ( std::list<stdstring>::const_iterator it = listPart.begin(); it != listPart.end(); ++it )
		strPath += *it;
	return strPath;
}

void FileSystem::splitPath(const stdstring & strFullPath, stdstring & strParentPath, stdstring & strFileName)
{
	size_t nLastSlashPos = strFullPath.rfind(RFC_CHAR_PATH_SLASH);
	if ( nLastSlashPos == stdstring::npos )
	{
		strParentPath.clear();
		strFileName = strFullPath;
	}
	else
	{
		strParentPath = strFullPath.substr(0, nLastSlashPos);
		strFileName = strFullPath.substr(nLastSlashPos + 1);
	}
}

void FileSystem::splitFile(const stdstring & strFile, stdstring & strFileName, stdstring & strFileExt)
{
	size_t nLastPointPos = strFile.rfind('.');
	if ( nLastPointPos == stdstring::npos )
	{
		strFileName = strFile;
		strFileExt.clear();
	}
	else
	{
		strFileName = strFile.substr(0, nLastPointPos);
		strFileExt = strFile.substr(nLastPointPos + 1);

	}
}

stdstring & FileSystem::toPathName(const stdstring & strSrc, stdstring & strPathName)
{
	strPathName = strSrc;
	if ( !strPathName.empty() && *strPathName.rbegin() != RFC_CHAR_PATH_SLASH )
		strPathName += RFC_CHAR_PATH_SLASH;
	return strPathName;
}

stdstring & FileSystem::joinToPath(const stdstring & strParentPath, const stdstring & strSubPath, stdstring & strFullPath)
{
	strFullPath = strParentPath;
	if ( !strFullPath.empty() && *strFullPath.rbegin() != RFC_CHAR_PATH_SLASH && !strSubPath.empty() && *strSubPath.begin() != RFC_CHAR_PATH_SLASH )
		strFullPath += RFC_CHAR_PATH_SLASH;
	strFullPath += strSubPath;
	toPathName(strFullPath, strFullPath);
	return strFullPath;
}

stdstring & FileSystem::joinToFile(const stdstring & strParentPath, const stdstring & strFileName, stdstring & strFullPath)
{
	toPathName(strParentPath, strFullPath);
	strFullPath += strFileName;
	return strFullPath;
}

bool FileSystem::getDiskFreeSpace(const stdstring & strPath, rfc_uint_64 & llTotal, rfc_uint_64 & llFree, rfc_uint_64 & llFreeForNonRoot)
{
	struct statvfs statFileSystem;
	if ( statvfs( strPath.c_str(), &statFileSystem) != 0 )
		return false;

	llTotal = static_cast<rfc_uint_64>(statFileSystem.f_bsize) * statFileSystem.f_blocks;
	llFree = static_cast<rfc_uint_64>(statFileSystem.f_bsize) * statFileSystem.f_bfree;
	llFreeForNonRoot = static_cast<rfc_uint_64>(statFileSystem.f_bsize) * statFileSystem.f_bavail;
	return true;
}

bool FileSystem::loadFileContent(const stdstring & strFullPath, stdstring & strContent)
{
	FileHandle fHandle;
	if ( fHandle.open(strFullPath.data(), O_RDONLY) < 0 )
		return false;

	AutoArray<char> auCache(new char[StreamBase::g_nBufferDefaultSize]);
	char * pBuffer = auCache.get();
	while( true )
	{
		size_t nReadCount = fHandle.read(pBuffer, StreamBase::g_nBufferDefaultSize);
		if ( StreamBase::resultIsEndOfStream(nReadCount) )
			break;
		else if ( StreamBase::resultIsError(nReadCount) )
			return false;
		strContent.append(pBuffer, nReadCount);
	} // while( true )
	return true;
}

bool FileSystem::saveFileContent(const stdstring & strFullPath, const stdstring & strContent)
{
	FileHandle fHandle;
	if ( fHandle.open(strFullPath.data(), O_WRONLY | O_CREAT | O_TRUNC) < 0 )
		return false;
	return ( fHandle.writeBlock(strContent.data(), strContent.size()) == strContent.size() );
}

bool FileSystem::appendFileContent(const stdstring & strFullPath, const stdstring & strContent)
{
	FileHandle fHandle;
	if ( fHandle.open(strFullPath.data(), O_WRONLY | O_CREAT | O_APPEND) < 0 )
		return false;
	return ( fHandle.writeBlock(strContent.data(), strContent.size()) == strContent.size() );
}

int FileSystem::RemovePathAction::onAction(const stdstring& strFullPath, const stdstring& strFileName, bool bDir)
{
	if ( bDir )
		return FileSystem::removeDir(strFullPath.c_str());
	else
		return FileSystem::unlinkFile( strFullPath.c_str() );
}

int FileSystem::GetPathAction::onAction(const stdstring& strFullPath, const stdstring& strFileName, bool bDir)
{
	if ( (bDir && !m_bNeedDirName) || (!bDir && !m_bNeedFileName) )
		return 0;	//return 0 to continue

	if ( m_bGetFullPath )
		m_listFilePath.push_back(strFullPath);
	else if ( !strFileName.empty() )
		m_listFilePath.push_back(strFileName);
	return 0;
}

int FileSystem::onScanPathRecursive(const stdstring& strBaseDir, int nRecursiveLevel, FileSystem::ScanPathAction& scanPathVisitor,
			const stdstring& strPrefix, const stdstring& strSkipPrefix, const stdstring& strPostfix, const stdstring& strSkipPostfix)
{
	if ( nRecursiveLevel < 0 )
		return 0;	//return normal to go on other path
	if ( !FileSystem::pathExist(strBaseDir.c_str()) )
		return -1;

	FileAttr fileAttr;
	DirScaner dirScaner;
	if ( !dirScaner.scanDir(strBaseDir) )
		return -1;

	int nRetCode = 0;
	while ( dirScaner.fetchNext(fileAttr) )
	{
		if ( fileAttr.isSystemDir() )
			continue;

		const stdstring & strFileName = fileAttr.getFileName();
		const stdstring & strFullPath = fileAttr.getFullPath();
		bool bMatchFilter = true;
		if ( (!strPrefix.empty() && !StringFunc::checkPrefix(strFileName, strPrefix))
			|| (!strSkipPrefix.empty() && StringFunc::checkPrefix(strFileName, strSkipPrefix))
			|| (!strPostfix.empty() && !StringFunc::checkPostfix(strFileName, strPostfix))
			|| (!strSkipPostfix.empty() && StringFunc::checkPostfix(strFileName, strSkipPostfix)) )
			bMatchFilter = false;

		if ( fileAttr.isDir() )
		{
			nRetCode = onScanPathRecursive(strFullPath, nRecursiveLevel - 1, scanPathVisitor,
						strPrefix, strSkipPrefix, strPostfix, strSkipPostfix);
			if ( nRetCode == 0 && bMatchFilter )
				nRetCode = scanPathVisitor.onAction(strFullPath, strFileName, true);
		}
		else if ( bMatchFilter )
		{
			nRetCode = scanPathVisitor.onAction(strFullPath, strFileName, false);
		}
		if ( nRetCode != 0 )
			return nRetCode;
	}//while ( dirScaner.fetch(fileAttr) )

	return nRetCode;
}

int FileSystem::getFileNameList(const stdstring& strBaseDir, bool bNeedDir, typeFilePathList& listFileName,
	const stdstring& strPrefix, const stdstring& strSkipPrefix, const stdstring& strPostfix, const stdstring& strSkipPostfix)
{
	bool bGetFullPath = false;
	int nRetCode = getFilePathRecursive(strBaseDir, 0, bNeedDir, bGetFullPath, listFileName,
		strPrefix, strSkipPrefix, strPostfix, strSkipPostfix);
	listFileName.sort();
	return nRetCode;
}

int FileSystem::getFilePathRecursive(const stdstring& strBaseDir, int nRecursiveLevel,
			bool bNeedDir, bool bGetFullPath, typeFilePathList& listFilePath,
			const stdstring& strPrefix, const stdstring& strSkipPrefix, const stdstring& strPostfix, const stdstring& strSkipPostfix)
{
	listFilePath.clear();
	GetPathAction getPathAction(bNeedDir, !bNeedDir, bGetFullPath, listFilePath);
	int nRetCode = onScanPathRecursive(strBaseDir, nRecursiveLevel, getPathAction, strPrefix, strSkipPrefix, strPostfix, strSkipPostfix);
	return nRetCode;
}

int FileSystem::removePathRecursive(const stdstring& strBaseDir, int nRecursiveLevel, bool bRemoveBaseDir)
{
	RemovePathAction removeAction;
	int nRetCode = onScanPathRecursive(strBaseDir, nRecursiveLevel, removeAction);
	if ( nRetCode == 0 && bRemoveBaseDir )
		return FileSystem::removeDir(strBaseDir.c_str());
	return nRetCode;
}

RFC_NAMESPACE_END
