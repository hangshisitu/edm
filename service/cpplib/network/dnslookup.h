/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_DNSLOOKUP_H_201008
#define RFC_DNSLOOKUP_H_201008

#include "network/internetaddr.h"
#include "system/resourcepool.h"
#include "base/globalfunc.h"
#include "base/autoarray.h"
#include "base/exception.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>

#include <list>

RFC_NAMESPACE_BEGIN

enum DNSRecordType
{
	DNS_RECORD_TYPE_A			= T_A,
	DNS_RECORD_TYPE_AAAA		= T_AAAA,
	DNS_RECORD_TYPE_MX			= T_MX,
	DNS_RECORD_TYPE_PTR			= T_PTR,
	DNS_RECORD_TYPE_TXT			= T_TXT,

	DNS_RECORD_TYPE_CNAME		= T_CNAME,
	DNS_RECORD_TYPE_ANY			= T_ANY,
};

DECLARE_EXCEPTION(DNSLookupException, Exception)

class DNSRecordEntry
{
public:
	DNSRecordEntry(void) : m_RecordType(DNS_RECORD_TYPE_ANY), m_RecordClass(C_IN), m_TimeToLive(0), m_MXPreference(10) {}
	virtual ~DNSRecordEntry(void) {}

	GETER_SETER_MEMBER(DNSRecordType, RecordType);			//dns record type: T_A, T_AAAA, T_MX, T_PTR, T_TXT
	GETER_SETER_MEMBER(rfc_uint_16, RecordClass);			//class: C_IN
	GETER_SETER_MEMBER(rfc_uint_32, TimeToLive);			//TTL
	GETER_SETER_MEMBER(stdstring, RecordName);
	GETER_SETER_MEMBER(stdstring, RecordResult);
	GETER_SETER_MEMBER(rfc_uint_16, MXPreference);			// only for mx record
};

typedef std::list<DNSRecordEntry>	typeDNSEntryList;
class DNSRecord
{
public:
	DNSRecord(void) : m_nMinTimeToLive(0) {}
	~DNSRecord() { clearAll(); }

	rfc_uint_32						getMinTTL(void) const { return m_nMinTimeToLive; }
	void							clearAll(void);
	void							addEntry(const DNSRecordEntry & dnsEntry);
	const typeDNSEntryList &		getEntryList(void) const { return m_listDNSEntry; }
	void							getRecordString(stdstring & strInfo) const;
	void							shuffleMXRecord(void);

private:
	rfc_uint_32						m_nMinTimeToLive;		//所有Record中TTL的最小值
	typeDNSEntryList				m_listDNSEntry;
};

class QueryRBLResult
{
public:
	QueryRBLResult(void) : m_RBLResult(0), m_TimeToLive(0) {}
	GETER_SETER_MEMBER(stdstring, RBLOrgDomain);			// 匹配rbl的组织域名, olny for query rbl
	GETER_SETER_MEMBER(rfc_uint_32, RBLResult);				// 匹配rbl的结果,一般是127.x.x.x的整形值
	GETER_SETER_MEMBER(rfc_uint_32, TimeToLive);			//TTL
};
typedef std::list<QueryRBLResult>	typeQueryRBLResultList;

//////////////////////////////////////////////////////////////////////////

enum DNSLookupRetCode
{
	DNS_LOOKUP_OK					= 0,
	DNS_LOOKUP_FAIL,				//严重错误
	DNS_LOOKUP_RETRY,				//临时错误
	DNS_LOOKUP_NOTFOUND,			//记录不存在
};

// 实现DNS查询,线程安全
class DNSResolver : private NonCopyAble
{
public:
	enum DNSConnectType
	{
		DNS_CONNECT_TYPE_UDP,			//default is udp
		DNS_CONNECT_TYPE_TCP,
		DNS_CONNECT_TYPE_DEFAULT		= DNS_CONNECT_TYPE_UDP,
	};

	enum DNSLookupConstant
	{
		DNS_REPLY_SIZE_UDP				= 512,
		DNS_REPLY_SIZE_TCP				= 4096,
		DNS_REPLY_BUFFER_SIZE			= 4096,			//max(DNS_REPLY_SIZE_UDP, DNS_REPLY_SIZE_TCP)

		DNS_DOMAIN_NAME_LEN				= 1024,
		DNS_TXT_RR_MAX_LEN				= 65536,		/* RFC 1035 section 3.3.14 */
		IPV4_INET_ADDR_LEN				= 4,
		IPV6_INET6_ADDR_LEN				= 16,

		DNS_RESOLVER_TIMEOUT_DEFAULT	= RES_TIMEOUT,
		DNS_RETRY_COUNT_DEFAULT			= 2,
		DNS_OPTION_DEFAULT				= RES_DEFAULT,
		DNS_APPLY_TIMEOUT_DEFAULT		= 3,
	};

