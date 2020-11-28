/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "network/internetaddr.h"
#include "network/networkfunc.h"
#include "base/byteorderfunc.h"
#include "base/globalfunc.h"

RFC_NAMESPACE_BEGIN

InternetAddrV4 InternetAddrV4::g_AddressNone( static_cast<InternetAddrV4::typeIPInteger>(INADDR_NONE) );
InternetAddrV4 InternetAddrV4::g_AddressAny( static_cast<InternetAddrV4::typeIPInteger>(INADDR_ANY) );
InternetAddrV4 InternetAddrV4::g_AddressBroadcast( static_cast<InternetAddrV4::typeIPInteger>(INADDR_BROADCAST) );
InternetAddrV4 InternetAddrV4::g_AddressMin( static_cast<InternetAddrV4::typeIPInteger>(0) );
InternetAddrV4 InternetAddrV4::g_AddressMax( static_cast<InternetAddrV4::typeIPInteger>(0xffffffff) );
InternetAddrV4 InternetAddrV4::g_AddressLocalHost( static_cast<InternetAddrV4::typeIPInteger>(0x7f000001) );	//127.0.0.1

InternetAddrV4::InternetAddrV4(void)
{
	m_inetAddr.s_addr = INADDR_NONE;
}

InternetAddrV4::InternetAddrV4(const InternetAddrV4 & ipv4Addr)
{
	setInetAddress(ipv4Addr.m_inetAddr);
}

InternetAddrV4::InternetAddrV4(const struct in_addr & inetAddr)
{
	setInetAddress(inetAddr);
}

InternetAddrV4::InternetAddrV4(InternetAddrV4::typeIPInteger nHostOrderIP)
{
	setIPInteger(nHostOrderIP);
}

InternetAddrV4::InternetAddrV4(const char * lpszIP)
{
	setIPAddress(lpszIP);
}

void InternetAddrV4::setInetAddress(const struct in_addr & inetAddr)
{
	m_inetAddr.s_addr = inetAddr.s_addr;
}

void InternetAddrV4::setIPInteger(InternetAddrV4::typeIPInteger nIP)
{
	m_inetAddr.s_addr = ByteOrderFunc::hton( nIP );
}

bool InternetAddrV4::setIPAddress(const char * lpszIP)
{
	m_inetAddr.s_addr = INADDR_NONE;
	rfc_uint_32 nIPInteger = 0;
	if ( !NetworkFunc::getIPV4Integer(lpszIP, nIPInteger) )
		return false;
	m_inetAddr.s_addr = ByteOrderFunc::hton( nIPInteger );
	return true;
}

InternetAddrV4::typeIPInteger InternetAddrV4::getIPInteger(void) const
{
	return ByteOrderFunc::ntoh( static_cast<typeIPInteger>(m_inetAddr.s_addr) );
}

stdstring InternetAddrV4::getIPAddress(void) const
{
	stdstring strIP;
	NetworkFunc::getIPV4String(getIPInteger(), strIP);
	return strIP;
}

void InternetAddrV4::getIPAddress(stdstring & strIP) const
{
	NetworkFunc::getIPV4String(getIPInteger(), strIP);
}

stdstring InternetAddrV4::getReverseIPAddress(void) const
{
	stdstring strReverseIP;
	NetworkFunc::getIPV4ReverseString(getIPInteger(), strReverseIP);
	return strReverseIP;
}

void InternetAddrV4::getReverseIPAddress(stdstring & strReverseIP) const
{
	NetworkFunc::getIPV4ReverseString(getIPInteger(), strReverseIP);
}

void InternetAddrV4::setSubnetMask(size_t nBits, bool bSetSubnetTo1)
{
	if ( nBits > 32 )
		nBits = 32;
	typeIPInteger nIP = getIPInteger();
	if ( bSetSubnetTo1 )
		nIP |= ( GlobalConstant::g_uMaxUInt32 >> nBits );
	else
		nIP &= ( GlobalConstant::g_uMaxUInt32 << (32 - nBits) );
	setIPInteger(nIP);
}

InternetAddrV4 & InternetAddrV4::operator = (const InternetAddrV4 & ipv4Addr)
{
	if ( this != &ipv4Addr )
		m_inetAddr.s_addr = ipv4Addr.m_inetAddr.s_addr;
	return *this;
}

int InternetAddrV4::compare(const InternetAddrV4 & ipv4Addr) const
{
	typeIPInteger n1 = getIPInteger();
	typeIPInteger n2 = ipv4Addr.getIPInteger();
	if ( n1 == n2 )
		return 0;
	return ( n1 > n2 ? 1 : -1 );
}

RFC_NAMESPACE_END
