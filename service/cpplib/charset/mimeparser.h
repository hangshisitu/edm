/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_MIMEPARSER_H_201008
#define RFC_MIMEPARSER_H_201008

#include "rfcdefine.h"
#include "mimepp/mimepp.h"

RFC_NAMESPACE_BEGIN

class MailboxVisitor
{
public:
	virtual void onVisitMailbox(const DwMailbox & aMailbox) = 0;
	virtual ~MailboxVisitor(void) {}
};

// 只获取第一个地址
class MailboxFirstAddressVisitor : public MailboxVisitor
{
public:
	MailboxFirstAddressVisitor(DwString & strAddress) : m_strFristAddress(strAddress) { m_strFristAddress.clear(); }
	void onVisitMailbox(const DwMailbox & aMailbox);

private:
	DwString & m_strFristAddress;
};

//get address list
class MailboxAddressVisitor : public MailboxVisitor
{
public:
	MailboxAddressVisitor(DwString & strAddressList) : m_strAddressList(strAddressList) {}
	void onVisitMailbox(const DwMailbox & aMailbox);
	DwString& getAddressList() const { return m_strAddressList; }

private:
	DwString & m_strAddressList;
};

class MimeParser
{
public:
	static const char			g_chAddressDelimiter;
	static void					appendAddress(DwString & strAddressList, const DwString & strOneAddress);

	//for From, Resent-From
	static void					parseMailBoxList(const DwMailboxList & dwMailBoxList, MailboxVisitor & aMailboxVisitor);
	static void					parseMailBoxList(const DwString & strMailBoxList, MailboxVisitor & aMailboxVisitor);

	//for To, Cc, Bcc, Resent-To, Resent-Cc, Resent-Bcc, Reply-To, Resent-Reply-To
	static void					parseAddressList(const DwAddressList & dwAddressList, MailboxVisitor & aMailboxVisitor);
	static void					parseAddressList(const DwString & strAddressList, MailboxVisitor & aMailboxVisitor);

	static void					parseAddress(const DwAddress & dwAddress, MailboxVisitor & aMailboxVisitor);
	static void					parseAddressField(const DwField & dwField, MailboxVisitor & aMailboxVisitor);
	static void					parseAddressField(const DwHeaders & dwHeader, const char * lpszFieldName, MailboxVisitor & aMailboxVisitor);

	static bool					getFirstAddress(const DwHeaders & dwHeader, const char * lpszFieldName, DwString & strFirstAddress);
	static bool					getAddressList(const DwHeaders & dwHeader, const char * lpszFieldName, DwString & strAddressList);

	// 查找信头字段,返回在dwHeader的下标,不存在时返回-1, @nIndex = -1表示查找倒数第一个
	static int					findHeaderField(const DwHeaders & dwHeader, const char * lpszFieldName, int nIndex = 0);
	static bool					getHeaderFieldBody(const DwHeaders & dwHeader, const char * lpszFieldName,
								DwString & strFieldBody, int nIndex = 0, bool bStripCRLF = true);
	static bool					getHeaderFieldBody(const char * pHeader, size_t nHeaderSize, const char * lpszFieldName,
								DwString & strFieldBody, int nIndex = 0, bool bStripCRLF = true);
	static bool					parseFieldBody(const DwHeaders & dwHeader, const char * lpszFieldName, DwFieldBody & dwFieldBody, int nIndex = 0);
	static void					stripHeaderFieldBody(DwString & strFieldBody);
	static int					getLoopFieldCount(const DwHeaders & dwHeader, const char * lpszFieldName);

	//根据 Content-Transfer-Encoding: base64 对body part解码
	static bool					isBase64Encoding(const char * lpszEncoding);
	static bool					isQPEncoding(const char * lpszEncoding);
	static bool					isUUEncodeEncoding(const char * lpszEncoding);
	static void					decodeTransfer(const char * lpszEncoding, const DwString & strSrc, DwString & strDest);
	static void					decodeTransfer(int nEncoding, const DwString & strSrc, DwString & strDest);

	static bool					isBounceMsgSender(const DwString & strSender); //根据发件人判断是不是退信
	static bool					isReportMsgSender(const DwString & strSender); //根据from检查是不是系统发出的通知信

