/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan Date: 2011-11-11  光棍节*/

#ifndef	APP_PARSEREQUEST_H_201108
#define APP_PARSEREQUEST_H_201108

#include "application/application.h"
#include "application/configuremgr.h"
#include "system/filehandle.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "application/filelog.h"
#include "cachedb.h"

using namespace std;

RFC_NAMESPACE_BEGIN

#define COMMOND_ABILITY_REPORT				  0x1 //能力报告
#define COMMOND_CHECK_TEMPLATE_REPORT		  0x2 //查询模板
#define COMMOND_RESULT_REPORT				  0x3 //结果报告
#define COMMOND_REQ_TASK					  0x4 //任务发送情况
#define COMMOND_REQ_URL						  0x5 //用户行为url
#define COMMOND_REQ_DOMAIN					  0x6 //每个域的情况
#define COMMOND_REQ_NEWDOMAIN				  0x7 //每个域的情况，有不存在的报告
#define COMMOND_REQ_DOMAIN_MORE				  0x8 //每个域的情况，有更多错误分类报告
#define COMMOND_REQ_TASK_MORE				  0x9 //任务发送情况，有更多错误分类报告

char *base64_decode(const char *data, int data_len);

class ParseRequest
{
public:
	ParseRequest();
	~ParseRequest();
	int ParseMessage(char * req, map<unsigned int,struct tinfo_node *> & tinfo_vec, map<unsigned int,struct url_node *> &,const char *,const char *);
	int HttpMsg2DayList(const char * req,const char * );
	void StrReplace(string & dest, char *src, const char *oldstr, const char *newstr, int len);
	int Response(int fd);
private:
	int m_ctype; //命令类型
	map<string,string> m_map_ip;
	int m_iter_ipcache;
};



RFC_NAMESPACE_END

#endif	//APP_PARSEREQUEST_H_201108
