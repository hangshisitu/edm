#include "iparea.h"


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



int CompareIp(DB *db, const DBT *dbt1, const DBT *dbt2)
{
	if(memcmp(dbt1->data, dbt2->data+4, 4) > 0)
		return 1;
	if(memcmp(dbt1->data+4, dbt2->data, 4) < 0)
		return -1;
		
	return 0;
}




int ReadIpArea(DB *db, char *sFileName)
{
	FILE *pFile;
	char sLine[1024];
	DBT dbtData, dbtKey;
	iconv_t ICONV;
	
	memset(&dbtData, 0x0, sizeof(DBT));
	memset(&dbtKey, 0x0, sizeof(DBT));
	
	ICONV = iconv_open("utf-8", "gbk");
	pFile = fopen(sFileName, "r");	
	while(fgets(sLine, sizeof(sLine), pFile))
	{
		char *sTmpB, *sTmpE;
		unsigned char sIp[12];
		char sUtf8[50];
		
		if(sscanf(sLine, "%d.%d.%d.%d|%d.%d.%d.%d", sIp, sIp+1, sIp+2, sIp+3, sIp+4, sIp+5, sIp+6, sIp+7) != 8)
			continue;
		sTmpB = strchr(sLine, '|');
		if(!sTmpB)
			continue;
		sTmpB = strchr(sTmpB+1, '|');
		if(!sTmpB)
			continue;
		sTmpB++;
		sTmpE = strchr(sTmpB, '|');
		if(!sTmpE)
			sTmpE = strchr(sTmpB, '\r');
		if(!sTmpE)
			sTmpE = strchr(sTmpB, '\n');
		if(sTmpE)
			*sTmpE = 0;
			
		{
			char *s1 = sTmpB, *s2 = sUtf8;
			size_t inleft = strlen(sTmpB)+1, outleft = sizeof(sUtf8);
			iconv(ICONV, &s1, &inleft, &s2, &outleft);	
		}	
		
		dbtKey.data = sIp;
		dbtKey.size = 8;
		dbtData.data = sUtf8;
		dbtData.size = strlen(sUtf8)+1;
		if(db->put(db, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE))
		{
			printf("add fail:%s.\n", sLine);	
		}
	}
	fclose(pFile);	
	iconv_close(ICONV);
	
	return 0;
}


int ShowIpArea(DB *db)
{
	DBC *dbc;
	DBT dbtKey, dbtData;
	
	db->cursor(db, NULL, &dbc, 0); 
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	
	while( dbc->c_get(dbc, &dbtKey, &dbtData, DB_NEXT) == 0)
	{
		unsigned char *s;
		s = (unsigned char*)(dbtKey.data);
		printf("%d.%d.%d.%d-%d.%d.%d.%d:%s\n", s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], (char*)(dbtData.data));
	}
	dbc->c_close(dbc);
	return 0;
}



/*
int main(int argc, char *argv[])
{
	DB *db;
	
	db = OpenDB(NULL, NULL, 64, 512, CompareIp);	
	
	ReadIpArea(db, argv[1]);
	ShowIpArea(db);
	
	CloseDB(db);
	return 0;
}
*/


