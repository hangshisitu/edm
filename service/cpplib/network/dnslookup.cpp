/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "network/dnslookup.h"
#include "network/networkfunc.h"
#include "base/stringtokenizer.h"
#include "base/formatstream.h"
#include <vector>

RFC_NAMESPACE_BEGIN

// 越小越优先
struct MXEntryCompare
{
	bool operator()(const DNSRecordEntry & mx1, const DNSRecordEntry & mx2 ) const
	{
		return ( mx1.getMXPreference() < mx2.getMXPreference() );
	}
};

struct MXShuffleRandom
{
	MXShuffleRandom(size_t nTotalRecordCount) : m_nRandomSeed(time(NULL)), m_nTotalRecordCount(nTotalRecordCount) {}
	size_t operator()(size_t) { return GlobalFunc::threadSafeRand( &m_nRandomSeed ) % m_nTotalRecordCount; }
private:
	rfc_uint_32			m_nRandomSeed;
	size_t				m_nTotalRecordCount;
};

void DNSRecord::clearAll(void)
{
	m_nMinTimeToLive = 0;
	m_listDNSEntry.clear();
}

void DNSRecord::addEntry(const DNSRecordEntry & dnsEntry)
{
	if ( m_listDNSEntry.empty() || dnsEntry.getTimeToLive() < m_nMinTimeToLive )
		m_nMinTimeToLive = dnsEntry.getTimeToLive();
	m_listDNSEntry.push_back( dnsEntry );
}

void DNSRecord::getRecordString(stdstring & strInfo) const
{
	strInfo.clear();
	typeDNSEntryList::const_iterator it, itEnd = m_listDNSEntry.end();
	for ( it = m_listDNSEntry.begin(); it != itEnd; ++it )
	{
		strInfo += it->getRecordResult();
		strInfo += ',';
	} // for ( it = m_listDNSEntry.begin(); it != itEnd; ++it )
	if ( !strInfo.empty() && *strInfo.rbegin() == ',' )
		strInfo.erase(strInfo.size() - 1);
}

void DNSRecord::shuffleMXRecord(void)
{
	if ( m_listDNSEntry.size() < 2 )
		return;

	std::vector<DNSRecordEntry>	vecMXEntry;
	std::copy(m_listDNSEntry.begin(), m_listDNSEntry.end(), std::back_inserter(vecMXEntry));
	MXShuffleRandom mxShuffleRandom(vecMXEntry.size());
	std::random_shuffle(vecMXEntry.begin(), vecMXEntry.end(), mxShuffleRandom);
	m_listDNSEntry.clear();
	std::copy(vecMXEntry.begin(), vecMXEntry.end(), std::back_inserter(m_listDNSEntry));
	m_listDNSEntry.sort( MXEntryCompare() );
}

//////////////////////////////////////////////////////////////////////////

#if (__RES < 19991006)
MutexLock DNSResolver::m_lockResolver;		// 旧版本的resolve库并不是线程安全的
#endif//__RES < 19991006

DNSResolver::DNSResolver(void) : m_nDNSConnectType(DNS_CONNECT_TYPE_DEFAULT), m_nResolverTimeout(DNS_RESOLVER_TIMEOUT_DEFAULT),
m_nRetryCount(DNS_RETRY_COUNT_DEFAULT), m_tvApplyTimeout(DNS_APPLY_TIMEOUT_DEFAULT, 0), m_ResourcePool(0), m_arResolverList(NULL)
{
}

DNSResolver::~DNSResolver(void)
{
	if ( m_arResolverList.get() != NULL )
	{
#if !(__RES < 19991006)
		for ( size_t i = 0; i < m_ResourcePool.getTotalCount(); ++i )
			res_nclose( &(m_arResolverList[i]) );
#endif//!(__RES < 19991006)
	} // if ( m_arResolverList.get() != NULL )
}

