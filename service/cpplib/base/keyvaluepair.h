/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_KEYVALUEPAIR_H_201008
#define RFC_KEYVALUEPAIR_H_201008

#include "base/globalfunc.h"

RFC_NAMESPACE_BEGIN

template<typename typeKey, typename typeValue>
class KeyValuePair : public std::pair<typeKey, typeValue>
{
public:
	typedef		std::pair<typeKey, typeValue>		typePairBase;

	KeyValuePair(void) : typePairBase() {}

	KeyValuePair(const typeKey & k, const typeValue & v) : typePairBase(k,v) {}

	KeyValuePair(const KeyValuePair & k) : typePairBase(k) {}

	template<typename typeOtherKey, typename typeOtherValue>
	KeyValuePair(const KeyValuePair<typeOtherKey,typeOtherValue> & k) : typePairBase(k.first,k.second) {}

	template<class typeOtherKey, class typeOtherValue> 
	bool operator == (const KeyValuePair<typeOtherKey, typeOtherValue> & k) const { return this->first == k.first; }

	template<class typeOtherKey, class typeOtherValue> 
	bool operator != (const KeyValuePair<typeOtherKey, typeOtherValue> & k) const { return !(this->first == k.first); }

	template<class typeOtherKey, class typeOtherValue> 
	bool operator <  (const KeyValuePair<typeOtherKey, typeOtherValue> & k) const { return this->first <  k.first; }

	template<class typeOtherKey, class typeOtherValue> 
	bool operator >  (const KeyValuePair<typeOtherKey, typeOtherValue> & k) const { return k.first < this->first; }

	template<class typeOtherKey, class typeOtherValue> 
	bool operator <= (const KeyValuePair<typeOtherKey, typeOtherValue> & k) const { return !(k.first < this->first); }

	template<class typeOtherKey, class typeOtherValue> 
	bool operator >= (const KeyValuePair<typeOtherKey, typeOtherValue> & k) const { return !(this->first < k.first); }

	bool operator == (const typeKey & k) const { return this->first == k; }
	bool operator != (const typeKey & k) const { return !(this->first == k); }
	bool operator < (const typeKey & k) const { return this->first < k; }
	bool operator > (const typeKey & k) const { return k < this->first; }
	bool operator <= (const typeKey & k) const { return !(k < this->first); }
	bool operator >= (const typeKey & k) const { return !(this->first < k); }
};

template<typename typeKey, typename typeValue>
extern bool operator == (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p);

template<typename typeKey, typename typeValue>
extern bool operator != (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p);

template<typename typeKey, typename typeValue>
extern bool operator < (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p);

template<typename typeKey, typename typeValue>
extern bool operator > (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p);

template<typename typeKey, typename typeValue>
extern bool operator <= (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p);

template<typename typeKey, typename typeValue>
extern bool operator >= (const typeKey & k, const KeyValuePair<typeKey, typeValue> & p);

RFC_NAMESPACE_END

#endif	//RFC_KEYVALUEPAIR_H_201008
