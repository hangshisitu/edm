/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_INTERNETADDR_H_201008
#define RFC_INTERNETADDR_H_201008

#include "base/string.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

RFC_NAMESPACE_BEGIN

class InternetAddrV4
{
public:
	typedef	rfc_uint_32			typeIPInteger;

	static InternetAddrV4		g_AddressNone;
	static InternetAddrV4		g_AddressAny;
	static InternetAddrV4		g_AddressBroadcast;
	static InternetAddrV4		g_AddressMin;
	static InternetAddrV4		g_AddressMax;
	static InternetAddrV4		g_AddressLocalHost;

	InternetAddrV4(void);
	InternetAddrV4(const InternetAddrV4 & ipv4Addr);
	explicit InternetAddrV4(const struct in_addr & inetAddr);
	explicit InternetAddrV4(typeIPInteger nHostOrderIP);
	explicit InternetAddrV4(const char * lpszIP);

	inline bool					isValid(void) const { return m_inetAddr.s_addr != static_cast<typeIPInteger>(INADDR_NONE); }

	void						setInetAddress(const struct in_addr & inetAddr);
	void						setIPInteger(typeIPInteger nIP);
	bool						setIPAddress(const char * lpszIP);

	inline const in_addr &		getInetAddress(void) const { return m_inetAddr; }
	typeIPInteger				getIPInteger(void) const;
	stdstring					getIPAddress(void) const;
	void						getIPAddress(stdstring & strIP) const;
	stdstring					getReverseIPAddress(void) const;
	void						getReverseIPAddress(stdstring & strReverseIP) const;

	void						setSubnetMask(size_t nBits, bool bSetSubnetTo1 = false);

	InternetAddrV4 &			operator = (const InternetAddrV4 & ipv4Addr);
	bool operator				==(const InternetAddrV4 & ipv4Addr) const { return compare(ipv4Addr) == 0; }
	bool operator				!=(const InternetAddrV4 & ipv4Addr) const { return compare(ipv4Addr) != 0; }
	bool operator				< (const InternetAddrV4 & ipv4Addr) const { return compare(ipv4Addr) < 0; }
	bool operator				>=(const InternetAddrV4 & ipv4Addr) const { return compare(ipv4Addr) >= 0; }
	bool operator				> (const InternetAddrV4 & ipv4Addr) const { return compare(ipv4Addr) > 0; }
	bool operator				<=(const InternetAddrV4 & ipv4Addr) const { return compare(ipv4Addr) <= 0; }

protected:
	int							compare(const InternetAddrV4 & ipv4Addr) const;

private:
	struct	in_addr				m_inetAddr;
};

typedef InternetAddrV4			typeDefaultINetIP;

RFC_NAMESPACE_END

#endif	//RFC_INTERNETADDR_H_201008