bool DNSResolver::setDNSServerList(size_t nResolverCount, const stdstring & strServerIPList, DNSConnectType nDNSConnectType,
									size_t nResolverTimeout, size_t nRetryCount)
{
	if ( nResolverCount <= 0 )
		return false;

	m_nDNSConnectType = nDNSConnectType;
	m_nResolverTimeout = nResolverTimeout;
	m_nRetryCount = nRetryCount;
	m_ResourcePool.resize(nResolverCount);
	m_arResolverList.reset( new typeDNSResolver[nResolverCount] );

	unsigned long nOptions = RES_DEFAULT;
	if ( nDNSConnectType == DNS_CONNECT_TYPE_TCP )
		nOptions |= (RES_USEVC | RES_STAYOPEN);
	for ( size_t i = 0; i < nResolverCount; ++i )
	{
#if (__RES < 19991006)
		AutoMutexLock auLock(m_lockResolver);
		if ( res_init() < 0 )
			return false;
		auLock.release();
#else
		memset(&m_arResolverList[i], 0, sizeof(typeDNSResolver));
		if ( res_ninit( &(m_arResolverList[i]) ) < 0 )
			return false;
#endif

		m_arResolverList[i].retrans = m_nResolverTimeout;
		m_arResolverList[i].retry = m_nRetryCount;
		m_arResolverList[i].options = nOptions;

		// Init Server IP List if necessary
		if ( !strServerIPList.empty() )
		{
			// Reset nsaddr_list first
			for ( int nNameServer = 0; nNameServer < MAXNS; ++nNameServer )
			{
				m_arResolverList[i].nsaddr_list[nNameServer].sin_family = AF_INET;
				m_arResolverList[i].nsaddr_list[nNameServer].sin_port = htons(NAMESERVER_PORT);
				m_arResolverList[i].nsaddr_list[nNameServer].sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			}

			m_arResolverList[i].nscount = 0;
			stdstring					strServerIP;
			StringTokenizer<stdstring>		tokenServerList(strServerIPList.data(), strServerIPList.size());
			while ( tokenServerList.getNext(strServerIP, ",;") && m_arResolverList[i].nscount < MAXNS )
			{
				m_arResolverList[i].nsaddr_list[m_arResolverList[i].nscount].sin_addr.s_addr = inet_addr(strServerIP.c_str());
				if ( m_arResolverList[i].nsaddr_list[m_arResolverList[i].nscount].sin_addr.s_addr == INADDR_NONE )
					return false;
				++m_arResolverList[i].nscount;
			}
		} // if ( !strServerIPList.empty() )
	} //for ( size_t i = 0; i < nResolverCount; ++i )

	return true;
}

bool DNSResolver::setRBLOrgDomainList(const stdstring & strRBLOrgDomainList)
{
	stdstring					strRBLOrgDomain;
	StringTokenizer<stdstring>	tokenRBLDomainList(strRBLOrgDomainList.data(), strRBLOrgDomainList.size());
	while ( tokenRBLDomainList.getNext(strRBLOrgDomain, ",; \t") )
	{
		if ( !NetworkFunc::isValidHostName(strRBLOrgDomain.c_str()) )
			return false;
		StringFunc::toLower(strRBLOrgDomain);
		if ( std::find(m_listRBLOrgDomain.begin(), m_listRBLOrgDomain.end(), strRBLOrgDomain) == m_listRBLOrgDomain.end() )
			m_listRBLOrgDomain.push_back(strRBLOrgDomain);
	}
	return true;
}

stdstring DNSResolver::convertPTRDomain(const stdstring & strIP)
{
	stdstring strReverseIP;
	NetworkFunc::getIPV4Reverse(strIP, strReverseIP);
	return strReverseIP + ".in-addr.arpa";
}

bool DNSResolver::isValidRBLResult(rfc_uint_32 nIPInteger)
{
	static const rfc_uint_32 g_nLocalhostBeginIP = 0x7F000001;			// 127.0.0.1
	static const rfc_uint_32 g_nLocalhostEndIP = 0x7F00FFFF;			// 127.0.255.255
	return ( nIPInteger >= g_nLocalhostBeginIP && nIPInteger <= g_nLocalhostEndIP );
}

