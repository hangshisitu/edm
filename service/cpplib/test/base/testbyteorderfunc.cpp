/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "base/byteorderfunc.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

static void onTestLittleEndian(const rfc_uint_8 * p, const rfc_uint_8 * pEnd, rfc_uint_32 nExpect)
{
	rfc_uint_32 n = 0;
	assertTest(ByteOrderFunc::readIntLittleEndian(p, pEnd, n));
	assertEqual(n, nExpect);
}

static void onTestBigEndian(const rfc_uint_8 * p, const rfc_uint_8 * pEnd, rfc_uint_32 nExpect)
{
	rfc_uint_32 n = 0;
	assertTest(ByteOrderFunc::readIntBigEndian(p, pEnd, n));
	assertEqual(n, nExpect);
}

onUnitTest(ByteOrderFunc)
{
	assertEqual( ByteOrderFunc::hton(static_cast<rfc_uint_8>(234)), static_cast<rfc_uint_8>(234));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_uint_8>(234)), static_cast<rfc_uint_8>(234));
	assertEqual( ByteOrderFunc::hton(static_cast<rfc_int_8>(-123)), static_cast<rfc_int_8>(-123));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_int_8>(-123)), static_cast<rfc_int_8>(-123));

	assertEqual( ByteOrderFunc::hton(static_cast<rfc_uint_16>(0x5678)), static_cast<rfc_uint_16>(0x7856));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_uint_16>(0x5678)), static_cast<rfc_uint_16>(0x7856));
	assertEqual( ByteOrderFunc::hton(static_cast<rfc_uint_16>(0x12)), static_cast<rfc_uint_16>(0x1200));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_uint_16>(0x12)), static_cast<rfc_uint_16>(0x1200));

	assertEqual( ByteOrderFunc::hton(static_cast<rfc_int_16>(-2)), static_cast<rfc_uint_16>(0xFEFF));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_int_16>(-2)), static_cast<rfc_uint_16>(0xFEFF));
	assertEqual( ByteOrderFunc::hton(static_cast<rfc_int_16>(-0x2F3d)), static_cast<rfc_uint_16>(0xC3D0));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_int_16>(-0x2F3d)), static_cast<rfc_uint_16>(0xC3D0));

	assertEqual( ByteOrderFunc::hton(static_cast<rfc_uint_32>(0x5678)), static_cast<rfc_uint_32>(0x78560000));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_uint_32>(0x5678)), static_cast<rfc_uint_32>(0x78560000));
	assertEqual( ByteOrderFunc::hton(static_cast<rfc_uint_32>(0x12fcabad)), static_cast<rfc_uint_32>(0xadabfc12));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_uint_32>(0x12fcabad)), static_cast<rfc_uint_32>(0xadabfc12));

	assertEqual( ByteOrderFunc::hton(static_cast<rfc_int_32>(-0x0E)), static_cast<rfc_uint_32>(0xF2FFFFFF));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_int_32>(-0x0E)), static_cast<rfc_uint_32>(0xF2FFFFFF));
	assertEqual( ByteOrderFunc::hton(static_cast<rfc_int_32>(0x12fcabad)), static_cast<rfc_uint_32>(0xadabfc12));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_int_32>(0x12fcabad)), static_cast<rfc_uint_32>(0xadabfc12));

	assertEqual( ByteOrderFunc::hton(static_cast<rfc_uint_64>(0x7890abcdefULL)), static_cast<rfc_uint_64>(0xefcdab9078000000ULL));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_uint_64>(0x7890abcdefULL)), static_cast<rfc_uint_64>(0xefcdab9078000000ULL));
	assertEqual( ByteOrderFunc::hton(static_cast<rfc_uint_64>(0xefcdab9078ULL)), static_cast<rfc_uint_64>(0x7890abcdef000000ULL));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_uint_64>(0xefcdab9078ULL)), static_cast<rfc_uint_64>(0x7890abcdef000000ULL));

	assertEqual( ByteOrderFunc::hton(static_cast<rfc_int_64>(0x12345678)), static_cast<rfc_uint_64>(0x7856341200000000ULL));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_int_64>(0x12345678)), static_cast<rfc_uint_64>(0x7856341200000000ULL));
	assertEqual( ByteOrderFunc::hton(static_cast<rfc_int_64>(-0x12fcab)), static_cast<rfc_uint_64>(0x5503EDFFFFFFFFFFULL));
	assertEqual( ByteOrderFunc::ntoh(static_cast<rfc_int_64>(-0x12fcab)), static_cast<rfc_uint_64>(0x5503EDFFFFFFFFFFULL));

	rfc_uint_8 p2[] = { 0x12, 0x34, 0x56, 0x78, 0x21, 0x43, 0x65, 0x87 };
	const rfc_uint_8 * p = p2;
	onTestLittleEndian(p, p + 8, static_cast<rfc_uint_32>(0x78563412));
	onTestBigEndian(p + 1, p + 8, static_cast<rfc_uint_32>(0x34567821));
	onTestLittleEndian(p + 2, p + 8, static_cast<rfc_uint_32>(0x43217856));
	onTestBigEndian(p + 3, p + 8, static_cast<rfc_uint_32>(0x78214365));
	onTestLittleEndian(p + 4, p + 8, static_cast<rfc_uint_32>(0x87654321));
	onTestBigEndian(p + 4, p + 8, static_cast<rfc_uint_32>(0x21436587));

	const rfc_uint_8 * pSave = p + 5;
	rfc_uint_32 n = 0;
	assertTest(!ByteOrderFunc::readIntLittleEndian(pSave, p + 8, n));
	assertTest(!ByteOrderFunc::readIntBigEndian(pSave, p + 8, n));
}

RFC_NAMESPACE_END
