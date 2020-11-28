#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <db.h>

#ifndef _H_tool
#define _H_tool


int CloseDB(DB *db);

DB* OpenDB(char *sFileName, char *sDbName, int nPageSize, int nCacheSize, int (*Compare)(DB *, const DBT *, const DBT *));


#endif