rfc_uint_32 DNSResolver::convertRBLResult(const typeDNSEntryList & listDNSEntry)
{
	rfc_uint_32 nRBLResult = 0, nIPInteger = 0;
	typeDNSEntryList::const_iterator it, itEnd = listDNSEntry.end();
	for ( it = listDNSEntry.begin(); it != itEnd; ++it )
	{
		if ( !NetworkFunc::getIPV4Integer(it->getRecordResult().c_str(), nIPInteger) )
			continue;
		if ( !isValidRBLResult(nIPInteger) )
			continue;
		nRBLResult |= nIPInteger;
	} // for ( it = listDNSEntry.begin(); it != itEnd; ++it )
	return nRBLResult;
}

rfc_uint_8 * DNSResolver::expandDomainName(rfc_uint_8 * pQueryBuffer, rfc_uint_8 * pQueryBufferEnd, rfc_uint_8 * pScanBufferPos,
										   char * pExpandDomain, int nExpandDomainLen)
{
	if ( pScanBufferPos >= pQueryBufferEnd )
		return NULL;

	int nCompressNameLen = dn_expand(pQueryBuffer, pQueryBufferEnd, pScanBufferPos, pExpandDomain, nExpandDomainLen);
	if ( nCompressNameLen < 0 )
		return NULL;
	pScanBufferPos += nCompressNameLen;
	return pScanBufferPos;
}

bool DNSResolver::getDNSReocrdEntry(rfc_uint_8 * pQueryBuffer, rfc_uint_8 * pQueryBufferEnd, rfc_uint_8 * pScanBufferPos,
									rfc_uint_16 nLength, DNSRecordEntry & dnsEntry)
{
	if ( pScanBufferPos + nLength > pQueryBufferEnd )
		return false;

	DNSRecordType nDNSType = dnsEntry.getRecordType();
	rfc_uint_32 nRRNameMaxLen = getRRNameMaxLen(dnsEntry.getRecordType());
	if ( nDNSType == DNS_RECORD_TYPE_A )
	{
		if ( nLength != IPV4_INET_ADDR_LEN || nLength > nRRNameMaxLen )
			return false;
		dnsEntry.setRecordResult(FormatString("%.%.%.%").arg(int(pScanBufferPos[0])).arg(int(pScanBufferPos[1]))
			.arg(int(pScanBufferPos[2])).arg(int(pScanBufferPos[3])).str());
		return true;
	}

	if ( nDNSType != DNS_RECORD_TYPE_MX && nDNSType != DNS_RECORD_TYPE_PTR && nDNSType != DNS_RECORD_TYPE_TXT )
		return false;

	AutoArray<char> auRRName( new char[nRRNameMaxLen] );
	memset(auRRName.get(), 0, nRRNameMaxLen);
	if ( nDNSType == DNS_RECORD_TYPE_TXT )
	{
		char * pData = auRRName.get();
		const char * pDataEnd = pData + GlobalFunc::getMin<rfc_uint_32>(nLength, nRRNameMaxLen);
		for ( ++pScanBufferPos; pData < pDataEnd; ++pData, ++pScanBufferPos )
		{
			if ( ::isprint(*pScanBufferPos) )
				*pData = *pScanBufferPos;
			else
				*pData = ' ';
		}
		*pData = RFC_CHAR_NULL;
	}
	else
	{
		if ( nDNSType == DNS_RECORD_TYPE_MX )
		{
			rfc_uint_16 nPreference = 0;
			GETSHORT(nPreference, pScanBufferPos);
			dnsEntry.setMXPreference(nPreference);
		}
		pScanBufferPos = expandDomainName(pQueryBuffer, pQueryBufferEnd, pScanBufferPos, auRRName.get(), nRRNameMaxLen);
		if ( pScanBufferPos == NULL )
			return false;
		if ( nDNSType == DNS_RECORD_TYPE_MX && !NetworkFunc::isValidHostAddr(auRRName.get()) && !NetworkFunc::isValidHostName(auRRName.get()) )
			return false;
	}
	dnsEntry.setRecordResult(stdstring(auRRName.get()));
	return true;
}

