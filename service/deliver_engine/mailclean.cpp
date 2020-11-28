/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan				Date: 2011-08      */

#include "mailclean.h"
#include "mutex/mutex.h"
#include "base/formatstream.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>



RFC_NAMESPACE_BEGIN
//FileLog		fileLog;
MxMgr mxmgr;



MailClean::MailClean(void) : Application("mailclean")
{

}

bool MailClean::daemon(void)
{
	pid_t pid;
	if((pid=fork()) < 0)
	{
		printf("fork fail \n");	
		return false;
	}
	else if(pid > 0)
	{
		printf("this is parent process\n");	
		exit(0);
	}
	else
	{
		printf("this is child process\n");
		return true;
	}
}

bool MailClean::onInit(int argc, char * argv[])
{
	ConfigureMgr conf;
	if(!conf.loadConfFile("./etc/mc.etc"))
	{
		printf("配置文件初始化失败\n");
		return false;
	}

	snprintf(m_in,sizeof(m_in),"%s/in",argv[1]);

	snprintf(m_outok,sizeof(m_outok),"%s/outok",argv[1]);

	snprintf(m_outno,sizeof(m_outno),"%s/outno",argv[1]);

	if(argc == 2)
		snprintf(m_vip,sizeof(m_vip),"%s","123");
	snprintf(m_vip,sizeof(m_vip),"%s",argv[2]);

	mxmgr.onInit(conf);

	return true;
}

void MailClean::Write2File()
{
	char path[128];
	FILE * pFile_ok = NULL;
	FILE * pFile_no = NULL;
	vector<string>::iterator it;
	char line[64];

	if((m_total - m_map_no.size()) !=0)
	{
		snprintf(path,sizeof(path),"%s/%s",m_outok,m_filename);
		pFile_ok=fopen(path, "w");
	}
	if(m_map_no.size() !=0)
	{
		snprintf(path,sizeof(path),"%s/%s",m_outno,m_filename);
		pFile_no=fopen(path, "w");
	}

	for(it = m_invec.begin();it!=m_invec.end();it++)
	{
		if(m_map_no.find(*it) == m_map_no.end())
		{
			memset(line,0,64);
			sprintf(line,"%s\n",(*it).c_str());
			fwrite(line,strlen(line),1,pFile_ok);
			fflush(pFile_ok);
		}
		else
		{
			memset(line,0,64);
			sprintf(line,"%s\n",(*it).c_str());
			fwrite(line,strlen(line),1,pFile_no);
			fflush(pFile_no);
		}
	}

	if((m_total - m_map_no.size()) !=0)
	{
		fclose(pFile_ok);
	}
	if(m_map_no.size() !=0)
	{
		fclose(pFile_no);
	}

	return;	
}

void MailClean::SendEml()
{
	//Make Class Map
	vector<string>::iterator it_v = m_invec.begin();
	char user[64];
	char domain[64];
	m_class_map.clear();
	for(;it_v!=m_invec.end();it_v++)
	{
		memset(user,0,64);
		memset(domain,0,64);
		sscanf((*it_v).c_str(),"%[^@]@%[^@]",user,domain);
		m_class_map[domain].push_back(*it_v);
	}

	m_map_no.clear();
	
	map<string,vector<string> >::iterator it_m = m_class_map.begin();

	Sender sender;
	unsigned int i = 0;

	for(;it_m!=m_class_map.end();it_m++)
	{
		i = 1;
		vector<string> tmp;
		tmp.clear();
		for(it_v=(it_m->second).begin();it_v!=(it_m->second).end();it_v++)
		{
			tmp.push_back(*it_v);
			if(i < 10 && i == it_m->second.size())   //小于10个
			{
				string strIP;
				rfc_uint_16 nPort;
				mxmgr.getMXRecord(it_m->first, strIP, nPort, 0);
				sender.Set(m_vip, strIP.c_str(), nPort);
				sender.Send2Domain(tmp,m_map_no);
				tmp.clear();
			}
			else if(i%10 == 0 || i == it_m->second.size())
			{
				string strIP;
				rfc_uint_16 nPort;
				mxmgr.getMXRecord(it_m->first, strIP, nPort, 0);
				sender.Set(m_vip, strIP.c_str(), nPort);	
				sender.Send2Domain(tmp,m_map_no);
				tmp.clear();
			}

			i++;
		}
	}

	fprintf(stdout,"ok %d\n",m_total - (unsigned int)m_map_no.size());
	fprintf(stdout,"no %d\n",(unsigned int)m_map_no.size());
	return;
}

int MailClean::onRun(void)
{
	DIR * dp;
	struct dirent *filename;

	dp = opendir(m_in);
	if (!dp)
	{
//		fileLog.writeLog("open directory error\n");
		return -1;
	}
	char filepath[128];
	struct stat sctFile;
	FILE * pFile;
	char * file;
	char * p;
	m_invec.clear();
	while((filename=readdir(dp)))
	{
		if(filename->d_name[0] == '.')
		{
			continue;
		}
		
//		fileLog.writeLog((FormatString("file name %\n").arg(filename->d_name).str()));		

		snprintf(m_filename,sizeof(m_filename),"%s",filename->d_name);
		snprintf(filepath,sizeof(filepath),"%s/%s",m_in,filename->d_name);

		fprintf(stdout,"file name %s\n",filepath);
		stat(filepath, &sctFile);

		pFile=fopen(filepath, "r");
		file = new char[sctFile.st_size+1];

		fread(file, sctFile.st_size, 1, pFile);
		p = file;
		char * tmp = file;
		m_total = 0;
		while(1)
		{
			if(*p == '\0')
			{
				break;
			}
			if(*p == '\n')
			{
			    *p = '\0';	
				if(*file != '\n')
				{
					m_total++;
					m_invec.push_back(file);
				}
				p++;
				file = p;
			}
			if(*p == '\r' && *(p+1) == '\n')
			{
				*p= '\0';
				*(p+1) = '\0';
				if(*file != '\r')
				{
					m_total++;
					m_invec.push_back(file);
				}
				p+=2;
				file = p;
			}
			p++;
		}
		delete tmp;
		fprintf(stdout,"total %d\n",m_total);
		SendEml();
		Write2File();
	}

	return 0;
}

RFC_NAMESPACE_END

MAIN_APP(MailClean)
