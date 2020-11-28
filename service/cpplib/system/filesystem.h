/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_FILESYSTEM_H_201008
#define RFC_FILESYSTEM_H_201008

#include "base/exception.h"
#include "base/globalfunc.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <list>

RFC_NAMESPACE_BEGIN

enum
{
	DEFAULT_OPEN_FILE_MODE = ( S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH ),	// 00664
	DEFAULT_MAKE_DIR_MODE = ( S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ),				// 00775
};

DECLARE_EXCEPTION(FileSystemException, Exception)

class FileAttr
{
public:
	enum
	{
		STAT_TYPE_FILE						= 1,
		STAT_TYPE_DIR						= (1 << 1),
		STAT_TYPE_LINK						= (1 << 2),
		STAT_TYPE_SYS_DIR					= (1 << 3),			//"." or ".."

		FILE_MODE_READ						= (1 << 4),
		FILE_MODE_WRITE						= (1 << 5),
		FILE_MODE_EXEC						= (1 << 6),
	};
	typedef			BitFlags<int>			RFileFlags;

	FileAttr(void) : m_nFileFlags(0), m_nSize(0), m_tCreate(0), m_tModify(0), m_tRead(0) {}
	bool			statFile(int nFileHandle, const stdstring& strFullPath);
	bool			statFile(const stdstring& strFullPath);

	const RFileFlags & getFileFlags(void) const { return m_nFileFlags; }
	bool			isFile(void) const { return m_nFileFlags.check(STAT_TYPE_FILE); }
	bool			isDir(void) const { return m_nFileFlags.check(STAT_TYPE_DIR); }
	bool			isSystemDir(void) const { return m_nFileFlags.check(STAT_TYPE_SYS_DIR); }
	bool			isLink(void) const { return m_nFileFlags.check(STAT_TYPE_LINK); }

	bool			checkReadPermission(void) const { return m_nFileFlags.check(FILE_MODE_READ); }
	bool			checkWritePermission(void) const { return m_nFileFlags.check(FILE_MODE_WRITE); }
	bool			checkExecPermission(void) const { return m_nFileFlags.check(FILE_MODE_EXEC); }
	size_t			getSize(void) const { return m_nSize; }

	time_t			getCreateTime(void) const { return m_tCreate; }
	time_t			getModifyTime(void) const { return m_tModify; }
	// return the last access time
	time_t			getReadTime(void) const { return m_tRead; }

	const stdstring & getFileName(void) const { return m_strFileName; }
	const stdstring & getFullPath(void) const { return m_strFullPath; }

protected:
	void			clear(void);
	bool			statFile(const struct stat& info, const stdstring& strFullPath);

private:
	RFileFlags						m_nFileFlags;
	size_t							m_nSize;
	time_t							m_tCreate, m_tModify, m_tRead;
	stdstring						m_strFileName, m_strFullPath;
};

typedef std::list<stdstring> typeFilePathList;
class FileSystem
{
public:	
	static int			unlinkFile(const char * lpszFileName);

	static int			removeDir(const char * lpszPath);

	static int			removeDirRecursive(const char * lpszPath);

	static int			renameFile(const char * lpszOldName, const char * lpszNewName);

	static int			makeDirRecursive(const char * lpszPath, int nOwnerID = -1, int nMode = DEFAULT_MAKE_DIR_MODE);

	static bool			pathExist(const char * lpszPath);

	// check errno when return false
	static bool			makeSymbolLink(const char * lpszTargetPath, const char * lpszLinkName, int nOwnerID = -1);

	enum FileAccessMode
	{
		FILE_ACCESS_MODE_R = 0x01,
		FILE_ACCESS_MODE_W = 0x02,
		FILE_ACCESS_MODE_RW = (FILE_ACCESS_MODE_R | FILE_ACCESS_MODE_W),
		FILE_ACCESS_MODE_X = 0x04,
		FILE_ACCESS_MODE_RX = (FILE_ACCESS_MODE_R | FILE_ACCESS_MODE_X),
		FILE_ACCESS_MODE_WX = (FILE_ACCESS_MODE_W | FILE_ACCESS_MODE_X),
		FILE_ACCESS_MODE_RWX = (FILE_ACCESS_MODE_R | FILE_ACCESS_MODE_W | FILE_ACCESS_MODE_X),
		FILE_ACCESS_MODE_F = 0x08, //Existence
	};
	static bool			accessPath(const stdstring& strPath, FileAccessMode nAccessMode);

	static stdstring	getCurrentDirectory(void) throw(FileSystemException);
	static void			changeCurrentDirectory(const stdstring & strPath) throw(FileSystemException);

	static int			changeOwner(const char * lpszFileName, int nOwnerID, int nGroupID);

	static int			changeOwner(int nFileHandle, int nOwnerID, int nGroupID);

