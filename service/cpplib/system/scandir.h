/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_SCANDIR_H_201008
#define RFC_SCANDIR_H_201008

#include "base/exception.h"
#include "base/globalfunc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

RFC_NAMESPACE_BEGIN

DECLARE_EXCEPTION(ScanDirException, Exception);

class FileAttr;

class DirScaner : private NonCopyAble
{
public:
	DirScaner(const stdstring & strDirPath = StringFunc::g_strEmptyString) : m_pEntry(NULL), m_nHandle(NULL),
		m_bHasOpenDir(false), m_strDirPath(strDirPath) {}
	~DirScaner(void);

	bool			scanDir(const stdstring & strDirPath);
	void			closeHandle(void);
	bool			fetchNext(FileAttr & fileAttr);
	

protected:
	struct dirent *		m_pEntry;
	DIR *				m_nHandle;
	bool				m_bHasOpenDir;
	stdstring			m_strDirPath;
};

RFC_NAMESPACE_END

#endif	//RFC_SCANDIR_H_201008

