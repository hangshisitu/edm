/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "network/tcpsocket.h"
#include "network/socketselector.h"
#include "network/networkfunc.h"

RFC_NAMESPACE_BEGIN

onUnitTest(SocketSyncMonitor)
{
	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(-1, TimeValue::g_tvZeroTime), -1);
	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(-1, TimeValue(1)), -1);
	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(-1, TimeValue::g_tvInfiniteTime), -1);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(-1, TimeValue::g_tvZeroTime), -1);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(-1, TimeValue(1)), -1);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(-1, TimeValue::g_tvInfiniteTime), -1);

	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(0, TimeValue::g_tvZeroTime), ETIMEDOUT);
	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(0, TimeValue(1)), ETIMEDOUT);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(0, TimeValue::g_tvZeroTime), 0);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(0, TimeValue(1)), 0);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(0, TimeValue::g_tvInfiniteTime), 0);

	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(1, TimeValue::g_tvZeroTime), ETIMEDOUT);
	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(1, TimeValue(1)), ETIMEDOUT);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(1, TimeValue::g_tvZeroTime), 0);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(1, TimeValue(1)), 0);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(1, TimeValue::g_tvInfiniteTime), 0);

	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(2, TimeValue::g_tvZeroTime), ETIMEDOUT);
	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(2, TimeValue(1)), ETIMEDOUT);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(2, TimeValue::g_tvZeroTime), 0);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(2, TimeValue(1)), 0);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(2, TimeValue::g_tvInfiniteTime), 0);

	stdstring strServerIP = "127.0.0.1";
	rfc_uint_16 uPort = 2025;
	TCPSocket tcpSocket;
	tcpSocket.connectTo(strServerIP, uPort, TimeValue(3));
	assertTest(tcpSocket.isValid());

	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(tcpSocket.getHandle(), TimeValue::g_tvZeroTime), ETIMEDOUT);
	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(tcpSocket.getHandle(), TimeValue(1)), 0);
	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(tcpSocket.getHandle(), TimeValue::g_tvInfiniteTime), 0);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(tcpSocket.getHandle(), TimeValue::g_tvZeroTime), 0);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(tcpSocket.getHandle(), TimeValue(1)), 0);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(tcpSocket.getHandle(), TimeValue::g_tvInfiniteTime), 0);

	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(1025, TimeValue::g_tvZeroTime), -1);
	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(1025, TimeValue(1)), -1);
	assertEqual(SocketSyncMonitor::socketReadCheckTimeout(1025, TimeValue::g_tvInfiniteTime), -1);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(1025, TimeValue::g_tvZeroTime), -1);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(1025, TimeValue(1)), -1);
	assertEqual(SocketSyncMonitor::socketWriteCheckTimeout(1025, TimeValue::g_tvInfiniteTime), -1);
}

onUnitTest(TCPSocket)
{
	TCPSocket tcpSocket;
	assertTest(!tcpSocket.isValid());
	assertTest(tcpSocket.isClosed());
	assertEqual(tcpSocket.getHandle(), INVALID_SOCKET);

	stdstring strServerIP = "218.204.255.116";
	rfc_uint_16 uPort = 25;
	tcpSocket.connectTo(strServerIP, uPort);
	assertTest(tcpSocket.isValid());
	assertTest(!tcpSocket.isClosed());
	assertGreater(tcpSocket.getHandle(), 2);

	tcpSocket.setHandle(12345);
	assertTest(tcpSocket.isValid());
	assertTest(!tcpSocket.isClosed());
	assertEqual(tcpSocket.getHandle(), 12345);

	tcpSocket.onlyClose();
	assertTest(!tcpSocket.isValid());
	assertTest(tcpSocket.isClosed());
	assertEqual(tcpSocket.getHandle(), INVALID_SOCKET);
}

RFC_NAMESPACE_END
