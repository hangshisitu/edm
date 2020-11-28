/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan Date: 2011-11-11  �����*/

#ifndef	APP_RequestMail_H_201111 
#define APP_RequestMail_H_201111

#include "deliverengine.h"
#include "application/application.h"
#include "application/configuremgr.h"
#include "system/filehandle.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "application/filelog.h"

using namespace std;

RFC_NAMESPACE_BEGIN

#define COMMOND_ABILITY_REPORT				  0x1 //��������
#define COMMOND_CHECK_TEMPLATE_REPORT		  0x2 //��ѯģ��
#define COMMOND_RESULT_REPORT				  0x3 //�������
#define COMMOND_RSP_ABILITY_REPORT            0x01000001 //��������Ӧ��
#define COMMOND_RSP_CHECK_TEMPLATE_REPORT     0x01000002 //��ѯģ��Ӧ��
#define COMMOND_RSP_RESULT_REPORT			  0x01000003 //�������Ӧ��
#define HEAD_LENTH							  0xc 
#define MUNITE								  109		//��ά��
#define QUATOR								  113		//15����ά�� 
#define HOUR								  104		//Сʱά�� 
#define DAY									  100		//��ά�� 

enum { REQ_MSG_MAX_LEN = 4096 };
class RequestMail
{
public:
	RequestMail();
	~RequestMail();
	int MailListReq(map<unsigned int,struct mail_node *> &vec_mail,  map<stdstring,struct domain_node> &map_domain,  char * req, int fd);
	int MakeMailList(char * rsp_msg, map<unsigned int,struct mail_node *> &map_mail,int mailnum);
	int InstallHead(char * req, int command_code);
	int SplitVariable(struct mail_node * p_node,char * variable,int mail_length);
	int ParseMailListRsp(char * rsp_msg, map<unsigned int,struct mail_node *> &map_mail);
	int TemplateSession(struct Task_Id &,int fd,stdstring &, map<unsigned int, struct mail_node *> &map_mail);
	int ParseRsp(stdstring &rsp, int, map<unsigned int, struct mail_node *>& map_mail);
	int SendReport(map<unsigned int,struct mail_node *> &map_mail,int ,vector<unsigned int> &);
	int handle_notify_msg(map<unsigned int, struct mail_node *> &map_mail, const std::vector<Task_Id>& stoptasks);
};

RFC_NAMESPACE_END

#endif	//APP_DELIVERENGINE_H_201108
