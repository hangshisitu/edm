/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan Date: 2011-11-08      */

#ifndef APP_MAILCLEAN_H
#define APP_MAILCLEAN_H

#include "application/application.h"
#include "application/configuremgr.h"
#include "system/filehandle.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "application/filelog.h"
#include <vector>
#include <map>
#include "mxmgr.h"
#include "sender.h"


using namespace std;

RFC_NAMESPACE_BEGIN


class MailClean: public Application
{
public:
	MailClean(void);


protected:
	bool				daemon(void);
	virtual bool		onInit(int argc, char * argv[]);
	virtual int			onRun(void);
	void				Write2File();
	void				SendEml();

private:
	char m_in[64];
	char m_outok[64];
	char m_outno[64];
	char m_vip[64];
	char m_filename[64];
	unsigned int m_total;
	map<string,int> m_map_no;
	vector<string> m_invec;
	map< string,vector<string> > m_class_map;
};

RFC_NAMESPACE_END

#endif	//APP_DELIVERENGINE_H_201108
