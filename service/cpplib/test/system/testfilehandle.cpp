/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "system/filehandle.h"
#include "system/filesystem.h"
#include "mutex/mutex.h"

RFC_NAMESPACE_BEGIN

onUnitTest(FileHandle)
{
	FileHandle fHandle;
	stdstring strFilePath = "/tmp/testhandle";
	//StringFunc::addIntToString(strFilePath, time(NULL));
	//assertTest( !FileSystem::pathExist(strFilePath.c_str()) );
	assertGreater( fHandle.open(strFilePath.c_str(), O_RDWR | O_CREAT | O_APPEND), 0 );
	assertTest( FileSystem::pathExist(strFilePath.c_str()) );
	assertTest( fHandle.processWriteLock(TimeValue::g_tvZeroTime) );
	assertTest( !fHandle.processWriteLock() );
	assertTest( !fHandle.processReadLock() );

	/*FileHandle fHandle2;
	assertGreater( fHandle2.open(strFilePath.c_str(), O_RDWR | O_CREAT | O_APPEND), 0 );
	assertTest( !fHandle2.processWriteLock(TimeValue::g_tvZeroTime) );
	assertTest( !fHandle2.processWriteLock(TimeValue(1)) );
	assertTest( !fHandle2.processWriteLock() );

	assertTest( !fHandle2.processReadLock(TimeValue::g_tvZeroTime) );
	assertTest( !fHandle2.processReadLock(TimeValue(1)) );
	assertTest( !fHandle2.processReadLock() );

	fHandle.close();
	assertTest( !fHandle.processReadLock(TimeValue::g_tvZeroTime) );
	assertTest( !fHandle.processReadLock(TimeValue(1)) );
	assertTest( !fHandle.processReadLock() );

	assertTest( !fHandle.processWriteLock(TimeValue::g_tvZeroTime) );
	assertTest( !fHandle.processWriteLock(TimeValue(1)) );
	assertTest( !fHandle.processWriteLock() );

	assertTest( fHandle2.processReadLock(TimeValue::g_tvZeroTime) );
	assertTest( !fHandle2.processReadLock(TimeValue::g_tvZeroTime) );
	assertTest( !fHandle2.processReadLock(TimeValue(1)) );
	assertTest( !fHandle2.processReadLock() );

	assertTest( !fHandle2.processWriteLock(TimeValue::g_tvZeroTime) );
	assertTest( !fHandle2.processWriteLock(TimeValue(1)) );
	assertTest( !fHandle2.processWriteLock() );*/

	delay(10000);
}

RFC_NAMESPACE_END
