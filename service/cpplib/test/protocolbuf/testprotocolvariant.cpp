/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/unittestenv.h"
#include "protocolbuf/protocolvariant.h"

RFC_NAMESPACE_BEGIN

static void onProtocolVarExample(void)
{
	//return;
	ProtocolVariant		varProtocolPackage;
	varProtocolPackage.set(1, (127<<24 | 1));			// IP
	varProtocolPackage.set(2, stdstring("sender@domain.com"));	//sender
	varProtocolPackage.set(3, 12.34);
	ProtocolVariant::typeVariantArray & varArrayRcpt = varProtocolPackage.set(4).asArray();
	varArrayRcpt.push_back("rcpt1@domain.com");
	varArrayRcpt.push_back("rcpt2@domain.com");
	varArrayRcpt.push_back("rcpt3@domain.com");

	const ProtocolVariant & varIP = varProtocolPackage.get(1);
	const ProtocolVariant & varSender = varProtocolPackage.get(2);
	std::cout << "IP: Type:" << ProtocolVariant::getTypeString( varIP.getVariantType() ) << ", Var:" << varIP.asInteger32() << std::endl;
	std::cout << "Sender: Type:" << ProtocolVariant::getTypeString( varSender.getVariantType() ) << ", Var:" << varSender.asString() << std::endl;

	int i = 0;
	const ProtocolVariant * pArray = varProtocolPackage.find(4);
	if ( pArray != NULL )
	{
		ProtocolVariant::typeVariantArray::const_iterator itRcpt, itRcptEnd = pArray->asArray().end();
		for ( itRcpt = pArray->asArray().begin(); itRcpt != itRcptEnd; ++itRcpt )
			std::cout << "index: " << i++ << ", Rcpt:" << itRcpt->asString() << std::endl;
	} //if ( pArray != NULL )

	i = 0;
	const ProtocolVariant::typeIntVariantMap & mapIntVar = varProtocolPackage.asIntMap();	//get map container
	ProtocolVariant::typeIntVariantMap::const_iterator it, itEnd = mapIntVar.end();
	for ( it = mapIntVar.begin(); it != itEnd; ++it )
	{
		const ProtocolVariant & varNode = it->second;
		std::cout << "index: " << i++ << ", Key:" << it->first << ", VarType:" << ProtocolVariant::getTypeString( varNode.getVariantType() );
		if ( varNode.isBool() )
			std::cout << ", Var:" << varNode.asBool() << std::endl;
		else if ( varNode.isInteger32() )
			std::cout << ", Var:" << varNode.asInteger32() << std::endl;
		else if ( varNode.isString() )
			std::cout << ", Var:" << varNode.asString() << std::endl;
		else
			std::cout << ", Unknown Value" << std::endl;
	} //for ( it = mapIntVar.begin(); it != itEnd; ++it )

	std::cout << varProtocolPackage << std::endl;
}

