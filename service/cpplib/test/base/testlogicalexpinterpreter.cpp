/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "base/logicalexpinterpreter.h"
#include "base/stringtokenizer.h"

RFC_NAMESPACE_BEGIN

static void onTestOperator(void)
{
	assertEqual(LogicalExpInterpreter::getOperator("!"), EXP_OP_NOT);
	assertEqual(LogicalExpInterpreter::getOperator("not"), EXP_OP_NOT);

	assertEqual(LogicalExpInterpreter::getOperator("&"), EXP_OP_AND);
	assertEqual(LogicalExpInterpreter::getOperator("&&"), EXP_OP_AND);
	assertEqual(LogicalExpInterpreter::getOperator("and"), EXP_OP_AND);

	assertEqual(LogicalExpInterpreter::getOperator("|"), EXP_OP_OR);
	assertEqual(LogicalExpInterpreter::getOperator("||"), EXP_OP_OR);
	assertEqual(LogicalExpInterpreter::getOperator("or"), EXP_OP_OR);

	assertEqual(LogicalExpInterpreter::getOperator("("), EXP_OP_LEFT_BRACKET);
	assertEqual(LogicalExpInterpreter::getOperator(")"), EXP_OP_RIGHT_BRACKET);

	assertEqual(LogicalExpInterpreter::getOperator("!!"), EXP_OP_UNKNOWN);
	assertEqual(LogicalExpInterpreter::getOperator("&&&"), EXP_OP_UNKNOWN);
	assertEqual(LogicalExpInterpreter::getOperator("|||"), EXP_OP_UNKNOWN);
	assertEqual(LogicalExpInterpreter::getOperator("abc"), EXP_OP_UNKNOWN);
}

static void onTestOperatorPriority(void)
{
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_NOT, EXP_OP_NOT), OP_PRIORITY_ERROR);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_NOT, EXP_OP_AND), OP_PRIORITY_ERROR);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_NOT, EXP_OP_OR), OP_PRIORITY_ERROR);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_NOT, EXP_OP_LEFT_BRACKET), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_NOT, EXP_OP_RIGHT_BRACKET), OP_PRIORITY_ERROR);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_NOT, EXP_OP_UNKNOWN), OP_PRIORITY_GREATER);

	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_AND, EXP_OP_NOT), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_AND, EXP_OP_AND), OP_PRIORITY_GREATER);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_AND, EXP_OP_OR), OP_PRIORITY_GREATER);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_AND, EXP_OP_LEFT_BRACKET), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_AND, EXP_OP_RIGHT_BRACKET), OP_PRIORITY_GREATER);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_AND, EXP_OP_UNKNOWN), OP_PRIORITY_GREATER);

	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_OR, EXP_OP_NOT), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_OR, EXP_OP_AND), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_OR, EXP_OP_OR), OP_PRIORITY_GREATER);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_OR, EXP_OP_LEFT_BRACKET), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_OR, EXP_OP_RIGHT_BRACKET), OP_PRIORITY_GREATER);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_OR, EXP_OP_UNKNOWN), OP_PRIORITY_GREATER);

	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_LEFT_BRACKET, EXP_OP_NOT), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_LEFT_BRACKET, EXP_OP_AND), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_LEFT_BRACKET, EXP_OP_OR), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_LEFT_BRACKET, EXP_OP_LEFT_BRACKET), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_LEFT_BRACKET, EXP_OP_RIGHT_BRACKET), OP_PRIORITY_EQUAL);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_LEFT_BRACKET, EXP_OP_UNKNOWN), OP_PRIORITY_ERROR);

	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_RIGHT_BRACKET, EXP_OP_NOT), OP_PRIORITY_ERROR);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_RIGHT_BRACKET, EXP_OP_AND), OP_PRIORITY_ERROR);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_RIGHT_BRACKET, EXP_OP_OR), OP_PRIORITY_ERROR);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_RIGHT_BRACKET, EXP_OP_LEFT_BRACKET), OP_PRIORITY_ERROR);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_RIGHT_BRACKET, EXP_OP_RIGHT_BRACKET), OP_PRIORITY_ERROR);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_RIGHT_BRACKET, EXP_OP_UNKNOWN), OP_PRIORITY_ERROR);

	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_UNKNOWN, EXP_OP_NOT), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_UNKNOWN, EXP_OP_AND), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_UNKNOWN, EXP_OP_OR), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_UNKNOWN, EXP_OP_LEFT_BRACKET), OP_PRIORITY_LESS);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_UNKNOWN, EXP_OP_RIGHT_BRACKET), OP_PRIORITY_ERROR);
	assertEqual(LogicalExpInterpreter::getOperatorPriority(EXP_OP_UNKNOWN, EXP_OP_UNKNOWN), OP_PRIORITY_EQUAL);
}

