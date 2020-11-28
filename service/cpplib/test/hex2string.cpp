/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "base/globalfunc.h"
#include "base/formatstream.h"

USING_RFC_NAMESPACE

int main(int argc, char * argv[])
{
	stdstring strBinary, strHex;
	if ( argc == 2 )
	{
		strHex = argv[1];
		GlobalFunc::hexToBinary(strHex.data(), strHex.size(), strBinary, ' ');
		std::cout << FormatString("strHex:[%], strBinary:[%]").arg(strHex).arg(strBinary).str() << std::endl;
	}

	while ( true )
	{
		strBinary.clear();
		strHex.clear();
		std::cout << "Please input hex string(ctrl+c to break):";
		if ( !std::getline(std::cin, strHex) )
			break;
		GlobalFunc::hexToBinary(strHex.data(), strHex.size(), strBinary, ' ');
		std::cout << FormatString("strHex:[%], strBinary:[%]").arg(strHex).arg(strBinary).str() << std::endl;
	} // while ( true )

	return 0;
}