DNSLookupRetCode DNSResolver::parseDNSReply(rfc_uint_8 * pBuffer, rfc_uint_8 * pBufferEnd, DNSRecord & dnsRecord)
{
	AutoArray<char> auDomainName( new char[DNS_DOMAIN_NAME_LEN] );
	memset(auDomainName.get(), 0, DNS_DOMAIN_NAME_LEN);
	HEADER * pReplyHeader = (HEADER *) pBuffer;
	rfc_uint_8 * pScanBufferPos = pBuffer + sizeof(HEADER);
	for ( int nQueryCount = ntohs(pReplyHeader->qdcount); nQueryCount > 0; --nQueryCount )
	{
		pScanBufferPos = expandDomainName(pBuffer, pBufferEnd, pScanBufferPos, auDomainName.get(), DNS_DOMAIN_NAME_LEN);
		if ( pScanBufferPos == NULL )
			return DNS_LOOKUP_FAIL;
		pScanBufferPos += QFIXEDSZ;
	}

	DNSLookupRetCode nDNSLookupRetCode = DNS_LOOKUP_NOTFOUND;
	DNSRecordEntry dnsEntry;
	for ( int nAnswerCount = ntohs(pReplyHeader->ancount); nAnswerCount > 0; --nAnswerCount )
	{
		pScanBufferPos = expandDomainName(pBuffer, pBufferEnd, pScanBufferPos, auDomainName.get(), DNS_DOMAIN_NAME_LEN);
		if ( pScanBufferPos == NULL )
			return DNS_LOOKUP_FAIL;

		// 获取固定位置的几个属性: type, class, ttl, length
		if ( pScanBufferPos + RRFIXEDSZ > pBufferEnd )
			return DNS_LOOKUP_FAIL;
		rfc_uint_16	nDNSReplyType = 0, nRecordClass = 0, nLength = 0;
		rfc_uint_32 nTimeToLive = 0;
		GETSHORT(nDNSReplyType, pScanBufferPos);
		GETSHORT(nRecordClass, pScanBufferPos);
		GETLONG(nTimeToLive, pScanBufferPos);
		GETSHORT(nLength, pScanBufferPos);
		if ( pScanBufferPos + nLength > pBufferEnd )
			return DNS_LOOKUP_FAIL;
		dnsEntry.setRecordType( static_cast<DNSRecordType>(nDNSReplyType) );
		dnsEntry.setRecordClass(nRecordClass);
		dnsEntry.setTimeToLive(nTimeToLive);
		dnsEntry.setRecordName(stdstring(auDomainName.get()));
		if ( getDNSReocrdEntry(pBuffer, pBufferEnd, pScanBufferPos, nLength, dnsEntry) )
		{
			nDNSLookupRetCode = DNS_LOOKUP_OK;
			dnsRecord.addEntry(dnsEntry);
		}		
		pScanBufferPos += nLength;
	} //for ( int nAnswerCount = ntohs(pReplyHeader->ancount); nAnswerCount > 0; --nAnswerCount )

	return nDNSLookupRetCode;
}

