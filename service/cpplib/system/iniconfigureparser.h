/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_INICONFIGUREPARSER_H_201008
#define RFC_INICONFIGUREPARSER_H_201008

#include "protocolbuf/protocolvariant.h"
#include "stream/linestream.h"

RFC_NAMESPACE_BEGIN

/*
"# this is a ini configure test\r\n"
"! comment should support ! # ;\r\n"
"; support xml format and ini format\\\r\n"
"second line\\\r\n\r\n"
"[Section 1]\r\n"
"Key1=\"111111111111\"	;comment\r\n"
"Key2='222222 \"hehehehe \\\" 22222222'	# comment2\r\n"
"Key3=333333333333\r\n"
"Key4 44444441 !conment: space split key and value\r\n"
"Key4	44444444442 #conment: tab split key and value\r\n\r\n"
"[Section 2/sub section/sub path2]\r\n"
"K1=\"111111111111\"					! split the line\\\r\n"
"second line comment\r\n"
"K1=\"222222222222222\\\r\n"
"333333333333\r\n"
"k1 = \"3333333333333\"";
*/

DECLARE_EXCEPTION(IniParserException, Exception)

class IniConfigureParser
{
public:
	IniConfigureParser(void) : m_bReplaceEnvVar(true), m_bSkipError(true) {}

	bool					getReplaceEnvVar(void) const { return m_bReplaceEnvVar; }
	void					setReplaceEnvVar(bool bReplaceEnvVar) { m_bReplaceEnvVar = bReplaceEnvVar; }
	bool					getSkipError(void) const { return m_bSkipError; }
	void					setSkipError(bool bSkipError) { m_bSkipError = bSkipError; }
	void					setReplaceEnvVar(const stdstring & strKey, const stdstring & strValue);
	bool					getReplaceEnvVar(const stdstring & strKey, stdstring & strValue) const;

	void					parseContent(const stdstring & strContent, PVariant & varIniConfigure);
	void					loadFromStream(ReadStream & inputStream, PVariant & varIniConfigure);
	void					loadFromLineStream(ReadLineStream & lineStream, PVariant & varIniConfigure);

protected:
	// like strchr, but skip the char after '\';
	static const char *		findTheEnd(const char * pBegin, const char * pEnd, char chEnd);
	static bool				needNextLine(const stdstring & strCurrentLine);

	// remove the ' or " in string header and tail
	// replcase env var
	void					decodeValue(const char * pBegin, const char * pEnd, stdstring & strValue) const;

	// 读取一行,如果以\结尾,则合并连接下一行
	bool					getIniLine(ReadLineStream & lineStream, stdstring & strLine, bool bTrimSpace) const;
	bool					parseLine(const stdstring & strLine, stdstring & strKey, stdstring & strValue, stdstring & strComment) const;

private:
	typedef std::map<stdstring, stdstring>		typeReplaceEnvVarMap;
	static const stdstring	m_strCommentSymbol;
	bool					m_bReplaceEnvVar;
	bool					m_bSkipError;
	typeReplaceEnvVarMap	m_mapReplaceEnvVar;
};




RFC_NAMESPACE_END

#endif	//RFC_INICONFIGUREPARSER_H_201008
