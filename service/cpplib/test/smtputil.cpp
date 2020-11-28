/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#include "thread/thread.h"
#include "system/filesystem.h"
#include "network/networkfunc.h"
#include "base/formatstream.h"
#include "base/stringtokenizer.h"
#include "base/datetime.h"
#include <mimepp/smtp.h>

USING_RFC_NAMESPACE

static void writeLogLine(const stdstring & strLine)
{
	DateTime dt(time(NULL));
	stdstring strData = FormatString("T:%(%) %").arg(ThreadFunc::getSelfThreadID()).arg(dt.formatTime()).arg(strLine).str();
	static MutexLock aLockForLog;
	AutoMutexLock auLock(aLockForLog);
	std::cout << strData << std::endl;
}

static bool isSmtpOK(int nSmtpReplyCode)
{
	return ( nSmtpReplyCode >= 200 && nSmtpReplyCode < 300 );
}

static void onSendMail(const stdstring & strHost, const stdstring & strMailFrom,
					   const stdstring & strRcptToList, const stdstring & strEmlContent, const stdstring & strBindLocalIP)
{
	SecondTimer aTimer;
	time_t tOpen = 0, tHelo = 0, tMail = 0, tRcpt = 0, tData = 0;
	stdstring strServer(strHost), strPort("25");
	size_t nPortPos = strHost.find(':');
	if ( nPortPos != stdstring::npos )
	{
		strServer = strHost.substr(0, nPortPos);
		strPort = strHost.substr(nPortPos + 1);
	}

	const char * lpszBindLocalIP = NULL;
	if ( !strBindLocalIP.empty() )
		lpszBindLocalIP = strBindLocalIP.c_str();
	int nSmtpReplyCode = 0;
	DwSmtpClient aSmtpClient;
	nSmtpReplyCode = aSmtpClient.Open(strServer.c_str(), StringFunc::stringToInt<DwUint16>(strPort.data(), strPort.size()), lpszBindLocalIP);
	if ( !isSmtpOK(nSmtpReplyCode) )
	{
		writeLogLine( FormatString("Connect smtp server [%:%] bind local ip [%] reply fail:%(%)")
			.arg(strServer).arg(strPort).arg(strBindLocalIP).arg(nSmtpReplyCode).arg(aSmtpClient.Response()).str() );
		return;
	}
	tOpen = aTimer.elapsedSecond();

	const char * lpszHeloDomain = "testsmtputil.com";
	nSmtpReplyCode = aSmtpClient.Helo(lpszHeloDomain);
	if ( !isSmtpOK(nSmtpReplyCode) )
	{
		writeLogLine( FormatString("Send Helo [%] to [%:%] reply fail:%(%)")
			.arg(lpszHeloDomain).arg(strServer).arg(strPort).arg(nSmtpReplyCode).arg(aSmtpClient.Response()).str() );
		return;
	}
	tHelo = aTimer.elapsedSecond();

	nSmtpReplyCode = aSmtpClient.Mail(strMailFrom.c_str());
	if ( !isSmtpOK(nSmtpReplyCode) )
	{
		writeLogLine( FormatString("Send Mail [%] to [%:%] reply fail:%(%)")
			.arg(strMailFrom).arg(strServer).arg(strPort).arg(nSmtpReplyCode).arg(aSmtpClient.Response()).str() );
		return;
	}
	tMail = aTimer.elapsedSecond();

	bool bRcptOK = false;
	StringTokenizer<stdstring> tokenRcptTo(strRcptToList.data(), strRcptToList.size());
	stdstring strRcptTo;
	while ( tokenRcptTo.getNext(strRcptTo, ",; \t\r\n") )
	{
		nSmtpReplyCode = aSmtpClient.Rcpt(strRcptTo.c_str());
		if ( !isSmtpOK(nSmtpReplyCode) )
		{
			writeLogLine( FormatString("Send Rcpt [%] to [%:%] reply fail:%(%)")
				.arg(strRcptTo).arg(strServer).arg(strPort).arg(nSmtpReplyCode).arg(aSmtpClient.Response()).str() );
			continue;
		}
		bRcptOK = true;
	} // while ( tokenRcptTo.getNext(strRcptTo, ",;") )
	tRcpt = aTimer.elapsedSecond();
	if ( bRcptOK )
	{
		nSmtpReplyCode = aSmtpClient.Data();
		if ( nSmtpReplyCode < 200 || nSmtpReplyCode >= 400 )
		{
			writeLogLine( FormatString("Send DATA command to [%:%] reply fail:%(%)")
				.arg(strServer).arg(strPort).arg(nSmtpReplyCode).arg(aSmtpClient.Response()).str() );
			return;
		}
		tData = aTimer.elapsedSecond();

		nSmtpReplyCode = aSmtpClient.SendData(strEmlContent.data(), strEmlContent.size());
		if ( !isSmtpOK(nSmtpReplyCode) )
		{
			writeLogLine( FormatString("Send DATA content to [%:%] reply fail:%(%)")
				.arg(strServer).arg(strPort).arg(nSmtpReplyCode).arg(aSmtpClient.Response()).str() );
		}
	} // if ( bRcptOK )
	
	writeLogLine( FormatString("smtp send mail to %:% OK. bind local ip:[%], from:%, rcptlist:%, tOpen:%, tHelo:%, tMail:%, tRcpt:%,"
		" tData:%, tContent:%, respond:%").arg(strServer).arg(strPort).arg(strBindLocalIP)
		.arg(strMailFrom).arg(strRcptToList).arg(tOpen).arg(tHelo).arg(tMail).arg(tRcpt).arg(tData)
		.arg(aTimer.elapsedSecond()).arg(aSmtpClient.Response()).str() );
}

