/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/hashfunc.h"

RFC_NAMESPACE_BEGIN

const typeHashInteger HashFunc::g_nMagicHashNumber = 16777619;

size_t HashFunc::getNextPrimeNumber(size_t nValue)
{
	static const unsigned long __stl_prime_list[] =
	{
		53ul,         97ul,         193ul,       389ul,       769ul,
		1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
		49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
		1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
		50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
		1610612741ul, 3221225473ul, 4294967291ul
	};
	static const size_t __stl_prime_list_size = sizeof(__stl_prime_list) / sizeof(__stl_prime_list[0]);
	static const unsigned long * __stl_prime_list_end = __stl_prime_list + __stl_prime_list_size;

	const unsigned long * pos = std::lower_bound(__stl_prime_list, __stl_prime_list_end, nValue);
	return (pos == __stl_prime_list_end) ? *(__stl_prime_list_end - 1) : *pos;
}

RFC_NAMESPACE_END
