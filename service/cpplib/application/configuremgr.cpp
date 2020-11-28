/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "application/configuremgr.h"
#include "system/iniconfigureparser.h"
#include "system/filehandle.h"
#include "base/stringtokenizer.h"

RFC_NAMESPACE_BEGIN

ConfigureMgr::ConfigureMgr(void) : m_bOverloaded(true)
{
}

bool ConfigureMgr::loadConfFile(const stdstring & strFullPath)
{
	FileHandle fHandle;
	if ( fHandle.open(strFullPath.data(), O_RDONLY) < 0 )
		return false;

	stdstring strKey = strFullPath.substr(strFullPath.find_last_of("/\\") + 1);
	size_t nPostfixPos = strKey.rfind('.');
	if ( nPostfixPos != stdstring::npos )
		strKey.erase(nPostfixPos);
	if ( !m_varAllConf.isNocaseStringMap() && !m_varAllConf.isStringMap() )
		m_varAllConf.asNocaseStringMap();
	IniConfigureParser iniParser;
	iniParser.loadFromStream(fHandle, m_varAllConf.set(strKey));
	return true;
}

bool ConfigureMgr::loadAllConfFile(const stdstring & strConfigureDir, const stdstring & strPostfix)
{
	typeFilePathList listFilePath;
	FileSystem::getFilePathRecursive(strConfigureDir, 0, false, true, listFilePath, "", "", strPostfix);
	typeFilePathList::const_iterator it, itEnd = listFilePath.end();
	for ( it = listFilePath.begin(); it != itEnd; ++it )
	{
		if ( !loadConfFile(*it) )
			throw ConfigureException( FormatString("Load conf file % error:%").arg(*it).arg(GlobalFunc::getSystemErrorInfo()).str() );
	}

	const ProtocolVariant * pVar = m_varAllConf.findByPath("/localhost/LocalhostID", false);
	if ( pVar != NULL )
		m_strLocalHostID = pVar->asString();
	return true;
}

stdstring ConfigureMgr::getHostConfigureFullPath(const stdstring & strKey) const
{
	return "/hosts/" + m_strLocalHostID + "/" + strKey;
}

bool ConfigureMgr::findOverloaded(const stdstring & strKey) const
{
	const ProtocolVariant * pVar = m_varAllConf.findByPath(getHostConfigureFullPath(strKey), false);
	if ( pVar == NULL )
		return false;
	return ( !pVar->isNULL() );
}

const PVariant & ConfigureMgr::getSubItems(const stdstring & strKey, bool bEnableOverloaded) const
{
	if ( strKey.empty() )
		return m_varAllConf;

	const ProtocolVariant * pVar = NULL;
	if ( bEnableOverloaded && isOverloaded() )
		pVar = m_varAllConf.findByPath(getHostConfigureFullPath(strKey), false);
	if ( (pVar == NULL) || pVar->isNocaseStringMap() )
		pVar = m_varAllConf.findByPath(strKey, false);
	if ( pVar == NULL )
		return PVariant::g_varNULL;
	return *pVar;
}

size_t ConfigureMgr::getSubItemsCount(const stdstring & strKey) const
{
	const PVariant & varConf = getSubItems(strKey);
	if ( varConf.isNULL() )
		return 0;

	if ( varConf.isNocaseStringMap() )
	{
		size_t nCount = 0;
		PVariant::typeNocaseStringVariantMap::const_iterator it, itEnd = varConf.asNocaseStringMap().end();
		for ( it = varConf.asNocaseStringMap().begin(); it != itEnd; ++it )
			nCount += it->second.arraySize();
		return nCount;
	} // if ( varConf.isNocaseStringMap() )
	return varConf.arraySize();
}

const stdstring & ConfigureMgr::getSubItemName(const stdstring & strKey, size_t nIndex) const
{
	const PVariant & varConf = getSubItems(strKey);
	if ( varConf.isNocaseStringMap() )
	{
		PVariant::typeNocaseStringVariantMap::const_iterator it, itEnd = varConf.asNocaseStringMap().end();
		for ( it = varConf.asNocaseStringMap().begin(); it != itEnd; ++it )
		{
			const PVariant & varValue = it->second;
			if ( nIndex < varValue.arraySize() )
				return it->first;
			nIndex -= varValue.arraySize();
		}
	} // if ( varConf.isNocaseStringMap() )
	return StringFunc::g_strEmptyString;
}

