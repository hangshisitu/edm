/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_GLOBALFUNC_H_201008
#define RFC_GLOBALFUNC_H_201008

#include "base/string.h"

#include <new>
#include <cctype>
#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

RFC_NAMESPACE_BEGIN

//! indicate where the seek operation begin from.
enum seek_position
{
	seek_begin = SEEK_SET,	// begin from the start
	seek_cur = SEEK_CUR,	// begin from current position
	seek_end = SEEK_END		// begin from the end
};

class NonCopyAble
{
public:
	NonCopyAble() {}
	~NonCopyAble() {}
private:
	NonCopyAble(const NonCopyAble & other);
	NonCopyAble & operator = (const NonCopyAble & other);
};

template <typename typeInteger>
class AutoIncrease
{
public:
	AutoIncrease(typeInteger & nCounter) : m_nCounter(nCounter) { ++m_nCounter; }
	~AutoIncrease(void) { --m_nCounter; }

private:
	typeInteger &				m_nCounter;
};

class GlobalConstant
{
public:
	enum RetCode
	{
		RET_OK						= 0,
		RET_ERROR					= -1,
	};
	static const rfc_int_32				g_nMaxInt32;
	static const rfc_int_32				g_nMinInt32;
	static const rfc_int_64				g_nMaxInt64;
	static const rfc_int_64				g_nMinInt64;

	static const rfc_uint_32			g_uMaxUInt32;
	static const rfc_uint_64			g_uMaxUInt64;
	static const size_t					g_uMaxSize;			//32位平台就是32bit, 64位平台就是64bit
};

struct IsSpacePred : public std::unary_function<int, bool>
{
	bool operator()(int ch) const { return ( ::isspace(ch) || ch == RFC_CHAR_NULL ); }
};

template<typename valueType>
class BitFlags
{
public:
	typedef valueType					value_type;
	typedef BitFlags<valueType>			typeSelf;

	BitFlags(void) : m_nBitFlags(0) {}
	BitFlags(valueType n) : m_nBitFlags(n) {}
	BitFlags(const BitFlags & f) : m_nBitFlags(f.m_nBitFlags) {}

	const BitFlags & operator=(valueType n) { m_nBitFlags = n; return *this; }
	const BitFlags & operator=(const BitFlags & v) { m_nBitFlags = v.m_nBitFlags; return *this; }

	inline	void	clearAll(void) { m_nBitFlags = 0; }
	inline	void	setFlag(valueType nFlag) { m_nBitFlags |= nFlag; }
	inline	void	clearFlag(valueType nFlag) { m_nBitFlags &= ~nFlag; }
	inline	void	setFlagToggle(bool bToggle, valueType nFlag) { if (bToggle) setFlag(nFlag); else clearFlag(nFlag); }
	inline	bool	check(valueType nFlag) const { return (m_nBitFlags & nFlag) != 0; }
	inline	bool	checkAll(valueType nFlag) const { return (m_nBitFlags & nFlag) == nFlag; }

	const valueType & getFlags(void) const { return m_nBitFlags; }
	void	swap(BitFlags & f) { std::swap(m_nBitFlags, f.m_nBitFlags); }

	BitFlags &	operator|=(const BitFlags & f)
	{
		m_nBitFlags |= f.m_nBitFlags;
		return *this;
	}

	BitFlags &	operator&=(const BitFlags & f)
	{
		m_nBitFlags &= f.m_nBitFlags;
		return *this;
	}

	BitFlags &	operator^=(const BitFlags & f)
	{
		m_nBitFlags ^= f.m_nBitFlags;
		return *this;
	}

protected:
	valueType		m_nBitFlags;
};

class CompareFunc
{
public:
	enum CompareOP
	{
		COMPARE_OP_EQAUL,							// ==
		COMPARE_OP_NOT_EQAUL,						// !=
		COMPARE_OP_GREATER_THAN,					// >
		COMPARE_OP_GREATER_OR_EQAUL,				// >=
		COMPARE_OP_LESS_THAN,						// <
		COMPARE_OP_LESS_OR_EQAUL,					// <=
	};

	static const char * getCompareSymbol(CompareOP nCompareOP);

