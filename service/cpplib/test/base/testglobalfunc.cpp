/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "base/globalfunc.h"

RFC_NAMESPACE_BEGIN

onUnitTest(GlobalFunc)
{
	assertEqual( static_cast<int>(GlobalFunc::calcBinaryBit1Count(14)), 3 );
	assertEqual( static_cast<int>(GlobalFunc::calcBinaryBit1Count(123)), 6 );
	assertEqual( static_cast<int>(GlobalFunc::calcBinaryBit1Count(1023)), 10 );

	rfc_uint_32 nIP;
	assertTest( GlobalFunc::getIPV4Integer("192.168.90.210", nIP) );
	assertEqual(nIP, static_cast<rfc_uint_32>((192 << 24) + (168 << 16) + (90 << 8) + 210));

	assertTest( !GlobalFunc::isValidIPV4Addr("") );
	assertTest( !GlobalFunc::isValidIPV4Addr(".192.168.90.210") );
	assertTest( !GlobalFunc::isValidIPV4Addr("192.168.90.210.") );
	assertTest( !GlobalFunc::isValidIPV4Addr("192.168.90") );
	assertTest( !GlobalFunc::isValidIPV4Addr("192.168.90.") );
	assertTest( !GlobalFunc::isValidIPV4Addr("192.168.90.210.1") );
	assertTest( !GlobalFunc::isValidIPV4Addr("192.168.090.210") );
	assertTest( !GlobalFunc::isValidIPV4Addr("192.268.90.210") );
	assertTest( !GlobalFunc::isValidIPV4Addr("292.168.90.210") );
	assertTest( !GlobalFunc::isValidIPV4Addr("192.168.90.310") );
	assertTest( GlobalFunc::isValidIPV4Addr("192.168.90.210") );

	assertTest( !GlobalFunc::isValidHostName("") );
	assertTest( !GlobalFunc::isValidHostName("abc") );
	assertTest( !GlobalFunc::isValidHostName(".192.168.90.210") );
	assertTest( !GlobalFunc::isValidHostName("192.168.90.210.") );
	assertTest( !GlobalFunc::isValidHostName("-192.168.90.210") );
	assertTest( !GlobalFunc::isValidHostName("192.168.90.210-") );
	assertTest( !GlobalFunc::isValidHostName("192.168.-90.210") );
	assertTest( !GlobalFunc::isValidHostName("192.168.90-.210") );
	assertTest( GlobalFunc::isValidHostName("192.168.90.210.1") );
	assertTest( GlobalFunc::isValidHostName("192.168.090.210") );
	assertTest( GlobalFunc::isValidHostName("192.268.90.210") );
	assertTest( GlobalFunc::isValidHostName("292.168.90.210") );
	assertTest( GlobalFunc::isValidHostName("192.168.90.310") );
	assertTest( GlobalFunc::isValidHostName("192.168.90.210") );

	stdstring strBinary = "1234ABCD";
	stdstring strHex;
	GlobalFunc::binaryToHex(strBinary.data(), strBinary.size(), strHex);
	assertEqual(strHex.size(), strBinary.size() * 2);

	stdstring strConvertBinary;
	GlobalFunc::hexToBinary(strHex.data(), strHex.size(), strConvertBinary);
	assertEqual(strBinary, strConvertBinary);
}

RFC_NAMESPACE_END
