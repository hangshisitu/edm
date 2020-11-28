/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_HTMLPARSER_H_201008
#define RFC_HTMLPARSER_H_201008

#include "base/globalfunc.h"
#include <list>
#include <set>

RFC_NAMESPACE_BEGIN

typedef std::pair<stdstring, stdstring>		typeTagAttributePair;
typedef std::list<typeTagAttributePair>		typeTagAttributeList;
typedef std::set<stdstring, IgnoreCaseStringCompare>	typeURLSet;

struct HtmlTag
{
	HtmlTag(void) : m_bClosedTag(false) {}
	bool						m_bClosedTag;
	stdstring					m_strTagName;
	typeTagAttributeList		m_listAttribute;
};

class HtmlParser
{
public:
	virtual ~HtmlParser(void) {}
	void				parseHtml(const char * pHtml, size_t nHtmlLen);

	static const stdstring		m_strPlainTextTag;
	static const stdstring		m_strXmpTag;
	static const stdstring		m_strTextAreaTag;
	static const stdstring		m_strStyleTag;
	static const stdstring		m_strScriptTag;

	static const FindSubString	m_strClosedXmp;
	static const FindSubString	m_strClosedTextArea;
	static const FindSubString	m_strClosedStyle;
	static const FindSubString	m_strClosedScript;

protected:
	virtual bool		mustExitParser(void) const { return false; }
	virtual void		onTagAttributeFormatError(void) {}
	virtual void		onHtmlTag(const HtmlTag & htmlTag) = 0;
	virtual void		onHtmlContent(const char * pContentBegin, const char * pContentEnd) = 0;
	virtual void		onHtmlComment(const char * pCommentBegin, const char * pCommentEnd) = 0;
	virtual void		onPlainContent(const char * pContentBegin, const char * pContentEnd) = 0;
	virtual void		onStyleContent(const char * pContentBegin, const char * pContentEnd) {}
	virtual void		onScriptContent(const char * pContentBegin, const char * pContentEnd) {}
	virtual void		onStart(void) {}			// 开始分析前初始化工作
	virtual void		onFinish(void) {}			// 完成分析后的工作

private:
	static bool			isLiteralTag(const stdstring & strTagName);
	static const char *	findLiteralClosedTag(const FindSubString & strPattern, const char * pBegin, const char * pHtmlEnd);
	const char *		parseLiteralTag(const stdstring & strTagName, const char * pBegin, const char * pHtmlEnd);
	const char *		parseHtmlTag(const char * pTagBegin, const char * pHtmlEnd);
	const char *		parseComment(const char * pTagBegin, const char * pHtmlEnd);
};

// 分析html文本内容 该类不是线程安全
class HtmlTextParser : public HtmlParser
{
public:
	virtual ~HtmlTextParser(void) {}
	const stdstring &	getFullText(void) const { return m_strFullText; }
	const typeURLSet &	getURLSet(void) const { return m_setURL; }

protected:
	virtual void		onHtmlTag(const HtmlTag & htmlTag);
	virtual void		onHtmlContent(const char * pContentBegin, const char * pContentEnd);
	virtual void		onHtmlComment(const char * pCommentBegin, const char * pCommentEnd) {}
	virtual void		onPlainContent(const char * pContentBegin, const char * pContentEnd);
	virtual void		onFinish(void);

	void				fetchURL(const HtmlTag & htmlTag);
private:
	stdstring			m_strFullText;
	typeURLSet			m_setURL;
};

RFC_NAMESPACE_END

#endif	//RFC_HTMLPARSER_H_201008
