/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan				Date: 2011-08      */

#include "requestmail.h"
#include <errno.h>
#include <math.h>
#include <time.h>


RFC_NAMESPACE_BEGIN
extern FileLog fileLog;
extern map<unsigned int, mail_node *>  m_map_mail;

RequestMail::RequestMail()
{

}

RequestMail::~RequestMail()
{

}

int RequestMail::InstallHead(char * req, int command_code)
{
	rfc_uint_16	nProtocolPrefix = htons(0xfffe);
	rfc_uint_16	nProtocolVersion = htons(1);
	rfc_uint_16	nCommandVersion = htons(1);
	rfc_uint_16	nClientID = htons(1);
	rfc_uint_32	nCmd ;

	switch(command_code)
	{
		case COMMOND_ABILITY_REPORT:
			nCmd = htonl(0x1); 
			break;	
		case COMMOND_CHECK_TEMPLATE_REPORT:
			nCmd = htonl(0x2);
			break;
		case COMMOND_RESULT_REPORT:
			nCmd = htonl(0x3);
			break;
	}

	req += 4;
	memcpy(req,&nProtocolPrefix,2);
	req += 2;
	memcpy(req,&nProtocolVersion,2);
	req += 2;
	memcpy(req,&nCommandVersion,2);
	req += 2;
	memcpy(req,&nClientID,2);
	req += 2;
	memcpy(req,&nCmd,4);
	req += 4;

	return 0;
}

int RequestMail::ParseMailListRsp(char * rsp_msg, map<unsigned int,struct mail_node *> &map_mail)
{
	rfc_uint_32 command_id;
	memcpy(&command_id, rsp_msg, 4);

	rfc_uint_32 info_lengh;
	memcpy(&info_lengh, rsp_msg+4, 4);
	rsp_msg += ntohl(info_lengh)+8;

	//fileLog.writeLog((FormatString("command id is [%] \n").arg(ntohl(command_id)).str()));
	if(ntohl(command_id) != 1)
	{
		fileLog.writeLog((FormatString("not a ABILITY_REPORT RSP command id is [%] \n").arg(ntohl(command_id)).str()));
		//fileLog.writeLog("this is not a ABILITY_REPORT RSP\n");	
		return 0;
	}
	rfc_uint_32 mailnum;
	memcpy(&mailnum,rsp_msg,4);
	mailnum = ntohl(mailnum);
	//fileLog.writeLog((FormatString("mailnum is % \n").arg(mailnum).str())); 

	MakeMailList(rsp_msg, map_mail, mailnum);
	fileLog.writeLog((FormatString("mailnum: [%] maillist size: [%] \n").arg(mailnum).arg(map_mail.size()).str()));
	return 0;
}

