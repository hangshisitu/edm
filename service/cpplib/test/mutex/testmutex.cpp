/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

static void onTestSpinLock(void)
{
	SpinLock aLock;
	std::cout << "before spin lock" << std::endl;
	aLock.lock();
	std::cout << "do spin lock OK" << std::endl;
	aLock.release();
	std::cout << "release spin lock OK" << std::endl;
	std::cout << "do spin try lock OK:" << aLock.tryLock() << std::endl;
}

static void onTestAutoSpinLock(void)
{
	SpinLock aLock;
	std::cout << "before auto spin lock" << std::endl;
	AutoSpinLock au(aLock);
	std::cout << "do auto spin lock OK" << std::endl;
	au.release();
	std::cout << "release auto spin lock OK" << std::endl;
	std::cout << "auto try lock OK: " << au.tryLock() << std::endl;
}

onUnitTest(SpinLock)
{
	assertNotEqual(SUPPORT_POSIX_SPINLOCK, 0);

	onTestSpinLock();
	onTestAutoSpinLock();	
}

RFC_NAMESPACE_END
