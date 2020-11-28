/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef APP_UNITTESTENV_H_201008
#define APP_UNITTESTENV_H_201008

#include "base/globalfunc.h"
#include <list>

RFC_NAMESPACE_BEGIN

class TestRegistry;

class UnitTestCase : private NonCopyAble
{
protected:
	UnitTestCase(const stdstring& strTestCaseName);
	virtual ~UnitTestCase(void) {}
	const stdstring& getTestCaseName(void) const { return m_strTestCaseName; }
	virtual void onInitArgs(int argc, char * argv[]) {}
	virtual void onRunTestCase(void) = 0;

	stdstring m_strTestCaseName;
	friend class TestRegistry;
};

class TestRegistry
{
public:
	static void registerTestCase(UnitTestCase * pTestCase) { return getInstance().registerTest(pTestCase); }
	static void onInitTestCaseArgs(int argc, char * argv[]) { return getInstance().onInitArgs(argc, argv); }
	static void runAllTestCase(void) { return getInstance().runAllTest(); }

	template<typename typeArg1, typename typeArg2>
	static void onAssert(const typeArg1& arg1, const typeArg2& arg2, CompareFunc::CompareOP nCompareOP, const char* pFile, int nLine)
	{
		return getInstance().onAssertTest(arg1, arg2, nCompareOP, pFile, nLine);
	}

private:
	TestRegistry(void);
	~TestRegistry(void);
	static TestRegistry& getInstance(void);
	void	registerTest(UnitTestCase * pTestCase);
	void	onInitArgs(int argc, char * argv[]);
	bool	skipThisTestCase(const stdstring& strTestCaseName) const;
	void	runAllTest();
	void	outputThisCaseResult(const stdstring& strTestCaseName);

	template<typename typeArg1, typename typeArg2>
	void	onAssertTest(const typeArg1& arg1, const typeArg2& arg2, CompareFunc::CompareOP nCompareOP, const char* pFile, int nLine)
	{
		if ( CompareFunc::compareOK(arg1, arg2, nCompareOP) )
		{
			++m_nTotalPassCount;
			++m_nThisCasePassCount;
			std::cout << "." << m_nThisCasePassCount << std::flush;
			if ( (m_nThisCasePassCount % 10) == 0 )				std::cout << std::endl;	
		}
		else
		{
			++m_nTotalFailCount;
			++m_nThisCaseFailCount;
			if ( (m_nThisCasePassCount % 10) != 0 )				std::cout << std::endl;	
			std::cout << "Assert [" << arg1 << "]" << CompareFunc::getCompareSymbol(nCompareOP) << "[" << arg2 << "] fail at file: " << pFile
				<< ", line: " << nLine << "!" << m_nThisCaseFailCount << std::endl;
			if ( m_bExitWhenFail )
				exit(1);
		} //if ( CompareFunc::compareOK(arg1, arg2, nCompareOP) )
	}

	bool	m_bExitWhenFail;
	stdstring m_strTestFilter;
	int		m_nTotalPassCount, m_nTotalFailCount;				// 实时更新的成功/失败的次数总和
	int		m_nThisCasePassCount, m_nThisCaseFailCount;			// 只记录当前用例成功/失败的次数总和
	typedef std::list<UnitTestCase*> typeUnitTestCaseList;
	typeUnitTestCaseList m_listTestCase;
};

//////////////////////////////////////////////////////////////////////////

#define assertTest(bOK) TestRegistry::onAssert(0, bOK? 0 : -1, CompareFunc::COMPARE_OP_EQAUL, __FILE__, __LINE__)
#define assertEqual(arg1, arg2) TestRegistry::onAssert(arg1, arg2, CompareFunc::COMPARE_OP_EQAUL, __FILE__, __LINE__)
#define assertNotEqual(arg1, arg2) TestRegistry::onAssert(arg1, arg2, CompareFunc::COMPARE_OP_NOT_EQAUL, __FILE__, __LINE__)
#define assertGreater(arg1, arg2) TestRegistry::onAssert(arg1, arg2, CompareFunc::COMPARE_OP_GREATER_THAN, __FILE__, __LINE__)
#define assertGreaterOrEqaul(arg1, arg2) TestRegistry::onAssert(arg1, arg2, CompareFunc::COMPARE_OP_GREATER_OR_EQAUL, __FILE__, __LINE__)
#define assertLess(arg1, arg2) TestRegistry::onAssert(arg1, arg2, CompareFunc::COMPARE_OP_LESS_THAN, __FILE__, __LINE__)
#define assertLessOrEqaul(arg1, arg2) TestRegistry::onAssert(arg1, arg2, CompareFunc::COMPARE_OP_LESS_OR_EQAUL, __FILE__, __LINE__)

#define onUnitTest(testName) class Test##testName : public UnitTestCase \
{ \
public: \
	Test##testName() : UnitTestCase("test"#testName) {} \
protected: \
	void onRunTestCase(void); \
} test##testName##Instance; \
void Test##testName::onRunTestCase(void)

RFC_NAMESPACE_END

#endif	//APP_UNITTESTENV_H_201008