DNSLookupRetCode DNSResolver::dnsLookup(const stdstring & strDomain, DNSRecordType nDNSRecordType, DNSRecord & dnsRecord)
{
	if ( strDomain.empty() )
		return DNS_LOOKUP_FAIL;
	if ( nDNSRecordType != DNS_RECORD_TYPE_A // && nDNSRecordType != DNS_RECORD_TYPE_AAAA
		&& nDNSRecordType != DNS_RECORD_TYPE_MX && nDNSRecordType != DNS_RECORD_TYPE_PTR && nDNSRecordType != DNS_RECORD_TYPE_TXT )
		return DNS_LOOKUP_FAIL;

	SafePoolAgent<ResourcePool>	auApplyResource(m_ResourcePool);
	size_t nIndex = auApplyResource.apply(m_tvApplyTimeout);
	if ( nIndex == ResourcePool::npos )
		return DNS_LOOKUP_RETRY;

	AutoArray<rfc_uint_8> auReplyBuffer( new rfc_uint_8[DNS_REPLY_BUFFER_SIZE] );
	memset(auReplyBuffer.get(), 0, DNS_REPLY_BUFFER_SIZE);
#if (__RES < 19991006)
	AutoMutexLock auLock(m_lockResolver);
	int nReplyLen = res_query(strDomain.c_str(), C_IN, nDNSRecordType, auReplyBuffer.get(), getDNSReplyBufferSize());
	int nQueryErrorNo = h_errno;
#else
	int nReplyLen = res_nquery(&(m_arResolverList[nIndex]), strDomain.c_str(), C_IN, nDNSRecordType, auReplyBuffer.get(), getDNSReplyBufferSize());
	int nQueryErrorNo = m_arResolverList[nIndex].res_h_errno;
#endif

	if ( nReplyLen < 0 )
	{
		switch ( nQueryErrorNo )
		{
		case HOST_NOT_FOUND:
		case NO_DATA:
			return DNS_LOOKUP_NOTFOUND;

		case NO_RECOVERY:
			return DNS_LOOKUP_FAIL;

		default:
			return DNS_LOOKUP_RETRY;
		}
	} //if ( nReplyLen <= 0 )

	rfc_uint_8 * pReplyBufferEnd = auReplyBuffer.get() + GlobalFunc::getMin<int>(nReplyLen, DNS_REPLY_BUFFER_SIZE);
	DNSLookupRetCode nRetCode = parseDNSReply(auReplyBuffer.get(), pReplyBufferEnd, dnsRecord);
	if ( nRetCode != DNS_LOOKUP_OK )
		return nRetCode;
	return ( dnsRecord.getEntryList().empty() ? DNS_LOOKUP_NOTFOUND : DNS_LOOKUP_OK );
}

bool DNSResolver::isRBLBlackIP(const stdstring & strReverseIP, bool bIPV6, typeQueryRBLResultList & listRBLResult)
{
	if ( m_listRBLOrgDomain.empty() )
		return false;

	bool bMatchRBL = false;
	DNSRecord dnsRecord;
	DNSRecordType nDNSType = ( bIPV6 ? DNS_RECORD_TYPE_AAAA : DNS_RECORD_TYPE_A );
	typeRBLOrgDomainList::const_iterator it, itEnd = m_listRBLOrgDomain.end();
	for ( it = m_listRBLOrgDomain.begin(); it != itEnd; ++it )
	{
		stdstring strQueryRBLDomain = strReverseIP;
		strQueryRBLDomain += '.';
		strQueryRBLDomain += ( *it );
		dnsRecord.clearAll();
		if ( dnsLookup(strQueryRBLDomain, nDNSType, dnsRecord) != DNS_LOOKUP_OK )
			continue;

		rfc_uint_32 nRBLResult = convertRBLResult(dnsRecord.getEntryList());
		if ( nRBLResult != 0 )			// 可能返回某些非RBL结果的IP, 所以检查合法性
		{
			QueryRBLResult rblResult;
			rblResult.setRBLOrgDomain( *it );
			rblResult.setRBLResult( nRBLResult );
			rblResult.setTimeToLive( dnsRecord.getMinTTL() );
			listRBLResult.push_back(rblResult);
			bMatchRBL = true;
		}
	} //for ( it = m_listRBLOrgDomain.begin(); it != itEnd; ++it )
	return bMatchRBL;
}

