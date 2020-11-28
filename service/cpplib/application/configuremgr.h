/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_CONFIGUREMGR_H_201008
#define RFC_CONFIGUREMGR_H_201008

#include "protocolbuf/protocolvariant.h"
#include "stream/linestream.h"
#include "stream/binarystream.h"
#include "system/filesystem.h"
#include "base/exception.h"
#include "base/formatstream.h"
#include <list>
#include <vector>

RFC_NAMESPACE_BEGIN

#define CONF_NAME_HOSTS							"hosts"
#define CONF_NAME_DATASOURCES					"datasources"
#define CONF_NAME_APPLICATION					"application"
#define CONF_NAME_PROGRAMSLIST					"ProgramsList"

DECLARE_EXCEPTION(ConfigureException, Exception)

typedef std::list<stdstring>					typeSubItemNameList;
typedef std::pair<stdstring, stdstring>			typeConfigurePair;
typedef std::list<typeConfigurePair>			typeConfPairList;

class ConfigureMgr
{
public:
	ConfigureMgr(void);
	bool					loadConfFile(const stdstring & strFullPath);
	bool					loadAllConfFile(const stdstring & strConfigureDir, const stdstring & strPostfix);
	void					clear(void) { m_varAllConf.setNULL(); m_strLocalHostID.clear(); }

	void					setOverloaded(bool bOverloaded) { m_bOverloaded = bOverloaded; }
	bool					isOverloaded(void) const { return m_bOverloaded; }
	bool					findOverloaded(const stdstring & strKey) const;
	const stdstring	&		getLocalHostID(void) const { return m_strLocalHostID; }
	// 读取hosts.conf[hostid]下面的配置项
	stdstring				getHostConfigureFullPath(const stdstring & strKey) const;

	bool					getBool(const stdstring & strKey, size_t nIndex = 0) const;
	bool					getBool(bool bDefault, const stdstring & strKey, size_t nIndex = 0) const;

	int						getInt(const stdstring & strKey, size_t nIndex = 0) const;
	int						getInt(int nDefault, const stdstring & strKey, size_t nIndex = 0) const;

	stdstring				getString(const stdstring & strKey, size_t nIndex = 0) const;
	stdstring				getString(const stdstring & strDefault, const stdstring & strKey, size_t nIndex = 0) const;

	/*DateTime				getDate(const stdstring & strKey, size_t nIndex = 0) const;
	DateTime				getDate(const DateTime & dtDefault, const stdstring & strKey, size_t nIndex = 0) const;

	DateTime				getTime(const stdstring & strKey, size_t nIndex = 0) const;
	DateTime				getTime(const DateTime & dtDefault, const stdstring & strKey, size_t nIndex = 0) const;

	DateTime				getDateTime(const stdstring & strKey, size_t nIndex = 0) const;
	DateTime				getDateTime(const DateTime & dtDefault, const stdstring & strKey, size_t nIndex = 0) const;*/

	rfc_int_64				getSizeToByte(const stdstring & strKey, size_t nIndex = 0) const;
	rfc_int_64				getSizeToByte(const rfc_int_64 & nDefault, const stdstring & strKey, size_t nIndex = 0) const;

	TimeValue				getPeriod(const stdstring & strKey, size_t nIndex = 0) const;
	TimeValue				getPeriod(const TimeValue & nDefault, const stdstring & strKey, size_t nIndex = 0) const;
	rfc_int_64				getPeriodToSecond(const stdstring & strKey, size_t nIndex = 0) const;
	rfc_int_64				getPeriodToSecond(const rfc_int_64 & nDefault, const stdstring & strKey, size_t nIndex = 0) const;
	rfc_int_64				getPeriodToMilliSecond(const stdstring & strKey, size_t nIndex = 0) const;
	rfc_int_64				getPeriodToMilliSecond(const rfc_int_64 & nDefault, const stdstring & strKey, size_t nIndex = 0) const;

	const PVariant &		getSubItems(const stdstring & strKey, bool bEnableOverloaded) const;
	const PVariant &		getSubItems(const stdstring & strKey) const { return getSubItems(strKey, true); }
	size_t					getSubItemsCount(const stdstring & strKey) const;
	const stdstring &		getSubItemName(const stdstring & strKey, size_t nIndex) const;
	size_t					getSubItemNameList(const stdstring & strKey, typeSubItemNameList & listSubItem) const;
	typeConfigurePair		getSubItemPair(const stdstring & strKey, size_t nIndex) const;
	size_t					getSubItemPairList(const stdstring & strKey, typeConfPairList & listConfPair) const;

	size_t					getDuplicatedValueList(const stdstring & strKey, typeSubItemNameList & listDuplicatedValue) const;
	const stdstring &		getDuplicatedValue(const stdstring & strKey, size_t nIndex) const;
	size_t					getDuplicatedValueCount(const stdstring & strKey) const;

protected:
	bool					getValue(stdstring & strValue, const PVariant & varValue, size_t nIndex = 0) const;
	bool					getValue(stdstring & strValue, const stdstring & strKey, size_t nIndex = 0) const;
	void					getValueException(stdstring & strValue, const stdstring & strKey, size_t nIndex = 0) const throw(ConfigureException);

protected:
	PVariant				m_varAllConf;
	bool					m_bOverloaded;					//hosts配置文件覆盖全局的配置项
	stdstring				m_strLocalHostID;
};

RFC_NAMESPACE_END

#endif	//RFC_CONFIGUREMGR_H_201008
