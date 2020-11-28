/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/logicalexpinterpreter.h"

RFC_NAMESPACE_BEGIN

bool ValueSetChecker::checkMatch(const stdstring & strOperand) const
{
	return ( m_setValue.find(strOperand) != m_setValue.end() );
}

ExpOperator LogicalExpInterpreter::getOperator(const stdstring & strOperator)
{
	if ( strOperator == "!" || StringFunc::strCaseEqual(strOperator, "NOT") )
		return EXP_OP_NOT;
	if ( strOperator == "&&" || strOperator == "&" || StringFunc::strCaseEqual(strOperator, "AND") )
		return EXP_OP_AND;
	if ( strOperator == "||" || strOperator == "|" || StringFunc::strCaseEqual(strOperator, "OR") )
		return EXP_OP_OR;
	if ( strOperator == "(" )
		return EXP_OP_LEFT_BRACKET;
	if ( strOperator == ")" )
		return EXP_OP_RIGHT_BRACKET;
	return EXP_OP_UNKNOWN;
}

OperatorPriority LogicalExpInterpreter::getOperatorPriority(ExpOperator nOp1, ExpOperator nOp2)
{
	static const char g_arOPPriorityTable[EXP_OP_UNKNOWN + 1][EXP_OP_UNKNOWN + 1] =
	{
		//行代表旧的操作符(上一个操作符)  列代表新的操作符
			// NOT  AND  OR   (    )   END		按照优先级优先级 NOT > AND > OR计算
/* NOT */	{ '#', '#', '#', '<', '#', '>', },
/* AND */	{ '<', '>', '>', '<', '>', '>', },
/* OR */	{ '<', '<', '>', '<', '>', '>', },
/* ( */		{ '<', '<', '<', '<', '=', '#', },
/* ) */		{ '#', '#', '#', '#', '#', '#', },
/* END */	{ '<', '<', '<', '<', '#', '=',},
	};

	if ( static_cast<int>(nOp1) < 0 || nOp1 > EXP_OP_UNKNOWN || static_cast<int>(nOp2) < 0 || nOp2 > EXP_OP_UNKNOWN )
		return OP_PRIORITY_ERROR;

	return static_cast<OperatorPriority>( g_arOPPriorityTable[nOp1][nOp2] );
}

bool LogicalExpInterpreter::splitExpression(const stdstring & strExpression, typeExpTokenList & listExpToken)
{
	listExpToken.clear();

	stdstring strToken;
	const char * const pEnd = strExpression.data() + strExpression.size();
	for ( const char * p = strExpression.data(); p < pEnd; ++p )
	{
		char ch = (*p);
		if ( ::isspace(ch) )
		{
			if ( !strToken.empty() )
				listExpToken.push_back(strToken);
			strToken.clear();
			continue;
		}

		if ( ch == '!' || ch == '&' || ch == '|' || ch == '(' || ch == ')' )
		{
			if ( !strToken.empty() )
				listExpToken.push_back(strToken);
			strToken.assign(1, ch);
			if ( (ch == '&' || ch == '|') && p + 1 < pEnd && ch == p[1] )
				strToken.append(1, *(++p));
			listExpToken.push_back(strToken);
			strToken.clear();
			continue;
		}

		if ( !::isalnum(ch) && ch != '_' )
			return false;
		strToken.append(1, ch);
	} // for ( const char * p = strExpression.data(); p < pEnd; ++p )

	if ( !strToken.empty() )
		listExpToken.push_back(strToken);
	return ( !listExpToken.empty() );
}