int RequestMail::MakeMailList(char * rsp_msg, map<unsigned int,struct mail_node *> &map_mail, int mailnum)
{
	int i = 0;
	char * mail = rsp_msg+4;
	int mail_length;
	static unsigned int seq_no = 0;
	char usrname[128];
	unsigned int j = 0;
	static unsigned int totala = 0;
	static unsigned int totalb = 0;

	while(i++ < mailnum)
	{
		unsigned int time;
		unsigned int m_id;
		unsigned int p_id;

		if(seq_no++ == 10000000)
		{
			seq_no = 0;
		}
		memcpy(&mail_length,mail,4);	
		mail_length = ntohl(mail_length);
		//fileLog.writeLog((FormatString("mail_length is [%] [%] [%]\n").arg(mail_length).arg(mailnum).arg(i).str()));
		if(mail_length == 0)
		{
			continue;
		}
		struct mail_node * p_node= new struct mail_node;

		p_node->status = 0; 
		p_node->failtimes = 0; 
		memset(p_node->mail,0,sizeof(p_node->mail));		

		memcpy(&time,mail+4,4);
		memcpy(&m_id,mail+8,4);
		memcpy(&p_id,mail+12,4);

		p_node->task_id.time = (unsigned int)ntohl(time);
		p_node->task_id.m_id = (unsigned int)ntohl(m_id);
		p_node->task_id.p_id = (unsigned int)ntohl(p_id);
		strncpy(p_node->mail,mail+16,strlen(mail+16));			//邮箱

		memset(p_node->domain,0,sizeof(p_node->domain));
		sscanf(p_node->mail,"%[^@]@%[^@]",usrname,p_node->domain);	//域名

        {
            char noblank[sizeof(p_node->domain)] = {0};
            char* p = noblank;

            for (int i=0; i<sizeof(p_node->domain); i++)
            {
                if ( !::isblank(p_node->domain[i])){
                    *p++ = p_node->domain[i];
                } 
            }

            memcpy(p_node->domain,noblank,sizeof(noblank));
        }

		//解析变量部分入变量队列
		fileLog.writeLog((FormatString("mail is [%.%.%] [%] [%]\n").arg(p_node->task_id.time).arg(p_node->task_id.m_id).arg(p_node->task_id.p_id).arg(seq_no).arg(mail + 16).str()));
		SplitVariable(p_node, mail + 16, mail_length);
		mail += 4 + mail_length;

		if(p_node->task_id.time > 3000000)
		{
			continue;
		}
		if(map_mail.find(seq_no) == map_mail.end())
		{
			fileLog.writeLog("add to sendlist\n");
			map_mail[seq_no] = p_node;
			j++;
		}
		else
		{
			fileLog.writeLog("key double, can't add to sendlist\n");
			map_mail[seq_no] = p_node;
		} 
	}
	if (j > 0 || mailnum > 0) {
	      totala += j;
	      totalb += mailnum;
	      fileLog.writeLog((FormatString("add to list num [%] [%] total[%] [%]\n").arg(j).arg(mailnum).arg(totala).arg(totalb).str()));
	}
	return 0;	
}

int RequestMail::SplitVariable(struct mail_node * p_node, char * variable, int  mail_length)
{
	int i = 0;
	int j = 0;
	char * p = variable;
	char tmp[1024*1024*4];
	char key[128];
	char value[1024*1024*4];
	char s[4];

	mail_length -= 12;
	while(1)
	{
		if(i == mail_length)
			break;

		if(*(variable+i) == '\0' && j != 0)  //变量 
		{
			memset(tmp,0,sizeof(tmp));
			strncpy(tmp,p,strlen(p));
			memset(key,0,sizeof(key));
			memset(value,0,sizeof(value));
			sscanf(tmp, "%[$]%[^$]$%[^$]", s,key,value);
			p_node->map_variable[key] = value;
			p = variable + i + 1;
			j++;
		}

		if(*(variable+i) == '\0' && j==0)	//第一次为邮箱，之后为变量
		{
			memset(tmp,0,sizeof(tmp));
			strncpy(tmp,p,strlen(p));
			p = variable + i + 1;
			j++;
		}
		i++;
	}
	return 0;
}

int RequestMail::handle_notify_msg(map<unsigned int, struct mail_node *> &m_map_mail, const std::vector<Task_Id>& stopstaks)
{
	//----------------标记停止任务已完成--------------
	if (!stopstaks.empty()) {
		std::vector<rfc::_task_id>::const_iterator stopit;
		map<unsigned int, struct mail_node *>::iterator iter;
		for (iter = m_map_mail.begin(); iter != m_map_mail.end(); ++iter) {
			for (stopit = stopstaks.begin(); stopit != stopstaks.end(); ++stopit) {
				if (iter->second->task_id == *stopit && iter->second->status != 1) {
					iter->second->status = 6;               //stop:标记为删除
					iter->second->stime = ::time(NULL);
					fileLog.writeLog((FormatString("find one, mark stop [%] [%] [%], mail[%]\n").arg(stopit->time).arg(stopit->p_id).arg(stopit->m_id).arg(iter->second->mail).str()));
				}
			}//for_stop_task  mini_loop               
		}//for_mail
	}
	//-------------------------------------------
	return 0;
}

