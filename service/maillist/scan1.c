#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <db.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>




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


int main(int argc, char *argv[])
{
	struct stat sFileStat;
	DIR* pDir;
	struct dirent *d_c;	
	int nID = atoi(argv[2]);	
	DB *db = OpenDB(argv[3], NULL, 64, 2048, NULL);
	DBT dbtKey, dbtData;
	DBC *dbc;
	FILE *pFile;
	
	memset(&dbtKey, 0x0, sizeof(DBT));
	memset(&dbtData, 0x0, sizeof(DBT));
	db->cursor(db, NULL, &dbc, 0);
	
	pDir = opendir(argv[1]);	
	while(d_c = readdir(pDir))
	{
		char sFileName[200];
		char sLine[102400];
		
		int nDay, nYear, nMonth;
		char sMsg[1024];
		char sDate[20];
		int nRet = 0;
		struct stat sctFileStat;
		
		if(strcmp(d_c->d_name, ".") == 0 || strcmp(d_c->d_name, "..") == 0)
			continue;
		{
			memcpy(sDate, d_c->d_name+12, 8);
			sDate[8] = 0;
			/*
			int kk;
			sscanf(d_c->d_name, "deliver.log.%d", &kk);
			nDay = kk % 100;
			kk = kk/100;
			nMonth = kk % 100;
			nYear = kk/100;
			*/
		}
		snprintf(sFileName, 200, "%s/%s", argv[1], d_c->d_name);
		stat(sFileName, &sctFileStat);
		if(S_ISREG(sctFileStat.st_mode) == 0)
			continue;
		
		pFile = fopen(sFileName, "r");
		while(fgets(sLine, sizeof(sLine), pFile))
		{
			char *sTmp, *sTmp2, *sTmp3;
			int nStat;
			char sTime[20];
			
			
			
			sTmp = strstr(sLine, "][");
			if(!sTmp)
				continue;
			sTmp += 2;
			if(strncmp(sTmp, "REPORT", 6))
				continue;	
			sTmp2 = strchr(sTmp, '\r');
			if(!sTmp2)
				sTmp2 = strchr(sTmp, '\n');
			if(sTmp2)
				*sTmp2 = 0;
			sTmp2 = strchr(sLine, ']');
			if(!sTmp2)
				continue;
			*sTmp2 = 0;
			snprintf(sTime, 20, "%s", sLine+1);
			sTmp += 7;
			sTmp = strstr(sTmp, "][");
			while(sTmp)
			{
				int nn = atoi(sTmp+2);
				
				if(nn == nID)
				{
					char *sTmp3 = NULL, *sTmp4 = NULL;
					char c = 0;
					
					sTmp3 = strchr(sTmp+2, '(');
					if(sTmp3)
					{
						*sTmp3 = 0;
						sTmp4 = strchr(sTmp3+1, '[');
						if(sTmp4)
							*sTmp4 = 0;
					}
					if(sTmp4 == NULL)
					{
						sTmp4 = strchr(sTmp+2, '[');
						if(sTmp4)
							*sTmp4 = 0;
					}
					
					
					dbtKey.size = strlen(sTmp+1)+1;
					dbtKey.data = sTmp+1; 
					sMsg[0] = *(sTmp-1);
					snprintf(sMsg+1, 20, "%s", sDate);
					snprintf(sMsg+21, 20, "%s", sTime);
					if(sTmp3)
					{
						snprintf(sMsg+41, 900, "%s", sTmp3+1);	
					}
					else
						sMsg[41] = 0;
					dbtData.data = sMsg;
					dbtData.size = strlen(sMsg+41)+42;
					nRet = db->put(db, NULL, &dbtKey, &dbtData, DB_NOOVERWRITE);
					if(nRet)
					{
						dbc->c_get(dbc, &dbtKey, &dbtData, DB_SET);
						if(*((char*)dbtData.data) != 's')
						{
							dbtData.data = sMsg;
							dbtData.size = strlen(sMsg+41)+42;
							dbc->c_put(dbc, &dbtKey, &dbtData, DB_CURRENT);	
						}	
					}
					if(sTmp3)
						*sTmp3 = '(';
					if(sTmp4)
						*sTmp4 = '[';
				}
				
				sTmp = strstr(sTmp+2, "][");	
			}
			//[00:00:55] [173.252.234.98.01015][REPORT][s][113.846.344]179454330@qq.com(ok)[s][113.846.344]179455804@qq.com(ok)[s][113.846.344]479
			//500@qq.com(ok)[s][113.846.344]688773@qq.com(ok)[s][113.846.344]798184610@qq.com(ok)	
			
		}
		fclose(pFile);
		
	}
	dbc->c_close(dbc);
	db->cursor(db, NULL, &dbc, 0);
	
	
	pFile = fopen(argv[4], "w");
	fprintf(pFile, "taskid,email,date,time,status,return\r\n");
	while(dbc->c_get(dbc, &dbtKey, &dbtData, DB_NEXT) == 0)
	{
		char *s1, *s2;
		char *sTmp;
		
		s1 = (char*)(dbtKey.data);
		s2 = (char*)(dbtData.data);	
		sTmp = strchr(s1, ']');
		if(!sTmp)
			continue;
		*sTmp = 0;
		fprintf(pFile, "%s,%s,", s1+1, sTmp+1);
		sTmp = s2+1;
		fprintf(pFile, "%s,%s,", sTmp, sTmp+20);
		if(*s2 == 's')
			fprintf(pFile, "success,");
		else if(*s2 == 'f')
			fprintf(pFile, "delay,");
		else
			fprintf(pFile, "fail,");
		if(dbtData.size > 43)
			fprintf(pFile, "(%s\r\n", s2+41);
		else
			fprintf(pFile, "\r\n");	
	}
	fclose(pFile);
	
	CloseDB(db);
	
	return 0;
}