int main(int argc, char * argv[])
{
	try
	{
		if ( argc < 5 )
		{
			std::cout << FormatString("Usage:% $Server[:$Port] $MailFrom $RcptList $EmlFilePath [$LoopCount] [$LocalIPList]").arg(argv[0]).str() << std::endl;
			std::cout << FormatString("% 127.0.0.1:2025 mail@from.com 1@rcpt.com,2@rcpt.com /home/var/1.eml 5 1.2.3.4,2.3.4.5").arg(argv[0]).str() << std::endl;
			return -1;
		}

		stdstring strRcptToList(argv[3]);
		if ( !strRcptToList.empty() && strRcptToList[0] == '@' )	// 从文件读取收件人列表
		{
			stdstring strRcptFilePath = strRcptToList.substr(1);
			strRcptToList.clear();
			if ( !FileSystem::loadFileContent(strRcptFilePath, strRcptToList) )
			{
				writeLogLine( FormatString("Load rcpt file % fail: %(%)").arg(strRcptFilePath).arg(errno).arg(GlobalFunc::getSystemErrorInfo()).str() );
				return -2;
			}
		} // if ( !strRcptToList.empty() && strRcptToList[0] == '@' )	// 从文件读取收件人列表

		stdstring strEmlContent;
		if ( !FileSystem::loadFileContent(argv[4], strEmlContent) )
		{
			writeLogLine( FormatString("Load eml file % fail: %(%)").arg(argv[4]).arg(errno).arg(GlobalFunc::getSystemErrorInfo()).str() );
			return -3;
		}

		int nLoopCount = 1;
		if ( argc >= 6 )
		{
			nLoopCount = StringFunc::stringToInt<int>(argv[5]);
		}

		bool bBindLocalIPOK = false;
		std::set<stdstring> setLocalIP;
		NetworkFunc::getAllLocalIP(setLocalIP);
		if ( argc >= 7 )
		{
			setLocalIP.clear();
			stdstring strBindIPList = argv[6];
			writeLogLine("input bind ip list" + strBindIPList);
			StringTokenizer<stdstring> tokenLocalIP(strBindIPList.data(), strBindIPList.size());
			stdstring strIP;
			while ( tokenLocalIP.getNext(strIP, ",;") )
				setLocalIP.insert(strIP);
		}

		SecondTimer aTimer;
		int nTotalSentCount = 0;
		for ( int i = 0; i < nLoopCount; ++i )
		{
			std::set<stdstring>::const_iterator itLocalIP, itLocalIPEnd = setLocalIP.end();
			for ( itLocalIP = setLocalIP.begin(); itLocalIP != itLocalIPEnd; ++itLocalIP )
			{
				const stdstring & strLocalIP = (*itLocalIP);
				writeLogLine("Local IP: " + strLocalIP);
				if ( strLocalIP == "127.0.0.1" )
					continue;
				bBindLocalIPOK = true;
				onSendMail(argv[1], argv[2], argv[3], strEmlContent, strLocalIP);
				++nTotalSentCount;
				writeLogLine( FormatString("Sent index:%, total count:%, total time:%, average time:%")
					.arg(i).arg(nTotalSentCount).arg(aTimer.elapsedSecond()).arg(aTimer.elapsedSecond()/nTotalSentCount).str() );
			} // for ( itLocalIP = setLocalIP.begin(); itLocalIP != itLocalIPEnd; ++itLocalIP )

			if ( !bBindLocalIPOK )
			{
				onSendMail(argv[1], argv[2], argv[3], strEmlContent, StringFunc::g_strEmptyString);
				writeLogLine( FormatString("Sent index:%, total count:%, total time:%, average time:%")
					.arg(i).arg(nTotalSentCount).arg(aTimer.elapsedSecond()).arg(aTimer.elapsedSecond()/nTotalSentCount).str() );
			}
		}
	}
	catch ( Exception & e )
	{
		std::cout << FormatString("Catch Exception in main:%").arg(e.what()).str() << std::endl;
	}
	catch ( std::exception & e )
	{
		std::cout << FormatString("Catch std::exception in main:%").arg(e.what()).str() << std::endl;
	}
	catch ( ... )
	{
		std::cout << FormatString("Catch unknown exception in main").str() << std::endl;
	}
	return 0;
}
