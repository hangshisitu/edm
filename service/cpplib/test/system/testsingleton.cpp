/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "system/singleton.h"

RFC_NAMESPACE_BEGIN

static const int g_nNULL = -1;
class SingletonInstance
{
public:
	static void		resetInstance(int nData);
	int				getData(void) const { return m_nData; }

protected:
	SingletonInstance(void) : m_nData(g_nNULL) {}
	SingletonInstance(int n) : m_nData(n) {}

private:
	int			m_nData;
};

void SingletonInstance::resetInstance(int nData)
{
	std::auto_ptr<SingletonInstance> auInstance( new SingletonInstance(nData) );
	SingletonProxy<SingletonInstance>::resetInstance(auInstance);
}

onUnitTest(SingletonProxy)
{
	SingletonInstance * pInstance1 = &SingletonProxy<SingletonInstance>::getInstance();
	assertEqual(pInstance1->getData(), g_nNULL);

	SingletonInstance::resetInstance(100);
	SingletonInstance * pInstance2 = &SingletonProxy<SingletonInstance>::getInstance();
	assertEqual(pInstance2->getData(), 100);
	assertNotEqual(pInstance1, pInstance2);

	SingletonInstance::resetInstance(1234);
	pInstance2 = &SingletonProxy<SingletonInstance>::getInstance();
	assertEqual(pInstance2->getData(), 1234);
}

RFC_NAMESPACE_END