bool DNSResolver::isRBLBlackDomain(const stdstring & strDomain, typeQueryRBLResultList & listRBLResult)
{
	if ( m_listRBLOrgDomain.empty() )
		return false;

	DNSRecord domainARecord;
	DNSLookupRetCode nRet = dnsLookup(strDomain, DNS_RECORD_TYPE_A, domainARecord);
	if ( nRet != DNS_LOOKUP_OK )
		return false;
	stdstring strReverseIP;
	typeDNSEntryList::const_iterator it, itEnd = domainARecord.getEntryList().end();
	for ( it = domainARecord.getEntryList().begin(); it != itEnd; ++it )
	{
		if ( !NetworkFunc::getIPV4Reverse(it->getRecordResult(), strReverseIP) )
			continue;
		if ( isRBLBlackIP(strReverseIP, false, listRBLResult) )
			return true;
	} // for ( it = domainARecord.getEntryList().begin(); it != itEnd; ++it )
	return false;
}

bool DNSResolver::isRBLBlackIP(const stdstring & strIP, typeQueryRBLResultList & listRBLResult)
{
	stdstring strReverseIP;
	if ( !NetworkFunc::getIPV4Reverse(strIP, strReverseIP) )
		return false;
	return isRBLBlackIP(strReverseIP, false, listRBLResult);
}

bool DNSResolver::isRBLBlackIP(const InternetAddrV4 & ipAddrV4, typeQueryRBLResultList & listRBLResult)
{
	stdstring strReverseIP;
	ipAddrV4.getReverseIPAddress(strReverseIP);
	return isRBLBlackIP(strReverseIP, false, listRBLResult);
}

DNSLookupRetCode DNSResolver::queryIPV4ARecord(const stdstring & strDomain, DNSRecord & dnsRecord)
{
	return dnsLookup(strDomain, DNS_RECORD_TYPE_A, dnsRecord);
}

DNSLookupRetCode DNSResolver::queryIPV6ARecord(const stdstring & strDomain, DNSRecord & dnsRecord)
{
	return dnsLookup(strDomain, DNS_RECORD_TYPE_AAAA, dnsRecord);
}

DNSLookupRetCode DNSResolver::queryMXRecord(const stdstring & strDomain, DNSRecord & dnsRecord)
{
	return dnsLookup(strDomain, DNS_RECORD_TYPE_MX, dnsRecord);
}

DNSLookupRetCode DNSResolver::queryMXIPV4Record(const stdstring & strDomain, DNSRecord & dnsRecord, size_t nNeedARecordCount)
{
	DNSRecord mxRecord;
	DNSLookupRetCode nRet = dnsLookup(strDomain, DNS_RECORD_TYPE_MX, mxRecord);
	if ( nRet != DNS_LOOKUP_OK )
		return nRet;

	mxRecord.shuffleMXRecord();
	const typeDNSEntryList & listMXRecord = mxRecord.getEntryList();
	typeDNSEntryList::const_iterator it, itEnd = listMXRecord.end();
	for ( it = listMXRecord.begin(); it != itEnd; ++it )
	{
		const stdstring & strHost = it->getRecordResult();
		if ( NetworkFunc::isValidHostAddr(strHost.c_str()) )
			dnsRecord.addEntry(*it);
		else
			nRet = dnsLookup(strHost, DNS_RECORD_TYPE_A, dnsRecord);
		if ( dnsRecord.getEntryList().size() >= nNeedARecordCount )
			break;
	} // for ( it = listMXRecord.begin(); it != itEnd; ++it )

	if ( dnsRecord.getEntryList().empty() )
		return ( nRet == DNS_LOOKUP_OK ? DNS_LOOKUP_NOTFOUND : nRet );
	return DNS_LOOKUP_OK;
}

DNSLookupRetCode DNSResolver::queryIPPTRRecord(const stdstring & strIP, DNSRecord & dnsRecord)
{
	return dnsLookup(convertPTRDomain(strIP), DNS_RECORD_TYPE_PTR, dnsRecord);
}

DNSLookupRetCode DNSResolver::queryTXTRecord(const stdstring & strDomain, DNSRecord & dnsRecord)
{
	return dnsLookup(strDomain, DNS_RECORD_TYPE_TXT, dnsRecord);
}

RFC_NAMESPACE_END

