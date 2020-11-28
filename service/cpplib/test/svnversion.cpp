/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2012-02      */

#include <string>
#include <iostream>

using std::string;

static const char g_pRevision[] __attribute__((used)) = { "$Id: svnversion.cpp 922 2012-02-02 13:23:30Z wengshanjin $" };

void getVersion(void)
{
	string strReleaseVersion = "1.1.1";
#ifdef RELEASEVERSION
	strReleaseVersion = RELEASEVERSION;
#endif

	string strBuildOS = "Linux";
#ifdef BUILDOS
	strBuildOS = BUILDOS;
#endif

	//int nBuildYear = 2011;
	string strBuildDate = "120201";
#ifdef BUILDDATE
	strBuildDate = BUILDDATE;
#endif

	std::cout << "Application Version: " << strReleaseVersion << ", OS:" << strBuildOS << ", Date:" << strBuildDate
		<< ", svnRevision:" << g_pRevision << std::endl;
}
