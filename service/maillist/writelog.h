#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "maillist_daemon.h"
#include "daemon_conf.h"

#ifndef _H_writelog
#define _H_writelog

int OpenLogFile(CONFIGURE *pConf, DABUDAI *pDa);

int WriteLog(CONFIGURE *pConf, DABUDAI *pDa, char *sLog, int nSize);















#endif

