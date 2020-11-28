/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_APPLICATION_H_201008
#define RFC_APPLICATION_H_201008

#include "base/globalfunc.h"
#include <string.h>
#include <memory>

RFC_NAMESPACE_BEGIN

class Application : private NonCopyAble
{
public:
	Application(const stdstring & strAppName);
	virtual ~Application(void) {}
	static const stdstring	g_strHomeEnvName;
	static const stdstring	g_strDefaultHomePath;

	int						run(int argc, char * argv[]);

	const stdstring &		getAppName(void) const { return m_strAppName; }
	const stdstring &		getAppFullPath(void) const { return m_strAppFullPath; }

	static Application &	getInstance(void) { return *m_pInstance; }
	static stdstring		getHomePath(void);
	static bool				setEnv(const stdstring & strKey, const stdstring & strValue);
	static bool				getEnv(const stdstring & strKey, const stdstring & strDefault, stdstring & strValue);
	static bool				getEnv(const stdstring & strKey, stdstring & strValue);
	static void				unsetEnv(const stdstring & strKey);

	static int				getPid(void);
	static stdstring		getTmpPath(void);
	static void				signalFunc(int nSignalNum);

protected:
	virtual bool			onInit(int argc, char * argv[]) = 0;
	virtual int				onRun(void) = 0;

	virtual void			onSignal(int nSignalNum) {}
	void					maskSignal(void);
	void					unmaskSignal(void);
	void					callDefaultSignal(int nSignalNum);

	enum ProcessPriority
	{
		PRIORITY_HIGHEST	=	7,
		PRIORITY_HIGH		=	3,
		PRIORITY_ABOVE		=	1,
		PRIORITY_NORMAL		=	0,
		PRIORITY_LOW		=	-3,
		PRIORITY_LOWEST		=	-7,
	};
	bool					setSelfPriority(ProcessPriority nPriority);

private:
	static Application *	m_pInstance;
	stdstring				m_strAppName;
	stdstring				m_strAppFullPath;
};

RFC_NAMESPACE_END

#define MAIN_APP(typeApp)	\
	int main(int argc, char * argv[])	\
	{	\
		std::auto_ptr<RFC_NAMESPACE_NAME::Application> auApp(new RFC_NAMESPACE_NAME::typeApp);	\
		return auApp->run(argc,argv);	\
	}

#endif	//RFC_APPLICATION_H_201008
