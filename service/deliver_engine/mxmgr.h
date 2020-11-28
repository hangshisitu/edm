/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-08      */

#ifndef APP_MXMGR_H_201108
#define APP_MXMGR_H_201108

#include "network/dnslookup.h"
#include "mutex/mutex.h"
#include "application/configuremgr.h"

RFC_NAMESPACE_BEGIN

class MxMgr : private NonCopyAble
{
public:
	MxMgr(void);

	bool				onInit(const ConfigureMgr & conf);

	// ��ȡmx��¼,����������Ӧ��IP�Ͷ˿�
	// ��transport�ڲ�����ʱ�����÷�smtp��׼�˿�
	// ������ʧ��ʱ��ͨ��nIndex���»�ȡ��һ��ip
	DNSLookupRetCode	getMXRecord(const stdstring & strDomain, stdstring & strIP, rfc_uint_16 & nPort, size_t nIndex = 0);
	void                eraseMXRecord(const stdstring& domain, stdstring& ip, rfc_uint_16 port);
protected:
	struct MXRecord
	{
		MXRecord(void) : m_nPort(25) {}
		rfc_uint_16			m_nPort;
		stdstring			m_strIP;
		bool				operator == (const MXRecord & aMXRecord) const;
	};
	typedef std::vector<MXRecord>		typeMXRecordList;
	struct MXRecordCacheNode
	{
		MXRecordCacheNode(void) : m_tExpiredTime(-1) {}
		time_t				m_tExpiredTime;
		typeMXRecordList	m_listMXRecord;
	};
	typedef std::map<stdstring, MXRecordCacheNode, IgnoreCaseStringCompare> typeMxRecordMap;

	static DNSLookupRetCode	getFromCache(const typeMxRecordMap & mapCache, const stdstring & strDomain,
										 stdstring & strIP, rfc_uint_16 & nPort, size_t nIndex);
	void				queryDNSRecord(const stdstring & strDomain, MXRecordCacheNode & aCacheNode);

	time_t				m_tDNSDefaultTTL;	// default time to live
	DNSResolver			m_DNSResolver;

	typeMxRecordMap		m_mapTransport;		// ���Ȳ��ұ������õ�transport,û������ʱ��ͨ��DNS��ѯ

	ReadWriteLock		m_DNSRecordLock;
	typeMxRecordMap		m_mapDNSRecord;
};

RFC_NAMESPACE_END

#endif	//APP_MXMGR_H_201108
