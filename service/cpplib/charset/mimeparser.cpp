/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "charset/mimeparser.h"
#include "base/globalfunc.h"
#include "base/datetime.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

void MailboxFirstAddressVisitor::onVisitMailbox(const DwMailbox & aMailbox)
{
	if ( !m_strFristAddress.empty() )
		return;
	m_strFristAddress = ( aMailbox.LocalPart() + '@' + aMailbox.Domain() );
	if ( !MimeParser::isValidEmailAddress(m_strFristAddress) )
		m_strFristAddress.clear();
}

void MailboxAddressVisitor::onVisitMailbox(const DwMailbox & aMailbox)
{
	DwString strUserAtDomain( aMailbox.LocalPart() + '@' + aMailbox.Domain() );
	if ( MimeParser::isValidEmailAddress(strUserAtDomain) )
		MimeParser::appendAddress(m_strAddressList, strUserAtDomain);
}

//////////////////////////////////////////////////////////////////////////
const char MimeParser::g_chAddressDelimiter = ',';
char MimeParser::m_lpValidEmailChar[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	//000 - 015		0
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	//016 - 031		1
	0,1,0,1,1,1,1,1,0,0,1,1,0,1,1,1,	//032 - 047		2
	1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,1,	//048 - 063		3
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	//064 - 079		4
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,	//080 - 095		5
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	//096 - 111		6
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	//112 - 127		7
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

void MimeParser::appendAddress(DwString & strAddressList, const DwString & strOneAddress)
{
	if ( !strAddressList.empty() )
		strAddressList += g_chAddressDelimiter;
	strAddressList += strOneAddress;
}

void MimeParser::parseMailBoxList(const DwMailboxList & dwMailBoxList, MailboxVisitor & aMailboxVisitor)
{
	int nMailBoxCount = dwMailBoxList.NumMailboxes();
	for ( int i = 0; i < nMailBoxCount; ++i )
	{
		const DwMailbox & aMailbox = dwMailBoxList.MailboxAt(i);
		if ( aMailbox.IsMailbox() )
			aMailboxVisitor.onVisitMailbox(aMailbox);
	} //for ( int i = 0; i < nMailBoxCount; ++i )
}

void MimeParser::parseMailBoxList(const DwString & strMailBoxList, MailboxVisitor & aMailboxVisitor)
{
	DwMailboxList aMailboxList(strMailBoxList);
	aMailboxList.Parse();
	return parseMailBoxList(aMailboxList, aMailboxVisitor);
}

void MimeParser::parseAddressList(const DwAddressList & dwAddressList, MailboxVisitor & aMailboxVisitor)
{
	int nAddrCnt = dwAddressList.NumAddresses();
	for ( int i = 0; i < nAddrCnt; ++i )
		parseAddress(dwAddressList.AddressAt(i), aMailboxVisitor);
}

void MimeParser::parseAddressList(const DwString & strAddressList, MailboxVisitor & aMailboxVisitor)
{
	DwAddressList aAddressList(strAddressList);
	aAddressList.Parse();
	return parseAddressList(aAddressList, aMailboxVisitor);
}

void MimeParser::parseAddress(const DwAddress & dwAddress, MailboxVisitor & aMailboxVisitor)
{
	if ( dwAddress.IsMailbox() )
	{
		aMailboxVisitor.onVisitMailbox( reinterpret_cast<const DwMailbox&>(dwAddress) );
	}
	else if ( dwAddress.IsGroup() )
	{
		const DwGroup & dwGroup = reinterpret_cast<const DwGroup &>(dwAddress);
		parseMailBoxList(dwGroup.MailboxList(), aMailboxVisitor);
	}
}

void MimeParser::parseAddressField(const DwField & dwField, MailboxVisitor & aMailboxVisitor)
{
	DwFieldBody * pFieldBody = dwField.FieldBody();
	if ( pFieldBody == NULL )
		return;

	int nFieldType = pFieldBody->ClassId();
	if ( nFieldType == DwMessageComponent::kCidMailbox )
		aMailboxVisitor.onVisitMailbox( reinterpret_cast<const DwMailbox &>(*pFieldBody) );
	else if ( nFieldType == DwMessageComponent::kCidMailboxList )
		parseMailBoxList(reinterpret_cast<const DwMailboxList &>(*pFieldBody), aMailboxVisitor);
	else if ( nFieldType == DwMessageComponent::kCidAddress )
		parseAddress(reinterpret_cast<const DwAddress &>(*pFieldBody), aMailboxVisitor);
	else if ( nFieldType == DwMessageComponent::kCidAddressList )
		parseAddressList(reinterpret_cast<const DwAddressList &>(*pFieldBody), aMailboxVisitor);
	else	//尝试以DwAddressList的格式再parse一次
		parseAddressList(pFieldBody->AsString(), aMailboxVisitor);
}

void MimeParser::parseAddressField(const DwHeaders & dwHeader, const char * lpszFieldName, MailboxVisitor & aMailboxVisitor)
{
	DwField * pField = dwHeader.FindField(lpszFieldName);
	if ( pField == NULL )
		return;
	parseAddressField(*pField, aMailboxVisitor);
}

bool MimeParser::getFirstAddress(const DwHeaders & dwHeader, const char * lpszFieldName, DwString & strFirstAddress)
{
	DwField * pField = dwHeader.FindField(lpszFieldName);
	if ( pField == NULL )
		return false;

	MailboxFirstAddressVisitor addressVisitor(strFirstAddress);
	parseAddressField(*pField, addressVisitor);
	return isValidEmailAddress(strFirstAddress);
}

bool MimeParser::getAddressList(const DwHeaders & dwHeader, const char * lpszFieldName, DwString & strAddressList)
{
	DwField * pField = dwHeader.FindField(lpszFieldName);
	if ( pField == NULL )
		return false;

	MailboxAddressVisitor addressVisitor(strAddressList);
	parseAddressField(*pField, addressVisitor);
	return ( !strAddressList.empty() );
}

int MimeParser::findHeaderField(const DwHeaders & dwHeader, const char * lpszFieldName, int nIndex)
{
	int nFieldIndex = -1;
	for ( int i = 0; i < dwHeader.NumFields(); ++i )
	{
		const DwField & dwField = dwHeader.FieldAt(i);
		if ( DwStrcasecmp(dwField.FieldNameStr(), lpszFieldName) != 0 )
			continue;
		nFieldIndex = i;
		if ( nIndex-- == 0 )
			break;
	} // for ( int i = 0; i < nFieldCount; ++i )
	return ( nIndex >= 0 ? -1 : nFieldIndex );
}

bool MimeParser::getHeaderFieldBody(const DwHeaders & dwHeader, const char * lpszFieldName,
									DwString & strFieldBody, int nIndex, bool bStripCRLF)
{
	int nFieldIndex = findHeaderField(dwHeader, lpszFieldName, nIndex);
	if ( nFieldIndex < 0 || nFieldIndex >= dwHeader.NumFields() )
		return false;

	const DwField & dwField = dwHeader.FieldAt(nFieldIndex);
	strFieldBody.assign(dwField.FieldBodyStr().data(), dwField.FieldBodyStr().length());
	if( bStripCRLF )
		stripHeaderFieldBody(strFieldBody);
	return ( !strFieldBody.empty() );
}

bool MimeParser::getHeaderFieldBody(const char * pHeader, size_t nHeaderSize, const char * lpszFieldName,
									DwString & strFieldBody, int nIndex, bool bStripCRLF)
{
	size_t nFieldLen = ( lpszFieldName == NULL ? 0 : StringFunc::length(lpszFieldName) );
	if ( nFieldLen == 0 )
		return false;

	const char * const pHeaderEnd = pHeader + nHeaderSize;
	const char * pFieldBegin = pHeader;
	while ( true )
	{
		if ( StringFunc::strCasecmp(pFieldBegin, GlobalFunc::getMin(nFieldLen, static_cast<size_t>(pHeaderEnd - pFieldBegin)), lpszFieldName, nFieldLen) == 0 )
		{
			for ( pFieldBegin += nFieldLen; pFieldBegin < pHeaderEnd && ::isspace(*pFieldBegin); )
				++pFieldBegin;
			if ( *pFieldBegin == ':' )
			{
				++pFieldBegin;
				if ( nIndex == 0 )
					break;
				--nIndex;
			}
		}

		const char * pCRLF = std::find( pFieldBegin, pHeaderEnd, '\n' );
		if ( pCRLF == pHeaderEnd )
			pCRLF = std::find( pFieldBegin, pHeaderEnd, '\r' );
		if ( pCRLF == pHeaderEnd )
			return false;
		pFieldBegin = pCRLF + 1;	//skip '\n'
	} // while ( true )

	if ( nIndex > 0 )
		return false;

	// Remove any white space at the begin of field body
	while ( pFieldBegin < pHeaderEnd && ::isspace(*pFieldBegin) )
		++pFieldBegin;
	const char * pFieldEnd = pFieldBegin;
	char chLast = ' ';
	for ( ; pFieldEnd < pHeaderEnd; ++pFieldEnd )
	{
		char ch = (*pFieldEnd);
		if ( (chLast == '\n' && ch != '\t' && ch != ' ') || (chLast == '\r' && ch != '\n' && ch != '\t' && ch != ' ') )
			break;
		chLast = ch;
	} //for ( ; pFieldEnd < pHeaderEnd; ++pFieldEnd )

	// Remove any white space at the end of field body
	while ( pFieldBegin + 1 < pFieldEnd && ::isspace(*(pFieldEnd-1)) )
		--pFieldEnd;

	strFieldBody.assign(pFieldBegin, pFieldEnd - pFieldBegin);
	if ( bStripCRLF )
		stripHeaderFieldBody(strFieldBody);
	return true;
}

bool MimeParser::parseFieldBody(const DwHeaders & dwHeader, const char * lpszFieldName, DwFieldBody & dwFieldBody, int nIndex)
{
	DwString strFieldBody;
	if ( !getHeaderFieldBody(dwHeader, lpszFieldName, strFieldBody, nIndex) )
		return false;

	dwFieldBody.FromString( strFieldBody );
	dwFieldBody.Parse();
	return true;
}

void MimeParser::stripHeaderFieldBody(DwString & strFieldBody)
{
	GlobalFunc::replace(strFieldBody, "\r\n\t", "");
	GlobalFunc::replace(strFieldBody, "\r\n ", "");
	GlobalFunc::replace(strFieldBody, "\n\t", "");
	GlobalFunc::replace(strFieldBody, "\n ", "");
}

int MimeParser::getLoopFieldCount(const DwHeaders & dwHeader, const char * lpszFieldName)
{
	int nLoopFieldCount = 0;
	for ( int i = 0; i < dwHeader.NumFields(); ++i )
	{
		const DwField & dwField = dwHeader.FieldAt(i);
		if ( DwStrcasecmp(dwField.FieldNameStr(), lpszFieldName) == 0 )
			++nLoopFieldCount;
	} // for ( int i = 0; i < dwHeader.NumFields(); ++i )
	return nLoopFieldCount;
}

bool MimeParser::isBase64Encoding(const char * lpszEncoding)
{
	return ( StringFunc::strCasecmp(lpszEncoding, "base64") == 0 );
}

bool MimeParser::isQPEncoding(const char * lpszEncoding)
{
	return ( StringFunc::strCasecmp(lpszEncoding, "quoted-printable") == 0 );
}

bool MimeParser::isUUEncodeEncoding(const char * lpszEncoding)
{
	return ( StringFunc::strCasecmp(lpszEncoding, "uuencode") == 0
		|| StringFunc::strCasecmp(lpszEncoding, "x-uuencode") == 0
		|| StringFunc::strCasecmp(lpszEncoding, "x-uue") == 0 );
}

void MimeParser::decodeTransfer(const char * lpszEncoding, const DwString & strSrc, DwString & strDest)
{
	if ( isBase64Encoding(lpszEncoding) )
		DwDecodeBase64(strSrc, strDest);
	else if ( isQPEncoding(lpszEncoding) )
		DwDecodeQuotedPrintable(strSrc, strDest);
	else // if ( isUUEncodeEncoding(lpszEncoding) )
		strDest = strSrc;
}

void MimeParser::decodeTransfer(int nEncoding, const DwString & strSrc, DwString & strDest)
{
	if ( nEncoding == DwMime::kCteBase64 )
		DwDecodeBase64(strSrc, strDest);
	else if ( nEncoding == DwMime::kCteQuotedPrintable )
		DwDecodeQuotedPrintable(strSrc, strDest);
	else
		strDest = strSrc;
}

bool MimeParser::isBounceMsgSender(const DwString & strSender)
{
	static const DwString strPostMasterAt = "postmaster@";
	return ( strSender.empty() || DwStrcasecmp(strSender, "postmaster") == 0 ||
			DwStrncasecmp(strSender, strPostMasterAt , strPostMasterAt.size()) == 0 );
}

bool MimeParser::isReportMsgSender(const DwString & strSender)
{
	static const DwString strMailerDaemon = "mailer-daemon@";
	static const DwString strNoReply1 = "noreply@";
	static const DwString strNoReply2 = "no-reply@";
	static const DwString strNoReply3 = "no_reply@";

	return ( isBounceMsgSender(strSender) ||
		DwStrncasecmp(strSender, strMailerDaemon, strMailerDaemon.size()) == 0 ||
		DwStrncasecmp(strSender, strNoReply1, strNoReply1.size()) == 0 ||
		DwStrncasecmp(strSender, strNoReply2, strNoReply2.size()) == 0 ||
		DwStrncasecmp(strSender, strNoReply3, strNoReply3.size()) == 0 );
}

bool MimeParser::getBounceTo(const DwHeaders & dwHeader, const DwString & strSender, DwString & strBounceTo)
{
	if ( isBounceMsgSender(strSender) )		// 如果原信就是退信,则不再产生新的退信
		return false;

	// Judge with the following sequence: "Errors-To" "Return-Path" "Resent-From" strSender
	if ( getFirstAddress(dwHeader, "Errors-To", strBounceTo) )
		return true;
	if ( getFirstAddress(dwHeader, "Return-Path", strBounceTo) )
		return true;
	if ( getFirstAddress(dwHeader, "Resent-From", strBounceTo) )
		return true;

	strBounceTo = strSender;
	return isValidEmailAddress(strBounceTo);
}

bool MimeParser::getBounceTo(const char * pHeader, size_t nHeaderSize, const DwString & strSender, DwString & strBounceTo)
{
	DwHeaders dwHeader( DwString(pHeader, nHeaderSize) );
	dwHeader.Parse();
	return getBounceTo(dwHeader, strSender, strBounceTo);
}

bool MimeParser::getReplyTo(const DwHeaders & dwHeader, const DwString & strSender, DwString & strReplyTo)
{
	if ( isBounceMsgSender(strSender) )		// 如果原信就是退信,则不再产生回复邮件
		return false;

	// Judge with the following sequence: "Reply-To" "Resent-From" strSender
	if ( getFirstAddress(dwHeader, "Reply-To", strReplyTo) )
		return true;
	if ( getFirstAddress(dwHeader, "Resent-From", strReplyTo) )
		return true;

	strReplyTo = strSender;
	return isValidEmailAddress(strReplyTo);
}

bool MimeParser::getReplyTo(const char * pHeader, size_t nHeaderSize, const DwString & strSender, DwString & strReplyTo)
{
	DwHeaders dwHeader( DwString(pHeader, nHeaderSize) );
	dwHeader.Parse();
	return getReplyTo(dwHeader, strSender, strReplyTo);
}

bool MimeParser::getSentDate(const DwHeaders & dwHeader, DwString & strSentDate)
{
	// 优先使用Received的时间,如果没有Received字段则使用Date字段的时间
	DateTime dtSentDate;
	const DwStamp * pReceived = getHeaderFieldBody<DwStamp>(dwHeader, "Received", -1);
	const DwDateTime * pDate = getHeaderFieldBody<DwDateTime>(dwHeader, "Date");
	if ( (pReceived != NULL) && pReceived->Date().Year() > 1970 )
		dtSentDate.setStandardTime( static_cast<time_t>( pReceived->Date().AsUnixTime()) );
	else if ( (pDate != NULL) && pDate->Year() > 1970 )
		dtSentDate.setStandardTime( static_cast<time_t>( pDate->AsUnixTime()) );
	else
		return false;

	strSentDate = dtSentDate.formatDateTime().c_str();
	return true;
}

bool MimeParser::getSentDate(const char * pHeader, size_t nHeaderSize, DwString & strSentDate)
{
	DwHeaders dwHeader( DwString(pHeader, nHeaderSize) );
	dwHeader.Parse();
	return getSentDate(dwHeader, strSentDate);
}

bool MimeParser::getContentTypeParameter(const DwMediaType & dwContentType, const char * lpszParamKey, DwString & strParamValue)
{
	for ( int i = 0; i < dwContentType.NumParameters(); ++i )
	{
		if ( DwStrcasecmp(dwContentType.ParameterAt(i).Attribute(), lpszParamKey) == 0 )
		{
			strParamValue = dwContentType.ParameterAt(i).Value();
			return true;
		}
	} // for ( int i = 0; i < dwContentType.NumParameters(); ++i )
	return false;
}

bool MimeParser::getContentTypeParameter(const DwHeaders & dwHeader, const char * lpszParamKey, DwString & strParamValue)
{
	const DwMediaType * pContentType = getHeaderFieldBody<DwMediaType>(dwHeader, "Content-Type");
	if ( pContentType == NULL )
		return false;
	return getContentTypeParameter(*pContentType, lpszParamKey, strParamValue);
}

void MimeParser::removeHeaderField(DwHeaders & dwHeader, const char * lpszFieldName)
{
	while ( true )
	{
		int nFieldIndex = findHeaderField(dwHeader, lpszFieldName);
		if ( nFieldIndex < 0 || nFieldIndex >= dwHeader.NumFields() )
			return;
		DwField * pDelField = dwHeader.RemoveFieldAt(nFieldIndex);
		delete pDelField;
	} // while ( true )
}

void MimeParser::insertHeaderField(DwHeaders & dwHeader, const DwString & strFieldName, const DwString & strFieldBody)
{
	DwField * pField = DwField::NewField("", &dwHeader);
	pField->SetFieldNameStr(strFieldName);
	DwFieldBody * pFieldBody = DwField::CreateFieldBody(strFieldName, strFieldBody, pField);
	pField->SetFieldBody(pFieldBody);
	dwHeader.AddField(pField);
}

void MimeParser::updateOrInsertHeaderField(DwHeaders & dwHeader, const char * lpszFieldName, const DwString & strFieldBody)
{
	DwFieldBody & dwFieldBody = dwHeader.FieldBody(lpszFieldName);
	dwFieldBody.FromString(strFieldBody);
}

void MimeParser::getTextBodyPart(const DwEntity & dwBodyPart, const DwEntity ** ppPlainPart, const DwEntity ** ppHtmlPart, int nNestedLevel)
{
	if ( nNestedLevel <= 0 )	// 不再继续往下嵌套查找
		return;

	int nType = DwMime::kTypeNull;
	int nSubType = DwMime::kSubtypeNull;
	if ( dwBodyPart.Headers().HasContentType() )
	{
		const DwMediaType & dwContentType = dwBodyPart.Headers().ContentType();
		nType =dwContentType.Type();
		nSubType = dwContentType.Subtype();
	}

	if ( nType == DwMime::kTypeMultipart )
	{
		int nNumBodyPart = dwBodyPart.Body().NumBodyParts();
		for ( int i = 0; i < nNumBodyPart; ++i )
		{
			DwEntity & dwReparseBodyPart = dwBodyPart.Body().BodyPartAt(i);
			dwReparseBodyPart.Parse();
			getTextBodyPart(dwReparseBodyPart, ppPlainPart, ppHtmlPart, nNestedLevel - 1);
			if ( *ppPlainPart != NULL && *ppHtmlPart != NULL)
				return;
		}
	}
	else if ( nType == DwMime::kTypeText || nType == DwMime::kTypeNull )
	{
		if ( (nSubType == DwMime::kSubtypePlain || nSubType == DwMime::kSubtypeNull) && *ppPlainPart == NULL )
			*ppPlainPart = &dwBodyPart;
		else if ( nSubType == DwMime::kSubtypeHtml && *ppHtmlPart == NULL )
			*ppHtmlPart = &dwBodyPart;
	}
}

bool MimeParser::isValidEmailAddress(const DwString & strEmailAddress)
{
	const char * const pMailBoxBegin = strEmailAddress.data();
	const char * const pMailBoxEnd = pMailBoxBegin + strEmailAddress.length();
	bool bGetAt = false, bGetDomainPoint = false;
	for ( const char * pCh = pMailBoxBegin; pCh < pMailBoxEnd; ++pCh )
	{
		if ( *pCh == '@' )
		{
			// '@'不能重复出现，并且不能是第一个或最后一个字符
			if ( bGetAt || pCh == pMailBoxBegin || pCh + 1 == pMailBoxEnd )
				return false;
			bGetAt = true;
		}
		else if ( bGetAt && *pCh == '.' )
		{
			if ( *(pCh - 1) == '@' || pCh + 1 >= pMailBoxEnd )		// 域名不能以.开头和结束
				return false;
			bGetDomainPoint = true;
		}
		else if ( !isValidEmailChar(*pCh) )
			return false;
	} //for ( const char * pCh = pMailBoxBegin; pCh < pMailBoxEnd; ++pCh )
	//return ( bGetAt && (*pMailBoxBegin != '\'' || *pMailBoxBegin != *(pMailBoxEnd - 1)) );
	return ( bGetAt && bGetDomainPoint );
}

void MimeParser::splitLetter(const char * pWholeLetter, size_t nWholeLetterSize, size_t & nHeaderSize, size_t & nBodySize)
{
	nHeaderSize = nBodySize = 0;
	if ( pWholeLetter == NULL || nWholeLetterSize == 0 )
		return;

	char chCur, chLast = '\0';
	const char * pCur = pWholeLetter;
	const char * pLetterEnd = pWholeLetter + nWholeLetterSize;
	for ( ; pCur < pLetterEnd; ++pCur )
	{
		chCur = (*pCur);
		if ( (chLast == '\r' && chCur == '\r') ||(chLast == '\n') && (chCur == '\r' || chCur == '\n') )
		{
			++pCur;
			if ( chCur == '\r' && pCur < pLetterEnd && *pCur == '\n' )
				++pCur;
			break;
		}
		chLast = chCur;
	} //for ( ; pCur < pLetterEnd; ++pCur )
	nHeaderSize = static_cast<size_t>( pCur - pWholeLetter );
	nBodySize = static_cast<size_t>( pLetterEnd - pCur );
}

void MimeParser::splitLetter(const DwString & strWholeLetter, size_t & nHeaderSize, size_t & nBodySize)
{
	return splitLetter(strWholeLetter.data(), strWholeLetter.size(), nHeaderSize, nBodySize);
}

RFC_NAMESPACE_END
