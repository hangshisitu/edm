/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "network/socketselector.h"
#include "network/networkfunc.h"
 
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

RFC_NAMESPACE_BEGIN

int SocketSyncMonitor::selectCheckTimeout(typeSocketHandle nSocketID, const TimeValue & tvTimeWait, bool bReadCheckTimeout)
{
	if ( !NetworkFunc::isValidHandle(nSocketID) || nSocketID >= MAX_FD_SETSIZE )
		return -1;

	fd_set fdSet;
	FD_ZERO(&fdSet);
	FD_SET(nSocketID, &fdSet);
	fd_set * pReadSet = ( bReadCheckTimeout ? &fdSet : NULL );
	fd_set * pWriteSet = ( bReadCheckTimeout ? NULL : &fdSet );

	struct timeval tvSpec;
	tvSpec.tv_sec = tvTimeWait.getSecond();
	tvSpec.tv_usec = (tvTimeWait == TimeValue::g_tvZeroTime) ? 1 : tvTimeWait.getMicroSecond();

	int nRetCode = select(nSocketID + 1, pReadSet, pWriteSet, NULL, ( tvTimeWait == TimeValue::g_tvInfiniteTime) ? NULL : &tvSpec);
	if ( nRetCode == -1 )
		return nRetCode;
	else if ( nRetCode == 0 )
		return ETIMEDOUT;

	return FD_ISSET(nSocketID, &fdSet) ? 0 : -1;
}

int SocketSyncMonitor::epollCheckTimeout(typeSocketHandle nSocketID, const TimeValue & tvTimeWait, bool bReadCheckTimeout)
{
	SocketSelectorEpoll epollSelector(16, nSocketID + 16);
	if ( !epollSelector.addSocket(nSocketID, bReadCheckTimeout, !bReadCheckTimeout) )
		return -1;
	int nRetCode = epollSelector.waitForEvent(nSocketID, bReadCheckTimeout, !bReadCheckTimeout, tvTimeWait);
	if ( nRetCode == -1 )
		return nRetCode;
	else if ( nRetCode == 0 )
		return ETIMEDOUT;

	return 0;
}

int SocketSyncMonitor::socketCheckTimeout(typeSocketHandle nSocketID, const TimeValue & tvTimeWait, bool bReadCheckTimeout)
{
	return selectCheckTimeout(nSocketID, tvTimeWait, bReadCheckTimeout);
}

RFC_NAMESPACE_END
