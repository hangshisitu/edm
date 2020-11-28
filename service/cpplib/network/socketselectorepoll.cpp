/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "network/socketselectorepoll.h"

RFC_NAMESPACE_BEGIN

SocketSelectorEpoll::SocketSelectorEpoll(int nMaxEventCount, int nMaxFDSetSize) : m_nEpollFD(-1), m_nMaxEventCount(0), m_pEvents(NULL)
{
	m_nEpollFD = epoll_create(nMaxFDSetSize);
	m_nMaxEventCount = nMaxEventCount;
	m_pEvents = (struct epoll_event *)malloc(sizeof(struct epoll_event) * m_nMaxEventCount);
}

SocketSelectorEpoll::~SocketSelectorEpoll(void)
{
	if ( m_nEpollFD > 0 )
		::close(m_nEpollFD);
	if ( m_pEvents != NULL )
		free(m_pEvents);
}

bool SocketSelectorEpoll::checkEnvValid(void) const
{
	return ( m_nEpollFD >= 0 && m_nMaxEventCount > 0 && m_pEvents != NULL );
}

void SocketSelectorEpoll::resetMaxEventCount(int nMaxEventCount)
{
	if ( nMaxEventCount > 0 )
	{
		struct epoll_event * pEvent = (struct epoll_event *) realloc(m_pEvents, nMaxEventCount * sizeof(struct epoll_event));
		if ( pEvent != NULL )
		{
			m_nMaxEventCount = nMaxEventCount;
			m_pEvents = pEvent;
		}
	}
}

bool SocketSelectorEpoll::addSocket(int nSocketID, bool bCheckReadable, bool bCheckWriteable)
{
	if ( !checkEnvValid() )
		return false;

	struct epoll_event epEvent;
	memset(&epEvent, 0, sizeof(epEvent));
	epEvent.data.fd = nSocketID;
	epEvent.events = EPOLLERR;
	if ( bCheckReadable )
		epEvent.events |= EPOLLIN;
	if ( bCheckWriteable )
		epEvent.events |= EPOLLOUT;
	
	int nCtlRet = epoll_ctl(m_nEpollFD, EPOLL_CTL_ADD, nSocketID, &epEvent);
	if ( nCtlRet != 0 && errno == EEXIST )
		nCtlRet = epoll_ctl(m_nEpollFD, EPOLL_CTL_MOD, nSocketID, &epEvent);
	return ( nCtlRet == 0 );
}

bool SocketSelectorEpoll::removeSocket(int nSocketID)
{
	if ( !checkEnvValid() )
		return false;

	struct epoll_event epEvent;
	memset(&epEvent, 0, sizeof(epEvent));
	epEvent.data.fd = nSocketID;
	epEvent.events = ( EPOLLERR | EPOLLIN | EPOLLOUT );

	int nCtlRet = epoll_ctl(m_nEpollFD, EPOLL_CTL_DEL, nSocketID, &epEvent);
	return ( (nCtlRet == 0) || (errno == ENOENT) );
}

int SocketSelectorEpoll::waitForEvent(const TimeValue & tvTimeWait)
{
	if ( !checkEnvValid() )
		return -1;

	int nTimeout = ( tvTimeWait == TimeValue::g_tvInfiniteTime ) ? -1 : static_cast<int>(tvTimeWait.getTotalMilliSecond());
	int nTotalEventCount = epoll_wait(m_nEpollFD, m_pEvents, m_nMaxEventCount, nTimeout);
	if ( nTotalEventCount == m_nMaxEventCount )	// auto realloc m_pEvents
		resetMaxEventCount(m_nMaxEventCount * 2);
	return nTotalEventCount;
}

int SocketSelectorEpoll::waitForEvent(int nSocketID, bool bCheckReadable, bool bCheckWriteable, const TimeValue & tvTimeWait)
{
	int nTotalEventCount = waitForEvent(tvTimeWait);
	for ( int i = 0; i < nTotalEventCount; ++i )
	{
		if ( m_pEvents[i].data.fd != nSocketID )
			continue;
		if ( (bCheckReadable && (m_pEvents[i].events & EPOLLIN) != 0)
			|| (bCheckWriteable && (m_pEvents[i].events & EPOLLOUT) != 0) )
			return 1;
	} // for ( int i = 0; i < nTotalEventCount; ++i )

	return nTotalEventCount;
}

int SocketSelectorEpoll::getEvents(const TimeValue & tvTimeWait, typeSocketIDList & listEventSocket, bool bCheckReadable, bool bCheckWriteable)
{
	int nTotalEventCount = waitForEvent(tvTimeWait);
	if ( nTotalEventCount <= 0 )
		return nTotalEventCount;

	int nAvailableEventCount = 0;
	for ( int i = 0; i < nTotalEventCount; ++i )
	{
		if ( (bCheckReadable && (m_pEvents[i].events & EPOLLIN) != 0)
			|| (bCheckWriteable && (m_pEvents[i].events & EPOLLOUT) != 0) )
		{
			++nAvailableEventCount;
			listEventSocket.push_back(m_pEvents[i].data.fd);
		}
	} // for ( int i = 0; i < nTotalEventCount; ++i )

	return nAvailableEventCount;
}

int SocketSelectorEpoll::getReadableEvents(const TimeValue & tvTimeWait, typeSocketIDList & listEventSocket)
{
	return getEvents(tvTimeWait, listEventSocket, true, false);
}

int SocketSelectorEpoll::getReadableEvents(typeSocketIDList & listEventSocket)
{
	return getEvents(TimeValue::g_tvInfiniteTime, listEventSocket, true, false);
}

int SocketSelectorEpoll::getWriteableEvents(const TimeValue & tvTimeWait, typeSocketIDList & listEventSocket)
{
	return getEvents(tvTimeWait, listEventSocket, false, true);
}

int SocketSelectorEpoll::getWriteableEvents(typeSocketIDList & listEventSocket)
{
	return getEvents(TimeValue::g_tvInfiniteTime, listEventSocket, false, true);
}

int SocketSelectorEpoll::getReadWriteEvents(const TimeValue & tvTimeWait, typeSocketIDList & listEventSocket)
{
	return getEvents(tvTimeWait, listEventSocket, true, true);
}

int SocketSelectorEpoll::getReadWriteEvents(typeSocketIDList & listEventSocket)
{
	return getEvents(TimeValue::g_tvInfiniteTime, listEventSocket, true, true);
}

RFC_NAMESPACE_END
