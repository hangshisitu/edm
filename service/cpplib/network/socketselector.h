/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_SOCKETSELECTOR_H_201008
#define RFC_SOCKETSELECTOR_H_201008

#include "network/socketselectorepoll.h"

RFC_NAMESPACE_BEGIN

class SocketSyncMonitor
{
public:
	static int			socketReadCheckTimeout(typeSocketHandle nSocketID, const TimeValue & tvTimeWait)
	{
		return			socketCheckTimeout(nSocketID, tvTimeWait, true);
	}

	static int			socketWriteCheckTimeout(typeSocketHandle nSocketID, const TimeValue & tvTimeWait)
	{
		return			socketCheckTimeout(nSocketID, tvTimeWait, false);
	}

protected:
	static int			selectCheckTimeout(typeSocketHandle nSocketID, const TimeValue & tvTimeWait, bool bReadCheckTimeout);
	static int			epollCheckTimeout(typeSocketHandle nSocketID, const TimeValue & tvTimeWait, bool bReadCheckTimeout);

	static int			socketCheckTimeout(typeSocketHandle nSocketID, const TimeValue & tvTimeWait, bool bReadCheckTimeout);
};


RFC_NAMESPACE_END

#endif	//RFC_SOCKETSELECTOR_H_201008
