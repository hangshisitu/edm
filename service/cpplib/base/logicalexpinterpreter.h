/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_LOGICAL_EXP_INTERPRETER_H_201008
#define RFC_LOGICAL_EXP_INTERPRETER_H_201008

#include "base/string.h"
#include <list>
#include <deque>
#include <set>

RFC_NAMESPACE_BEGIN

enum ExpOperator
{
	EXP_OP_NOT,
	EXP_OP_AND,
	EXP_OP_OR,
	EXP_OP_LEFT_BRACKET,
	EXP_OP_RIGHT_BRACKET,
	EXP_OP_UNKNOWN,
};

enum OperatorPriority
{
	OP_PRIORITY_LESS = '<',
	OP_PRIORITY_EQUAL = '=',
	OP_PRIORITY_GREATER = '>',
	OP_PRIORITY_ERROR = '#',
};

enum ExpRetCode
{
	EXP_RET_FALSE		= 0,	// 逻辑表达式返回false
	EXP_RET_TRUE		= 1,	// 逻辑表达式返回ture
	EXP_RET_ERROR		= -1,	// 逻辑表达式语法错误
};

typedef std::list<stdstring>		typeExpTokenList;		// 包括运算符和运算对象
typedef std::set<stdstring>			typeValueSet;
typedef std::list<ExpOperator>		typeOperatorList;
typedef std::deque<bool>			typeMatchResultList;

class ExpValueChecker
{
public:
	virtual ~ExpValueChecker(void) {}
	virtual bool			checkMatch(const stdstring & strOperand) const = 0;
};

class ValueSetChecker : public ExpValueChecker
{
public:
	ValueSetChecker(const typeValueSet & setValue) : m_setValue(setValue) {}
	virtual bool			checkMatch(const stdstring & strOperand) const;
protected:
	const typeValueSet &	m_setValue;
};

class LogicalExpInterpreter
{
public:
	static ExpOperator		getOperator(const stdstring & strOperator);

	//nOp1代表旧的操作符(上一个操作符)  nOp2代表新的操作符
	static OperatorPriority	getOperatorPriority(ExpOperator nOp1, ExpOperator nOp2);

	static bool				splitExpression(const stdstring & strExpression, typeExpTokenList & listExpToken);

	static ExpRetCode		checkMatch(const ExpValueChecker & checker, const typeExpTokenList & listExpToken);
	static ExpRetCode		checkMatch(const ExpValueChecker & checker, const stdstring & strExpression);

	static ExpRetCode		checkMatch(const typeValueSet & setValue, const typeExpTokenList & listExpToken);
	static ExpRetCode		checkMatch(const typeValueSet & setValue, const stdstring & strExpression);

	static bool				isValidExpression(const typeExpTokenList & listExpToken);
	static bool				isValidExpression(const stdstring & strExpression);

protected:
	// 检查listExpOperator的顶部是不是NOT操作符
	static void				checkNotOperation(typeOperatorList & listExpOperator, typeMatchResultList & listMatchResult);

	// EXP_OP_NOT 或 EXP_OP_AND 运算,如果出错返回false
	static bool				onBinaryOperation(ExpOperator nOP, typeMatchResultList & listMatchResult);
};

RFC_NAMESPACE_END

#endif	//RFC_LOGICAL_EXP_INTERPRETER_H_201008
