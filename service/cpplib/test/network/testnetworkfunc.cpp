/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "network/tcpsocket.h"
#include "network/internetaddr.h"
#include "network/networkfunc.h"
#include "base/unittestenv.h"

RFC_NAMESPACE_BEGIN

onUnitTest(NetworkFunc)
{
	typeSocketHandle nSocketID = NetworkFunc::createSocket();
	assertGreater(nSocketID, 2);
	assertEqual(NetworkFunc::setSocketBlocking(nSocketID, true), 0);
	assertEqual(NetworkFunc::setSocketBlocking(nSocketID, false), 0);
	assertEqual(NetworkFunc::setTCPDelay(nSocketID, true), 0);
	assertEqual(NetworkFunc::setTCPDelay(nSocketID, false), 0);
	assertEqual(NetworkFunc::closeSocket(nSocketID), 0);

	in_addr inetAddr;
	stdstring strIP, strHostName;
	assertTest( NetworkFunc::getHostByName("127.0.0.1", inetAddr) );
	assertTest( InternetAddrV4(inetAddr).isValid() );
	InternetAddrV4(inetAddr).getIPAddress(strIP);
	assertEqual(strIP, stdstring("127.0.0.1"));

	assertTest( NetworkFunc::getHostByName("localhost.localdomain", inetAddr) );
	assertTest( InternetAddrV4(inetAddr).isValid() );
	InternetAddrV4(inetAddr).getIPAddress(strIP);
	assertEqual(strIP, stdstring("127.0.0.1"));

	NetworkFunc::getHostByAddr(inetAddr, strHostName);
	assertEqual(strHostName, stdstring("localhost.localdomain"));

	TCPSocket tcpSocket;
	rfc_uint_16 uPort = 0;
	nSocketID = tcpSocket.connectTo("127.0.0.1", 25);		// smtp
	assertTest( tcpSocket.isValid() );
	assertEqual(NetworkFunc::shutdown(nSocketID, true, true), 0);
	assertTest( NetworkFunc::getPeerInfo(nSocketID, inetAddr) );
	InternetAddrV4(inetAddr).getIPAddress(strIP);
	assertEqual(strIP, stdstring("127.0.0.1"));
	assertTest( NetworkFunc::getPeerInfo(nSocketID, inetAddr, uPort) );
	InternetAddrV4(inetAddr).getIPAddress(strIP);
	assertEqual(strIP, stdstring("127.0.0.1"));
	assertEqual(uPort, static_cast<rfc_uint_16>(25));

	assertTest( NetworkFunc::getLocalInfo(nSocketID, inetAddr) );
	InternetAddrV4(inetAddr).getIPAddress(strIP);
	assertEqual(strIP, stdstring("127.0.0.1"));
	assertTest( NetworkFunc::getLocalInfo(nSocketID, inetAddr, uPort) );
	InternetAddrV4(inetAddr).getIPAddress(strIP);
	assertEqual(strIP, stdstring("127.0.0.1"));

	assertTest( NetworkFunc::getLocalInfo(nSocketID, inetAddr) );
	InternetAddrV4(inetAddr).getIPAddress(strIP);
	assertEqual(strIP, stdstring("127.0.0.1"));
	assertTest( NetworkFunc::getLocalInfo(nSocketID, inetAddr, uPort) );
	InternetAddrV4(inetAddr).getIPAddress(strIP);
	assertEqual(strIP, stdstring("127.0.0.1"));

	assertTest( !NetworkFunc::isValidIPV4Addr(".127.0.0.1") );
	assertTest( !NetworkFunc::isValidIPV4Addr("127.0.0.1.") );
	assertTest( !NetworkFunc::isValidIPV4Addr("127.0.0") );
	assertTest( !NetworkFunc::isValidIPV4Addr("127.0.0.") );
	assertTest( !NetworkFunc::isValidIPV4Addr("127.0.0.1.1") );
	assertTest( NetworkFunc::isValidIPV4Addr("127.0.0.1") );

	rfc_uint_32 nIP;
	assertTest( NetworkFunc::getIPV4Integer("127.0.0.1", nIP) );
	assertEqual(nIP, static_cast<rfc_uint_32>((127 << 24) + 1));
	NetworkFunc::getIPV4String(nIP, strIP);
	assertEqual(strIP, stdstring("127.0.0.1"));
	NetworkFunc::getIPV4ReverseString(nIP, strIP);
	assertEqual(strIP, stdstring("1.0.0.127"));
	assertTest( NetworkFunc::getIPV4Reverse("127.0.0.1", strIP) );
	assertEqual(strIP, stdstring("1.0.0.127"));

	std::set<stdstring> setLocalIP;
	assertTest( NetworkFunc::getAllLocalIP(setLocalIP) );
	stdstring strLocalIPList;
	std::set<stdstring>::const_iterator itLocalIP, itLocalIPEnd = setLocalIP.end();
	for ( itLocalIP = setLocalIP.begin(); itLocalIP != itLocalIPEnd; ++itLocalIP )
	{
		if ( !strLocalIPList.empty() )
			strLocalIPList += ',';
		strLocalIPList += (*itLocalIP);
	}
	std::cout << "\r\nAll local ip list: " << strLocalIPList << std::endl;
}

RFC_NAMESPACE_END
