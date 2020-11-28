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
		strBinary = argv[1];
		GlobalFunc::binaryToHex(strBinary.data(), strBinary.size(), strHex, ' ');
		std::cout << FormatString("strBinary:[%], strHex:[%]").arg(strBinary).arg(strHex).str() << std::endl;
	}
	
	while ( true )
	{
		strBinary.clear();
		strHex.clear();
		std::cout << "Please input binary string(ctrl+c to break):";
		if ( !std::getline(std::cin, strBinary) )
			break;
		GlobalFunc::binaryToHex(strBinary.data(), strBinary.size(), strHex, ' ');
		std::cout << FormatString("strBinary:[%], strHex:[%]").arg(strBinary).arg(strHex).str() << std::endl;
	} // while ( true )

	return 0;
}
