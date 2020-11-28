/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan Date: 2011-11-08      */

#ifndef APP_DELIVERENGINE_H_201108
#define APP_DELIVERENGINE_H_201108

#include "application/application.h"
#include "application/configuremgr.h"
#include "system/filehandle.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "application/filelog.h"
#include <vector>
#include <map>
#include "requestmail.h"
#include "mxmgr.h"
#include "emlsender.h"

using namespace std;

RFC_NAMESPACE_BEGIN


#define random(x) (rand()%x)

typedef struct Task_Id
{
	unsigned int time;
	unsigned int m_id;
	unsigned int p_id;
//	unsigned short t_id;		//任务内可能有多个模板，暂时不用
	bool operator < ( const Task_Id &td) const 
	{
		if (time< td.time)         //时间升序
		{
			return true;
		}
		else if (time == td.time)   //如果时间相同，按机器号升序排序
		{
			if(m_id < td.m_id)
			{
				return true;          //升序排列
			}
			else if(m_id == m_id)
			{
				if(p_id < td.p_id)
				{
					return true;    //前两个都相同的时候，按p_id升序
				}
			}
		}
		return false;
	}
        bool operator ==(const Task_Id& td) 
	{
		return td.time == time && td.m_id == m_id && td.p_id == p_id;
	}
} _task_id;

typedef struct thread_param
{
	unsigned int seqno;
	size_t from_seq;
	stdstring	 ip;
	stdstring	 port;
	stdstring	 mail_text;
	stdstring	 mail;
	stdstring	 vip;
	stdstring	 from;
	stdstring	 is_auth;
	stdstring	 auth_info;
	stdstring	 sender;
	stdstring	 helo;
	stdstring	 mail_from;
	unsigned int from_flag;
	bool same_flag;
	int random;
	char		 task_id[100];
	vector<string> vec_helo;
    int            sms;
    stdstring     subject;
}_thread_param;

struct mail_node
{
	unsigned char			 status;          //0表示未发送，1表示发送成功，2表示正在发送，会锁住再次发送，5发送了，但未成功
	unsigned short			 failtimes;		  //表示失败的次数
	char					 domain[128];	  //域名，"other"表示除常用域名外其他域名
	char					 mail[128];	  //邮箱
	string					 errstr;	  //对端返回错误，格式为step 1-->550 5.1.1 <angel_178@sohu.com>: Recipient address rejected: User unknown in local recipient table，无错位ok
	time_t					 stime;			//单次发送时间戳
	_task_id				 task_id;         //任务ID,标识唯一任务 
	map<stdstring,stdstring> map_variable;    //邮箱变量队列
};

typedef struct template_node
{
	char mail_from[1024];
	char sender[1024]; 
	char helo[64];
	unsigned char from_flag;
	bool same_flag;
	string templatestr;
    int            sms;
    stdstring     subject;
    int            modula_type_eml;
}_template_node;

struct tmp_node
{
	unsigned char		    flag;
	time_t					stime;
	string					errstr;		//对端返回错误
};

/*
typedef struct tmp_mail_node
{
	char                     domain[64];      //域名，"other"表示除常用域名外其他域名
	char                     mail[128];   //邮箱
	map<stdstring,stdstring> map_variable;    //邮箱变量队列
}_tmp_mail_node;
*/

struct domain_node
{
	unsigned int fail;       //发送失败数 天
	unsigned int total;      //发送总数 天

	unsigned int fail_m;       //发送失败 分
	unsigned int total_m;      //发送总数 分

	unsigned int fail_q;       //发送失败数 15分钟
	unsigned int total_q;      //发送总数 15分钟 

	unsigned int fail_h;       //发送失败数 小时
	unsigned int total_h;      //发送总数 小时 

	unsigned int fail_cur;       //当前策略维度 失败数
	unsigned int total_cur;      //当前策略维度 总数

	unsigned char flag_cur;      //当前策略维度 ，1为分，2为刻，3为时，4为日，其他为日

	unsigned int ability;    //当前能力值，该值通过上一轮的fail/total来定，失败率高于某值时该值为0，否则为10(后续可以做成配置)
	unsigned int oriability;    //初始能力值，该值通过上一轮的fail/total来定，失败率高于某值时该值为0，否则为10(后续可以做成配置)
};

int CreateClient(void);

class DeliverEngineApp : public Application
{
public:
	DeliverEngineApp(void);

	int CheckTemplate(_task_id & ,stdstring &);
	int MailListRequest(void);
	int SendReport(void);
	int Response(void);		
	int ScanList(void);     //扫描发送队列，组装eml文件，开session发送
	int MakeMailText(char * template_text, map<stdstring,stdstring> &map_variable,string & mail_text,const char * mail,int &, char *);
	void StrReplace(string &dest, char *src, const char *oldstr, const char *newstr, int len);
	int UpdateList();
	int EnCode64(string &);
	void UpdateTotle();
	int Protemplate(_template_node &node,string & template_str);
	int GetRandomDomain(char * sender,const int );
	int dkim_signature(const char* eml, string& result);
protected:
	bool				daemon(void);
	virtual bool		onInit(int argc, char * argv[]);
	virtual int			onRun(void);

protected:
	FileHandle			m_fDaemonFileLock;

private:
	short  m_circleiterval;
	short  m_reportiterval;
	short  m_tempiterval;
	time_t m_iterval;
	time_t m_iter;
	time_t m_iterday;
	time_t m_iterhour;
	time_t m_iterquater;
	time_t m_itermunite;
	char * m_req_tmgr;   //发送调度机请求报文
	char * m_rsp_tmgr;     //接收调度机应答报文
	map<stdstring, domain_node>	m_map_domain;    //装载所有需要报告能力的domain，domain_node含义具体见上，每一轮计算一次
//	map<unsigned int, mail_node *>	m_map_mail;   //全量邮箱map，key为报文流水号，mail_node保存邮箱发送情况，作为能力计算的依据，申请邮箱应答后写入，在投递的session会话中更新，每10分钟报告之后清空

	map<stdstring, _template_node> m_map_template; //任务id和邮件模板的对应map，组装时查询，如查询不到发送给调度机查询
	vector<string> m_vec_helo;
	std::map<std::string, std::string> m_domain_map;
};

RFC_NAMESPACE_END

#endif	//APP_DELIVERENGINE_H_201108
