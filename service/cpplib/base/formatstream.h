/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_FORMATSTREAM_H_201008
#define RFC_FORMATSTREAM_H_201008

#include "base/exception.h"
#include "base/globalfunc.h"

#include <sstream>
#include <string.h>

RFC_NAMESPACE_BEGIN

DECLARE_EXCEPTION(FormatStreamException, Exception)

typedef std::istream				stdistream;
typedef std::ostream				stdostream;
typedef std::iostream				stdiostream;
typedef std::streambuf				stdstreambuf;

typedef std::istringstream			stdistringstream;
typedef std::ostringstream			stdostringstream;
typedef std::stringstream			stdstringstream;

struct FormatFlags
{
	enum	BASE_FLAG
	{
		BASE10	=	std::ios::dec,
		BASE8	=	std::ios::oct,
		BASE16	=	std::ios::hex
	};

	enum	ADJUSTFIELD_FLAG
	{
		ADJUST_LEFT			= std::ios::left,
		ADJUST_RIGHT		= std::ios::right,
		ADJUST_INTERNAL		= std::ios::internal,
	};
};

#if 0
1, 默认以'%'作为占位符，表示后续的变量替换的位置
2, 连续出现多个'%'时,所有'%'都是占位符,如果要输出'%',则在相应的占位符调用arg('%'),FormatStream("%%").arg('%').arg(80) 输出"%80"
3, 要求传入的lpszFormat的生命期要大于FormatStream的生命期, 错误用法: std::string getString(void) {...} stream.format(getString().c_str());
4, 当输入参数比占位符多时，多余的参数都会输出到格式字符串的后面
5, 当输入参数比占位符少时，输出则停留在第一个多余的占位符的前面,所以最好在获取字符串前调用一次flushStream
6, 可调用str()接口来获得格式化后的字符串
7, stdostringstream strStream; FormatOutputStream fmtStream(strStream.rdbuf()); fmtStream.format("xx").arg() std::string str = strStream.str();
#endif

template<typename typeOutputStream>
class FormatStream : public typeOutputStream, private NonCopyAble
{
public:
	FormatStream(const char * lpszFormat = NULL, char chFormatSymbol = '%')
		: typeOutputStream(), m_lpszFormat(NULL), m_chFormatSymbol(chFormatSymbol) { format(lpszFormat, chFormatSymbol); }

	template<typename typeArg1>
	FormatStream(const typeArg1 & arg1) : typeOutputStream(arg1), m_lpszFormat(NULL), m_chFormatSymbol('%') {}

	//! flush the format rest string
	inline FormatStream &		flushStream(void)
	{
		if ( m_lpszFormat != NULL )
		{
			getOutputStream() << m_lpszFormat;
			m_lpszFormat = NULL;
		}
		return *this;
	}

	inline bool					isNULL(void) const { return ( m_lpszFormat == NULL ); }

	//! set the format string
	inline FormatStream &		format(const char * lpszFormat, char chFormatSymbol = '%')
	{ 
		flushStream();
		m_lpszFormat = lpszFormat;
		m_chFormatSymbol = chFormatSymbol;
		return outputNextBlock();
	}

	// 使用val替换所有%参数
	template<typename typeArg1>
	inline FormatStream &		argAll(const typeArg1 & val)
	{
		arg(val);
		while ( !isNULL() )
			arg(val);
		return *this;
	}

	//! output a value
	template<typename typeArg1>
	inline FormatStream &		arg(const typeArg1 & val)
	{
		getOutputStream() << val;
		return outputNextBlock();
	}

	template<typename typeArg1, typename typeArg2>
	inline FormatStream &		arg(const typeArg1 & val1, const typeArg2 & val2)
	{
		getOutputStream() << val1 << val2;
		return outputNextBlock();
	}

	template<typename typeArg1, typename typeArg2, typename typeArg3>
	inline FormatStream &		arg(const typeArg1 & val1, const typeArg2 & val2, const typeArg3 & val3)
	{
		getOutputStream() << val1 << val2 << val3;
		return outputNextBlock();
	}

