#include "tool.h"

DB* OpenDB(char *sFileName, char *sDbName, int nPageSize, int nCacheSize, int (*Compare)(DB *, const DBT *, const DBT *))
{
	DB *db;
	int nRet = 0;
	int nFlag;
	
	nRet = db_create(&db, NULL, 0);
	if(nRet)
		return NULL;
	if(Compare)
	{
		nRet = db->set_bt_compare(db, Compare);
		if(nRet)
		{
			db->close(db, 0);
			printf("[[%s]]\n", db_strerror(nRet));	
			return NULL;
		}
	}
	nRet = db->set_pagesize(db, nPageSize*1024);
	if(nRet)
	{
		db->close(db, 0);
		printf("[[%s]]\n", db_strerror(nRet));
		return NULL;
	}
	nRet =  db->set_cachesize(db, 0, nCacheSize*1024*1024, 1);
	if(nRet)
	{	
		db->close(db, 0);
		printf("[[%s]]\n", db_strerror(nRet));
		return NULL;
	}
	nFlag = DB_CREATE | DB_THREAD;
	nRet = db->open(db, NULL, sFileName, sDbName, DB_BTREE, nFlag, 0);
	if(nRet)
	{
		printf("open fail.<<%s>>\n", db_strerror(nRet));
		db->close(db, 0);
		return NULL;
	}
	
	return db;
}



int CloseDB(DB *db)
{
	int nRet;
	nRet = db->close(db, 0);
	if(nRet)
		return -1;
	return 0;
}









