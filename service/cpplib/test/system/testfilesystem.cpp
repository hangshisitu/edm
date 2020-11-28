/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "system/filesystem.h"

RFC_NAMESPACE_BEGIN

static void onTestFixPath(const stdstring & strSrcPath, const stdstring & strPath)
{
	stdstring strSrc = strSrcPath;
	assertEqual( FileSystem::fixPath(strSrc), strPath );
}

onUnitTest(FileSystem)
{
	onTestFixPath("/tmp/file1","/tmp/file1");
	onTestFixPath("/tmp/dir1/","/tmp/dir1/");
	onTestFixPath("/tmp/./file1","/tmp/file1");
	onTestFixPath("/tmp/./dir1/","/tmp/dir1/");
	onTestFixPath("/tmp/././file1","/tmp/file1");
	onTestFixPath("/tmp/././dir1/","/tmp/dir1/");
	onTestFixPath("/tmp/./././////./file1","/tmp/file1");
	onTestFixPath("/tmp/./././////.//././dir1/","/tmp/dir1/");
	onTestFixPath("/tmp/dir1/../file2","/tmp/file2");
	onTestFixPath("/tmp/dir1/../dir2/","/tmp/dir2/");
	onTestFixPath("/tmp/dir1/dir11/../../file3","/tmp/file3");
	onTestFixPath("/tmp/dir1/dir11/../../dir3/","/tmp/dir3/");
	onTestFixPath("../dir1/dir11/../../file3","../file3");
	onTestFixPath("../dir1/dir11/../../dir3/","../dir3/");
	return;

	typeFilePathList listFilePath;
	FileSystem::getFilePathRecursive("/home/richmail/conf/", 0, false, true, listFilePath, "", "", ".conf", "");
	std::cout << "get .conf file from /home/richmail/conf/ count:" << listFilePath.size() << std::endl;
	typeFilePathList::const_iterator it, itEnd = listFilePath.end();
	for ( it = listFilePath.begin(); it != itEnd; ++it )
	{
		std::cout << *it << std::endl;
	}

	stdstring strFullPath = "/home/wengshanjin/testfilesystem.txt";
	stdstring strContent = "Content-Type: multipart/alternative; boundary=\"===============0122511605==\"\r\n"
		"MIME-Version: 1.0\r\n"
		"From: \"=?gb18030?b?zfjS19PKvP7W0NDE?=\" <mail@service.netease.com>\r\n"
		"To: \"=?gb18030?b?zfjS19PKz+TTw7un?=\" <user@netease.com>\r\n"
		"Date: Fri, 29 Oct 2010 16:07:53 +0800\r\n"
		"Subject: =?gb18030?b?1fmw1Mj8vNPI69DCzea3qKOsss6807y0y80yMNSquuyw/KOh?=\r\n";

	FileSystem::unlinkFile( strFullPath.c_str() );
	assertTest( FileSystem::saveFileContent(strFullPath, strContent) );
	assertTest( FileSystem::pathExist(strFullPath.c_str()) );

	bool bMakeLinkOK = FileSystem::makeSymbolLink(strFullPath.c_str(), "/tmp/wengshanjin/testfilesystem.txt.link");
	assertTest( bMakeLinkOK );
	if ( !bMakeLinkOK )
		std::cout << GlobalFunc::getSystemErrorInfo() << ", errno:" << errno << std::endl;
	assertTest(FileSystem::pathExist("/tmp/wengshanjin/testfilesystem.txt.link"));

	stdstring strLoadContent;
	assertTest( FileSystem::loadFileContent(strFullPath, strLoadContent) );
	assertEqual( strLoadContent, strContent );
	assertEqual( FileSystem::unlinkFile(strFullPath.c_str()), 0 );
	assertTest( !FileSystem::pathExist(strFullPath.c_str()) );

	FileSystem::removeDirRecursive("/tmp/wengshanjin/1Mj8vNPI69DCzea3qKOsss680");
	assertTest(!FileSystem::pathExist("/tmp/wengshanjin/1Mj8vNPI69DCzea3qKOsss680"));
	FileSystem::makeDirRecursive("/tmp/wengshanjin/1Mj8vNPI69DCzea3qKOsss680/0y80yMNSquuy/zfjS19PKvP7W0NDE");
	assertTest(FileSystem::pathExist("/tmp/wengshanjin/1Mj8vNPI69DCzea3qKOsss680/0y80yMNSquuy"));
	assertTest(FileSystem::pathExist("/tmp/wengshanjin/1Mj8vNPI69DCzea3qKOsss680/0y80yMNSquuy/zfjS19PKvP7W0NDE"));

	stdstring strCurrentDir1 = FileSystem::getCurrentDirectory();
	FileSystem::changeCurrentDirectory("/tmp/wengshanjin/1Mj8vNPI69DCzea3qKOsss680/0y80yMNSquuy");
	stdstring strCurrentDir2 = FileSystem::getCurrentDirectory();
	assertEqual( strCurrentDir2, stdstring("/tmp/wengshanjin/1Mj8vNPI69DCzea3qKOsss680/0y80yMNSquuy") );

	FileSystem::changeCurrentDirectory(strCurrentDir1);
	assertEqual( FileSystem::getCurrentDirectory(), strCurrentDir1 );
}

RFC_NAMESPACE_END