	//获取退信的收件人地址
	static bool					getBounceTo(const DwHeaders & dwHeader, const DwString & strSender, DwString & strBounceTo);
	static bool					getBounceTo(const char * pHeader, size_t nHeaderSize, const DwString & strSender, DwString & strBounceTo);

	//获取回复邮件的收件人地址
	static bool					getReplyTo(const DwHeaders & dwHeader, const DwString & strSender, DwString & strReplyTo);
	static bool					getReplyTo(const char * pHeader, size_t nHeaderSize, const DwString & strSender, DwString & strReplyTo);

	//获取发信时间
	static bool					getSentDate(const DwHeaders & dwHeader, DwString & strSentDate);
	static bool					getSentDate(const char * pHeader, size_t nHeaderSize, DwString & strSentDate);

	static bool					getContentTypeParameter(const DwMediaType & dwContentType, const char * lpszParamKey, DwString & strParamValue);
	static bool					getContentTypeParameter(const DwHeaders & dwHeader, const char * lpszParamKey, DwString & strParamValue);

	// 删除所有lpszFieldName字段, call dwHeader.Assemble() outside if dwHeader.IsModified()
	static void					removeHeaderField(DwHeaders & dwHeader, const char * lpszFieldName);
	static void					insertHeaderField(DwHeaders & dwHeader, const DwString & strFieldName, const DwString & strFieldBody);
	static void					updateOrInsertHeaderField(DwHeaders & dwHeader, const char * lpszFieldName, const DwString & strFieldBody);

	static void			getTextBodyPart(const DwEntity & dwBodyPart, const DwEntity ** ppPlainPart, const DwEntity ** ppHtmlPart, int nNestedLevel = 5);

	static inline bool			isValidEmailChar(char ch)
	{
		size_t nIndex = static_cast<unsigned char>(ch);
		return ( nIndex < 256 && m_lpValidEmailChar[nIndex] != 0 );
	}
	static bool					isValidEmailAddress(const DwString & strEmailAddress);

	static void					splitLetter(const char * pWholeLetter, size_t nWholeLetterSize, size_t & nHeaderSize, size_t & nBodySize);
	static void					splitLetter(const DwString & strWholeLetter, size_t & nHeaderSize, size_t & nBodySize);

	//将"user@domain"切割得到"user"和"domain", 返回是否获得domain
	//如果传入的strEmailAddress没有domain,则strUserPart=strEmailAddress,strDomainPart为空
	template<class typeString>
	static bool					splitEmailAddress(const typeString & strEmailAddress, typeString & strUserPart, typeString & strDomainPart)
	{
		size_t nAtPos = strEmailAddress.find('@');
		if( nAtPos == typeString::npos ) //没有'@'
		{
			strUserPart = strEmailAddress;
			strDomainPart.clear();
		}
		else
		{
			strUserPart = strEmailAddress.substr(0, nAtPos);
			strDomainPart = strEmailAddress.substr(nAtPos + 1);
		}
		return ( !strDomainPart.empty() );
	}

	template<class typeString>
	static typeString &			makeWildCharEmailAddressByUser(typeString & strUserPart)
	{
		if ( strUserPart.find('@') == DwString::npos )
			strUserPart += "@*";
		return strUserPart;
	}

	template<class typeString>
	static typeString &			makeWildCharEmailAddressByDomain(typeString & strDomainPart)
	{
		if ( strDomainPart.find('@') == DwString::npos )
			strDomainPart = "*@" + strDomainPart;
		return strDomainPart;
	}

	static bool					isSameDomain(const DwString & strEmailAddr1, const DwString & strEmailAddr2);

	template<typename typeFieldBody>
	static const typeFieldBody * getHeaderFieldBody(const DwHeaders & dwHeader, const char * lpszFieldName, int nIndex = 0)
	{
		int nFieldIndex = findHeaderField(dwHeader, lpszFieldName, nIndex);
		if ( nFieldIndex < 0 || nFieldIndex >= dwHeader.NumFields() )
			return NULL;
		const DwField & dwField = dwHeader.FieldAt(nFieldIndex);
		return dynamic_cast<const typeFieldBody *>( dwField.FieldBody() );
	}

private:
	static char					m_lpValidEmailChar[256];
};

RFC_NAMESPACE_END

#endif	//RFC_MIMEPARSER_H_201008
