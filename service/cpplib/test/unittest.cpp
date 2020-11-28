/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "application/application.h"
#include "base/unittestenv.h"
#include "base/datetime.h"
#include "base/formatstream.h"
#include <fstream>

RFC_NAMESPACE_BEGIN

class UnitTestApp : public Application
{
public:
	UnitTestApp(void) : Application("unittest") {}
protected:
	virtual bool			onInit(int argc, char * argv[]);
	virtual int				onRun(void);
};

bool UnitTestApp::onInit(int argc, char * argv[])
{
	TestRegistry::onInitTestCaseArgs(argc, argv);
	return true;
}

int UnitTestApp::onRun(void)
{
	TestRegistry::runAllTestCase();
	std::cout << "app full path: " << getAppFullPath() << std::endl;
	return 0;
}

RFC_NAMESPACE_END

MAIN_APP(UnitTestApp)