size_t ConfigureMgr::getSubItemNameList(const stdstring & strKey, typeSubItemNameList & listSubItem) const
{
	listSubItem.clear();
	const PVariant & varConf = getSubItems(strKey);
	if ( varConf.isNocaseStringMap() )
	{
		PVariant::typeNocaseStringVariantMap::const_iterator it, itEnd = varConf.asNocaseStringMap().end();
		for ( it = varConf.asNocaseStringMap().begin(); it != itEnd; ++it )
		{
			const PVariant & varValue = it->second;
			for ( size_t i = 0; i < varValue.arraySize(); ++i )
				listSubItem.push_back(it->first);
		}
	} // if ( varConf.isNocaseStringMap() )
	return 0;
}

typeConfigurePair ConfigureMgr::getSubItemPair(const stdstring & strKey, size_t nIndex) const
{
	const PVariant & varConf = getSubItems(strKey);
	if ( varConf.isNocaseStringMap() )
	{
		PVariant::typeNocaseStringVariantMap::const_iterator it, itEnd = varConf.asNocaseStringMap().end();
		for ( it = varConf.asNocaseStringMap().begin(); it != itEnd; ++it )
		{
			const PVariant & varValue = it->second;
			if ( (nIndex < varValue.arraySize()) && varValue[nIndex].isString() )
				return typeConfigurePair(it->first, varValue[nIndex].asString());
			else if ( nIndex < varValue.arraySize() )
				return typeConfigurePair(it->first, StringFunc::g_strEmptyString);
			nIndex -= varValue.arraySize();
		}
	} // if ( varConf.isNocaseStringMap() )
	return typeConfigurePair();
}

size_t ConfigureMgr::getSubItemPairList(const stdstring & strKey, typeConfPairList & listConfPair) const
{
	listConfPair.clear();
	const PVariant & varConf = getSubItems(strKey);
	if ( varConf.isNocaseStringMap() )
	{
		PVariant::typeNocaseStringVariantMap::const_iterator it, itEnd = varConf.asNocaseStringMap().end();
		for ( it = varConf.asNocaseStringMap().begin(); it != itEnd; ++it )
		{
			const PVariant & varValue = it->second;
			for ( size_t i = 0; i < varValue.arraySize(); ++i )
				listConfPair.push_back( typeConfigurePair(it->first, varValue[i].asString()) );
		}
	} // if ( varConf.isNocaseStringMap() )
	return listConfPair.size();
}

size_t ConfigureMgr::getDuplicatedValueList(const stdstring & strKey, typeSubItemNameList & listDuplicatedValue) const
{
	listDuplicatedValue.clear();
	const PVariant & varConf = getSubItems(strKey);
	if ( varConf.isString() )
		listDuplicatedValue.push_back(varConf.asString());
	else if ( varConf.isArray() )
	{
		ProtocolVariant::typeVariantArray::const_iterator it, itEnd = varConf.asArray().end();
		for ( it = varConf.asArray().begin(); it != itEnd; ++it )
			listDuplicatedValue.push_back( it->asString() );
	}

	return listDuplicatedValue.size();
}

const stdstring & ConfigureMgr::getDuplicatedValue(const stdstring & strKey, size_t nIndex) const
{
	const PVariant & varConf = getSubItems(strKey);
	if ( varConf.isNULL() || nIndex >= varConf.arraySize() )
		return StringFunc::g_strEmptyString;
	return varConf.getArrayItem(nIndex).asString();
}

size_t ConfigureMgr::getDuplicatedValueCount(const stdstring & strKey) const
{
	const PVariant & varConf = getSubItems(strKey);
	if ( varConf.isNULL() )
		return 0;
	return varConf.arraySize();
}

bool ConfigureMgr::getValue(stdstring & strValue, const PVariant & varValue, size_t nIndex) const
{
	if ( varValue.isNULL() || nIndex >= varValue.arraySize() )
		return false;
	strValue = varValue.getArrayItem(nIndex).asString();
	return true;
}

bool ConfigureMgr::getValue(stdstring & strValue, const stdstring & strKey, size_t nIndex) const
{
	return getValue(strValue, getSubItems(strKey), nIndex);
}

