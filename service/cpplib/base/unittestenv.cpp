/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"

RFC_NAMESPACE_BEGIN

UnitTestCase::UnitTestCase(const stdstring& strTestCaseName)
{
	m_strTestCaseName = strTestCaseName;
	TestRegistry::registerTestCase(this);
}

TestRegistry::TestRegistry(void)
{
	m_bExitWhenFail = false;
	m_nTotalPassCount = m_nTotalFailCount = m_nThisCasePassCount = m_nThisCaseFailCount = 0;
}

TestRegistry::~TestRegistry(void)
{
	if ( m_nTotalPassCount != 0 || m_nTotalFailCount != 0 )
	{
		std::cout << std::endl << "---------------- Test result ---------------- " << std::endl;
		std::cout << "PassCount: " << m_nTotalPassCount << ", FailCount: " << m_nTotalFailCount << std::endl;
	}
}

TestRegistry& TestRegistry::getInstance(void)
{
	static TestRegistry testRegistry;
	return testRegistry;
}

void TestRegistry::registerTest(UnitTestCase * pTestCase)
{
	if ( pTestCase == NULL )
		return;
	typeUnitTestCaseList::const_iterator it, itEnd = m_listTestCase.end();
	for ( it = m_listTestCase.begin(); it != itEnd; ++it )
	{
		const UnitTestCase * pListCase = (*it);
		if ( pListCase->getTestCaseName() == pTestCase->getTestCaseName() ) //has exist
		{
			std::cout << "Test case " << pListCase->getTestCaseName() << " has exist, skip it" << std::endl;
			return;
		}
	} //for ( it = m_listTestCase.begin(); it != itEnd; ++it )
	m_listTestCase.push_back(pTestCase);
}

void TestRegistry::onInitArgs(int argc, char * argv[])
{
	stdstring strFilterOption("-t");
	for ( int i = 1; i < argc; ++i )
	{
		if ( strFilterOption == argv[i] )
		{
			if ( i + 1 < argc )
				m_strTestFilter = argv[i + 1];
			StringFunc::toLower(m_strTestFilter);
			break;
		}
	} // for ( ; i < argc; ++i )

	typeUnitTestCaseList::iterator it, itEnd = m_listTestCase.end();
	for ( it = m_listTestCase.begin(); it != itEnd; ++it )
	{
		UnitTestCase * pListCase = (*it);
		pListCase->onInitArgs(argc, argv);
	} // for ( it = m_listTestCase.begin(); it != itEnd; ++it )
}

bool TestRegistry::skipThisTestCase(const stdstring& strTestCaseName) const
{
	if ( m_strTestFilter.empty() )
		return false;
	stdstring strLowerCaseName(strTestCaseName);
	StringFunc::toLower(strLowerCaseName);
	return ( strLowerCaseName.find(m_strTestFilter) == stdstring::npos );
}

void TestRegistry::runAllTest(void)
{
	typeUnitTestCaseList::iterator it, itEnd = m_listTestCase.end();
	for ( it = m_listTestCase.begin(); it != itEnd; ++it )
	{
		UnitTestCase * pListCase = (*it);
		if ( skipThisTestCase(pListCase->getTestCaseName()) )
			continue;
		std::cout << "Begin testing " << pListCase->getTestCaseName() << std::endl;
		try
		{
			pListCase->onRunTestCase();
		}
		catch( ... )
		{
			std::cout << std::endl;
			std::cout << "Catch exception on test case: " << pListCase->getTestCaseName()  << std::endl;
			outputThisCaseResult(pListCase->getTestCaseName());
			throw;
		}
		outputThisCaseResult(pListCase->getTestCaseName());
	} // for ( it = m_listTestCase.begin(); it != itEnd; ++it )
}

void TestRegistry::outputThisCaseResult(const stdstring& strTestCaseName)
{
	std::cout << std::endl << "Finish testing " << strTestCaseName
		<< ", pass: " << m_nThisCasePassCount << ", fail: " << m_nThisCaseFailCount << std::endl;
	m_nThisCasePassCount = m_nThisCaseFailCount = 0;
}

RFC_NAMESPACE_END

