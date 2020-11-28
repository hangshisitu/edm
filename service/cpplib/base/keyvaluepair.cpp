/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/keyvaluepair.h"

RFC_NAMESPACE_BEGIN

template<typename typeKey, typename typeValue>
inline bool operator == (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p) { return p.operator==(k); }

template<typename typeKey, typename typeValue>
inline bool operator != (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p) { return p.operator!=(k); }

template<typename typeKey, typename typeValue>
inline bool operator < (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p) { return p.operator>(k); }

template<typename typeKey, typename typeValue>
inline bool operator > (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p) { return p.operator<(k); }

template<typename typeKey, typename typeValue>
inline bool operator <= (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p) { return p.operator>=(k); }

template<typename typeKey, typename typeValue>
inline bool operator >= (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p) { return p.operator<=(k); }

RFC_NAMESPACE_END
