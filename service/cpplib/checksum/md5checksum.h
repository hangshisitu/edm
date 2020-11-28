/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_MD5CHECKSUM_H_201008
#define RFC_MD5CHECKSUM_H_201008

#include "checksum/checksum.h"

RFC_NAMESPACE_BEGIN

class MD5Checksum : public Checksum
{
public:
	enum { MD5_DIGEST_SIZE = 16 };
	MD5Checksum(void);
	virtual ~MD5Checksum(void) {}

	virtual void			reset(void);
	virtual size_t			write(const void * pBuffer, size_t nSize);
	virtual size_t			flush(void);

	bool					empty(void) const { return ( m_Context.count[0] == 0 && m_Context.count[1] == 0 ); }
	const rfc_uint_8 *		getMD5Digest(void) { flush(); return m_arDigest; }
	const rfc_uint_8 *		getMD5Digest(void) const { return m_arDigest; }
	stdstring				getMD5String(void);
	stdstring				getMD5String(void) const;

	const void *			getCheckSum(void) { return getMD5Digest(); }
	const void *			getCheckSum(void) const { return getMD5Digest(); }
	size_t					getCheckSumSize(void) const { return MD5_DIGEST_SIZE; }

protected:
	void					MD5Update(const rfc_uint_8 * src, rfc_uint_32 len);
	void					MD5Final(void);
	static void				MD5Transform(rfc_uint_32 state[4], const rfc_uint_8 block[64]);
	static void				MD5Encode(rfc_uint_8 * output, const rfc_uint_32 * input, rfc_uint_32 len);
	static void				MD5Decode(rfc_uint_32 * output, const rfc_uint_8 * input, rfc_uint_32 len);

	struct	MD5Context
	{
		rfc_uint_32			state[4];				/* state (ABCD) */
		rfc_uint_32			count[2];				/* number of bits, modulo 2^64 (lsb first) */
		rfc_uint_8			buffer[64];				/* input buffer */
	};

	bool					m_bHasFlush;
	rfc_uint_8				m_arDigest[MD5_DIGEST_SIZE];
	MD5Context				m_Context;
};

RFC_NAMESPACE_END

#endif	//RFC_MD5CHECKSUM_H_201008
