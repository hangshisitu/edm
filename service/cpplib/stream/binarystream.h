/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_BINARYSTREAM_H_201008
#define RFC_BINARYSTREAM_H_201008

#include "stream/streambase.h"
#include "base/globalfunc.h"

RFC_NAMESPACE_BEGIN

DECLARE_EXCEPTION(BinaryStreamException, StreamException)
class BinaryStream : public StreamBase
{
public:
	//typedef rfc_uint_8					value_type;

	// -1: 总空间大小自动翻倍
	// > 0:如果总空间大于nAutoGrowUpSize,则自动翻倍;否则自动申请多nAutoGrowUpSize
	// 其他值: 根据实际需要申请空间
	explicit BinaryStream(int nAutoGrowUpSize = 0);
	BinaryStream(void * lpBuffer, size_t nBufLen, bool bAutoDelete);
	~BinaryStream(void);

	int				getAutoGrowUpSize(void) const { return m_nAutoGrowUpSize; }
	void			setAutoGrowUpSize(int nAutoGrowUpSize) { m_nAutoGrowUpSize = nAutoGrowUpSize; }

	void			clear(void);
	void			clearAndReserve(size_t nSize);
	void			reserve(size_t nSize);
	void			resize(size_t nSize);
	void			resize(size_t nSize, rfc_uint_8 bFillter);

	off_t			tellg(void) const { return m_nCurPos; }
	off_t			seekg(off_t nOffset, seek_position nWhence = seek_begin);

	size_t			getBuffer(void * lpBuffer, size_t nSize) const;
	size_t			read(void * lpBuffer, size_t nSize);
	size_t			write(const void * lpBuffer, size_t nSize);
	//修改buffer为nSize,会自动增大或减小m_nDataSize,自动增加的部分自动填充为'\0'
	bool			truncate(off_t nSize);

	void			attach(void * lpBuffer, size_t nBufLen, bool bAutoDelete);
	rfc_uint_8 *	detach(void);
	bool			isAttach(void) const { return !m_nInternalFalgs.check(FLAGS_AUTODEL); }

	rfc_uint_8 *	data(void) { return m_lpBuffer; }
	const rfc_uint_8 * data(void) const { return m_lpBuffer; }
	rfc_uint_8 *	current_data(void) { return m_lpBuffer + m_nCurPos; }
	const rfc_uint_8 * current_data(void) const { return m_lpBuffer + m_nCurPos; }
	size_t			size(void) const { return m_nDataSize; }
	size_t			capacity(void) const { return m_nBufferSize; }

	rfc_uint_8 & operator [](size_t nIndex);
	rfc_uint_8 operator [](size_t nIndex) const;

private:
	enum	InternalFlags
	{
		FLAGS_NONE					= 0,
		FLAGS_AUTODEL				= 1,
	};

	rfc_uint_8 *		m_lpBuffer;
	size_t				m_nBufferSize;				// m_lpBuffer分配的大小
	size_t				m_nDataSize;				// m_lpBuffer保存数据的大小
	off_t				m_nCurPos;
	BitFlags<rfc_uint_8> m_nInternalFalgs;
	int					m_nAutoGrowUpSize;
};

RFC_NAMESPACE_END

#endif	//RFC_BINARYSTREAM_H_201008