	template<typename typeArg1, typename typeArg2>
	static bool compareOK(const typeArg1& arg1, const typeArg2& arg2, CompareOP nCompareOP)
	{
		if ( nCompareOP == COMPARE_OP_EQAUL )
			return (arg1 == arg2);
		else if ( nCompareOP == COMPARE_OP_NOT_EQAUL )
			return (arg1 != arg2);
		else if ( nCompareOP == COMPARE_OP_GREATER_THAN )
			return (arg1 > arg2);
		else if ( nCompareOP == COMPARE_OP_GREATER_OR_EQAUL )
			return (arg1 >= arg2);
		else if ( nCompareOP == COMPARE_OP_LESS_THAN )
			return (arg1 < arg2);
		else if ( nCompareOP == COMPARE_OP_LESS_OR_EQAUL )
			return (arg1 <= arg2);
		return false;
	}

	static bool compareOK(const double& arg1, const double& arg2, CompareOP nCompareOP);
	static bool compareOK(const float& arg1, const float& arg2, CompareOP nCompareOP)
	{
		return compareOK(static_cast<double>(arg1), static_cast<double>(arg2), nCompareOP);
	}
};

class GlobalFunc
{
public:
	static inline const char * getSystemErrorInfo(void) { return strerror(errno); }

	static rfc_uint_32 threadSafeRand(rfc_uint_32 * pRandomSeed);

	static rfc_uint_8 calcBinaryBit1Count(rfc_uint_64 n);

	static bool			getIPV4Integer(const char * pHostAddr, size_t nHostAddrSize, rfc_uint_32 & nIPInteger);
	static bool			getIPV4Integer(const char * lpszHostAddr, rfc_uint_32 & nIPInteger);
	static bool			getIPV4Integer(const stdstring & strHostAddr, rfc_uint_32 & nIPInteger);

	static bool			isValidIPV4Addr(const char * pHostAddr, size_t nHostAddrSize);
	static bool			isValidIPV4Addr(const char * lpszHostAddr);
	static bool			isValidIPV4Addr(const stdstring & strHostAddr) { return isValidIPV4Addr(strHostAddr.data(), strHostAddr.size()); }

	static bool			isValidHostAddr(const char * pHostAddr, size_t nHostAddrSize) { return isValidIPV4Addr(pHostAddr, nHostAddrSize); }
	static bool			isValidHostAddr(const char * lpszHostAddr) { return isValidIPV4Addr(lpszHostAddr); }
	static bool			isValidHostAddr(const stdstring & strHostAddr) { return isValidIPV4Addr(strHostAddr.data(), strHostAddr.size()); }

	static bool			isValidHostName(const char * pHostName, size_t nHostNameSize);
	static bool			isValidHostName(const char * lpszHostName);
	static bool			isValidHostName(const stdstring & strHostName) { return isValidHostName(strHostName.data(), strHostName.size()); }

	static bool			isValidDomain(const char * pDomain, size_t nDomainSize) { return isValidHostName(pDomain, nDomainSize); }
	static bool			isValidDomain(const char * lpszDomain) { return isValidHostName(lpszDomain); }
	static bool			isValidDomain(const stdstring & strDomain) { return isValidHostName(strDomain.data(), strDomain.size()); }

	template<typename typeVal>
	static inline const typeVal & getMin(const typeVal & a, const typeVal & b)
	{
		return (a < b) ? a : b;
	}

	template<typename typeVal>
	static inline const typeVal & getMax(const typeVal & a, const typeVal & b)
	{
		return (a > b) ? a : b;
	}

	template<typename typeVal>
	static inline const typeVal & getMiddle(const typeVal & a, const typeVal & nLower, const typeVal & nUpper)
	{
		return getMin(nUpper, getMax(a, nLower));
	}

	//在strReplace中将pOld替换成pNew
	template<class typeString>
	static bool replace(typeString & strReplace, const typename typeString::value_type * pOld, const typename typeString::value_type * pNew)
	{
		size_t nOldLen = StringFunc::length(pOld);
		if ( nOldLen == 0 )	//skip empty string
			return false;

		typeString strRes;
		size_t nBegin = 0;
		size_t nFindPos = strReplace.find(pOld, nBegin);
		if ( nFindPos != typeString::npos )	//reserve memory first
			strRes.reserve(strReplace.size());
		while ( nFindPos != typeString::npos )
		{
			strRes.append(strReplace.data() + nBegin, nFindPos - nBegin);
			strRes.append(pNew);
			nBegin = nFindPos + nOldLen;
			nFindPos = strReplace.find(pOld, nBegin);
		}
		if ( nBegin != 0 )
		{
			strRes.append(strReplace.data() + nBegin, strReplace.size() - nBegin);
			strReplace.swap(strRes);
		}
		return ( nBegin != 0 ); //返回是否做了替换
	}

