/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "protocolbuf/protocolencoder.h"

RFC_NAMESPACE_BEGIN

static void onTestProtocolEncoder(const ProtocolVariant & tVar, rfc_uint_32 nExpectedSize)
{
	rfc_uint_32 nProtocolVarSize = ProtocolEncoder::calcVariantSize(tVar);
	assertEqual(8 + nProtocolVarSize, nExpectedSize);

	BinaryStream binBuffer;
	assertTest( ProtocolEncoder::saveToBinary(tVar, binBuffer) );
	assertEqual(static_cast<rfc_uint_32>(binBuffer.size()), nExpectedSize);

	ProtocolVariant tDecode;
	assertTest( ProtocolEncoder::loadFromBinary(binBuffer, tDecode) );	
	assertEqual(tVar, tDecode);

	tDecode.setNULL();
	assertTest( ProtocolEncoder::loadWithoutSize(binBuffer.data() + sizeof(rfc_uint_32), binBuffer.size() - sizeof(rfc_uint_32), tDecode) );	
	assertEqual(tVar, tDecode);

	binBuffer.clear();
	assertTest( ProtocolEncoder::saveWithoutSize(tVar, binBuffer) );
	assertEqual(static_cast<rfc_uint_32>(binBuffer.size()) + sizeof(rfc_uint_32), nExpectedSize);
	tDecode.setNULL();
	assertTest( ProtocolEncoder::loadWithoutSize(binBuffer, tDecode) );	
	assertEqual(tVar, tDecode);
}

static void buildIntVariantMap(rfc_int_32 & nKey, const ProtocolVariant & tVar, ProtocolVariant::typeIntVariantMap & mapData,
							   rfc_uint_32 nProtocolVarSize, rfc_uint_32 & nExpectedTotalSize)
{
	mapData[nKey] = tVar;
	nExpectedTotalSize += nProtocolVarSize;
	nKey += nKey;	
}

template<typename typeStringVarMap>
static void buildStringVariantMap(stdstring & strKey, const ProtocolVariant & tVar, typeStringVarMap & mapData,
								  rfc_uint_32 nProtocolVarSize, rfc_uint_32 & nExpectedTotalSize)
{
	mapData[strKey] = tVar;
	nExpectedTotalSize += ( strKey.size() > 0x7F ) ? sizeof(rfc_uint_32) : sizeof(rfc_uint_8);
	nExpectedTotalSize += static_cast<rfc_uint_32>( strKey.size() );
	nExpectedTotalSize += nProtocolVarSize;
	strKey += strKey;	
}

static void onTestIntVariantMap(rfc_int_32 nBegin)
{
	bool bInt16 = (nBegin >= -0x7FFF && nBegin <= 0x7FFF);
	rfc_uint_32 nExpectedTotalSize = 8 + 1 + 1;		//8 + type + byte( map.size() )
	ProtocolVariant::typeIntVariantMap mapData;
	buildIntVariantMap(nBegin, ProtocolVariant::g_varNULL, mapData, 1, nExpectedTotalSize);
	buildIntVariantMap(nBegin, true, mapData, 1, nExpectedTotalSize);
	buildIntVariantMap(nBegin, false, mapData, 1, nExpectedTotalSize);
	buildIntVariantMap(nBegin, -0x7FFF, mapData, 3, nExpectedTotalSize);
	buildIntVariantMap(nBegin, 0x7FFF, mapData, 3, nExpectedTotalSize);
	buildIntVariantMap(nBegin, -0x8000, mapData, 5, nExpectedTotalSize);
	buildIntVariantMap(nBegin, 0x8000, mapData, 5, nExpectedTotalSize);
	buildIntVariantMap(nBegin, static_cast<rfc_int_64>(-12345678), mapData, 9, nExpectedTotalSize);
	buildIntVariantMap(nBegin, static_cast<rfc_uint_64>(12345678), mapData, 9, nExpectedTotalSize);
	buildIntVariantMap(nBegin, "1234567890", mapData, 12, nExpectedTotalSize);
	BinaryStream binData;
	binData.write("1234567890", 10);
	buildIntVariantMap(nBegin, binData, mapData, 12, nExpectedTotalSize);
	buildIntVariantMap(nBegin, stdstring(1234, 'a'), mapData, 1 + 4 + 1234, nExpectedTotalSize);

	if ( bInt16 && (nBegin >= -0x7FFF && nBegin <= 0x7FFF) )
		nExpectedTotalSize += mapData.size() * sizeof(rfc_int_16);
	else
		nExpectedTotalSize += mapData.size() * sizeof(rfc_int_32);
	onTestProtocolEncoder(mapData, nExpectedTotalSize);
}

