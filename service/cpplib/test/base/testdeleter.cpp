/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "base/deleter.h"

RFC_NAMESPACE_BEGIN

onUnitTest(AutoDeleter)
{
	AutoDeleter<int> auDeleter;
	assertTest( auDeleter.isNULL() );
	
}

RFC_NAMESPACE_END
