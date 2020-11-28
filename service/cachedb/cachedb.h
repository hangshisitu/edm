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
    string tb_triger; //����
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
	unsigned char    tag;				//1Ϊ�����2Ϊ���ʼ�, 3�˶�, 4Ͷ��
	char			 mail[128];			//������û������ַ 
	char			 url[256];			//url��ַ
	char			 area[64];			//url��ַ
	unsigned int	 template_id;		//ģ��ID
	unsigned int	 corp_id;			//��ҵID
	unsigned int	 task_id;			//����ID
	unsigned int	 hour;				//����Сʱͳ��
	unsigned char	 osid;				//�ͻ��˲���ϵͳ
	unsigned char	 browserid;			//�ͻ��������
	unsigned char	 slangid;			//�ͻ�������
}_url_node;

typedef struct tinfo_node
{
//	unsigned char tag;				//1Ϊ�Ѵ���0δ����
	unsigned char send_flag;		// �������Ƿ��Ѿ�������ϣ�1��ʾ�����У�0��ʾ�������
	unsigned int suc_send;		//�ɹ�������
	unsigned int total;			//Ӧ������
	unsigned int send;			//������ʧ����
	unsigned int template_id;       //ģ��ID
	unsigned int corp_id;           //��ҵID
	unsigned int task_id;           //����ID
	unsigned int hard_bounce_count;           //Ӳ���ش���
	unsigned int soft_bounce_count;           //���ش���
	unsigned int none_exist_count;           //������������
	unsigned int none_exist_count1;           //��������������������
	unsigned int dns_fail_count;           //������������
	unsigned int spam_garbage_count;           //�����ʼ�
}_tinfo_node;

typedef struct domain_node
{
	unsigned int suc_send;      //�ɹ�������
	unsigned int total;         //Ӧ������
	unsigned int send;          //������ʧ����
	unsigned int template_id;       //ģ��ID
	unsigned int corp_id;           //��ҵID
	unsigned int task_id;           //����ID
	unsigned int hard_bounce_count;           //Ӳ���ش���
	unsigned int soft_bounce_count;           //���ش���
	unsigned int none_exist_count;           //������������
	unsigned int none_exist_count1;           //��������������������
	unsigned int dns_fail_count;           //������������
	unsigned int spam_garbage_count;           //�����ʼ�
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
		int    m_kdpfd;   //epoll���
		int    m_curfds;   //��ǰepoll������
		map<unsigned int,_url_node *> m_url_map;
		map<unsigned int,_tinfo_node *> m_task_map;
};

RFC_NAMESPACE_END

#endif	//APP_CACHEDB_H_201108
