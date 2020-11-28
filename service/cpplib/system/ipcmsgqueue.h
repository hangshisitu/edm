/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_IPCMSGQUEUE_H_201008
#define RFC_IPCMSGQUEUE_H_201008

#include "base/datetime.h"
#include <sys/ipc.h>
#include <sys/msg.h>

RFC_NAMESPACE_BEGIN

struct IPCMsg
{
	IPCMsg(void) : m_nMsgType(0) { m_pMsgData[0] = RFC_CHAR_NULL; }
	int			m_nMsgType;
	char		m_pMsgData[IPC_MSG_DATA_SIZE];
};

class IPCMsgQueue
{
public:
	static const int g_nDefaultCreateMode;

	IPCMsgQueue(void) : m_nMsgQueueID(-1) {}
	IPCMsgQueue(int nMsgQueueKey, int nMode = g_nDefaultCreateMode) : m_nMsgQueueID(-1) { createMsgQueue(nMsgQueueKey, nMode); }
	~IPCMsgQueue() { destroyMsgQueue(); }

	bool		createMsgQueue(int nMsgQueueKey, int nMode = g_nDefaultCreateMode);
	bool		destroyMsgQueue();

	//@param pMessageBuffer: a struct include msgType(long) and msgData
	//@param nMsgDataLen: length of msgData
	bool		sendMessage(const void * pMessageBuffer, size_t nMsgDataLen) const { return sendMessage(pMessageBuffer, nMsgDataLen, 0); }
	bool		sendMessageNoWait(const void * pMessageBuffer, size_t nMsgDataLen) const { return sendMessage(pMessageBuffer, nMsgDataLen, IPC_NOWAIT); }
	bool		sendMessageTimeout(const void * pMessageBuffer, size_t nMsgDataLen, const TimeValue & tvTimeout) const;

	bool		sendMessage(const IPCMsg & ipcMsg, size_t nMsgDataLen = IPC_MSG_DATA_SIZE) const { return sendMessage(&ipcMsg, nMsgDataLen, 0); }
	bool		sendMessageNoWait(const IPCMsg & ipcMsg, size_t nMsgDataLen = IPC_MSG_DATA_SIZE) const { return sendMessageNoWait(&ipcMsg, nMsgDataLen); }
	bool		sendMessageTimeout(const IPCMsg & ipcMsg, const TimeValue & tvTimeout, size_t nMsgDataLen =IPC_MSG_DATA_SIZE) const
	{ return	sendMessageTimeout(&ipcMsg, nMsgDataLen, tvTimeout); }

	int			recvMessage(void * pMessageBuffer, size_t nMsgDataLen, long nMsgType) const { return recvMessage(pMessageBuffer, nMsgDataLen, nMsgType, 0); }
	int			recvMessageNoWait(void * pMessageBuffer, size_t nMsgDataLen, long nMsgType) const
	{ return	recvMessage(pMessageBuffer, nMsgDataLen, nMsgType, IPC_NOWAIT); }
	int			recvMessageTimeout(void * pMessageBuffer, size_t nMsgDataLen, long nMsgType, const TimeValue & tvTimeout) const;

	int			recvMessage(IPCMsg & ipcMsg, long nMsgType) const { return recvMessage(&ipcMsg, IPC_MSG_DATA_SIZE, nMsgType); }
	int			recvMessageNoWait(IPCMsg & ipcMsg, long nMsgType) const { return recvMessageNoWait(&ipcMsg, IPC_MSG_DATA_SIZE, nMsgType); }
	int			recvMessageTimeout(IPCMsg & ipcMsg, long nMsgType, const TimeValue & tvTimeout) const
	{ return	recvMessageTimeout(&ipcMsg, IPC_MSG_DATA_SIZE, nMsgType, tvTimeout); }

	//bool		recvAllTypeMessage(void * pMessageBuffer, size_t nMsgDataLen) const { return recvMessage(pMessageBuffer, nMsgDataLen, 0); }
	//bool		recvAllTypeMessageNoWait(void * pMessageBuffer, size_t nMsgDataLen) const { return recvMessageNoWait(pMessageBuffer, nMsgDataLen, 0); }

	bool		statMsgQueue(stdstring & str) const;

protected:
	bool		sendMessage(const void * pMessageBuffer, size_t nMsgDataLen, int nFlags) const;
	int			recvMessage(void * pMessageBuffer, size_t nMsgDataLen, long nMsgType, int nFlags) const;

private:
	int						m_nMsgQueueID;
};

RFC_NAMESPACE_END

#endif	//RFC_IPCMSGQUEUE_H_201008