	template<typename typeArg1, typename typeArg2, typename typeArg3, typename typeArg4>
	inline FormatStream &		arg(const typeArg1 & val1, const typeArg2 & val2, const typeArg3 & val3, const typeArg4 & val4)
	{
		getOutputStream() << val1 << val2 << val3 << val4;
		return outputNextBlock();
	}

	template<typename typeArg1, typename typeArg2, typename typeArg3, typename typeArg4, typename typeArg5>
	inline FormatStream &		arg(const typeArg1 & val1, const typeArg2 & val2, const typeArg3 & val3, const typeArg4 & val4, const typeArg5 & val5)
	{
		getOutputStream() << val1 << val2 << val3 << val4 << val5;
		return outputNextBlock();
	}

	//! output a rang set of values
	template<typename typeIterator>
	inline FormatStream &		argRang(typeIterator itBegin, typeIterator itEnd, const char * lpszDelimiter = NULL)
	{
		std::copy( itBegin, itEnd, std::ostream_iterator<typename typeIterator::value_type>(*this, lpszDelimiter) );
		return outputNextBlock();
	}

	//! add a integer value with format
	//! \param nBase display the value as the a nBase integer.
	//! \param bUpcase whether display the char as up case while display in 16-base integer.
	//! \param bShowPos whether display the '+' before positive number
	//! \param bShowBase whether display the base information before the integer.
	//! \param nAdjust where the integer display, left, right or internal.
	//! \param nWidth the width of the integer. if nWidth == -1, that means needn't to adjust the width. else will fill with chFill.
	//! \param chFill if the integer display is less than nWidth, will fill with this char.
	template<typename typeInteger>
	FormatStream &				argInteger(typeInteger nVal, FormatFlags::BASE_FLAG nBase = FormatFlags::BASE10,
									bool bUpcase = true, bool bShowPos = false, bool bShowBase = false,
									FormatFlags::ADJUSTFIELD_FLAG nAdjust = FormatFlags::ADJUST_LEFT, int nWidth = -1, char chFill = ' ')
	{
		std::ios::fmtflags nFlags = typeOutputStream::flags();
		typeOutputStream::setf(static_cast<std::ios::fmtflags>(nBase), std::ios::basefield);

		if ( bUpcase )
			typeOutputStream::setf(std::ios::uppercase);
		else
			typeOutputStream::unsetf(std::ios::uppercase);

		if ( bShowPos )
			typeOutputStream::setf(std::ios::showpos);
		else
			typeOutputStream::unsetf(std::ios::showpos);

		if ( bShowBase )
			typeOutputStream::setf(std::ios::showbase);
		else
			typeOutputStream::unsetf(std::ios::showbase);
		typeOutputStream::setf(static_cast<std::ios::fmtflags>(nAdjust), std::ios::adjustfield);

		nWidth = static_cast<int>( typeOutputStream::width(nWidth) );
		chFill = typeOutputStream::fill(chFill);
		getOutputStream() << nVal;
		typeOutputStream::flags(nFlags);
		typeOutputStream::width(nWidth);
		typeOutputStream::fill(chFill);

		return outputNextBlock();
	}

	//! add a double value with format
	//! \param bFixed display the value as fixed format or scientific format.
	//! \param bShowPoint whether display the point at the end of number
	//! \param bUpcase whether display the char as up case while display in 16-base integer.
	//! \param bShowPos whether display the '+' before positive number
	//! \param nAdjust where the integer display, left, right or internal.
	//! \param nPrecision indicate the precision of the value.
	//! \param nWidth the width of the double value. if nWidth == -1, that means needn't to adjust the width. else will fill with chFill.
	//! \param chFill if the double value display is less than nWidth, will fill with this char.
	FormatStream &				argDouble(double dbVal, bool bFixed = true, bool bShowPoint = true,
		bool bUpcase = true, bool bShowPos = false, FormatFlags::ADJUSTFIELD_FLAG nAdjust = FormatFlags::ADJUST_LEFT,
		int nPrecision = -1, int nWidth = -1, char chFill = ' ');