	DNSResolver(void);
	~DNSResolver(void);

	void				setApplyTimeout(const TimeValue & tvApplyTimeout) { m_tvApplyTimeout = tvApplyTimeout; }
	//strServerIPList以','或";"
	bool				setDNSServerList(size_t nResolverCount = 50,  const stdstring & strServerIPList = "",
										DNSConnectType nDNSConnectType = DNS_CONNECT_TYPE_UDP,
										size_t nResolverTimeout = DNS_RESOLVER_TIMEOUT_DEFAULT,
										size_t nRetryCount = DNS_RETRY_COUNT_DEFAULT);
	//strRBLOrgDomainList以','或";"
	bool				setRBLOrgDomainList(const stdstring & strRBLOrgDomainList);

	bool				isRBLBlackDomain(const stdstring & strDomain, typeQueryRBLResultList & listRBLResult);
	bool				isRBLBlackIP(const stdstring & strIP, typeQueryRBLResultList & listRBLResult);
	bool				isRBLBlackIP(const InternetAddrV4 & ipAddrV4, typeQueryRBLResultList & listRBLResult);

	// query A record
	DNSLookupRetCode	queryIPV4ARecord(const stdstring & strDomain, DNSRecord & dnsRecord);
	DNSLookupRetCode	queryIPV6ARecord(const stdstring & strDomain, DNSRecord & dnsRecord);

	// 根据域名查询MX记录,dnsRecord保存的结果可能还是域名
	DNSLookupRetCode	queryMXRecord(const stdstring & strDomain, DNSRecord & dnsRecord);
	DNSLookupRetCode	queryMXIPV4Record(const stdstring & strDomain, DNSRecord & dnsRecord, size_t nNeedARecordCount = 5);
	DNSLookupRetCode	queryMXIPV6Record(const stdstring & strDomain, DNSRecord & dnsRecord);

	DNSLookupRetCode	queryIPPTRRecord(const stdstring & strIP, DNSRecord & dnsRecord);

	DNSLookupRetCode	queryTXTRecord(const stdstring & strDomain, DNSRecord & dnsRecord);

	static bool			isValidRBLResult(rfc_uint_32 nIPInteger);

protected:
	static stdstring	convertPTRDomain(const stdstring & strIP);
	static rfc_uint_32	convertRBLResult(const typeDNSEntryList & listDNSEntry);

	int					getDNSReplyBufferSize(void) const
	{
		return ( m_nDNSConnectType == DNS_CONNECT_TYPE_UDP ) ? DNS_REPLY_SIZE_UDP : DNS_REPLY_SIZE_TCP;
	}

	rfc_uint_32			getRRNameMaxLen(DNSRecordType nDNSRecordType) const
	{
		return (nDNSRecordType == DNS_RECORD_TYPE_TXT) ? DNS_TXT_RR_MAX_LEN : DNS_DOMAIN_NAME_LEN;
	}

	//return new_query_start if success, or else return NULL
	rfc_uint_8 *		expandDomainName(rfc_uint_8 * pQueryBuffer, rfc_uint_8 * pQueryBufferEnd, rfc_uint_8 * pScanBufferPos,
										char * pExpandDomain, int nExpandDomainLen);
	bool				getDNSReocrdEntry(rfc_uint_8 * pQueryBuffer, rfc_uint_8 * pQueryBufferEnd, rfc_uint_8 * pScanBufferPos,
										  rfc_uint_16 nLength, DNSRecordEntry & dnsEntry);
	DNSLookupRetCode	parseDNSReply(rfc_uint_8 * pBuffer, rfc_uint_8 * pBufferEnd, DNSRecord & dnsRecord);
	
	DNSLookupRetCode	dnsLookup(const stdstring & strDomain, DNSRecordType nDNSRecordType, DNSRecord & dnsRecord);

	bool				isRBLBlackIP(const stdstring & strReverseIP, bool bIPV6, typeQueryRBLResultList & listRBLResult);

	typedef struct __res_state		typeDNSResolver;
	typedef std::list<stdstring>	typeRBLOrgDomainList;

	DNSConnectType					m_nDNSConnectType;		// udp or tcp
	size_t							m_nResolverTimeout;
	size_t							m_nRetryCount;
	TimeValue						m_tvApplyTimeout;
	ResourcePool					m_ResourcePool;
	AutoArray<typeDNSResolver>		m_arResolverList;
	typeRBLOrgDomainList			m_listRBLOrgDomain;

#if (__RES < 19991006)
	static MutexLock				m_lockResolver;		// 旧版本的resolve库并不是线程安全的
#endif//__RES < 19991006
};

RFC_NAMESPACE_END

#endif	//RFC_DNSLOOKUP_H_201008
