/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan Date: 2011-11-11  �����*/

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

#define COMMOND_ABILITY_REPORT				  0x1 //��������
#define COMMOND_CHECK_TEMPLATE_REPORT		  0x2 //��ѯģ��
#define COMMOND_RESULT_REPORT				  0x3 //�������
#define COMMOND_REQ_TASK					  0x4 //���������
#define COMMOND_REQ_URL						  0x5 //�û���Ϊurl
#define COMMOND_REQ_DOMAIN					  0x6 //ÿ��������
#define COMMOND_REQ_NEWDOMAIN				  0x7 //ÿ�����������в����ڵı���
#define COMMOND_REQ_DOMAIN_MORE				  0x8 //ÿ�����������и��������౨��
#define COMMOND_REQ_TASK_MORE				  0x9 //������������и��������౨��

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
	int m_ctype; //��������
	map<string,string> m_map_ip;
	int m_iter_ipcache;
};



RFC_NAMESPACE_END

#endif	//APP_PARSEREQUEST_H_201108