	//! add a string value with format
	//! \param nWidth the width of the string. if nWidth == -1, that means needn't to adjust the width. else will fill with chFill.
	//! \param chFill if the string display is less than nWidth, will fill with this char.
	FormatStream &				argString(const char * lpszString, size_t nLength);
	FormatStream &				argString(const stdstring & strVal,
		FormatFlags::ADJUSTFIELD_FLAG nAdjust = FormatFlags::ADJUST_LEFT, int nWidth = -1, char chFill = ' ');

protected:
	FormatStream &				outputNextBlock(void);
	inline typeOutputStream &	getOutputStream(void) { return *this; }

	const char *				m_lpszFormat;
	char						m_chFormatSymbol;
};

//////////////////////////////////////////////////////////////////////////

template<class typeOutputStream>
FormatStream<typeOutputStream> & FormatStream<typeOutputStream>::outputNextBlock(void)
{
	if ( m_lpszFormat == NULL )
		return *this;

	const char * lpPos = strchr(m_lpszFormat, m_chFormatSymbol);
	if ( lpPos == NULL )
		return flushStream();

	typeOutputStream::write(m_lpszFormat, lpPos - m_lpszFormat);
	m_lpszFormat = lpPos + 1;
	return *this;
}

template<typename typeOutputStream>
FormatStream<typeOutputStream> & FormatStream<typeOutputStream>::argDouble(double dbVal, bool bFixed, bool bShowPoint,
						bool bUpcase, bool bShowPos, FormatFlags::ADJUSTFIELD_FLAG nAdjust, int nPrecision, int nWidth, char chFill)
{
	std::ios::fmtflags nFlags = typeOutputStream::flags();
	if ( bFixed )
		typeOutputStream::setf(std::ios::fixed, std::ios::floatfield);
	else
		typeOutputStream::setf(std::ios::scientific, std::ios::floatfield);

	if ( bShowPoint )
		typeOutputStream::setf(std::ios::showpoint);
	else
		typeOutputStream::unsetf(std::ios::showpoint);

	if ( bUpcase )
		typeOutputStream::setf(std::ios::uppercase);
	else
		typeOutputStream::unsetf(std::ios::uppercase);

	if ( bShowPos )
		typeOutputStream::setf(std::ios::showpos);
	else
		typeOutputStream::unsetf(std::ios::showpos);
	typeOutputStream::setf(static_cast<std::ios::fmtflags>(nAdjust), std::ios::adjustfield);

	nPrecision = static_cast<int>( typeOutputStream::precision(nPrecision) );
	nWidth = static_cast<int>( typeOutputStream::width(nWidth) );
	chFill = typeOutputStream::fill(chFill);
	getOutputStream() << dbVal;

	typeOutputStream::flags(nFlags);
	typeOutputStream::precision(nPrecision);
	typeOutputStream::width(nWidth);
	typeOutputStream::fill(chFill);
	return outputNextBlock();
}

template<typename typeOutputStream>
FormatStream<typeOutputStream> & FormatStream<typeOutputStream>::argString(const char * lpszString, size_t nLength)
{
	typeOutputStream::write(lpszString, nLength);
	return outputNextBlock();
}

template<typename typeOutputStream>
FormatStream<typeOutputStream> & FormatStream<typeOutputStream>::argString(const stdstring & strVal,
																		   FormatFlags::ADJUSTFIELD_FLAG nAdjust, int nWidth, char chFill)
{
	std::ios::fmtflags nFlags = typeOutputStream::flags();
	typeOutputStream::setf(static_cast<std::ios::fmtflags>(nAdjust), std::ios::adjustfield);
	nWidth = typeOutputStream::width(nWidth);
	chFill = typeOutputStream::fill(chFill);

	getOutputStream() << strVal;

	typeOutputStream::flags(nFlags);
	typeOutputStream::width(nWidth);
	typeOutputStream::fill(chFill);
	return outputNextBlock();
}

//////////////////////////////////////////////////////////////////////////

typedef FormatStream<stdostream>					FormatOutputStream;
typedef FormatStream<stdiostream>					FormatIOStream;
typedef FormatStream<stdostringstream>				FormatString;

RFC_NAMESPACE_END

#endif	//RFC_FORMATSTREAM_H_201008
