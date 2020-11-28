/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "system/atomicinteger.h"

RFC_NAMESPACE_BEGIN

template<typename typeInteger, typename typeLock>
void onTestAtomicInteger(void)
{
	AtomicInteger<typeInteger, typeLock> atomicInt1;
	assertEqual(atomicInt1.getValue(), 0);

	typeInteger n1 = 1000;
	atomicInt1 = n1;
	assertEqual(atomicInt1.getValue(), n1);
	assertEqual(atomicInt1++.getValue(), n1);
	assertEqual((++atomicInt1).getValue(), n1 + 2);
	assertEqual((--atomicInt1).getValue(), n1 + 1);
	assertEqual(atomicInt1--.getValue(), n1 + 1);
	assertEqual(atomicInt1.getValue(), n1);

	typeInteger n2 = 2000;
	assertEqual(atomicInt1.getAndSet(n2), n1);
	assertEqual(atomicInt1.getValue(), n2);
	assertEqual(atomicInt1.addAndGet(100), n2 + 100);
	assertEqual(atomicInt1.incAndGet(), n2 + 100 + 1);
	assertEqual(atomicInt1.getAndAdd(100), n2 + 100 + 1);
	assertEqual(atomicInt1.getValue(), n2 + 100 + 1 + 100);
	assertEqual(atomicInt1.getAndInc(), n2 + 100 + 1 + 100);
	assertEqual(atomicInt1.getValue(), n2 + 100 + 1 + 100 + 1);

	assertEqual(atomicInt1.swap(n2), n2 + 100 + 1 + 100 + 1);
	assertEqual(atomicInt1.getValue(), n2);
	assertEqual(atomicInt1.subAndGet(100), n2 - 100);
	assertEqual(atomicInt1.decAndGet(), n2 - 100 - 1);
	assertEqual(atomicInt1.getAndSub(100), n2 - 100 - 1);
	assertEqual(atomicInt1.getValue(), n2 - 100 - 1 - 100);
	assertEqual(atomicInt1.getAndDec(), n2 - 100 - 1 - 100);
	assertEqual(atomicInt1.getValue(), n2 - 100 - 1 - 100 - 1);
}

onUnitTest(AtomicInteger)
{
	onTestAtomicInteger<rfc_int_32, NullLock>();
	onTestAtomicInteger<rfc_int_32, MutexLock>();
	onTestAtomicInteger<rfc_int_64, NullLock>();
	onTestAtomicInteger<rfc_int_64, MutexLock>();
}

RFC_NAMESPACE_END
