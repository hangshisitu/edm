/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_MIMEDECODER_H_201008
#define RFC_MIMEDECODER_H_201008

#include "charset/mimeparser.h"
#include <list>

RFC_NAMESPACE_BEGIN

struct DecodeMailbox
{
	DwString								m_strFullName;
	DwString								m_strAddress;
};
typedef std::list<DecodeMailbox>			typeDecodeMailboxList;

//get decode full name and address list
class DecodeMailboxVisitor : public MailboxVisitor
{
public:
	DecodeMailboxVisitor(typeDecodeMailboxList & listDecodeMailbox);
	DecodeMailboxVisitor(typeDecodeMailboxList & listDecodeMailbox, const DwString & strDestCharset);
	void onVisitMailbox(const DwMailbox & aMailbox);

	static void convertListToString(const typeDecodeMailboxList & listDecodeMailbox, DwString & strDecodeList);

private:
	typeDecodeMailboxList &	m_listDecodeMailbox;
	DwString				m_strDestCharset;
};

class MimeDecoder
{
public:
	static const DwString & getDefaultCharset(void) { return m_strDefaultCharset; }
	static void		setDefaultCharset(const DwString & strDefaultCharset) { m_strDefaultCharset = strDefaultCharset; }
	
	static void		decodeMimeString(const DwString & strSrcContent, DwString & strDestContent, bool bSubjectFiled,
									 const DwString & strDestCharset = m_strDefaultCharset);

	// The chEncoding must be one of: 'Q', 'q', 'B', 'b', or NUL.
	static void		encodeMimeString(const DwString & strSrcContent, const DwString & strSrcCharset, DwString & strDestContent, 
									 const DwString & strDestCharset = m_strDefaultCharset, char chEncoding = RFC_CHAR_NULL);

	static void		convertCharset(const char * pSrcContent, size_t nSrcLen, const DwString & strSrcCharset, DwString & strDestContent, 
								  const DwString & strDestCharset = m_strDefaultCharset);
	static void		convertCharset(const DwString & strSrcContent, const DwString & strSrcCharset, DwString & strDestContent, 
								  const DwString & strDestCharset = m_strDefaultCharset);

	static void		decodeMailbox(const DwMailbox & aMailbox, typeDecodeMailboxList & listDecodeMailbox,
								  const DwString & strDestCharset = m_strDefaultCharset);
	static void		decodeAddress(const DwAddress & dwAddress, typeDecodeMailboxList & listDecodeMailbox,
								  const DwString & strDestCharset = m_strDefaultCharset);
	static void		decodeAddressField(const DwField & dwField, typeDecodeMailboxList & listDecodeMailbox,
								  const DwString & strDestCharset = m_strDefaultCharset);

	//for From, Resent-From
	static void		decodeMailboxList(const DwMailboxList & dwMailBoxList, typeDecodeMailboxList & listDecodeMailbox,
								 const DwString & strDestCharset = m_strDefaultCharset);
	static void		decodeMailboxList(const DwString & strMailBoxList, typeDecodeMailboxList & listDecodeMailbox,
								 const DwString & strDestCharset = m_strDefaultCharset);
	static void		decodeMailboxList(const DwMailboxList & dwMailBoxList, DwString & strDecodeMailboxList,
								 const DwString & strDestCharset = m_strDefaultCharset);

	//for To, Cc, Bcc, Resent-To, Resent-Cc, Resent-Bcc, Reply-To, Resent-Reply-To
	static void		decodeAddressList(const DwAddressList & dwAddressList, typeDecodeMailboxList & listDecodeMailbox,
								 const DwString & strDestCharset = m_strDefaultCharset);
	static void		decodeAddressList(const DwString & strAddressList, typeDecodeMailboxList & listDecodeMailbox,
								 const DwString & strDestCharset = m_strDefaultCharset);
	static void		decodeAddressList(const DwAddressList & dwAddressList, DwString & strDecodeMailboxList,
								 const DwString & strDestCharset = m_strDefaultCharset);

	static void		decodeSubject(const DwHeaders & dwHeader, DwString & strSubject, const DwString & strDestCharset = m_strDefaultCharset);

protected:
	static void		trimMimeString(const DwString & strEncodeSrc, DwString & strTrimEncodeDest);
	static void		decodeOneWord(const DwEncodedWord & dwWord, const DwString & strDestCharset, DwString & strPreCharset,
								  DwString & strPreEncodedData, DwString & strDestContent);

private:
	static DwString			m_strDefaultCharset;
};


RFC_NAMESPACE_END

#endif	//RFC_MIMEDECODER_H_201008
