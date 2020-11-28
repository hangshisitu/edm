#ifndef _TASK_HANDLER_H_
#define _TASK_HANDLER_H_

#include "protocol.h"
#include "dwstring.h"

extern int file_get_taskinf(const char* file, taskinf_t* task);
extern int taskinf_dump(taskinf_t* task, const char* file);

#endif