onUnitTest(ProtocolVariant)
{
	ProtocolVariant tVar;
	assertTest( tVar.isNULL() );
	assertEqual(tVar, ProtocolVariant::g_varNULL);

	tVar = true;
	assertTest( tVar.isBool() );
	assertTest( tVar.asBool() );
	assertEqual(tVar, true);
	
	tVar = false;
	assertTest( tVar.isBool() );
	assertTest( !tVar.asBool() );
	assertEqual(tVar, false);

	tVar = 3;
	assertTest( tVar.isInteger32() );
	assertEqual(tVar.asInteger32(), 3);
	assertEqual(tVar, 3);

	tVar = static_cast<rfc_int_64>(9527);
	assertTest( tVar.isInteger64() );
	assertEqual(tVar.asInteger64(), static_cast<rfc_int_64>(9527));
	assertEqual(tVar, static_cast<rfc_int_64>(9527));

	tVar = 12.34;
	assertTest( tVar.isDouble() );
	assertEqual(tVar.asDouble(), 12.34);

	tVar = "char*";
	assertTest( tVar.isString() );
	assertEqual(tVar.asString(), "char*");
	assertEqual(tVar, "char*");

	tVar = stdstring("string");
	assertTest( tVar.isString() );
	assertEqual(tVar.asString(), stdstring("string"));
	assertEqual(tVar, stdstring("string"));

	ProtocolVariant::typeIntVariantMap mapIntVar;
	mapIntVar[1] = ProtocolVariant::g_varNULL;
	mapIntVar[2] = true;
	mapIntVar[3] = false;
	mapIntVar[4] = 3;	
	mapIntVar[5] = 12.34;
	mapIntVar[6] = "char*";
	mapIntVar[7] = "string";
	mapIntVar[8] = static_cast<rfc_int_64>(9527);
	tVar = mapIntVar;
	assertTest( tVar.isIntMap() );
	assertEqual(tVar, mapIntVar);
	assertEqual(tVar.get(0), ProtocolVariant::g_varNULL);
	assertEqual(tVar.get(1), ProtocolVariant::g_varNULL);
	assertEqual(tVar.get(2), true);
	assertEqual(tVar.get(3), false);
	assertEqual(tVar.get(4), 3);
	assertEqual(tVar.get(5).asDouble(), 12.34);
	assertEqual(tVar.get(6), "char*");
	assertEqual(tVar.get(7), stdstring("string"));
	assertEqual(tVar.get(8), static_cast<rfc_int_64>(9527));
	assertEqual(tVar.getException(1), ProtocolVariant::g_varNULL);
	assertEqual(tVar.getException(2), true);
	assertEqual(tVar.getException(3), false);
	assertEqual(tVar.getException(4), 3);
	assertEqual(tVar.getException(5).asDouble(), 12.34);
	assertEqual(tVar.getException(6), "char*");
	assertEqual(tVar.getException(7), stdstring("string"));
	assertEqual(tVar.getException(8), static_cast<rfc_int_64>(9527));

	assertTest(!tVar.getDefault(0, false));
	assertTest(tVar.getDefault(0, true));
	assertTest(tVar.getDefault(2, true));
	assertTest(tVar.getDefault(2, false));
	assertTest(!tVar.getDefault(3, true));
	assertTest(!tVar.getDefault(3, false));

	assertEqual(tVar.getDefault(0, -1), -1);
	assertEqual(tVar.getDefault(4, -1), 3);
	assertEqual(tVar.getDefault(8, -1), static_cast<rfc_int_64>(9527));

	ProtocolVariant::typeStringVariantMap mapStrVar;
	mapStrVar["1"] = ProtocolVariant::g_varNULL;
	mapStrVar["2"] = true;
	mapStrVar["3"] = false;
	mapStrVar["4"] = 3;
	mapStrVar["5"] = 12.34;
	mapStrVar["6"] = "char*";
	mapStrVar["7"] = "string";
	mapStrVar["8"] = static_cast<rfc_int_64>(9527);
	tVar = mapStrVar;
	assertTest( tVar.isStringMap() );
	assertEqual(tVar, mapStrVar);
	assertEqual(tVar.get("0"), ProtocolVariant::g_varNULL);
	assertEqual(tVar.get("1"), ProtocolVariant::g_varNULL);
	assertEqual(tVar.get("2"), true);
	assertEqual(tVar.get("3"), false);
	assertEqual(tVar.get("4"), 3);
	assertEqual(tVar.get("5").asDouble(), 12.34);
	assertEqual(tVar.get("6"), "char*");
	assertEqual(tVar.get("7"), stdstring("string"));
	assertEqual(tVar.get("8"), static_cast<rfc_int_64>(9527));
	assertEqual(tVar.getException("1"), ProtocolVariant::g_varNULL);
	assertEqual(tVar.getException("2"), true);
	assertEqual(tVar.getException("3"), false);
	assertEqual(tVar.getException("4"), 3);
	assertEqual(tVar.getException("5").asDouble(), 12.34);
	assertEqual(tVar.getException("6"), "char*");
	assertEqual(tVar.getException("7"), stdstring("string"));
	assertEqual(tVar.getException("8"), static_cast<rfc_int_64>(9527));

	assertTest(!tVar.getDefault("0", false));
	assertTest(tVar.getDefault("0", true));
	assertTest(tVar.getDefault("2", true));
	assertTest(tVar.getDefault("2", false));
	assertTest(!tVar.getDefault("3", true));
	assertTest(!tVar.getDefault("3", false));

	assertEqual(tVar.getDefault("0", -1), -1);
	assertEqual(tVar.getDefault("4", -1), 3);
	assertEqual(tVar.getDefault("8", -1), static_cast<rfc_int_64>(9527));

	onProtocolVarExample();
}

RFC_NAMESPACE_END