	static void					binaryToHex(const void * pSrcBinary, size_t nSize, stdstring & strRetHex, char chDelimiter = RFC_CHAR_NULL); //小于16的字符在前面补'0'
	static bool					hexToBinary(const void * pSrcHex, size_t nSize, stdstring & strRetBinary, char chDelimiter = RFC_CHAR_NULL);

	template<typename typeList>
	class RemoveValuePred
	{
	public:
		RemoveValuePred(const typeList & listValueToRemove) : m_listValueToRemove(listValueToRemove) {}
		inline bool operator()(const typename typeList::value_type & aNode) const
		{
			return ( std::find(m_listValueToRemove.begin(), m_listValueToRemove.end(), aNode) != m_listValueToRemove.end() );
		}

	protected:
		const typeList	&	m_listValueToRemove;
	};

	template<typename typeList>
	static bool					removeValue(typeList & listValue, const typename typeList::value_type & aNode)
	{
		typename typeList::iterator it = std::remove( listValue.begin(), listValue.end(), aNode );
		if ( it == listValue.end() )
			return false;
		listValue.erase(it, listValue.end());
		return true;
	}

	template<typename typeList, typename typeRemovePred>
	static bool					removeValueByPred(typeList & listValue, const typeRemovePred & removePred)
	{
		typename typeList::iterator it = std::remove_if( listValue.begin(), listValue.end(), removePred );
		if ( it == listValue.end() )
			return false;
		listValue.erase(it, listValue.end());
		return true;
	}

	template<typename typeList>
	static bool					removeValue(typeList & listValue, const typeList & listValueToRemove)
	{
		return removeValueByPred(listValue, RemoveValuePred<typeList>(listValueToRemove));
	}

	static bool					removeSpace(stdstring & str)
	{
		return removeValueByPred(str, IsSpacePred());
	}

	//for_each_pair is like std::for_each, but just for map
	template<typename typeMap, typename typeVisitor>
	static void					for_each_pair(typeMap & mapData, typeVisitor & visitor)
	{
		typename typeMap::iterator it = mapData.begin();
		typename typeMap::iterator itEnd = mapData.end();
		while ( it != itEnd )
		{
			typename typeMap::iterator itNext = it;
			++itNext;
			if ( !visitor(it->first, it->second) )
				mapData.erase(it);
			it = itNext;
		}
	}

	template<typename typeMap, typename typeVisitor>
	static void					for_each_pair(const typeMap & mapData, typeVisitor & visitor)
	{
		typename typeMap::const_iterator it = mapData.begin();
		typename typeMap::const_iterator itEnd = mapData.end();
		for ( ; it != itEnd; ++it )
			visitor(it->first, it->second);
	}

	template<typename typeFirst, typename typeSecond>
	struct typePair
	{
		typeFirst				m_nKey;
		typeSecond				m_strValue;
	};
	template<typename typeFirst, typename typeSecond>
	static const typeSecond & getPairValue(const typeFirst & nKey, const typeSecond & strDefaultValue,
											const typePair<typeFirst, typeSecond> * pPair, size_t nSize)
	{
		for ( size_t i = 0; i < nSize; ++i )
		{
			if ( pPair[i].m_nKey == nKey )
				return pPair[i].m_strValue;
		} // for ( size_t i = 0; i < nSize; ++i )
		return strDefaultValue;
	}
};

#define		GETER_SETER_MEMBER(valType, strMemberName)			\
	public:	\
	inline const valType &	get##strMemberName(void) const { return m_##strMemberName; } \
	inline void				set##strMemberName(const valType & n) { m_##strMemberName = n; } \
	protected:	\
	valType		m_##strMemberName

#define		GETER_SETER_BITFLAGS(m_nFlags, nFlagBitMask, strMemberName)			\
	public:	\
	inline bool				get##strMemberName(void) const { return m_nFlags.check(nFlagBitMask); } \
	inline void				set##strMemberName(bool bToggle) { m_nFlags.setFlagToggle(bToggle, nFlagBitMask); }

RFC_NAMESPACE_END

#endif	//RFC_GLOBALFUNC_H_201008