int RequestMail::ParseRsp(stdstring &rsp, int fd, map<unsigned int, struct mail_node *>& map_mail)
{
	fd_set ReadSet;

	FD_ZERO(&ReadSet);
	FD_SET(fd, &ReadSet);
	struct timeval timeout;
	timeout.tv_sec = 60;
	timeout.tv_usec= 0;
	char c_len[5];
	int ret;
	unsigned int length;
	if((ret = select(fd+1,&ReadSet,NULL,NULL,&timeout)) == -1)
	{
		fileLog.writeLog(( FormatString("[%] select fail\n").arg(errno).str() ));
		return -1;
	}
	else if(ret == 0)
	{
		//fileLog.writeLog("select time out\n");
		return 0;
	}
	else
	{
		//fileLog.writeLog("have data to read\n");
	}       

	memset(c_len,0,sizeof(c_len));
        ret = recv(fd, c_len, 4, 0);
	if( ret != 4)
	{
		fileLog.writeLog(( FormatString("recv rsp fail [%] [%] [%]\n").arg(ret).arg(c_len).arg(strerror(errno)).str() ));
		return -1;
	}

	memcpy(&length, c_len, 4);
	unsigned int h_length = ntohl(length);
	fileLog.writeLog(( FormatString("recv rspmsg length [%] \n").arg(h_length).str() ));
	if(h_length >= 1024*1024)
	{
		fileLog.writeLog(( FormatString("invalid msg length [%] \n").arg(h_length).str() ));
		return -1;
	}
	char * rsp_msg = new char [h_length+1];
	memset(rsp_msg,0,h_length+1);

	unsigned int i = 0;
	unsigned int j = h_length;
	while(1)
	{
		if(i == h_length)
			break;

		if((ret = select(fd+1,&ReadSet,NULL,NULL,&timeout)) == -1)
		{
			fileLog.writeLog(( FormatString("[%] select fail\n").arg(errno).str() ));
			delete [] rsp_msg; 
			return -1;
		}
		else if(ret == 0)
		{
			fileLog.writeLog("recv imsg body select time out\n");
			delete [] rsp_msg; 
			return -1;
		}
		else
		{
		//	fileLog.writeLog("recv msg have data to read\n");
		}

		ret = recv(fd, rsp_msg + i, j, 0);
		if(ret == 0)
		{
			fileLog.writeLog("peer close\n");
			delete [] rsp_msg;
			return -1;
		}

		j -= ret;
		i += ret;
	}

	int rsp_code;
	unsigned int describe;
	memcpy(&rsp_code, rsp_msg, 4);
	if (ntohl(rsp_code) == 4) {
		unsigned int content_len = 0;
		memcpy(&content_len, rsp_msg + 4, 4);
		content_len = ntohl(content_len);
		fileLog.writeLog((FormatString("this is a noitfy, size[%] \n").arg(content_len).str()));
		if (content_len > sizeof(int)) {
		 	//存在通知
			std::vector<rfc::_task_id> stopstaks;
			int task_num = 0;
			memcpy(&task_num, rsp_msg + 8, 4);
			task_num = ntohl(task_num);
			char* notify_msg = rsp_msg + 12;
			char* dot = NULL, *taskitem = NULL;
			int taskid_len = 0;
			_task_id taskid;
			fileLog.writeLog((FormatString("---recv stop notify:[%][%] des\n").arg(task_num).arg(rsp_msg + 12).str()));
			while (task_num-- > 0) {
				taskitem = notify_msg;
				taskid_len = strlen(taskitem);
				dot = strchr(taskitem, '.'); *dot = '\0';
				taskid.time = atoi(taskitem);
				taskitem = dot + 1;
				//
				dot = strchr(taskitem, '.'); *dot = '\0';
				taskid.m_id = atoi(taskitem);
				taskitem = dot + 1;
				//
				taskid.p_id = atoi(taskitem);
				stopstaks.push_back(taskid);
		 		notify_msg += (taskid_len + 1);
			}
			//
			if (!stopstaks.empty()) {
				handle_notify_msg(map_mail, stopstaks);
			}
		}
		//需要再次解析
		delete [] rsp_msg;
		return ParseRsp(rsp, fd, map_mail);
	}		
	else if(ntohl(rsp_code) == 2)
	{
		fileLog.writeLog("this is a check rsp\n");
		memcpy(&describe, rsp_msg+4, 4);
		rsp = rsp_msg+ntohl(describe)+24;
	}
	else if(ntohl(rsp_code) == 3)
	{
		fileLog.writeLog("this is a report rsp\n");	
	}
	else if(ntohl(rsp_code) == 1)
	{
		fileLog.writeLog(( FormatString("this is a mail list rsp [%]\n").arg(ntohl(rsp_code)).str()));
		ParseMailListRsp(rsp_msg, m_map_mail);
	}
	
	delete [] rsp_msg;
	return 0;	
}

