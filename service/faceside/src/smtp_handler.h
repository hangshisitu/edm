#ifndef _SMTP_HANDLER_H_
#define _SMTP_HANDLER_H_

#include "protocol.h"

extern int smtp_getline(int sock, char* buffer, int len, int timeout);
extern int smtp_post(const char* file, int refcnt);

#endif
