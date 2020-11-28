/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_SOCKETSELECTOREPOLL_H_201008
#define RFC_SOCKETSELECTOREPOLL_H_201008

#include "base/datetime.h"
#include <sys/epoll.h>
#include <vector>

RFC_NAMESPACE_BEGIN

typedef std::vector<int>	typeSocketIDList;

class SocketSelectorEpoll
{
public:
	SocketSelectorEpoll(int nMaxEventCount = 16, int nMaxFDSetSize = MAX_FD_SETSIZE);
	~SocketSelectorEpoll(void);

	bool					checkEnvValid(void) const;

	void					resetMaxEventCount(int nMaxEventCount);
	int						getMaxEventCount(void) const { return m_nMaxEventCount; }

	bool					addSocket(int nSocketID, bool bCheckReadable, bool bCheckWriteable);
	bool					removeSocket(int nSocketID);
	int						waitForEvent(const TimeValue & tvTimeWait);
	int						waitForEvent(int nSocketID, bool bCheckReadable, bool bCheckWriteable, const TimeValue & tvTimeWait);

	int						getReadableEvents(const TimeValue & tvTimeWait, typeSocketIDList & listEventSocket);
	int						getReadableEvents(typeSocketIDList & listEventSocket);

	int						getWriteableEvents(const TimeValue & tvTimeWait, typeSocketIDList & listEventSocket);
	int						getWriteableEvents(typeSocketIDList & listEventSocket);

	int						getReadWriteEvents(const TimeValue & tvTimeWait, typeSocketIDList & listEventSocket);
	int						getReadWriteEvents(typeSocketIDList & listEventSocket);

protected:
	int						getEvents(const TimeValue & tvTimeWait, typeSocketIDList & listEventSocket, bool bCheckReadable, bool bCheckWriteable);

protected:
	int						m_nEpollFD;
	int						m_nMaxEventCount;
	struct epoll_event *	m_pEvents;
};

RFC_NAMESPACE_END

#endif	//RFC_SOCKETSELECTOREPOLL_H_201008
