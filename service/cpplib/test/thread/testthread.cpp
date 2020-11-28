/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "thread/threadpool.h"
#include "base/formatstream.h"
#include <list>

RFC_NAMESPACE_BEGIN

static void printLogLine(const stdstring & strLine)
{
	DateTime dt = DateTime::getLocalZonePresentTime();
	stdstring strData = FormatString("T:%(%_%) %").arg(ThreadFunc::getSelfThreadID()).arg(dt.formatTime())
		.arg(dt.getFractionMilli()).arg(strLine).str();
	static MutexLock g_lockForPrint;
	AutoMutexLock auLock(g_lockForPrint);
	std::cout << strData << std::endl;
}

class MyThread : public Thread
{
public:
	MyThread(int nThreadIndex, int nLoopCount = -1) : m_nIndex(nThreadIndex), m_nLoopCount(nLoopCount) {}
	virtual int		run(void);
private:
	int				m_nIndex;
	int				m_nLoopCount;
};

int MyThread::run(void)
{
	rfc_uint_32 nRandSeed;
	while ( !shouldExit() && m_nLoopCount != 0 )
	{
		GlobalFunc::threadSafeRand( &nRandSeed );
		nRandSeed %= 900;
		nRandSeed += 100;
		printLogLine( FormatString("ThreadIndex:%, ThreadID:%, will delay % microseconds").arg(m_nIndex).arg(getThreadID()).arg(nRandSeed).str() );
		suspend( TimeValue(0, static_cast<time_t>(nRandSeed * 1000)) );
		--m_nLoopCount;
	} // while ( !shouldExit() )
	return 0;
}

typedef std::list<MyThread *> typeThreadList;
onUnitTest(Thread)
{
	typeThreadList listThread;
	size_t nTotalThreadCount = 3;
	for ( size_t i = 0; i < nTotalThreadCount; ++i )
	{
		MyThread * pThread = new MyThread(i + 1);
		listThread.push_back(pThread);
	} // for ( i = 0; i < nTotalThreadCount; ++i )

	typeThreadList::iterator it;
	printLogLine( FormatString("Begin createAndStart % threads").arg(nTotalThreadCount).str() );
	for ( it = listThread.begin(); it != listThread.end(); ++it )
	{
		MyThread * pThread = (*it);
		pThread->createAndStart();
	}

	printLogLine( FormatString("After createAndStart % threads, main thread delay 2 seconds").arg(nTotalThreadCount).str() );
	delay(2);

	printLogLine( FormatString("Now main thread stop all threads").str() );
	for ( it = listThread.begin(); it != listThread.end(); ++it )
	{
		MyThread * pThread = (*it);
		pThread->stop();
	}

	printLogLine( FormatString("Now main thread wait all threads stop").str() );
	for ( it = listThread.begin(); it != listThread.end(); ++it )
	{
		MyThread * pThread = (*it);
		pThread->waitUntilStop();
	}

	printLogLine( FormatString("Now main thread delete all threads").str() );
	for ( it = listThread.begin(); it != listThread.end(); ++it )
	{
		MyThread * pThread = (*it);
		delete pThread;
	}
}

onUnitTest(ThreadPool)
{
	ThreadPool aThreadPool("TestThreadPool");
	aThreadPool.initPool(2, 3, 1000, 65536, 30);
	for ( int i = 0; i < 10; ++i )
	{
		if ( !aThreadPool.tryPushTask(new MyThread(i + 1, 1), true) )
		{
			printLogLine( FormatString("push task fail:%").arg(i + 1).str() );
		}
	}
	printLogLine( FormatString("Now main thread wait all task finish").str() );
	//delay(TimeValue(300));
	aThreadPool.waitUntilAllTaskFinish();
	printLogLine( FormatString("All task done").str() );
}

RFC_NAMESPACE_END