int RequestMail::TemplateSession(struct Task_Id & taskid, int fd, stdstring & str_template, map<unsigned int, struct mail_node *> &map_mail)
{
	char req[64] = {};	
	char * p = req;

	unsigned int len = htonl(24);
	memcpy(p, &len, 4);

	InstallHead(p, COMMOND_CHECK_TEMPLATE_REPORT);
	p+=16;

	//任务ID
	unsigned int time;
	unsigned int m_id;
	unsigned int p_id;

	time = taskid.time;
	m_id = taskid.m_id;
	p_id = taskid.p_id;

	time = htonl(time);
	m_id = htonl(m_id);
	p_id = htonl(p_id);
	memcpy(p,&time,4);
	memcpy(p+4,&m_id,4);
	memcpy(p+8,&p_id,4);
	
	char test[64] = {};
	sprintf(test,"[%02x] [%02x] [%02x] [%02x]",req[0],req[1],req[2],req[3]);
	fileLog.writeLog(( FormatString("check template send %\n").arg(test).str() ));
	if(send(fd, req, 28, 0) != 28)
	{
		fileLog.writeLog(( FormatString("[%] [%] send mail request fail\n").arg(strerror(errno)).arg(errno).str() ));
		return -1;
	}

	int iret;
	if((iret=ParseRsp(str_template, fd, map_mail)) == -1)
	{
		fileLog.writeLog("check template fail\n");
		return -1;
	}
	else if(iret == 2)
	{
		fileLog.writeLog("rsp is not template , check template fail\n");
		return -1; 
	}
	if(strlen((char *)str_template.c_str()) == 0)
	{
		fileLog.writeLog((FormatString("template is null [%.%.%] [%]\n").arg(ntohl(time)).arg(ntohl(m_id)).arg(ntohl(p_id)).arg(str_template).str() ));
		return -1;
	}
	fileLog.writeLog(( FormatString("templatetemplate [%.%.%] is [%]\n").arg(ntohl(time)).arg(ntohl(m_id)).arg(ntohl(p_id)).arg(str_template).str() ));
	return 0;
}