ExpRetCode LogicalExpInterpreter::checkMatch(const ExpValueChecker & checker, const typeExpTokenList & listExpToken)
{
	typeOperatorList	listExpOperator;
	typeMatchResultList	listMatchResult;

	bool bLastTokenIsOperand = false;	// 不可以连续出现连个操作数
	typeExpTokenList::const_iterator it, itEnd = listExpToken.end();
	for ( it = listExpToken.begin(); it != itEnd; ++it )
	{
		const stdstring & strToken = (*it);
		ExpOperator nOldOP = ( listExpOperator.empty() ? EXP_OP_UNKNOWN : listExpOperator.back() );
		ExpOperator nNewOP = getOperator(strToken);
		if ( nNewOP != EXP_OP_UNKNOWN )		// 当前token是操作符
		{
			bLastTokenIsOperand = false;
			OperatorPriority nCmpPriority = getOperatorPriority(nOldOP, nNewOP);
			if ( nCmpPriority == OP_PRIORITY_LESS )
				listExpOperator.push_back(nNewOP);		// 上一个操作符的优先级低于当前操作符,把当前操作符push到listExpOperator
			else if ( nCmpPriority == OP_PRIORITY_EQUAL )
			{
				listExpOperator.pop_back();				// 从listExpOperator pop出一个操作符
				checkNotOperation(listExpOperator, listMatchResult);
			}
			else if ( nCmpPriority == OP_PRIORITY_GREATER )
			{
				listExpOperator.pop_back();				// 从listExpOperator pop出一个操作符
				if ( !onBinaryOperation(nOldOP, listMatchResult) )
					return EXP_RET_ERROR;
				checkNotOperation(listExpOperator, listMatchResult);

				// 找到和")"配对的"(",把"(" pop出来
				while ( nNewOP == EXP_OP_RIGHT_BRACKET )
				{
					if ( listExpOperator.empty() )
						return EXP_RET_ERROR;
					nOldOP = listExpOperator.back();
					listExpOperator.pop_back();
					checkNotOperation(listExpOperator, listMatchResult);
					if ( nOldOP == EXP_OP_LEFT_BRACKET )
						break;
					if ( !onBinaryOperation(nOldOP, listMatchResult) )
						return EXP_RET_ERROR;
				} // while ( nNewOP == EXP_OP_RIGHT_BRACKET )

				if ( nNewOP != EXP_OP_RIGHT_BRACKET )
					listExpOperator.push_back(nNewOP);
			}
			else
				return EXP_RET_ERROR;
		}
		else	// 当前token是操作数
		{
			if ( bLastTokenIsOperand )		// 不可以连续出现连个操作数
				return EXP_RET_ERROR;
			bLastTokenIsOperand = true;
			listMatchResult.push_back( checker.checkMatch(strToken) );
			checkNotOperation(listExpOperator, listMatchResult);			
		}
	} // for ( it = listExpToken.begin(); it != itEnd; ++it )

	while ( !listExpOperator.empty() )
	{
		if ( getOperatorPriority(listExpOperator.back(), EXP_OP_UNKNOWN) != OP_PRIORITY_GREATER )
			return EXP_RET_ERROR;
		if ( !onBinaryOperation(listExpOperator.back(), listMatchResult) )
			return EXP_RET_ERROR;
		listExpOperator.pop_back();				// 从listExpOperator pop出一个操作符
		checkNotOperation(listExpOperator, listMatchResult);
	}

	if ( !listExpOperator.empty() || listMatchResult.size() != 1 )
		return EXP_RET_ERROR;
	return ( listMatchResult.back() ? EXP_RET_TRUE : EXP_RET_FALSE );
}

ExpRetCode LogicalExpInterpreter::checkMatch(const ExpValueChecker & checker, const stdstring & strExpression)
{
	typeExpTokenList listExpToken;
	if ( !splitExpression(strExpression, listExpToken) )
		return EXP_RET_ERROR;
	return checkMatch(checker, listExpToken);
}

ExpRetCode LogicalExpInterpreter::checkMatch(const typeValueSet & setValue, const typeExpTokenList & listExpToken)
{
	ValueSetChecker checker(setValue);
	return checkMatch(checker, listExpToken);
}

ExpRetCode LogicalExpInterpreter::checkMatch(const typeValueSet & setValue, const stdstring & strExpression)
{
	typeExpTokenList listExpToken;
	if ( !splitExpression(strExpression, listExpToken) )
		return EXP_RET_ERROR;
	return checkMatch(setValue, listExpToken);
}

bool LogicalExpInterpreter::isValidExpression(const typeExpTokenList & listExpToken)
{
	return ( checkMatch(typeValueSet(), listExpToken) != EXP_RET_ERROR );
}

bool LogicalExpInterpreter::isValidExpression(const stdstring & strExpression)
{
	return ( checkMatch(typeValueSet(), strExpression) != EXP_RET_ERROR );
}

void LogicalExpInterpreter::checkNotOperation(typeOperatorList & listExpOperator, typeMatchResultList & listMatchResult)
{
	if ( (!listExpOperator.empty()) && listExpOperator.back() == EXP_OP_NOT && !listMatchResult.empty() )
	{
		listMatchResult.back() = ( !listMatchResult.back() );
		listExpOperator.pop_back();
	}
}

bool LogicalExpInterpreter::onBinaryOperation(ExpOperator nOP, typeMatchResultList & listMatchResult)
{
	// 从操作数stack pop出两个操作数, 并对前面pop出的两个操作数做操作, 得出操作结果
	if ( listMatchResult.size() < 2 || (nOP != EXP_OP_AND && nOP != EXP_OP_OR) )
		return false;
	bool bTopResult1 = listMatchResult.back();
	listMatchResult.pop_back();
	bool bTopResult2 = listMatchResult.back();
	listMatchResult.pop_back();					
	bool bResult = ( nOP == EXP_OP_AND ? (bTopResult1 && bTopResult2) : (bTopResult1 || bTopResult2) );
	listMatchResult.push_back(bResult);
	return true;
}

RFC_NAMESPACE_END