void ConfigureMgr::getValueException(stdstring & strValue, const stdstring & strKey, size_t nIndex) const throw(ConfigureException)
{
	if ( !getValue(strValue, strKey, nIndex) )
		throw ConfigureException( FormatString("[%] not found in configure, index:%").arg(strKey).arg(nIndex).str() );
}

bool ConfigureMgr::getBool(const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	getValueException(strValue, strKey, nIndex);
	return ( StringFunc::strCaseEqual(strValue, "on") ||
			StringFunc::strCaseEqual(strValue, "yes") ||
			StringFunc::strCaseEqual(strValue, "y") ||
			StringFunc::strCaseEqual(strValue, "true") ||
			StringFunc::stringToInt<int>(strValue.data(), strValue.size()) != 0 );
}

bool ConfigureMgr::getBool(bool bDefault, const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	if ( !getValue(strValue, strKey, nIndex) )
		return bDefault;
	return ( StringFunc::strCaseEqual(strValue, "on") ||
		StringFunc::strCaseEqual(strValue, "yes") ||
		StringFunc::strCaseEqual(strValue, "y") ||
		StringFunc::strCaseEqual(strValue, "true") ||
		StringFunc::stringToInt<int>(strValue.data(), strValue.size()) != 0 );
}

int ConfigureMgr::getInt(const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	getValueException(strValue, strKey, nIndex);
	return StringFunc::stringToInt<int>(strValue.data(), strValue.size());
}

int ConfigureMgr::getInt(int nDefault, const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	if ( !getValue(strValue, strKey, nIndex) )
		return nDefault;
	return StringFunc::stringToInt<int>(strValue.data(), strValue.size());
}

stdstring ConfigureMgr::getString(const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	getValueException(strValue, strKey, nIndex);
	return strValue;
}

stdstring ConfigureMgr::getString(const stdstring & strDefault, const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	if ( !getValue(strValue, strKey, nIndex) )
		return strDefault;
	return strValue;
}

rfc_int_64 ConfigureMgr::getSizeToByte(const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	getValueException(strValue, strKey, nIndex);
	return StringFunc::convertSizeUnitToByte(strValue.data(), strValue.size());
}

rfc_int_64 ConfigureMgr::getSizeToByte(const rfc_int_64 & nDefault, const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	if ( !getValue(strValue, strKey, nIndex) )
		return nDefault;
	return StringFunc::convertSizeUnitToByte(strValue.data(), strValue.size());
}

TimeValue ConfigureMgr::getPeriod(const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	getValueException(strValue, strKey, nIndex);
	TimeValue tvPeriond;
	tvPeriond.setTotalMilliSecond( StringFunc::convertPeriodUnitToMilliSecond(strValue.data(), strValue.size()) );
	return tvPeriond;
}

TimeValue ConfigureMgr::getPeriod(const TimeValue & nDefault, const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	if ( !getValue(strValue, strKey, nIndex) )
		return nDefault;
	TimeValue tvPeriond;
	tvPeriond.setTotalMilliSecond( StringFunc::convertPeriodUnitToMilliSecond(strValue.data(), strValue.size()) );
	return tvPeriond;
}

rfc_int_64 ConfigureMgr::getPeriodToSecond(const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	getValueException(strValue, strKey, nIndex);
	return StringFunc::convertPeriodUnitToMilliSecond(strValue.data(), strValue.size()) / 1000;
}

rfc_int_64 ConfigureMgr::getPeriodToSecond(const rfc_int_64 & nDefault, const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	if ( !getValue(strValue, strKey, nIndex) )
		return nDefault;
	return StringFunc::convertPeriodUnitToMilliSecond(strValue.data(), strValue.size()) / 1000;
}

rfc_int_64 ConfigureMgr::getPeriodToMilliSecond(const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	getValueException(strValue, strKey, nIndex);
	return StringFunc::convertPeriodUnitToMilliSecond(strValue.data(), strValue.size());
}

rfc_int_64 ConfigureMgr::getPeriodToMilliSecond(const rfc_int_64 & nDefault, const stdstring & strKey, size_t nIndex) const
{
	stdstring strValue;
	if ( !getValue(strValue, strKey, nIndex) )
		return nDefault;
	return StringFunc::convertPeriodUnitToMilliSecond(strValue.data(), strValue.size());
}

RFC_NAMESPACE_END
