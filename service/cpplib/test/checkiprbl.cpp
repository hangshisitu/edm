/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "application/application.h"
#include "base/formatstream.h"

RFC_NAMESPACE_BEGIN

class CheckIPRBLApp : public Application
{
public:
	CheckIPRBLApp(void) : Application("checkiprbl") {}

protected:
	virtual bool			onInit(int argc, char * argv[]);
	virtual int				onRun(void);

protected:
	void					displayUsage(const char * lpszAppName) const;
	void					loadReputation(void) const;

protected:
	stdstring				m_strReputationFilePath;
	stdstring				m_strRBLDirPath;
	stdstring				m_strToCheckIPList;
};

bool CheckIPRBLApp::onInit(int argc, char * argv[])
{
	char ch;
	while ( (ch = getopt(argc, argv, "h::p::r::i::")) != -1 )
	{
		if ( ch == 'h' )
		{
			displayUsage(argv[0]);
			return false;
		}
		else if ( optind < argc && argv[optind][0] != '-' )
		{
			if ( ch == 'p' )
				m_strReputationFilePath = argv[optind++];
			else if ( ch == 'r' )
				m_strRBLDirPath = argv[optind++];
			else if ( ch == 'i' )
				m_strToCheckIPList = argv[optind++];
		}
	} // while ( (ch = getopt(argc, argv, "h::p::r::i::")) != -1 )

	std::cout << FormatString("iprep:%, rbl:%, iplist:").arg(m_strReputationFilePath).arg(m_strRBLDirPath).arg(m_strToCheckIPList).str() << std::endl;
	return true;
}

int CheckIPRBLApp::onRun(void)
{
	std::cout << "app full path: " << getAppFullPath() << std::endl;
	return 0;
}

void CheckIPRBLApp::displayUsage(const char * lpszAppName) const
{
	std::cout << FormatString("Usage: % -p $IPReputaionFilePath -r $RBLDirPath -i $ToCheckIPList").arg(lpszAppName).str() << std::endl;
}

RFC_NAMESPACE_END

MAIN_APP(CheckIPRBLApp)
