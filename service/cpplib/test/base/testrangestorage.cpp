/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "base/rangestorage.h"

RFC_NAMESPACE_BEGIN

static const int g_nNotExistKey = 0x7FFFFFFF;

static void onTestRangeDataMap(RangeDataMap<int, int> & intRangeStorage, const int nBeginKey, const int nEndKey, const int nExtraData)
{
	bool bNegative = intRangeStorage.isNegative();
	intRangeStorage.updateOrInsert(nBeginKey, nEndKey, nExtraData);

	assertEqual(bNegative, intRangeStorage.findInRange(g_nNotExistKey));
	assertEqual(bNegative, !intRangeStorage.findInRange(nBeginKey));
	assertEqual(bNegative, !intRangeStorage.findInRange((nBeginKey + nEndKey) / 2));
	assertEqual(bNegative, !intRangeStorage.findInRange(nEndKey));

	int nRetBeginKey = g_nNotExistKey, nRetEndKey = g_nNotExistKey, nRetExtraData = g_nNotExistKey;
	assertEqual(bNegative, intRangeStorage.findInRange(g_nNotExistKey, nRetExtraData));
	assertEqual(bNegative, !intRangeStorage.findInRange(nBeginKey, nRetExtraData));
	assertEqual(nRetExtraData, nExtraData);
	assertEqual(bNegative, !intRangeStorage.findInRange((nBeginKey + nEndKey) / 2, nRetExtraData));
	assertEqual(nRetExtraData, nExtraData);
	assertEqual(bNegative, !intRangeStorage.findInRange(nEndKey, nRetExtraData));
	assertEqual(nRetExtraData, nExtraData);

	assertEqual(bNegative, !intRangeStorage.findInRange(nBeginKey, nRetBeginKey, nRetEndKey, nRetExtraData));
	assertEqual(nRetBeginKey, nBeginKey);
	assertEqual(nRetEndKey, nEndKey);
	assertEqual(nRetExtraData, nExtraData);
	assertEqual(bNegative, !intRangeStorage.findInRange((nBeginKey + nEndKey) / 2, nRetBeginKey, nRetEndKey, nRetExtraData));
	assertEqual(nRetBeginKey, nBeginKey);
	assertEqual(nRetEndKey, nEndKey);
	assertEqual(nRetExtraData, nExtraData);
	assertEqual(bNegative, !intRangeStorage.findInRange(nEndKey, nRetBeginKey, nRetEndKey, nRetExtraData));
	assertEqual(nRetBeginKey, nBeginKey);
	assertEqual(nRetEndKey, nEndKey);
	assertEqual(nRetExtraData, nExtraData);
}

static void onTestRangeDataMap(bool bNegative)
{
	RangeDataMap<int, int> intRangeStorage;
	intRangeStorage.setNegative(bNegative);
	assertEqual(bNegative, intRangeStorage.isNegative());
	onTestRangeDataMap(intRangeStorage, 100, 200, 1);
	onTestRangeDataMap(intRangeStorage, 1000, 2000, 2);
	onTestRangeDataMap(intRangeStorage, -200, -100, 3);
	onTestRangeDataMap(intRangeStorage, -2000, -1000, 4);
}

class IPRangeVisitor
{
public:
	IPRangeVisitor(stdstring & str) : m_strIPRange(str) {}
	bool operator()(const rfc_uint_32 & nBegin, const RangeDataMap<rfc_uint_32, int>::value_type & aRangeDataNode)
	{
		StringFunc::addIntToString(m_strIPRange, nBegin);
		if ( nBegin != aRangeDataNode.first )
		{
			m_strIPRange += '-';
			StringFunc::addIntToString(m_strIPRange, aRangeDataNode.first);
		}
		m_strIPRange += ',';
		return true;
	}
protected:
	stdstring &			m_strIPRange;
};

onUnitTest(RangeDataMap)
{
	stdstring strIPRange;
	IPRangeVisitor rangeVisitor(strIPRange);
	RangeDataMap<rfc_uint_32, int>	mapIPRange;
	assertTest(mapIPRange.empty());
	mapIPRange.updateOrInsert(1000, 2000, 0);
	assertTest( !mapIPRange.findInRange(999) );
	assertTest( mapIPRange.findInRange(1000) );
	assertTest( mapIPRange.findInRange(1500) );
	assertTest( mapIPRange.findInRange(2000) );
	assertTest( !mapIPRange.findInRange(2001) );
	strIPRange.clear();
	GlobalFunc::for_each_pair(mapIPRange.getRangeMap(), rangeVisitor);
	assertEqual(strIPRange, stdstring("1000-2000,"));

	mapIPRange.updateOrInsert(3000, 0);
	assertTest( mapIPRange.findInRange(3000) );
	assertTest( !mapIPRange.findInRange(3001) );
	strIPRange.clear();
	GlobalFunc::for_each_pair(mapIPRange.getRangeMap(), rangeVisitor);
	assertEqual(strIPRange, stdstring("1000-2000,3000,"));

	mapIPRange.erase(1500, 1800);
	assertTest( !mapIPRange.findInRange(999) );
	assertTest( mapIPRange.findInRange(1000) );
	assertTest( mapIPRange.findInRange(1499) );
	assertTest( !mapIPRange.findInRange(1500) );
	assertTest( !mapIPRange.findInRange(1800) );
	assertTest( mapIPRange.findInRange(1801) );
	assertTest( mapIPRange.findInRange(2000) );
	assertTest( !mapIPRange.findInRange(2001) );
	assertTest( mapIPRange.findInRange(3000) );
	assertTest( !mapIPRange.findInRange(3001) );
	strIPRange.clear();
	GlobalFunc::for_each_pair(mapIPRange.getRangeMap(), rangeVisitor);
	assertEqual(strIPRange, stdstring("1000-1499,1801-2000,3000,"));

	mapIPRange.updateOrInsert(1600, 1900, 0);
	assertTest( !mapIPRange.findInRange(999) );
	assertTest( mapIPRange.findInRange(1000) );
	assertTest( mapIPRange.findInRange(1499) );
	assertTest( mapIPRange.findInRange(1600) );
	assertTest( mapIPRange.findInRange(1801) );
	assertTest( mapIPRange.findInRange(1900) );
	assertTest( mapIPRange.findInRange(2000) );
	assertTest( !mapIPRange.findInRange(2001) );
	assertTest( mapIPRange.findInRange(3000) );
	assertTest( !mapIPRange.findInRange(3001) );
	strIPRange.clear();
	GlobalFunc::for_each_pair(mapIPRange.getRangeMap(), rangeVisitor);
	assertEqual(strIPRange, stdstring("1000-1499,1600-1900,1901-2000,3000,"));

	mapIPRange.compact();
	strIPRange.clear();
	GlobalFunc::for_each_pair(mapIPRange.getRangeMap(), rangeVisitor);
	assertEqual(strIPRange, stdstring("1000-1499,1600-2000,3000,"));

	onTestRangeDataMap(false);
	onTestRangeDataMap(true);
}

RFC_NAMESPACE_END
