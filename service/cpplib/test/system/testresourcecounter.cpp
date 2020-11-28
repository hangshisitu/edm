/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "system/resourcecounter.h"

RFC_NAMESPACE_BEGIN

template<typename typeKey, typename typeLock>
void onTestResourceCounter(const typeKey & strKey1, const typeKey & strKey2)
{
	ResourceCounter<typeKey, typeLock> resourceCounter;
	assertEqual(resourceCounter.getCounter(strKey1), static_cast<size_t>(0));
	assertEqual(resourceCounter.getCounter(strKey2),  static_cast<size_t>(0));

	assertEqual(resourceCounter.addAndGetCounter(strKey1, 100),  static_cast<size_t>(100));
	assertEqual(resourceCounter.getCounter(strKey1),  static_cast<size_t>(100));
	assertEqual(resourceCounter.incAndGetCounter(strKey1),  static_cast<size_t>(101));
	assertEqual(resourceCounter.getCounter(strKey1),  static_cast<size_t>(101));

	assertEqual(resourceCounter.addAndGetCounter(strKey2, 200),  static_cast<size_t>(200));
	assertEqual(resourceCounter.getCounter(strKey2),  static_cast<size_t>(200));
	assertEqual(resourceCounter.incAndGetCounter(strKey2),  static_cast<size_t>(201));
	assertEqual(resourceCounter.getCounter(strKey2),  static_cast<size_t>(201));

	assertEqual(resourceCounter.subAndGetCounter(strKey1, 50),  static_cast<size_t>(51));
	assertEqual(resourceCounter.getCounter(strKey1),  static_cast<size_t>(51));
	assertEqual(resourceCounter.incAndGetCounter(strKey1),  static_cast<size_t>(52));
	assertEqual(resourceCounter.getCounter(strKey1),  static_cast<size_t>(52));
	assertEqual(resourceCounter.subAndGetCounter(strKey1, 100),  static_cast<size_t>(0));
	assertEqual(resourceCounter.getCounter(strKey1),  static_cast<size_t>(0));

	assertEqual(resourceCounter.getCounter(strKey2),  static_cast<size_t>(201));
	resourceCounter.setCounter(strKey1,  static_cast<size_t>(1000));
	assertEqual(resourceCounter.getCounter(strKey1),  static_cast<size_t>(1000));
	resourceCounter.subAndGetCounter(strKey1, 1234);
	resourceCounter.subAndGetCounter(strKey2, 1234);
	assertEqual(resourceCounter.getCounter(strKey1),  static_cast<size_t>(0));
	assertEqual(resourceCounter.getCounter(strKey2),  static_cast<size_t>(0));
}

onUnitTest(ResourceCounter)
{
	onTestResourceCounter<rfc_int_32, NullLock>(1111, 22222);
	onTestResourceCounter<rfc_int_32, MutexLock>(12345, 67890);
	onTestResourceCounter<rfc_int_64, NullLock>(23423, 78788);
	onTestResourceCounter<rfc_int_64, MutexLock>(9876, 2345);
	onTestResourceCounter<stdstring, NullLock>("kaka", "uiTGYHUJIKO");
	onTestResourceCounter<stdstring, MutexLock>("FGHJaafd", "5678hj%*&^%dfs");
}

RFC_NAMESPACE_END
