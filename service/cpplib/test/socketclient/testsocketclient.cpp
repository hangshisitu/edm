/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2011-10      */

#include "socketclient/binarysocketclient.h"
#include "socketclient/socketstreamencoder.h"
#include "base/unittestenv.h"

RFC_NAMESPACE_BEGIN

std::ostream &  operator << (std::ostream & stream, const DateTime & dtVar)
{
	return stream << dtVar.formatDateTime();
}

template<typename typeObject>
static void onTestSocketStreamEncoder(const typeObject & n)
{
	BinaryStream binData;
	assertTest( SocketStreamEncoder::smartWrite(n, binData) );
	
	typeObject nDecode;
	const rfc_uint_8 * pData = binData.data();
	assertTest( SocketStreamEncoder::smartRead(pData, pData + binData.size(), nDecode) );
	assertEqual(nDecode, n);
}

onUnitTest(SocketStreamEncoder)
{
	onTestSocketStreamEncoder(static_cast<rfc_int_8>(123));
	onTestSocketStreamEncoder(static_cast<rfc_uint_8>(234));
	onTestSocketStreamEncoder(static_cast<rfc_int_16>(1234));
	onTestSocketStreamEncoder(static_cast<rfc_uint_16>(4567));
	onTestSocketStreamEncoder(static_cast<rfc_int_32>(12345));
	onTestSocketStreamEncoder(static_cast<rfc_uint_32>(45678));
	onTestSocketStreamEncoder(static_cast<rfc_int_64>(123456));
	onTestSocketStreamEncoder(static_cast<rfc_uint_64>(456789));

	onTestSocketStreamEncoder(123.456789);
	onTestSocketStreamEncoder(DateTime(time(NULL), 1234));
	onTestSocketStreamEncoder(stdstring("abc.defg"));
}

RFC_NAMESPACE_END
