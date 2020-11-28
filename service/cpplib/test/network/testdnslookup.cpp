/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "network/dnslookup.h"
#include "network/networkfunc.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

static void printEntryList(const DNSRecord & dnsRecord)
{
	std::cout << std::endl;
	std::cout << FormatString("DNS record count:%, TTL:%").arg(dnsRecord.getEntryList().size()).arg(dnsRecord.getMinTTL()).str() << std::endl;
	typeDNSEntryList::const_iterator it, itEnd = dnsRecord.getEntryList().end();
	for ( it = dnsRecord.getEntryList().begin(); it != itEnd; ++it )
	{
		const DNSRecordEntry & dnsEntry = (*it);
		std::cout << FormatString("DNS record type:%, class:%, ttl:%, name:%, result:%, mxpref:%").arg(dnsEntry.getRecordType())
			.arg(dnsEntry.getRecordClass()).arg(static_cast<int>(dnsEntry.getTimeToLive())).arg(dnsEntry.getRecordName())
			.arg(dnsEntry.getRecordResult()).arg(dnsEntry.getMXPreference()).str() << std::endl;
	} // for ( it = listEntry.begin(); it != itEnd; ++it )
}

static void onTestRBLResult(DNSResolver & dnsResolver, const stdstring & strIP)
{
	typeQueryRBLResultList listRBLResult;
	assertTest( dnsResolver.isRBLBlackIP(strIP, listRBLResult) );
	std::cout << std::endl;
	stdstring strRBLResult;
	typeQueryRBLResultList::const_iterator it, itEnd = listRBLResult.end();
	for ( it = listRBLResult.begin(); it != itEnd; ++it )
	{
		const QueryRBLResult & rblResult = (*it);
		NetworkFunc::getIPV4String(rblResult.getRBLResult(), strRBLResult);
		std::cout << FormatString("Query RBL org:%, ip:%, result:%, ttl:%")
			.arg(rblResult.getRBLOrgDomain())
			.arg(strIP).arg(strRBLResult).arg(rblResult.getTimeToLive()).str() << std::endl;
	} // for ( it = listRBLResult.begin(); it != itEnd; ++it )
}

static void onTestMXPerformance(DNSResolver & dnsResolver, const stdstring & strDomain, int nLoopCount)
{
	std::cout << FormatString("begin test mx performance, domain:%, total count:%").arg(strDomain).arg(nLoopCount).str() << std::endl;
	MicroSecondTimer testMXPerformanceTimer;
	int nFailCount = 0;
	for ( int i = 0; i < nLoopCount; ++i )
	{
		DNSRecord dnsRecord;
		DNSLookupRetCode nRet = dnsResolver.queryMXIPV4Record(strDomain, dnsRecord);
		if ( nRet != DNS_LOOKUP_OK || dnsRecord.getEntryList().empty() )
		{
			++nFailCount;
			std::cout << FormatString("query mx record retcode:%, recordCount:%, index:%, total count:%, fail count:%, use % micro seconds")
				.arg(nRet).arg(dnsRecord.getEntryList().size()).arg(i).arg(nLoopCount).arg(nFailCount)
				.arg(testMXPerformanceTimer.elapsedMicroSecond()).str() << std::endl;
		}
	} // for ( int i = 0; i < nLoopCount; ++i )
	
	std::cout << FormatString("finish test mx performance, domain:%, total count:%, fail count:%, use % micro seconds")
		.arg(strDomain).arg(nLoopCount).arg(nFailCount).arg(testMXPerformanceTimer.elapsedMicroSecond()).str() << std::endl;
}

onUnitTest(DNSResolver)
{
	DNSResolver dnsResolver;
	DNSRecord dnsRecord;
	assertTest( dnsResolver.setDNSServerList() );
	stdstring strRBLDomainList = "zen.spamhaus.org,list.dnswl.org,bl.spamcop.net,dnsbl.sorbs.net,combined.njabl.org,list.dsbl.org";
	assertTest( dnsResolver.setRBLOrgDomainList(strRBLDomainList) );

	/*assertEqual(dnsResolver.queryIPV4ARecord("163.com", dnsRecord), DNS_LOOKUP_OK);
	printEntryList(dnsRecord);

	dnsRecord.clearAll();
	assertEqual(dnsResolver.queryMXRecord("richinfo.cn", dnsRecord), DNS_LOOKUP_OK);
	printEntryList(dnsRecord);

	dnsRecord.clearAll();
	assertEqual(dnsResolver.queryMXIPV4Record("richinfo.cn", dnsRecord), DNS_LOOKUP_OK);
	printEntryList(dnsRecord);

	dnsRecord.clearAll();
	assertEqual(dnsResolver.queryIPPTRRecord("220.181.12.64", dnsRecord), DNS_LOOKUP_OK);
	printEntryList(dnsRecord);*/

	dnsRecord.clearAll();
	assertEqual(dnsResolver.queryMXIPV4Record("163.com", dnsRecord), DNS_LOOKUP_OK);
	printEntryList(dnsRecord);

	dnsRecord.clearAll();
	assertEqual(dnsResolver.queryTXTRecord("163.com", dnsRecord), DNS_LOOKUP_OK);
	printEntryList(dnsRecord);

	dnsRecord.clearAll();
	assertEqual(dnsResolver.queryTXTRecord("qq.com", dnsRecord), DNS_LOOKUP_OK);
	printEntryList(dnsRecord);

	dnsRecord.clearAll();
	assertEqual(dnsResolver.queryTXTRecord("a.iyoutui.com", dnsRecord), DNS_LOOKUP_OK);
	printEntryList(dnsRecord);

	/*onTestRBLResult(dnsResolver, "59.54.54.75");
	onTestRBLResult(dnsResolver, "125.39.93.111");
	onTestRBLResult(dnsResolver, "58.54.20.96");
	onTestRBLResult(dnsResolver, "202.43.217.65");
	onTestRBLResult(dnsResolver, "210.75.9.250");
	onTestRBLResult(dnsResolver, "74.125.83.198");

	onTestMXPerformance(dnsResolver, "richinfo.cn", 1000);*/
}

RFC_NAMESPACE_END