template<typename typeStringVarMap>
static void onTestStringVariantMap(const stdstring & strBegin)
{
	rfc_uint_32 nExpectedTotalSize = 8 + 1 + 1;		//8 + type + byte( map.size() )
	stdstring str(strBegin);
	typeStringVarMap mapData;
	buildStringVariantMap(str, ProtocolVariant::g_varNULL, mapData, 1, nExpectedTotalSize);
	buildStringVariantMap(str, true, mapData, 1, nExpectedTotalSize);
	buildStringVariantMap(str, false, mapData, 1, nExpectedTotalSize);
	buildStringVariantMap(str, -0x7FFF, mapData, 3, nExpectedTotalSize);
	buildStringVariantMap(str, 0x7FFF, mapData, 3, nExpectedTotalSize);
	buildStringVariantMap(str, -0x8000, mapData, 5, nExpectedTotalSize);
	buildStringVariantMap(str, 0x8000, mapData, 5, nExpectedTotalSize);
	buildStringVariantMap(str, static_cast<rfc_int_64>(-12345678), mapData, 9, nExpectedTotalSize);
	buildStringVariantMap(str, static_cast<rfc_uint_64>(12345678), mapData, 9, nExpectedTotalSize);
	buildStringVariantMap(str, "1234567890", mapData, 12, nExpectedTotalSize);
	BinaryStream binData;
	binData.write("1234567890", 10);
	buildStringVariantMap(str, binData, mapData, 12, nExpectedTotalSize);
	buildStringVariantMap(str, stdstring(1234, 'a'), mapData, 1 + 4 + 1234, nExpectedTotalSize);
	onTestProtocolEncoder(mapData, nExpectedTotalSize);
}

onUnitTest(ProtocolEncoder)
{
	onTestProtocolEncoder(ProtocolVariant::g_varNULL, 9);
	onTestProtocolEncoder(true, 9);
	onTestProtocolEncoder(false, 9);
	onTestProtocolEncoder(-0x7FFF, 11);
	onTestProtocolEncoder(0x7FFF, 11);
	onTestProtocolEncoder(-0x8000, 13);
	onTestProtocolEncoder(0x8000, 13);
	onTestProtocolEncoder(static_cast<rfc_int_64>(-12345678), 17);
	onTestProtocolEncoder(static_cast<rfc_uint_64>(12345678), 17);
	onTestProtocolEncoder("1234567890", 20);
	BinaryStream binData;
	binData.write("1234567890", 10);
	onTestProtocolEncoder(binData, 20);
	onTestProtocolEncoder(stdstring(1234, 'a'), 8 + 1 + 4 + 1234);

	onTestIntVariantMap(1);
	onTestIntVariantMap(16384);
	onTestIntVariantMap(65536);
	onTestStringVariantMap<ProtocolVariant::typeStringVariantMap>(stdstring("a"));
	onTestStringVariantMap<ProtocolVariant::typeStringVariantMap>(stdstring(100, 'a'));
	//onTestStringVariantMap<ProtocolVariant::typeNocaseStringVariantMap>(stdstring("a"));
	//onTestStringVariantMap<ProtocolVariant::typeNocaseStringVariantMap>(stdstring(100, 'a'));
}

RFC_NAMESPACE_END