static void onTestValidExpression(void)
{
	assertTest(!LogicalExpInterpreter::isValidExpression("not"));
	assertTest(!LogicalExpInterpreter::isValidExpression("and"));
	assertTest(!LogicalExpInterpreter::isValidExpression("or"));
	assertTest(!LogicalExpInterpreter::isValidExpression("("));
	assertTest(!LogicalExpInterpreter::isValidExpression(")"));
	assertTest(!LogicalExpInterpreter::isValidExpression("()"));
	assertTest(!LogicalExpInterpreter::isValidExpression(""));

	assertTest(!LogicalExpInterpreter::isValidExpression("and exp1"));
	assertTest(!LogicalExpInterpreter::isValidExpression("exp1 and"));
	assertTest(!LogicalExpInterpreter::isValidExpression("and exp1 and"));
	assertTest(!LogicalExpInterpreter::isValidExpression("and exp1 and exp2"));
	assertTest(!LogicalExpInterpreter::isValidExpression("exp1 exp2 and"));

	assertTest(!LogicalExpInterpreter::isValidExpression("or exp1"));
	assertTest(!LogicalExpInterpreter::isValidExpression("exp1 or"));
	assertTest(!LogicalExpInterpreter::isValidExpression("or exp1 or"));
	assertTest(!LogicalExpInterpreter::isValidExpression("or exp1 or exp2"));
	assertTest(!LogicalExpInterpreter::isValidExpression("exp1 exp2 or"));

	assertTest(!LogicalExpInterpreter::isValidExpression("!!exp1"));
	assertTest(!LogicalExpInterpreter::isValidExpression("exp1 &&& exp2"));
	assertTest(!LogicalExpInterpreter::isValidExpression("exp1 ||| exp2"));
	assertTest(LogicalExpInterpreter::isValidExpression("!exp1"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 & exp2"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 && exp2"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 | exp2"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 || exp2"));
	
	assertTest(LogicalExpInterpreter::isValidExpression("exp1"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1"));

	assertTest(LogicalExpInterpreter::isValidExpression("exp1 and exp2"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 and exp2"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 and not exp2"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 and not exp2"));

	assertTest(LogicalExpInterpreter::isValidExpression("exp1 or exp2"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 or exp2"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 or not exp2"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 or not exp2"));

	assertTest(LogicalExpInterpreter::isValidExpression("exp1 and exp2 or exp3"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 and exp2 or exp3"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 and not exp2 or exp3"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 and exp2 or not exp3"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 and not exp2 or exp3"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 and exp2 or not exp3"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 and not exp2 or not exp3"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 and not exp2 or not exp3"));

	assertTest(LogicalExpInterpreter::isValidExpression("exp1 or exp2 and exp3 and exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 and exp2 and exp3  and exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 or not exp2 and exp3 and exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 or exp2 and not exp3 and exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 or exp2 and not exp3 and not exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 or not exp2 and exp3 and exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 or exp2 and not exp3 and exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 or not exp2 and not exp3 and exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 or not exp2 and not exp3 and exp4"));

	assertTest(LogicalExpInterpreter::isValidExpression("(exp1)"));
	assertTest(LogicalExpInterpreter::isValidExpression("(not exp1)"));
	assertTest(LogicalExpInterpreter::isValidExpression("(exp1 or exp2) and exp3"));

	assertTest(LogicalExpInterpreter::isValidExpression("(not exp1 or exp2 ) and not exp3"));
	assertTest(LogicalExpInterpreter::isValidExpression("(not exp1 or not exp2 ) or not exp3"));
	assertTest(LogicalExpInterpreter::isValidExpression("(not exp1 and not exp2 ) or exp3"));

	assertTest(LogicalExpInterpreter::isValidExpression("(exp1 or exp2) and exp3 and exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("not (exp1 and (exp2 or exp3)  and exp4)"));
	assertTest(LogicalExpInterpreter::isValidExpression("not (exp1 and exp3 and exp4)"));
	assertTest(LogicalExpInterpreter::isValidExpression("not (exp3 and exp4)"));
	assertTest(LogicalExpInterpreter::isValidExpression("not (exp4)"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp4"));

	assertTest(LogicalExpInterpreter::isValidExpression("exp1 or not (exp2 and exp3) and exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 or (exp2 and not exp3) and exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 or (exp2 and not exp3 and not exp4)"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 or not (exp2 and exp3) and exp4"));
	assertTest(LogicalExpInterpreter::isValidExpression("not exp1 or exp2 and not (exp3 and exp4)"));
	assertTest(LogicalExpInterpreter::isValidExpression("exp1 or not (exp2 and not exp3 and exp4)"));
	assertTest(LogicalExpInterpreter::isValidExpression("not (exp1 or not exp2 and not exp3 and exp4)"));
	assertTest(LogicalExpInterpreter::isValidExpression("not (exp1 or exp2 and exp3 and exp4)"));
	assertTest(LogicalExpInterpreter::isValidExpression("not (exp1 or exp2 and exp3)"));
	assertTest(LogicalExpInterpreter::isValidExpression("not (exp1 or exp2)"));
}

static bool onCheckExpMatch(const stdstring & strValueList, const stdstring & strExpression)
{
	typeValueSet setValue;
	StringTokenizer<stdstring> tokenValue(strValueList.data(), strValueList.size());
	stdstring strValue;
	while ( tokenValue.getNext(strValue, ",; \t\r\n") )
		setValue.insert(strValue);

	ExpRetCode nRet = LogicalExpInterpreter::checkMatch(setValue, strExpression);
	assertTest(nRet != EXP_RET_ERROR);
	return ( nRet == EXP_RET_TRUE );
}

static void onTestExpMatch(void)
{
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1"));

	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 and exp2"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 and exp2"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 and not exp2"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 and not exp2"));

	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 or exp2"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 or exp2"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 or not exp2"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 or not exp2"));

	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 and exp2 or exp3"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 and exp2 or exp3"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 and not exp2 or exp3"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 and exp2 or not exp3"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 and not exp2 or exp3"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 and exp2 or not exp3"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 and not exp2 or not exp3"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 and not exp2 or not exp3"));

	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 or exp2 and exp3 and exp4"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 and exp2 and exp3  and exp4"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 or not exp2 and exp3 and exp4"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 or exp2 and not exp3 and exp4"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 or exp2 and not exp3 and not exp4"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 or not exp2 and exp3 and exp4"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 or exp2 and not exp3 and exp4"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 or not exp2 and not exp3 and exp4"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 or not exp2 and not exp3 and exp4"));

	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "(exp1)"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "(not exp1)"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "(exp1 or exp2) and exp3"));

	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "(not exp1 or exp2 ) and not exp3"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "(not exp1 or not exp2 ) or not exp3"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "(not exp1 and not exp2 ) or exp3"));

	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "(exp1 or exp2) and exp3 and exp4"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not (exp1 and (exp2 or exp3)  and exp4)"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not (exp1 and exp3 and exp4)"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not (exp3 and exp4)"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not (exp4)"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp4"));

	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 or ! (exp2 and exp3) and exp4"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 or (exp2 & not exp3) and exp4"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 or (exp2 and not exp3 && not exp4)"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 | not (exp2 and exp3) and exp4"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not exp1 || exp2 and not (exp3 and exp4)"));
	assertTest(onCheckExpMatch("exp1,exp2,exp3,exp4", "exp1 or not (exp2 and not exp3 and exp4)"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not (exp1 or not exp2 and not exp3 and exp4)"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not (exp1 or exp2 and exp3 and exp4)"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not (exp1 or exp2 and exp3)"));
	assertTest(!onCheckExpMatch("exp1,exp2,exp3,exp4", "not (exp1 or exp2)"));
}

onUnitTest(LogicalExpInterpreter)
{
	onTestOperator();

	onTestOperatorPriority();

	onTestValidExpression();

	onTestExpMatch();
}

RFC_NAMESPACE_END