	static int			changeMode(const char * lpszFileName, int nMode);

	static int			changeMode(int nFileHandle, int nMode);

	static int			sync(int nFileHandle);

	// make the path to normal, remove the . and .. in the path
	static stdstring &	fixPath(const stdstring & strSrcPath, stdstring & strPath);
	static stdstring &	fixPath(stdstring & strPath) { return fixPath(strPath, strPath); }

	static void			splitPath(const stdstring & strPath, std::list<stdstring> & listPart);
	static stdstring &	joinPath(const std::list<stdstring> & listPart, stdstring & strPath);
	static void			splitPath(const stdstring & strPath, stdstring & strParentPath, stdstring & strFileName);
	static void			splitFile(const stdstring & strFile, stdstring & strFileName, stdstring & strFileExt);
	static stdstring &	toPathName(const stdstring & strSrc, stdstring & strPathName);
	static stdstring &	joinToPath(const stdstring & strParentPath, const stdstring & strSubPath, stdstring & strFullPath);
	static stdstring &	joinToFile(const stdstring & strParentPath, const stdstring & strFileName, stdstring & strFullPath);

	static bool			getDiskFreeSpace(const stdstring & strPath, rfc_uint_64 & llTotal, rfc_uint_64 & llFree, rfc_uint_64 & llFreeForNonRoot);

	static bool			loadFileContent(const stdstring & strFullPath, stdstring & strContent);
	static bool			saveFileContent(const stdstring & strFullPath, const stdstring & strContent);
	static bool			appendFileContent(const stdstring & strFullPath, const stdstring & strContent);

	class ScanPathAction
	{
	public:
		virtual ~ScanPathAction() {}
		virtual int onAction(const stdstring& strFullPath, const stdstring& strFileName, bool bDir) = 0; // { return 0; }
	};

	class RemovePathAction : public ScanPathAction
	{
	public:
		virtual int onAction(const stdstring& strFullPath, const stdstring& strFileName, bool bDir);
	};

	class GetPathAction : public ScanPathAction
	{
	public:
		GetPathAction(bool bNeedDirName, bool bNeedFileName, bool bGetFullPath, typeFilePathList& listFilePath)
			: m_bNeedDirName(bNeedDirName), m_bNeedFileName(bNeedFileName), m_bGetFullPath(bGetFullPath), m_listFilePath(listFilePath) {}

		virtual int onAction(const stdstring& strFullPath, const stdstring& strFileName, bool bDir);

	private:
		bool m_bNeedDirName, m_bNeedFileName, m_bGetFullPath;
		typeFilePathList& m_listFilePath;
	};

	static int onScanPathRecursive(const stdstring& strBaseDir, int nRecursiveLevel, ScanPathAction& scanPathVisitor,
		const stdstring& strPrefix = StringFunc::g_strEmptyString, const stdstring& strSkipPrefix = StringFunc::g_strEmptyString,
		const stdstring& strPostfix = StringFunc::g_strEmptyString, const stdstring& strSkipPostfix = StringFunc::g_strEmptyString);

	static int onScanBaseDirOnly(const stdstring& strBaseDir, ScanPathAction& scanPathVisitor,
		const stdstring& strPrefix = StringFunc::g_strEmptyString, const stdstring& strSkipPrefix = StringFunc::g_strEmptyString,
		const stdstring& strPostfix = StringFunc::g_strEmptyString, const stdstring& strSkipPostfix = StringFunc::g_strEmptyString)
	{
		return onScanPathRecursive(strBaseDir, 0, scanPathVisitor, strPrefix, strSkipPrefix, strPostfix, strSkipPostfix);
	}

	//Only get File Rename, not full path
	static int getFileNameList(const stdstring& strBaseDir, bool bNeedDir, typeFilePathList& listFileName,
		const stdstring& strPrefix = StringFunc::g_strEmptyString, const stdstring& strSkipPrefix = StringFunc::g_strEmptyString,
		const stdstring& strPostfix = StringFunc::g_strEmptyString, const stdstring& strSkipPostfix = StringFunc::g_strEmptyString);

	static int getFilePathRecursive(const stdstring& strBaseDir, int nRecursiveLevel,
		bool bNeedDir, bool bGetFullPath, typeFilePathList& listFilePath,
		const stdstring& strPrefix = StringFunc::g_strEmptyString, const stdstring& strSkipPrefix = StringFunc::g_strEmptyString,
		const stdstring& strPostfix = StringFunc::g_strEmptyString, const stdstring& strSkipPostfix = StringFunc::g_strEmptyString);

	static int removePathRecursive(const stdstring& strBaseDir, int nRecursiveLevel, bool bRemoveBaseDir);
};

RFC_NAMESPACE_END

#endif	//RFC_FILESYSTEM_H_201008

