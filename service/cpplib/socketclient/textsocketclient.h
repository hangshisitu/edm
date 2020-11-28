/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-10      */

#ifndef APPLIB_TEXTSOCKETCLIENT_H_201110
#define APPLIB_TEXTSOCKETCLIENT_H_201110

#include "socketclient/socketclient.h"

RFC_NAMESPACE_BEGIN

class TextSocketClient : public SocketClient<stdstring, stdstring>
{
public:
	typedef SocketClient<stdstring, stdstring>	typeSocketClient;
	TextSocketClient(typeSocketHandle nSocketID) : typeSocketClient(nSocketID) {}
	TextSocketClient(const SocketStream & s) : typeSocketClient(s) {}
	virtual ~TextSocketClient(void) {}

	virtual SocketClientRetCode		onSend(const stdstring & varRequest);
	virtual SocketClientRetCode		onRecv(stdstring & varRespond);

protected:
	static  size_t					getLineEndPos(const char * pBuffer, size_t nSize);
	virtual bool					checkRespond(const char * & tp, size_t nReadNum, stdstring & res, size_t & nCurStack) { return false; }

private:
	stdstring						m_strRespondRest;
};

RFC_NAMESPACE_END

#endif	//APPLIB_TEXTSOCKETCLIENT_H_201110
