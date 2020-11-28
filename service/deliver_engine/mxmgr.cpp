/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-08      */

#include "mxmgr.h"

RFC_NAMESPACE_BEGIN

//extern FileLog fileLog;

bool MxMgr::MXRecord::operator == (const MXRecord & aMXRecord) const
{
	return ( m_nPort == aMXRecord.m_nPort && m_strIP == aMXRecord.m_strIP );
}

MxMgr::MxMgr(void) : m_tDNSDefaultTTL(600)
{
}

bool MxMgr::onInit(const ConfigureMgr & conf)
{
	m_tDNSDefaultTTL = static_cast<time_t>( conf.getInt(600, "/iedm/application/DNSTimeToLive") );
	m_DNSResolver.setDNSServerList();

	typeConfPairList listTransPort;
	conf.getSubItemPairList("/iedm/transport", listTransPort);
	MXRecord aMXRecord;
	for ( typeConfPairList::const_iterator it = listTransPort.begin(); it != listTransPort.end(); ++it )
	{
		typeMXRecordList & listMXRecord = m_mapTransport[it->first].m_listMXRecord;
		aMXRecord.m_strIP = it->second;
		aMXRecord.m_nPort = 25;
		size_t nPortPos = aMXRecord.m_strIP.find(':');
		if ( nPortPos != stdstring::npos )
		{
			aMXRecord.m_nPort = StringFunc::stringToInt<rfc_uint_16>(aMXRecord.m_strIP.data() + nPortPos + 1);
			aMXRecord.m_strIP.erase(nPortPos);
		}

		// ¿¿¿¿¿¿¿¿¿¿
		typeMXRecordList::const_iterator itExist = std::find(listMXRecord.begin(), listMXRecord.end(), aMXRecord);
		if ( itExist == listMXRecord.end() )
			listMXRecord.push_back(aMXRecord);
	} // for ( typeConfPairList::const_iterator it = listTransPort.begin(); it != listTransPort.end(); ++it )
	return true;
}

DNSLookupRetCode MxMgr::getMXRecord(const stdstring & strDomain, stdstring & strIP, rfc_uint_16 & nPort, size_t nIndex)
{
	if ( strDomain.empty() )
	{
		return DNS_LOOKUP_NOTFOUND;
	}

	// ¿¿¿transport¿¿,¿¿transport¿¿¿¿¿dns¿¿
	DNSLookupRetCode nRetCode = getFromCache(m_mapTransport, strDomain, strIP, nPort, nIndex);
	if ( nRetCode != DNS_LOOKUP_RETRY )
	{
		return nRetCode;
	}

	AutoReadLock auReadLock(m_DNSRecordLock);
	nRetCode = getFromCache(m_mapDNSRecord, strDomain, strIP, nPort, nIndex);
	if ( nRetCode != DNS_LOOKUP_RETRY )
	{
		return nRetCode;
	}
	auReadLock.release();

	// ¿¿¿¿¿¿¿¿¿¿¿cache,¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿DNS
	AutoWriteLock auWriteLock(m_DNSRecordLock);
	nRetCode = getFromCache(m_mapDNSRecord, strDomain, strIP, nPort, nIndex);
	if ( nRetCode != DNS_LOOKUP_RETRY )
	{
		return nRetCode;
	}

	MXRecordCacheNode aDNSRecordNode;
	queryDNSRecord(strDomain, aDNSRecordNode);
	MXRecordCacheNode & aCacheNode = m_mapDNSRecord[strDomain];
	aCacheNode.m_tExpiredTime = time(NULL) + m_tDNSDefaultTTL;
	aCacheNode.m_listMXRecord.swap(aDNSRecordNode.m_listMXRecord);

    //FIXBUG
    if (aCacheNode.m_listMXRecord.size() < 1)
        return DNS_LOOKUP_NOTFOUND;

	nIndex = nIndex % (aCacheNode.m_listMXRecord.size());
	if ( aCacheNode.m_listMXRecord.size() <= nIndex )
		return DNS_LOOKUP_NOTFOUND;	// ¿¿¿¿¿¿
	strIP = aCacheNode.m_listMXRecord[nIndex].m_strIP;
	nPort = aCacheNode.m_listMXRecord[nIndex].m_nPort;
	return DNS_LOOKUP_OK;
}

DNSLookupRetCode MxMgr::getFromCache(const MxMgr::typeMxRecordMap & mapCache, const stdstring & strDomain,
									 stdstring & strIP, rfc_uint_16 & nPort, size_t nIndex)
{
	typeMxRecordMap::const_iterator it = mapCache.find(strDomain);
	if ( it == mapCache.end() ) 
        it = mapCache.find("other"); 

	if ( it == mapCache.end() )
		return DNS_LOOKUP_RETRY;	// ¿¿¿¿?¿¿¿DNS¿¿¿¿

	const MXRecordCacheNode & aCacheNode = it->second;
	if ( aCacheNode.m_tExpiredTime > 0 && aCacheNode.m_tExpiredTime < time(NULL) )
		return DNS_LOOKUP_RETRY;	// ¿¿¿¿?¿¿¿DNS¿¿¿¿

	size_t nTotalMxRecordCount = aCacheNode.m_listMXRecord.size();

    //FIXBUG
    if (nTotalMxRecordCount < 1)
        return DNS_LOOKUP_NOTFOUND;

	nIndex = nIndex % nTotalMxRecordCount;
	if ( nTotalMxRecordCount <= nIndex )
		return DNS_LOOKUP_NOTFOUND;	// ¿¿¿¿¿¿

	if ( nTotalMxRecordCount > 1 )	// ¿¿¿IP,¿¿¿¿¿¿
	{
		rfc_uint_32 pRandomSeed = static_cast<rfc_uint_32>( time(NULL) + nIndex * 1000 );
		nIndex = GlobalFunc::threadSafeRand(&pRandomSeed) % nTotalMxRecordCount;
	}

	strIP = aCacheNode.m_listMXRecord[nIndex].m_strIP;
	nPort = aCacheNode.m_listMXRecord[nIndex].m_nPort;
	return DNS_LOOKUP_OK;
}

void MxMgr::queryDNSRecord(const stdstring & strDomain, MXRecordCacheNode & aCacheNode)
{
	// ¿DNS¿¿¿¿
	DNSRecord dnsRecord;
	DNSLookupRetCode nRetCode = m_DNSResolver.queryMXIPV4Record(strDomain, dnsRecord, 1024);	// ¿¿¿¿1024¿IP
	if ( nRetCode == DNS_LOOKUP_NOTFOUND )	// ¿¿¿¿¿MX¿¿,¿¿A¿¿
		nRetCode = m_DNSResolver.queryIPV4ARecord(strDomain, dnsRecord);

	typeMXRecordList & listMXRecord = aCacheNode.m_listMXRecord;
	listMXRecord.clear();
	MXRecord aMXRecord;
	typeDNSEntryList::const_iterator itDNS, itDNSEnd = dnsRecord.getEntryList().end();
	for ( itDNS = dnsRecord.getEntryList().begin(); itDNS != itDNSEnd; ++itDNS )
	{
		aMXRecord.m_strIP = itDNS->getRecordResult();

		// ¿¿¿¿¿¿¿¿¿¿
		typeMXRecordList::const_iterator itExist = std::find(listMXRecord.begin(), listMXRecord.end(), aMXRecord);
		if ( itExist == listMXRecord.end() )
			listMXRecord.push_back(aMXRecord);
	} // for ( itDNS = dnsRecord.getEntryList().begin(); itDNS != itDNSEnd; ++itDNS )
}

RFC_NAMESPACE_END

