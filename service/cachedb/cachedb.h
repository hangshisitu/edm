/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan Date: 2011-11-08      */

#ifndef APP_CACHEDB_H_201108
#define APP_CACHEDB_H_201108

#include "parserequest.h" 
#include "application/application.h"
#include "application/configuremgr.h"
#include "system/filehandle.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "application/filelog.h"
#include <map>
#include <sys/epoll.h> 
#include "database/databaseenv.h"
#include "database/mysqldb.h"
#include "database/database.h"
#include <list>

#define MAXEPOLLSIZE 10000

using namespace std;

RFC_NAMESPACE_BEGIN

//+++
typedef struct  
{
    string tb_domain_map;
    string tb_region_info;
    string tb_task;
    string tb_triger; //无用
    string tb_result;
    string tb_result_by_domain;
    string tb_result_by_hour;
    string tb_result_by_region;
    string tb_result_by_url;
    string tb_stat_havior;
    string tb_stat_task_unsubscribe;
} table_name_t;
//+++

typedef struct url_node
{
	unsigned int     seq_no;
	unsigned char    tag;				//1为点击，2为打开邮件, 3退订, 4投诉
	char			 mail[128];			//点击的用户邮箱地址 
	char			 url[256];			//url地址
	char			 area[64];			//url地址
	unsigned int	 template_id;		//模板ID
	unsigned int	 corp_id;			//企业ID
	unsigned int	 task_id;			//任务ID
	unsigned int	 hour;				//用于小时统计
	unsigned char	 osid;				//客户端操作系统
	unsigned char	 browserid;			//客户端浏览器
	unsigned char	 slangid;			//客户端语言
}_url_node;

typedef struct tinfo_node
{
//	unsigned char tag;				//1为已处理，0未处理
	unsigned char send_flag;		// 该任务是否已经发送完毕，1表示发送中，0表示发送完毕
	unsigned int suc_send;		//成功发送数
	unsigned int total;			//应发总数
	unsigned int send;			//包括了失败数
	unsigned int template_id;       //模板ID
	unsigned int corp_id;           //企业ID
	unsigned int task_id;           //任务ID
	unsigned int hard_bounce_count;           //硬弹回次数
	unsigned int soft_bounce_count;           //软弹回次数
	unsigned int none_exist_count;           //不存在邮箱数
	unsigned int none_exist_count1;           //不存在邮箱数，新增的
	unsigned int dns_fail_count;           //不存在邮箱数
	unsigned int spam_garbage_count;           //垃圾邮件
}_tinfo_node;

typedef struct domain_node
{
	unsigned int suc_send;      //成功发送数
	unsigned int total;         //应发总数
	unsigned int send;          //包括了失败数
	unsigned int template_id;       //模板ID
	unsigned int corp_id;           //企业ID
	unsigned int task_id;           //任务ID
	unsigned int hard_bounce_count;           //硬弹回次数
	unsigned int soft_bounce_count;           //软弹回次数
	unsigned int none_exist_count;           //不存在邮箱数
	unsigned int none_exist_count1;           //不存在邮箱数，新增的
	unsigned int dns_fail_count;           //不存在邮箱数
	unsigned int spam_garbage_count;           //垃圾邮件
	char domain[64];
}_tdomain_node;

int DBUpdateTask(_tinfo_node * tn);
int DBUpdateUrl(_url_node * tn);
int DBUpdateTaskDomain(_tdomain_node * td);
int CheckHistory(_url_node * un,char *,int);
unsigned int GetRegionId(const char * area, unsigned int & region_id);
int Region_Update(_url_node * un,int flag,bool is_upusernum);
int Hour_Update(_url_node * un, int flag);
int Domain_Update(_url_node * un,int flag,bool is_upusernum);


class CacheDbApp: public Application
{
	public:
		CacheDbApp(void);
		static string m_database;
	protected:
		bool				daemon(void);
		virtual bool		onInit(int argc, char * argv[]);
		virtual int			onRun(void);
		int CreateServer();
		int ReceiveRequest();
		int DBProcess();
		int HandleMessage(int,const char *);
		int DBUpdateTask(_tinfo_node *, unsigned int );
		int DBUpdateUrl(_url_node * tn);

	private:
		int	   m_listener;
		struct epoll_event m_ev;  
		struct epoll_event m_events[MAXEPOLLSIZE];
		char   m_cachedbsvrport[16];
		string m_nginxdomain;
		int    m_kdpfd;   //epoll句柄
		int    m_curfds;   //当前epoll监听数
		map<unsigned int,_url_node *> m_url_map;
		map<unsigned int,_tinfo_node *> m_task_map;
};

RFC_NAMESPACE_END

#endif	//APP_CACHEDB_H_201108
