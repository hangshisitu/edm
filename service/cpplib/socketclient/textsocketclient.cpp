/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-10      */

#include "socketclient/textsocketclient.h"

RFC_NAMESPACE_BEGIN

static const size_t g_nReadLineBufferSize = 64 * 1024;

size_t TextSocketClient::getLineEndPos(const char * pBuffer, size_t nSize)
{
	for ( size_t nPos = 0; nPos < nSize; ++nPos )
	{
		if ( pBuffer[nPos] == '\r' || pBuffer[nPos] == '\n' )
		{
			if ( pBuffer[nPos] == '\r' && nPos + 1 < nSize && pBuffer[nPos + 1] == '\n' )
				++nPos;
			return nPos + 1;	// add 1 position for first \r
		}
	} // for ( size_t nPos = 0; nPos < nSize; ++nPos )
	return stdstring::npos;
}

SocketClientRetCode TextSocketClient::onSend(const stdstring & strRequest)
{
	if ( writeBlock(strRequest.data(), strRequest.size()) != strRequest.size() )
		return SOCKET_CLIENT_RET_SEND_NET_ERROR;
	return SOCKET_CLIENT_RET_OK;
}

SocketClientRetCode TextSocketClient::onRecv(stdstring & strRespond)
{
	strRespond.swap(m_strRespondRest);
	m_strRespondRest.clear();
	size_t nLineEndPos = getLineEndPos(strRespond.data(), strRespond.size());
	if ( nLineEndPos != stdstring::npos && nLineEndPos != strRespond.size() )		// 把多余的部分保存到m_strRespondRest
	{
		m_strRespondRest.assign(strRespond.data() + nLineEndPos, strRespond.size() - nLineEndPos);
		strRespond.erase(nLineEndPos);
	} // if ( nLineEndPos != stdstring::npos )
	if ( nLineEndPos != stdstring::npos )
		return SOCKET_CLIENT_RET_OK;

	strRespond.reserve(g_nReadLineBufferSize);
	char * pBuffer = const_cast<char *>(strRespond.data());
	size_t nHasReadCount = strRespond.size();
	for ( int nRetryCount = 0; nRetryCount < 50; ++nRetryCount )
	{
		size_t nReadCount = read(pBuffer + nHasReadCount, g_nReadLineBufferSize - nHasReadCount);
		if ( StreamBase::resultIsError(nReadCount) || StreamBase::resultIsEndOfStream(nReadCount) )
			return SOCKET_CLIENT_RET_RECV_NET_ERROR;

		nHasReadCount += nReadCount;
		nLineEndPos = getLineEndPos(pBuffer, nHasReadCount);
		if ( nLineEndPos != stdstring::npos )
		{
			if ( nLineEndPos != nHasReadCount )
				m_strRespondRest.assign(pBuffer + nLineEndPos, nHasReadCount - nLineEndPos);
			strRespond.resize(nLineEndPos);
			return SOCKET_CLIENT_RET_OK;
		} // if ( nLineEndPos != stdstring::npos )
		if ( nHasReadCount >= g_nReadLineBufferSize )
			return SOCKET_CLIENT_RET_RECV_FORMAT_ERROR;		
	} // for ( int nRetryCount = 0; nRetryCount < 50; ++nRetryCount )

	return SOCKET_CLIENT_RET_RECV_NET_ERROR;
}

RFC_NAMESPACE_END