int RequestMail::SendReport(map<unsigned int,struct mail_node *> & map_mail, int fd, vector<unsigned int> &vec)
{
	map<unsigned int,struct mail_node *>::iterator iter = map_mail.begin();
	//fileLog.writeLog("men beg\n");
	char * req = new char [1024*1024*8];
	memset(req,0,1024*1024*8);
	//fileLog.writeLog("mem end\n");

	InstallHead(req, COMMOND_RESULT_REPORT);	

	char * p = req;

	char * head = p;
	p += 20;

	unsigned int time;
	unsigned int m_id;
	unsigned int p_id;

	unsigned char stat;
	unsigned int maillen = 0;

	unsigned int suc = 0;
	unsigned int fai= 0;
	vec.clear();

	while(1)
	{
		if(iter == map_mail.end())
		{
			break;
		}
		else
		{
			time = iter->second->task_id.time;		
			m_id = iter->second->task_id.m_id;		
			p_id = iter->second->task_id.p_id;		

			time = htonl(time);
			m_id = htonl(m_id);
			p_id = htonl(p_id);

			if(iter->second->status == 1)
			{
				stat = 's';
				fileLog.writeLog(( FormatString("SendReport suc [%.%.%] [%] [%]\n").arg(iter->second->task_id.time).arg(iter->second->task_id.m_id).arg(iter->second->task_id.p_id).arg(iter->second->mail).arg(iter->first).str() ));
				suc++;
				vec.push_back(iter->first);
			}
			else if(iter->second->status == 5)			//硬弹回报告
			{
				fileLog.writeLog(( FormatString("SendReport hardfail [%.%.%] [%] [%]\n").arg(ntohl(time)).arg(ntohl(m_id)).arg(ntohl(p_id)).arg(iter->second->mail).arg(iter->first).str() ));
				stat = 'f';
				fai++;
				vec.push_back(iter->first);
			}
			else if(iter->second->status == 6)                      //删除报告
                        {   
                                fileLog.writeLog(( FormatString("SendReport del_task [%.%.%] [%] [%]\n").arg(ntohl(time)).arg(ntohl(m_id)).arg(ntohl(p_id)).arg(iter->second->mail).arg(iter->first).str() ));
                                stat = 'x';
                                fai++;
                                vec.push_back(iter->first);
                        }
			else if(iter->second->status == 4 && iter->second->failtimes == 3)			//软弹回重试3次后报告
			{
				fileLog.writeLog(( FormatString("SendReport softfail [%.%.%] [%] [%]\n").arg(ntohl(time)).arg(ntohl(m_id)).arg(ntohl(p_id)).arg(iter->second->mail).arg(iter->first).str() ));
				stat = 'd';
				fai++;
				vec.push_back(iter->first);
			}
			else
			{
				iter++;
				continue;
			}

			memcpy(p,&time,4);
			p += 4;

			memcpy(p,&m_id,4);
			p += 4;

			memcpy(p,&p_id,4);
			p += 4;

			memcpy(p,&stat,1);
			p++;

			maillen += strlen(iter->second->mail);
			memcpy(p,iter->second->mail,strlen(iter->second->mail));
			p += strlen(iter->second->mail);

			stat = '\0';
			memcpy(p,&stat,1);
			p++;
			memcpy(p,(char*)(iter->second->errstr.c_str()),strlen((char *)iter->second->errstr.c_str()));
			maillen += strlen((char *)iter->second->errstr.c_str());
			p += strlen((char *)iter->second->errstr.c_str());
			memcpy(p,&stat,1);

			p++;
			fileLog.writeLog(( FormatString("SendReport all [%] [%] [%]\n").arg(iter->first).arg(iter->second->mail).arg(iter->second->errstr).str() ));
		}
		iter++;
	}

	unsigned int mailnum = suc+fai;
	mailnum = htonl(mailnum);
	memcpy(head+16, &mailnum, 4);

	unsigned int totallen = maillen + 15*(suc+fai) + 16;
	totallen = htonl(totallen);
	memcpy(head,&totallen,4);	
	
	//fileLog.writeLog(( FormatString("SendReport suc1 [%] fai[%] all[%]\n").arg(suc).arg(fai).arg(suc+fai).str() ));

	int nSendBuf=500*1024;//设置为500K
	setsockopt(fd,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
	//分包发送
	unsigned int i,j,ret;
	i = (ntohl(totallen)+4)/1024;
	
	for(j=0;j<i;j++)
	{
		if((ret = send(fd, head+1024*j, 1024, 0)) != 1024 )
		{
			fileLog.writeLog(( FormatString("[%] [%] send report fail 1 ret [%] [%] [%]\n").arg(strerror(errno)).arg(errno).arg(ret).arg(ntohl(totallen)+4).arg(j).str() ));
			delete [] req;
			req = NULL;
			return -1;
		}
	}

	if((ret = send(fd, head + 1024*i, ntohl(totallen)+4-i*1024, 0)) != ntohl(totallen)+4-i*1024)
	{
		fileLog.writeLog(( FormatString("[%] [%] send report fail 2 ret [%]\n").arg(strerror(errno)).arg(errno).arg(ret).str() ));
		delete [] req;
		req = NULL;
		return -1;
	}

	delete [] req;
	req = NULL;
	string rsp;

	int iret;
	if((iret = ParseRsp(rsp,fd, map_mail)) == -1)
		return -1;

	fileLog.writeLog(( FormatString("SendReport info  suc[%] fai[%] all[%] mailnum[%] len[%] [%]\n").arg(suc).arg(fai).arg(suc+fai).arg(ntohl(mailnum)).arg(ntohl(totallen)).str() ));
	return 0;	 
}

int RequestMail::MailListReq(map<unsigned int,struct mail_node *> &map_mail,  map<stdstring,struct domain_node> &map_domain,  char * req, int fd)
{
	map<unsigned int,struct mail_node *>::iterator iter = map_mail.begin();
	char * tmp_head = req;
	map<stdstring,struct domain_node>::iterator it;
	double failrate;
	while(1)
	{
		if(iter == map_mail.end())
			break;

		if( (it=map_domain.find(iter->second->domain) ) != map_domain.end() && iter->second->status == 5)	//还未发送成功的失败次数累加,总数累加
		{
			map_domain[iter->second->domain].fail  += iter->second->failtimes;
			map_domain[iter->second->domain].total += iter->second->failtimes;

			map_domain[iter->second->domain].fail_m += iter->second->failtimes;
			map_domain[iter->second->domain].fail_q += iter->second->failtimes;
			map_domain[iter->second->domain].fail_h += iter->second->failtimes;

			map_domain[iter->second->domain].total_m += iter->second->failtimes;
			map_domain[iter->second->domain].total_q += iter->second->failtimes;
			map_domain[iter->second->domain].total_h += iter->second->failtimes;
			iter->second->failtimes = 0;
		}
		else if( it!= map_domain.end() && iter->second->status == 1)         //已经成功的失败次数累加，总数在失败次数基础上加1
		{
			map_domain[iter->second->domain].fail  += iter->second->failtimes;	
			map_domain[iter->second->domain].total += iter->second->failtimes+1;	

			map_domain[iter->second->domain].fail_m += iter->second->failtimes;
			map_domain[iter->second->domain].fail_q += iter->second->failtimes;
			map_domain[iter->second->domain].fail_h += iter->second->failtimes;

			map_domain[iter->second->domain].total_m += iter->second->failtimes+1;
			map_domain[iter->second->domain].total_q += iter->second->failtimes+1;
			map_domain[iter->second->domain].total_h += iter->second->failtimes+1;
			iter->second->failtimes = 0;
		}

		else if( it== map_domain.end() && iter->second->status == 5)  
		{
			map_domain["other"].fail  += iter->second->failtimes;
			map_domain["other"].total += iter->second->failtimes;

			map_domain["other"].fail_m += iter->second->failtimes;
			map_domain["other"].fail_q += iter->second->failtimes;
			map_domain["other"].fail_h += iter->second->failtimes;

			map_domain["other"].total_m += iter->second->failtimes; 
			map_domain["other"].total_q += iter->second->failtimes;
			map_domain["other"].total_h += iter->second->failtimes;
			iter->second->failtimes = 0;
		}

		else if( it== map_domain.end() && iter->second->status == 1)         
		{
			map_domain["other"].fail  += iter->second->failtimes;
			map_domain["other"].total += iter->second->failtimes+1;

			map_domain["other"].fail_m += iter->second->failtimes;
			map_domain["other"].fail_q += iter->second->failtimes;
			map_domain["other"].fail_h += iter->second->failtimes;

			map_domain["other"].total_m += iter->second->failtimes+1;
			map_domain["other"].total_q += iter->second->failtimes+1;
			map_domain["other"].total_h += iter->second->failtimes+1;
			iter->second->failtimes = 0;
		}
		iter++;
	}

	int req_lenth = 12;

	//遍历map_domain，计算能力值，组包发送
	it = map_domain.begin();
	InstallHead(req,COMMOND_ABILITY_REPORT);
	char * tmp = req + 16;
	req += 20;
	req_lenth += 4;
	char domain[32];
	int  domain_lenth=0;
	int num_domain_size = 0;
	for(;it != map_domain.end();it++)
	{
		//先指定当前维度的失败数和总数
		if(it->second.flag_cur == MUNITE)
		{
			it->second.fail_cur = it->second.fail_m;	
			it->second.total_cur = it->second.total_m;	
		}
		if(it->second.flag_cur == QUATOR)
		{
			it->second.fail_cur = it->second.fail_q;	
			it->second.total_cur = it->second.total_q;	
		}
		if(it->second.flag_cur == HOUR)
		{
			it->second.fail_cur = it->second.fail_h;	
			it->second.total_cur = it->second.total_h;	
		}
		if(it->second.flag_cur == DAY)
		{
			it->second.fail_cur = it->second.fail;	
			it->second.total_cur = it->second.total;	
		}

		//根据当前值计算能力
		if(it->second.fail_cur == 0 || it->second.total_cur == 0)
		{
			failrate = 0;	
		}
		else
		{
			failrate = (double)it->second.fail_cur/(double)it->second.total_cur;
		}

		if(it->second.total_cur != 0 && failrate >= 0.7)    //失败率>0.7的domain，能力值更新为0。
		{
			it->second.ability = 0;	
		}
		else if(it->second.total_cur != 0 && failrate > 0.3 && failrate < 0.7)  //
		{
			it->second.ability = it->second.oriability / 2;
		}
		else if(it->second.total_cur != 0 && failrate <= 0.3)    //失败率<0.3的能力值为原始值
		{
			it->second.ability = it->second.oriability;	
		}
		else if(it->second.total_cur == 0)	//上轮没有发送，本轮能力值恢复到原始
		{
			it->second.ability = it->second.oriability;
		}
	

		memset(domain,0,sizeof(domain));
		sprintf(domain,"domain=%s&cnt=%d", it->first.c_str(),it->second.ability);
		if (failrate > 0 || it->second.fail_cur > 0 || it->second.total_cur) {
		fileLog.writeLog(( FormatString("domain [%] failrate [%] fail_cur [%] total_cur [%] flag_cur [%]\n").arg(domain).arg(failrate).arg(it->second.fail_cur).arg(it->second.total_cur).arg(it->second.flag_cur).str() ));
		}
		num_domain_size = strlen(domain);
		strncpy(req,domain, num_domain_size);
		req += num_domain_size;
		req += 1;
		req_lenth += num_domain_size;
		req_lenth += 1;
		if (req_lenth > (REQ_MSG_MAX_LEN - 4)) {
			req -= (num_domain_size + 1);
			req_lenth -= (num_domain_size + 1);
			fileLog.writeLog("request msg too big\n");
			break;
		}
		domain_lenth++;
	}

	int net_domain_lenth = htonl(domain_lenth);
	int net_req_lenth	 = htonl(req_lenth);
	memcpy(tmp,&net_domain_lenth,4);
	memcpy(tmp_head,&net_req_lenth,4);

	if(send(fd, tmp_head, req_lenth+4, 0) != req_lenth+4)
	{
		fileLog.writeLog(( FormatString("[%] [%] send mail request fail\n").arg(strerror(errno)).arg(errno).str() ));
		return -1;
	}

	fileLog.writeLog(( FormatString("[%] request mail ok\n").arg(req_lenth+4).str() ));
	return 0;	
}


RFC_NAMESPACE_END
