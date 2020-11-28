/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "application/application.h"
#include "system/filesystem.h"
#include "base/autoarray.h"
#include "base/formatstream.h"

#include <sys/types.h>
#include <sys/resource.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/utsname.h>
//
#include <string.h>

RFC_NAMESPACE_BEGIN

Application * Application::m_pInstance = NULL;
const stdstring	Application::g_strHomeEnvName("IYOUTUI_HOME");
const stdstring	Application::g_strDefaultHomePath("/home/iyoutui/");

Application::Application(const stdstring & strAppName) : m_strAppName(strAppName)
{
	m_pInstance = this;
}

int Application::run(int argc, char * argv[])
{
	try
	{
		if ( argv[0][0] == RFC_CHAR_PATH_SLASH )
			m_strAppFullPath = argv[0];
		else
		{
			m_strAppFullPath = FileSystem::getCurrentDirectory();
			FileSystem::toPathName(m_strAppFullPath, m_strAppFullPath);
			m_strAppFullPath += argv[0];
			FileSystem::fixPath(m_strAppFullPath);
		}
		
		if ( !onInit(argc, argv) )
			return -1;
		
		return onRun();
	}
	catch( Exception & exp )
	{
		std::cout << FormatString("Got Exception on Application::run:%, errorInfo:%, errcode:%")
			.arg(exp.what()).arg(GlobalFunc::getSystemErrorInfo()).arg(exp.getErrorCode()).str() << std::endl;
		return -1;
	}
	catch( std::exception & stdExp )
	{
		std::cout << FormatString("Got std::exception on Application::run:%, errorInfo:%, errcode:%")
			.arg(stdExp.what()).arg(GlobalFunc::getSystemErrorInfo()).arg(errno).str() << std::endl;
		return -1;
	}
	catch ( const char * tp )
	{
		std::cout << FormatString("Got string exception on Application::run:%, errorInfo:%, errcode:%")
			.arg(tp).arg(GlobalFunc::getSystemErrorInfo()).arg(errno).str() << std::endl;
		return -1;
	}
	catch ( int nErrno )
	{
		std::cout << FormatString("Got integer exception on Application::run:%, errorInfo:%, errcode:%")
			.arg(nErrno).arg(GlobalFunc::getSystemErrorInfo()).arg(errno).str() << std::endl;
		return -1;
	}
	catch ( ... )
	{
		std::cout << FormatString("Got unknown exception on Application::run, errorInfo:%, errcode:%")
			.arg(GlobalFunc::getSystemErrorInfo()).arg(errno).str() << std::endl;
		return -1;
	}

	return 0;
}

stdstring Application::getHomePath(void)
{
	stdstring strHomePath;
	getEnv(g_strHomeEnvName, g_strDefaultHomePath, strHomePath);
	return strHomePath;
}

bool Application::setEnv(const stdstring & strKey, const stdstring & strValue)
{
	stdstring strEnvPair = strKey;
	strEnvPair += '=';
	strEnvPair += strValue;

	AutoArray<char> auPath( new char[strEnvPair.size() + 1] );
	strncpy( auPath.get(), strEnvPair.c_str() , strEnvPair.size());
	auPath[strEnvPair.size()] = RFC_CHAR_NULL;
	return ( putenv(auPath.get()) == 0 );
}

bool Application::getEnv(const stdstring & strKey, const stdstring & strDefault, stdstring & strValue)
{
	const char * lpszEnv = getenv(strKey.c_str());
	if ( lpszEnv == NULL )
		strValue = strDefault;
	else
		strValue = lpszEnv;
	return ( lpszEnv != NULL );
}

bool Application::getEnv(const stdstring & strKey, stdstring & strValue)
{
	return getEnv(strKey, strValue, strValue);
}

void Application::unsetEnv(const stdstring & strKey)
{
	size_t nSize = strKey.size();
	for ( char ** pEnvList = environ; *pEnvList != NULL; ++pEnvList )
	{
		char * pEnv = *pEnvList;
		if ( strncmp(pEnv, strKey.data(), nSize) == 0 && pEnv[nSize] == '=' )
		{
			// Found it.  Remove this pointer by moving later ones back
			for ( char ** pEnvMove = pEnvList; *pEnvMove != NULL; ++pEnvMove )
				pEnvMove[0] = pEnvMove[1];
		}
	} // for ( const char ** pEnvList = environ; *pEnvList != NULL; ++pEnvList )
}

int Application::getPid(void)
{
	return static_cast<int>( getpid() );
}

stdstring Application::getTmpPath(void)
{
	stdstring strTmpPath("/tmp/");
	getEnv("TMPDIR", strTmpPath, strTmpPath);
	FileSystem::toPathName(strTmpPath, strTmpPath);
	return strTmpPath;
}

void Application::signalFunc(int nSignalNum)
{
	if ( m_pInstance != NULL )
		m_pInstance->onSignal(nSignalNum);
}

void Application::maskSignal(void)
{
	sigset_t sigSet;
	memset(&sigSet, 0, sizeof(sigSet));
	sigfillset(&sigSet);
	pthread_sigmask(SIG_BLOCK, &sigSet, NULL);
}

void Application::unmaskSignal(void)
{
	sigset_t sigSet;
	memset(&sigSet, 0, sizeof(sigSet));
	sigfillset(&sigSet);

	sigaddset(&sigSet, SIGTERM);
	sigaddset(&sigSet, SIGINT);
	sigaddset(&sigSet, SIGTSTP);
	sigaddset(&sigSet, SIGCONT);

	sigaddset(&sigSet, SIGSEGV);
	sigaddset(&sigSet, SIGILL);
	sigaddset(&sigSet, SIGABRT);

	pthread_sigmask(SIG_UNBLOCK, &sigSet, NULL);
	signal(SIGTERM, signalFunc);
	signal(SIGINT, signalFunc);
	signal(SIGSEGV, signalFunc);
	signal(SIGILL, signalFunc);
	signal(SIGABRT, signalFunc);
	signal(SIGPIPE, SIG_IGN);

#ifdef SIGXFSZ
	signal(SIGXFSZ, SIG_IGN);
#endif
}

void Application::callDefaultSignal(int nSignalNum)
{
	signal (nSignalNum, SIG_DFL);
}

#define NZERO 20
bool Application::setSelfPriority(ProcessPriority nPriority)
{
	int nValue = 0;
	if ( nPriority == PRIORITY_HIGHEST )
		nValue = -NZERO;
	else if ( nPriority == PRIORITY_HIGH )
		nValue = -NZERO / 2;
	else if ( nPriority == PRIORITY_ABOVE )
		nValue = -NZERO / 3;
	else if ( nPriority == PRIORITY_LOW )
		nValue = NZERO / 2;
	else if ( nPriority == PRIORITY_LOWEST )
		nValue = NZERO - 1;

	return ( setpriority(PRIO_PROCESS, 0, nValue) == 0 );
}

RFC_NAMESPACE_END
