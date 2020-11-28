#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <db.h>
#include <iconv.h>


#ifndef _D_H_iparea
#define _D_H_iparea

int CloseDB(DB *db);

DB* OpenDB(char *sFileName, char *sDbName, int nPageSize, int nCacheSize, int (*Compare)(DB *, const DBT *, const DBT *));

int CompareIp(DB *db, const DBT *dbt1, const DBT *dbt2);

int ReadIpArea(DB *db, char *sFileName);





#endif


