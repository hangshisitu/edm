/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan				Date: 2011-08      */

#include "parserequest.h"
#include <errno.h>
#include <math.h>
#include "ip_area.h"





RFC_NAMESPACE_BEGIN
extern FileLog fileLog;
//extern LISTTASK list_task;
//extern LISTURL  list_url;
extern pthread_mutex_t  mutex_task;
extern pthread_mutex_t  mutex_list;
extern list<struct tinfo_node *> list_task;
extern list<struct url_node *> list_url;
extern list<struct domain_node *> list_task_domain;
//IP_AREA_OBJ * obj;
extern vector<_ipline *> vec_ip;

extern string lbsvr_host;
extern int lbsvr_port;

const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
static char find_pos(char ch)
{
	char *ptr = (char*)strrchr(base, ch);
	//the last position (the only) in base[]
	return (ptr - base);
}

char *base64_decode(const char *data, int data_len)
{
	int ret_len = (data_len / 4) * 3;
	int equal_count = 0;
	char *ret = NULL;
	char *f = NULL;
	int tmp = 0;
	int temp = 0;
	char need[3];
	int prepare = 0;
	int i = 0;
	if (*(data + data_len - 1) == '=')
	{
		equal_count += 1;
	}
	if (*(data + data_len - 2) == '=')
	{
		equal_count += 1;
	}
	if (*(data + data_len - 3) == '=')
	{
		//seems impossible
		equal_count += 1;
	}
	switch (equal_count)
	{
		case 0:
			ret_len += 4;//3 + 1 [1 for NULL]
			break;
		case 1:
			ret_len += 4;//Ceil((6*3)/8)+1
			break;
		case 2: 
			ret_len += 3;//Ceil((6*2)/8)+1
			break;
		case 3: 
			ret_len += 2;//Ceil((6*1)/8)+1
			break;
	}    
	ret = (char *)malloc(ret_len);
	if (ret == NULL)
	{
		printf("No enough memory.\n");
		exit(0);
	}    

	memset(ret, 0, ret_len);
	f = ret;    
	while (tmp < (data_len - equal_count))
	{
		temp = 0;
		prepare = 0;
		memset(need, 0, 4);
		while (temp < 4)
		{
			if (tmp >= (data_len - equal_count))
			{
				break;
			}
			prepare = (prepare << 6) | (find_pos(data[tmp]));
			temp++;
			tmp++;
		}
		prepare = prepare << ((4-temp) * 6);
		for (i=0; i<3 ;i++ )
		{
			if (i == temp)
			{
				break;
			}
			*f = (char)((prepare>>((2-i)*8)) & 0xFF);
			f++;
		}    
	}    
	*f = '\0';    
	return ret;
}


ParseRequest::ParseRequest()
{
	m_iter_ipcache = 0;
	m_map_ip.clear();
}

ParseRequest::~ParseRequest()
{

}

void ParseRequest::StrReplace(string & dest, char *src, const char *oldstr, const char *newstr, int len)
{
	if(strcmp(oldstr, newstr)==0)
	{
		dest = src;
		return;
	}

	char *needle;
	char *tmp;
	char *tmp1;
	dest = src;

	while((needle = (char*)strstr(dest.c_str(), oldstr)) != NULL)
	{
		tmp1 = (char *)dest.c_str();
		tmp=(char*)malloc(strlen(tmp1)+(strlen(newstr)-strlen(oldstr))+1);
		strncpy(tmp, (char *)dest.c_str(), needle-tmp1);
		tmp[needle-tmp1]='\0';

		strcat(tmp, newstr);
		strcat(tmp, needle+strlen(oldstr));

		dest = tmp;
		free(tmp);
	}

	return ;
}

int temp_insert_labels(const char* labels, const char* email, char action)
{
    static FILE* output = NULL;
    static time_t point = time(0);
    static string oldpath = "./data/iqzero";
    static char newpath[256];

    time_t now = time(0);

    if (now-point > 60*30){ 

        if (output){
            fclose(output);
            output = NULL;
        }

        point = now;
        snprintf(newpath, sizeof(newpath), "%s-%u.o", oldpath.c_str(), (uint32_t)now);
        rename(oldpath.c_str(), newpath);
    }

    if (!output)
        output = fopen(oldpath.c_str(), "a");

    if (output)
        fprintf(output,"%s|%s|%c\n", email, labels, action);

    return 0;
}

