/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "system/ipcmsgqueue.h"

RFC_NAMESPACE_BEGIN

const int IPCMsgQueue::g_nDefaultCreateMode = 0666;

bool IPCMsgQueue::statMsgQueue(stdstring & str) const
{
	if ( m_nMsgQueueID < 0 )
		return false;

	struct msqid_ds msgQueueInfo;
	int nRet = msgctl(m_nMsgQueueID, IPC_STAT, &msgQueueInfo);
	if ( nRet < 0 )
		return false;

	// man msgctl to see msqid_ds
	return true;
}

bool IPCMsgQueue::createMsgQueue(int nMsgQueueKey, int nMode)
{
	if ( !destroyMsgQueue() )
		return false;
	m_nMsgQueueID = msgget(nMsgQueueKey, IPC_CREAT | nMode);
	return ( m_nMsgQueueID >= 0 );
}

bool IPCMsgQueue::destroyMsgQueue()
{
	if ( m_nMsgQueueID < 0 )
		return true;
	if ( msgctl(m_nMsgQueueID, IPC_RMID, NULL) != 0 )
		return false;
	m_nMsgQueueID = -1;
	return true;
}

bool IPCMsgQueue::sendMessageTimeout(const void * pMessageBuffer, size_t nMsgDataLen, const TimeValue & tvTimeout) const
{
	if ( sendMessageNoWait(pMessageBuffer, nMsgDataLen) )
		return true;
	if ( errno != EAGAIN )
		return false;
	//wait tvTimeout
	return sendMessageNoWait(pMessageBuffer, nMsgDataLen);
}

int IPCMsgQueue::recvMessageTimeout(void * pMessageBuffer, size_t nMsgDataLen, long nMsgType, const TimeValue & tvTimeout) const
{
	int nRet = recvMessageNoWait(pMessageBuffer, nMsgDataLen, nMsgType);
	if ( nRet >= 0 || errno != ENOMSG )
		return nRet;
	//wait tvTimeout
	return recvMessageNoWait(pMessageBuffer, nMsgDataLen, nMsgType);
}

bool IPCMsgQueue::sendMessage(const void * pMessageBuffer, size_t nMsgDataLen, int nFlags) const
{
	if ( m_nMsgQueueID < 0 )
		return false;
	return ( msgsnd(m_nMsgQueueID, pMessageBuffer, nMsgDataLen, nFlags) == 0 );
}

int IPCMsgQueue::recvMessage(void * pMessageBuffer, size_t nMsgDataLen, long nMsgType, int nFlags) const
{
	if ( m_nMsgQueueID < 0 )
		return -1;
	return msgrcv(m_nMsgQueueID, pMessageBuffer, nMsgDataLen, nMsgType, nFlags);
}

RFC_NAMESPACE_END

