/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "system/scandir.h"
#include "system/filesystem.h"

RFC_NAMESPACE_BEGIN

DirScaner::~DirScaner(void)
{
	closeHandle();
	if ( m_pEntry != NULL )
	{
		rfc_uint_8 * tp = reinterpret_cast<rfc_uint_8 *>(m_pEntry);
		delete []tp;
	}
}

bool DirScaner::scanDir(const stdstring & strDirPath)
{
	closeHandle();

	m_nHandle = opendir(strDirPath.c_str());
	if ( m_nHandle != NULL && m_pEntry == NULL )
		m_pEntry = reinterpret_cast<struct dirent *>(new rfc_uint_8[sizeof(struct dirent) + pathconf(strDirPath.c_str(),_PC_NAME_MAX) +1]);
	m_bHasOpenDir = ( m_nHandle != NULL && m_pEntry != NULL );
	m_strDirPath = strDirPath;
	return m_bHasOpenDir;
}

void DirScaner::closeHandle(void)
{
	if ( m_nHandle != NULL )
		closedir(m_nHandle);
	m_nHandle = NULL;
	m_bHasOpenDir = false;
}

bool DirScaner::fetchNext(FileAttr & fileAttr)
{
	if ( !m_bHasOpenDir )
		scanDir(m_strDirPath);
	if ( !m_bHasOpenDir )
		return false;

	struct dirent * dp = NULL;
	readdir_r(m_nHandle, m_pEntry, &dp);
	if ( dp == NULL )
		return false;
	
	stdstring strFullPath;
	FileSystem::toPathName(m_strDirPath, strFullPath);
	strFullPath += dp->d_name;
	return fileAttr.statFile(strFullPath);
}

RFC_NAMESPACE_END