int insert_label_inf(const char* labels, const char* email, char action)
{
    char buffer[512];
    int bytes = snprintf(buffer, sizeof(buffer), 
        "GET /insert?label=%s&email=%s&action=%c HTTP/1.1\r\n\r\n",
        labels, email, action);

    fileLog.writeLog(( FormatString("insert label [% bytes][%]\n").arg(bytes).arg(buffer).str() ));

    sockaddr_in peer;
    bzero(&peer, sizeof(sockaddr_in));
    peer.sin_family = PF_INET;
    peer.sin_port = htons(lbsvr_port);
    peer.sin_addr.s_addr = inet_addr(lbsvr_host.c_str());

    int fd = -1;
    if((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        fileLog.writeLog(( FormatString("socket error[%][%]\n").arg(strerror(errno)).arg(errno).str() ));
        return -1;
    }

    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    ::connect(fd,(sockaddr*)&peer, sizeof(sockaddr_in));

    fd_set wrds;
    FD_ZERO(&wrds);
    FD_SET(fd,&wrds);
    timeval timeout = {3,0};

    if ( select(fd+1, NULL, &wrds, NULL,&timeout) < 1){
        fileLog.writeLog(( FormatString("connect error[%][%]\n").arg(strerror(errno)).arg(errno).str() ));
        close(fd);
        return -1;
    }

    if(send(fd, buffer, bytes, 0) != bytes)
    {
        fileLog.writeLog(( FormatString("[%] [%] send Label request fail\n").arg(strerror(errno)).arg(errno).str() ));
        close(fd);
        return -1;
    }

    close(fd);
    fileLog.writeLog("sendto LabelServer ok\n");
    return 0;
}


int ParseRequest::ParseMessage(char * req, 
                               map<unsigned int,struct tinfo_node *> & tinfo_map, 
                               map<unsigned int,struct url_node *> & url_map,
                               const char * ip,
                               const char * domain)
{
	rfc_uint_32 nCmd;
	memcpy(&nCmd,req,4);
	nCmd = ntohl(nCmd);

	unsigned short int num;	
	int i= 0;

	unsigned int     template_id;       //模板ID
	unsigned int     corp_id;           //企业ID
	unsigned int     task_id;           //任务ID	
	static unsigned int seq_no = 0;
	int tmp;
	unsigned int suc_send;      
	unsigned int total;         
	unsigned int send;          
	unsigned char send_flag;			//通过任务结束时间
	unsigned int none_exist_count = 0; 
	unsigned int none_exist_count1 = 0;
	unsigned int dns_fail_count = 0;  
	unsigned int spam_garbage_count = 0;
	unsigned int hard_bounce_count = 0;
	unsigned int soft_bounce_count = 0;

	char r_ip[64] = {0};
	char area[100] = {0};
	fileLog.writeLog((FormatString("COMMOND [%]\n").arg(nCmd).str()));

    //调度机向统计模块发送任务发送情况，统计模块返回确认
    /*
    | 4 bytes | 8 bytes |   2 bytes   | 12 bytes | ---- 4 bytes ---- | ----- 4 bytes ----- |-- 4 bytes --| ----- 4 bytes ---- | ---- 4 bytes ---- | - 1 bytes - |
    | Command |  Fill   | Task Number | Task ID | Total Send Number | Succeed Send Number | Send Number | Hard Failed Number | Soft Failed Number| Task Status |
    */
	if(nCmd == COMMOND_REQ_TASK || nCmd == COMMOND_REQ_TASK_MORE)
	{
		memcpy(&num, req+12, 2);
		num = ntohs(num);

		fileLog.writeLog((FormatString("this is a COMMOND_REQ_TASK req tasknum [%]\n").arg(num).str()));
		req += 14;
		while(i<num)
		{
			m_ctype = COMMOND_REQ_TASK;
		//	if(seq_no++ == 50000000) seq_no = 0;
			memcpy(&corp_id,req,4);
			corp_id = ntohl(corp_id);

			memcpy(&task_id,req+4,4);
			task_id = ntohl(task_id);

			memcpy(&template_id,req+8,4);
			template_id = ntohl(template_id);

			memcpy(&total,req+12,4);
			memcpy(&suc_send,req+16,4);
			memcpy(&send,req+20,4);

			if(nCmd == COMMOND_REQ_TASK_MORE)
			{
				memcpy(&hard_bounce_count,req+24,4);
				memcpy(&soft_bounce_count,req+28,4);
				memcpy(&none_exist_count,req+32,4);
				memcpy(&none_exist_count1,req+36,4);
				memcpy(&dns_fail_count,req+40,4);
				memcpy(&spam_garbage_count,req+44,4);
				memcpy(&send_flag,req+48,1);

				none_exist_count = ntohl(none_exist_count);
				none_exist_count1 = ntohl(none_exist_count1);
				dns_fail_count = ntohl(dns_fail_count);
				spam_garbage_count = ntohl(spam_garbage_count);
				hard_bounce_count = ntohl(hard_bounce_count);
				soft_bounce_count = ntohl(soft_bounce_count);
			}
			else
			{
				memcpy(&send_flag,req+24,1);
			}

			total=ntohl(total);
			suc_send=ntohl(suc_send);
			send=ntohl(send);

			_tinfo_node * node = new tinfo_node;
			node->template_id = template_id;
			node->corp_id = corp_id;
			node->task_id = task_id;

			node->suc_send = suc_send;
			node->total = total;
			node->send = send;
			node->send_flag = send_flag;

			tmp = send_flag;

			node->none_exist_count = none_exist_count;
			node->none_exist_count1 = none_exist_count1;
			node->dns_fail_count = dns_fail_count;
			node->spam_garbage_count = spam_garbage_count;
			node->hard_bounce_count = hard_bounce_count;
			node->soft_bounce_count = soft_bounce_count;

			if(pthread_mutex_lock(&mutex_task) != 0)
			{
				fileLog.writeLog((FormatString("mutex_task lock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
			}

			list_task.push_back (node);

			if(pthread_mutex_unlock(&mutex_task) != 0)
			{
				fileLog.writeLog((FormatString("mutex_task unlock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
			}
			if(nCmd == COMMOND_REQ_TASK_MORE) req += 49;
			else req += 25;
			fileLog.writeLog(( FormatString("req seqno[%] template_id [%] corp_id [%] task_id [%] suc_send [%] total [%] send [%] none_exist_count [%] none_exist_count1 [%] dns_fail_count [%] spam_garbage_count [%] hard_bounce_count [%] soft_bounce_count [%] send_flag [%]\n").arg(seq_no).arg(template_id).arg(corp_id).arg(task_id).arg(suc_send).arg(total).arg(send).arg(node->none_exist_count).arg(node->none_exist_count1).arg(node->dns_fail_count).arg(node->spam_garbage_count).arg(node->hard_bounce_count).arg(node->soft_bounce_count).arg(tmp).str() ));
			i++;
		}
	}

    //跳转模块向统计模块发送用户行为url，统计模块返回确认
    /*
    | 4 bytes | 8 bytes |  2 bytes   | ...                    |
    | Command |  Fill   | URL Number | url1+ url2 + url3 ...  |
    */
	else if(nCmd == COMMOND_REQ_URL)
	{
		time_t timep;
		time(&timep);
		struct tm * lt = localtime(&timep);
		

		fileLog.writeLog("this is a COMMOND_REQ_URL \n");
		req += 4;
		m_ctype = COMMOND_REQ_URL;
		if(seq_no++ == 50000000) seq_no = 0;

		char * begin=NULL;
		char * end =NULL;
		char email[128];
		char ip[128];
		if((begin=strstr(req,"&e=")) != NULL)         //email参数
		{
			begin +=  3;
			end = begin;
			while(1)
			{
				if(*(end) == '\0' || *(end) == '&')
					break;
				end++;
			}

			snprintf(email,(end - begin)+1,"%s",begin);
		}
		if((begin=strstr(req,"&ip=")) != NULL)         //ip参数
		{
			begin +=  4;
			end = begin;
			while(1)
			{
				if(*(end) == '\0' || *(end) == '&')
					break;
				end++;
			}

			snprintf(ip,(end - begin)+1,"%s",begin);
		}

		string email_decode;
		if(strchr(email,'@') == NULL)	//需要base64解密
		{
			char * p_email = base64_decode(email,strlen(email));	
			StrReplace(email_decode, req, email, p_email, 1);	
			memset(req,0,strlen(req));
			strcpy(req, (char *)email_decode.c_str());
			snprintf(email,sizeof(email),"%s",p_email);
			free(p_email);
		}
		//查找IP缓存，未打开过IP进行过滤

        bool isQQ = strcasestr(email, "@qq.com") 
            || strcasestr(email, "@vip.qq.com") 
            || strcasestr(email, "@foxmail.com");

		if( isQQ && m_map_ip.find(email) != m_map_ip.end())				
		{
			if(strcmp((char*)m_map_ip[email].c_str(),ip) != 0)			//缓存里有但IP不一致的过滤掉
			{
				fileLog.writeLog((FormatString("click ip error email[%] ip[%]\n").arg(email).arg(ip).str()));
				return -1;
			}
		}
		if(strchr(email,'@') == NULL)	//无@的乱码记录不需要入库
		{
			fileLog.writeLog((FormatString("dirty email[%]\n").arg(email).str()));
			return -1;
		}
		fileLog.writeLog((FormatString("req [%] \n").arg(req).str()));

		//HttpMsg2DayList(req,domain);

        //////////////////////////////////////////////////////////////////////////
        char labels[128];
        char lb_action = '0';
        if((begin=strstr(req,"&la=")) != NULL)         //label参数
        {
            begin +=  4;
            end = begin;
            while(1){
                if(*(end) == '\0' || *(end) == '&')
                    break;
                end++;
            }

            snprintf(labels,(end - begin)+1,"%s",begin);
            fileLog.writeLog((FormatString("Labels [%] \n").arg(labels).str()));
        }
        //////////////////////////////////////////////////////////////////////////

		char * test2 = strstr(req,"test2.php");
		char taskid[64];
		char templateid[64];
		char corpid[64];
		char url[256];
		char sbflag[2];
		char osid[2];
		char browserid[2];
		char slangid[2];

		bool flag_e = false;
		bool flag_c = false;
		bool flag_te = false;
		bool flag_ta = false;
		bool flag_u = false;
		bool flag_i = false;
		bool flag_f = false;
		bool flag_bf = false;
		bool flag_of = false;
		bool flag_lf = false;
		if(test2 != NULL)			//带php为邮件打开标志
		{
			/*
			if((begin=strstr(test2,"139edm.com/logo.gif")) == NULL)
			{
				fileLog.writeLog("not open mail url!\n");
				return 1;
			}
			*/

			if((begin=strstr(test2,"&e=")) != NULL)			//email参数
			{
				flag_e = true;
				begin +=  3;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(email,(end - begin)+1,"%s",begin);
				//				fileLog.writeLog((FormatString("param email [%]         ").arg(email).str()));
			}
			if((begin=strstr(test2,"&c=")) != NULL)			//企业id
			{
				flag_c = true;
				begin +=  3;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(corpid,(end - begin)+1,"%s",begin);
				//				fileLog.writeLog((FormatString("param corp_id [%]          ").arg(corpid).str()));
			}
			if((begin=strstr(test2,"&te=")) != NULL)		//模板id
			{
				flag_te = true;
				begin +=  4;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(templateid,(end - begin)+1,"%s",begin);
				//				fileLog.writeLog((FormatString("param template_id [%]       ").arg(templateid).str()));
			}
			if((begin=strstr(test2,"&ta=")) != NULL)		//任务id
			{
				flag_ta = true;
				begin +=  4;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(taskid,(end - begin)+1,"%s",begin);
				//				fileLog.writeLog((FormatString("param task_id [%]        ").arg(taskid).str()));
			}
			if((begin=strstr(test2,"&ip=")) != NULL)        //ip
			{
				flag_i = true;
				begin +=  4;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(r_ip,(end - begin)+1,"%s",begin);
				//			ipv4_area_search(obj,r_ip,area);	
				if (find_ip(vec_ip, r_ip, area)) {
					fileLog.writeLog((FormatString("ip[%] area[%]\n").arg(r_ip).arg(area).str()));
				}
				else {
					fileLog.writeLog((FormatString("not found, ip_db_size[%] ip[%] area[%]\n").arg(vec_ip.size()).arg(r_ip).arg(area).str()));
				}
			}
			if((begin=strstr(test2,"&oid=")) != NULL)        //标记，客户端操作系统id
			{
				flag_of = true;
				begin +=  5;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(osid,(end - begin)+1,"%s",begin);
				//      fileLog.writeLog((FormatString("ip[%] area[%]\n").arg(r_ip).arg(area).str()));
			}
			if((begin=strstr(test2,"&lid=")) != NULL)        //标记，客户端语言id
			{
				flag_lf = true;
				begin +=  5;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(slangid,(end - begin)+1,"%s",begin);
				//      fileLog.writeLog((FormatString("ip[%] area[%]\n").arg(r_ip).arg(area).str()));
			}
			if((begin=strstr(test2,"&bid=")) != NULL)        //标记，客户端浏览器id
			{
				flag_bf = true;
				begin +=  5;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(browserid,(end - begin)+1,"%s",begin);
				//      fileLog.writeLog((FormatString("ip[%] area[%]\n").arg(r_ip).arg(area).str()));
			}

			if(flag_c && flag_e && flag_ta && flag_te && flag_i && flag_bf && flag_lf && flag_of)
			{
				if(time(NULL) - m_iter_ipcache > 15*60)                //清除IP地址缓存，key：email value: ip
				{
					fileLog.writeLog("clear m_map_ip\n");
					m_map_ip.clear();
					m_iter_ipcache = time(NULL);
				}

                lb_action = '1'; //
				m_map_ip[email] = r_ip;
				fileLog.writeLog((FormatString("open param seq_no [%] email [%] corp_id [%] template_id[%] task_id[%] ip[%] area[%]\n").arg(seq_no).arg(email).arg(corpid).arg(templateid).arg(taskid).arg(r_ip).arg(area).str()));
				_url_node * node = new _url_node;
				snprintf(node->mail,sizeof(node->mail)+1,"%s",email);
				snprintf(node->area,sizeof(node->area)+1,"%s",area);
				node->corp_id = atoi(corpid);
				node->template_id = atoi(templateid);
				node->hour = lt->tm_hour;
				node->task_id = atoi(taskid);
				node->tag = 2;
				node->seq_no = seq_no;
				node->osid	 = atoi(osid);
				node->browserid= atoi(browserid);
				node->slangid= atoi(slangid);
				if(pthread_mutex_lock(&mutex_task) != 0)
				{
					fileLog.writeLog((FormatString("mutex_list lock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
				}

				list_url.push_back(node);	

				if(pthread_mutex_unlock(&mutex_task) != 0)
				{
					fileLog.writeLog((FormatString("mutex_list lock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
				}
			}
			else
			{
				fileLog.writeLog("the param is invalid , can't to update db\n");  
			}
		}
		else
		{
			char * test1=(char *)req;
			char * p = test1;
			while(1)
			{
				if(*(p) == '&' && *(p+1) == '&')
					break;
				p++;
			}
			*p='\0';
			test2 = p+2;

			if((begin=strstr(test1,"url=")) != NULL)         //url
			{
				flag_u = true;
				begin +=  4;
				end = begin;
				while(1) 
				{
					if(*(end) == '\0')
						break;
					end++;
				}   

				snprintf(url,(end - begin)+1,"%s",begin);
				fileLog.writeLog((FormatString("param url[%]\n").arg(url).str()));
			}
			if((begin=strstr(test2,"&e=")) != NULL)         //email参数
			{
				flag_e = true;
				begin +=  3;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(email,(end - begin)+1,"%s",begin);
				//              fileLog.writeLog((FormatString("param email [%]         ").arg(email).str()));
			}
			if((begin=strstr(test2,"&c=")) != NULL)         //企业id
			{
				flag_c = true;
				begin +=  3;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(corpid,(end - begin)+1,"%s",begin);
				//              fileLog.writeLog((FormatString("param corp_id [%]          ").arg(corpid).str()));
			}
			if((begin=strstr(test2,"&te=")) != NULL)        //模板id
			{
				flag_te = true;
				begin +=  4;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(templateid,(end - begin)+1,"%s",begin);
			}
			if((begin=strstr(test2,"&ta=")) != NULL)        //任务id
			{
				flag_ta = true;
				begin +=  4;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(taskid,(end - begin)+1,"%s",begin);
				//              fileLog.writeLog((FormatString("param task_id [%]        ").arg(taskid).str()));
			}
			if((begin=strstr(test2,"&ip=")) != NULL)        //ip
			{
				flag_i = true;
				begin +=  4;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(r_ip,(end - begin)+1,"%s",begin);
				//				ipv4_area_search(obj,r_ip,area);
				if (find_ip(vec_ip, r_ip, area)) {
					fileLog.writeLog((FormatString("ip[%] area[%]\n").arg(r_ip).arg(area).str()));
				}
				else {
					fileLog.writeLog((FormatString("not found, ip_db_size[%] ip[%] area[%]\n").arg(vec_ip.size()).arg(r_ip).arg(area).str()));
				}
			}
			if((begin=strstr(test2,"&f=")) != NULL)        //标记,可选,u退订，c投诉
			{
				flag_f = true;
				begin +=  3;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(sbflag,(end - begin)+1,"%s",begin);
				//		fileLog.writeLog((FormatString("ip[%] area[%]\n").arg(r_ip).arg(area).str()));
			}
			if((begin=strstr(test2,"&oid=")) != NULL)        //标记，客户端操作系统id
			{
				flag_of = true;
				begin +=  5;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(osid,(end - begin)+1,"%s",begin);
				//      fileLog.writeLog((FormatString("ip[%] area[%]\n").arg(r_ip).arg(area).str()));
			}
			if((begin=strstr(test2,"&lid=")) != NULL)        //标记，客户端语言id
			{
				flag_lf = true;
				begin +=  5;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(slangid,(end - begin)+1,"%s",begin);
				//      fileLog.writeLog((FormatString("ip[%] area[%]\n").arg(r_ip).arg(area).str()));
			}
			if((begin=strstr(test2,"&bid=")) != NULL)        //标记，客户端浏览器id
			{
				flag_bf = true;
				begin +=  5;
				end = begin;
				while(1)
				{
					if(*(end) == '\0' || *(end) == '&')
						break;
					end++;
				}

				snprintf(browserid,(end - begin)+1,"%s",begin);
				//      fileLog.writeLog((FormatString("ip[%] area[%]\n").arg(r_ip).arg(area).str()));
			}


			if(!flag_f && (flag_c && flag_e && flag_ta && flag_te && flag_u && flag_i && flag_bf && flag_lf && flag_of))
			{
                lb_action = '2';
				fileLog.writeLog((FormatString("click seq_no [%] param email [%] corp_id [%] template_id[%] task_id[%] ip[%] area[%]\n").arg(seq_no).arg(email).arg(corpid).arg(templateid).arg(taskid).arg(r_ip).arg(area).str()));
				_url_node * node = new _url_node;
				snprintf(node->mail,sizeof(node->mail)+1,"%s",email);
				snprintf(node->area,sizeof(node->area)+1,"%s",area);
				snprintf(node->url,sizeof(node->url)+1,"%s",url);
				node->corp_id = atoi(corpid);
				node->template_id = atoi(templateid);
				node->task_id = atoi(taskid);
				node->hour = lt->tm_hour;
				node->tag = 1;
				node->seq_no = seq_no;
				node->osid   = atoi(osid);
				node->browserid= atoi(browserid);
				node->slangid= atoi(slangid);
				if(pthread_mutex_lock(&mutex_task) != 0)
				{
					fileLog.writeLog((FormatString("mutex_list lock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
				}

				list_url.push_back(node);

				if(pthread_mutex_unlock(&mutex_task) != 0)
				{
					fileLog.writeLog((FormatString("mutex_list lock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
				}
			}
			else if(flag_f && (flag_c && flag_e && flag_u && flag_i && flag_te && flag_ta && flag_bf && flag_lf && flag_of))
			{
                lb_action = '2';
				fileLog.writeLog((FormatString("click seq_no [%] param email [%] corp_id [%] template_id[%] task_id[%] flag[%] ip[%] area[%]\n").arg(seq_no).arg(email).arg(corpid).arg(templateid).arg(taskid).arg(sbflag).arg(r_ip).arg(area).str()));
				_url_node * node = new _url_node;
				snprintf(node->mail,sizeof(node->mail)+1,"%s",email);
				snprintf(node->area,sizeof(node->area)+1,"%s",area);
				snprintf(node->url,sizeof(node->url)+1,"%s",url);
				node->corp_id = atoi(corpid);
				node->template_id = atoi(templateid);
				node->task_id = atoi(taskid);
				if(sbflag[0] == 'u')
					node->tag = 3;
				if(sbflag[0] == 'c')
					node->tag = 4;
				if(sbflag[0] == 'f')
					node->tag = 5;
				node->seq_no = seq_no;
				node->osid   = atoi(osid);
				node->browserid= atoi(browserid);
				node->slangid= atoi(slangid);
				if(pthread_mutex_lock(&mutex_task) != 0)
				{
					fileLog.writeLog((FormatString("mutex_list lock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
				}

				list_url.push_back(node);

				if(pthread_mutex_unlock(&mutex_task) != 0)
				{
					fileLog.writeLog((FormatString("mutex_list lock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
				}
			}
			else
			{
				fileLog.writeLog("the click msg param is invalid , can't to update db\n");
			}
		}

        if (lb_action == '1' || lb_action == '2')
        {
            insert_label_inf(labels, email, lb_action);
        }
	}

    //调度机向统计模块发送任务的各域发送情况
    /*
    | 4 bytes | -- 4 bytes -- | 12 bytes| --4 bytes--| --4 bytes--| 4 bytes | --4 bytes--| --4 bytes--| v bytes | 
    | Command | Domain Number | Task ID | 接收人总数 | 已发送数量 | 到达数量| 软弹出数量 | 硬弹回数量 | 域名    |
    */
	else if (nCmd == COMMOND_REQ_DOMAIN || nCmd == COMMOND_REQ_NEWDOMAIN || nCmd == COMMOND_REQ_DOMAIN_MORE)				//任务域名情况
	{
		rfc_uint_32 domain_num;
		memcpy(&domain_num,req+4,4);
		domain_num = ntohl(domain_num);

		memcpy(&corp_id,req+8,4);
		corp_id = ntohl(corp_id);

		memcpy(&task_id,req+12,4);
		task_id = ntohl(task_id);

		memcpy(&template_id,req+16,4);
		template_id = ntohl(template_id);


		unsigned int i = 0;
		char * p = (char *)(req + 20);
		for(;i<domain_num;i++)
		{
			_tdomain_node * node = new _tdomain_node;
			node->none_exist_count=0;
			node->none_exist_count1=0;
			node->dns_fail_count=0;
			node->spam_garbage_count=0;

			node->corp_id = corp_id;
			node->task_id = task_id;
			node->template_id = template_id;

			memcpy(&node->total,p,4);
			node->total = ntohl(node->total);

			p += 4;
			memcpy(&node->send,p,4);
			node->send = ntohl(node->send);

			p += 4;
			memcpy(&node->suc_send,p,4);
			node->suc_send = ntohl(node->suc_send);

			p += 4;
			memcpy(&node->soft_bounce_count,p,4);
			node->soft_bounce_count = ntohl(node->soft_bounce_count);

			p += 4;
			memcpy(&node->hard_bounce_count,p,4);
			node->hard_bounce_count = ntohl(node->hard_bounce_count);

			if(nCmd == COMMOND_REQ_NEWDOMAIN)
			{
				p += 4;
				memcpy(&node->none_exist_count,p,4);
				node->none_exist_count = ntohl(node->none_exist_count);
			}
			else if(nCmd == COMMOND_REQ_DOMAIN_MORE)
			{
				p += 4;
				memcpy(&node->none_exist_count,p,4);
				node->none_exist_count = ntohl(node->none_exist_count);

				p += 4;
				memcpy(&node->none_exist_count1,p,4);
				node->none_exist_count1 = ntohl(node->none_exist_count1);

				p += 4;
				memcpy(&node->dns_fail_count,p,4);
				node->dns_fail_count= ntohl(node->dns_fail_count);

				p += 4;
				memcpy(&node->spam_garbage_count,p,4);
				node->spam_garbage_count = ntohl(node->spam_garbage_count);
			}

			p += 4;
			strcpy(node->domain,p);
			p += (strlen(node->domain) + 1);

			if(node->total == 0)        //过滤为总量为0的
			{
				fileLog.writeLog(( FormatString("req task domain template_id [%] corp_id [%] task_id [%] domain[%] suc_send equal 0\n").arg(template_id).arg(corp_id).arg(task_id).arg(node->domain).str() ));
				continue;
			}

			if(pthread_mutex_lock(&mutex_task) != 0)
			{
				fileLog.writeLog((FormatString("mutex_task lock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
			}

			list_task_domain.push_back (node);
			fileLog.writeLog(( FormatString("req task domain template_id [%] corp_id [%] task_id [%] domain[%] suc_send [%] total [%] send [%] hard_bounce_count [%] soft_bounce_count [%] none_exist_count [%] none_exist_count1 [%] dns_fail_count [%] spam_garbage_count [%]\n").arg(template_id).arg(corp_id).arg(task_id).arg(node->domain).arg(node->suc_send).arg(node->total).arg(node->send).arg(node->hard_bounce_count).arg(node->soft_bounce_count).arg(node->none_exist_count).arg(node->none_exist_count1).arg(node->dns_fail_count).arg(node->spam_garbage_count).str() ));

			if(pthread_mutex_unlock(&mutex_task) != 0)
			{
				fileLog.writeLog((FormatString("mutex_task unlock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
			}
		}

	}

	return 0;
}

int ParseRequest::HttpMsg2DayList(const char * req,const char * domain)
{
	char tmp[32];
	char msg[1024];
	snprintf(msg,sizeof(msg),"%s","POST /daylist1.php HTTP/1.1\n");
	snprintf(tmp,sizeof(tmp),"Host: %s\n", domain);
	strcat(msg,tmp);
	strcat(msg,"Referer: \n");
	strcat(msg,"Content-type: application/x-www-form-urlencoded\n");

	snprintf(tmp,sizeof(tmp),"Content-length: %ld\n", strlen(req));
	strcat(msg,tmp);
	strcat(msg,"Connection: close\n\n");
	strcat(msg,req);
	strcat(msg,"\n");

	int ret;
	struct sockaddr_in addr = {0};
	struct in_addr x;
	inet_aton(domain,&x);
	addr.sin_family = AF_INET;
	addr.sin_addr = x;
	addr.sin_port = htons((short )80);

	int fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd == -1)
	{
		printf("error:%s\n",strerror(errno));
		return fd;
	}

//	fileLog.writeLog(( FormatString("debug 1 [%] [%]\n").arg(domain).arg(msg).str() ));
	while(1)
	{
		ret = connect(fd,(sockaddr*)&addr,sizeof(addr));
		if(ret != 0)
		{
			fileLog.writeLog(( FormatString("connect fail error [%] str [%]\n").arg(strerror(errno)).arg(errno).str() ));
			sleep(3);
			continue;
		}
		if(ret == 0)
		{
			fileLog.writeLog("connect nginx ok\n");
			break;
		}
	}

	ret = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, ret | O_NONBLOCK);

	if(send(fd, msg, strlen(msg), 0) != (int)strlen(msg))
	{
		fileLog.writeLog(( FormatString("[%] [%] send mail request fail\n").arg(strerror(errno)).arg(errno).str() ));
		close(fd);
		return -1;
	}

	close(fd);
//	fileLog.writeLog(( FormatString("send mail request fail [%]\n").arg(msg).str() ));
	fileLog.writeLog("sendto 21nginx mail ok\n");

	return 0;
}

int ParseRequest::Response(int fd)
{
	char res[13];
	memset(res,0,13);
	char * p = res;
	unsigned int length=8;
	length = htonl(length);
	memcpy(p,&length,4);
	unsigned int ncmd;

	if(m_ctype == COMMOND_REQ_TASK)
	{
		ncmd= htonl(4);
	}
	else if(m_ctype == COMMOND_REQ_DOMAIN) 
	{
		ncmd= htonl(6);
	}
	else
	{
		return 0;
	}

	memcpy(p+4,&ncmd,4);
	if(send(fd, res, 12, 0) != 12)
	{
		fileLog.writeLog(( FormatString("[%] [%] send response fail\n").arg(strerror(errno)).arg(errno).str() ));
		return -1;
	}	

	fileLog.writeLog("response ok\n");
	return 0;
}

RFC_NAMESPACE_END
