/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "database/dbfield.h"

RFC_NAMESPACE_BEGIN

DBField::DBField(void) : m_FieldMaxSize(0), m_OriginalFieldType(0), m_LocaleFieldType(DBFIELD_TYPE_NULL) {}

DBField::DBField(const stdstring & strFieldName, int nFieldMaxSize, int nOriginalFieldType, DBFieldType nLocaleFieldType)
: m_FieldName(strFieldName), m_FieldMaxSize(nFieldMaxSize), m_OriginalFieldType(nOriginalFieldType), m_LocaleFieldType(nLocaleFieldType)
{}

RFC_NAMESPACE_END
