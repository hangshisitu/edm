/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "checksum/md5checksum.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

onUnitTest(MD5Checksum)
{
	MD5Checksum md5sum;
	assertTest(md5sum.empty());
	assertEqual(md5sum.getCheckSumSize(), static_cast<size_t>(16));
	stdstring strContent = "111111";
	md5sum.write(strContent.data(), strContent.size());
	assertEqual(md5sum.getMD5String(), stdstring("96E79218965EB72C92A549DD5A330112"));
	assertTest(!md5sum.empty());

	md5sum.reset();
	assertTest(md5sum.empty());
	strContent = "222224343545453535";
	md5sum.write(strContent.data(), strContent.size());
	assertEqual(md5sum.getMD5String(), stdstring("7378E8F28BDF3F5D1DD3E5C5147B1B19"));
}

RFC_NAMESPACE_END
