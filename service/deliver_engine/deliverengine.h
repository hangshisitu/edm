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
//	unsigned short t_id;		//�����ڿ����ж��ģ�壬��ʱ����
	bool operator < ( const Task_Id &td) const 
	{
		if (time< td.time)         //ʱ������
		{
			return true;
		}
		else if (time == td.time)   //���ʱ����ͬ������������������
		{
			if(m_id < td.m_id)
			{
				return true;          //��������
			}
			else if(m_id == m_id)
			{
				if(p_id < td.p_id)
				{
					return true;    //ǰ��������ͬ��ʱ�򣬰�p_id����
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
	unsigned char			 status;          //0��ʾδ���ͣ�1��ʾ���ͳɹ���2��ʾ���ڷ��ͣ�����ס�ٴη��ͣ�5�����ˣ���δ�ɹ�
	unsigned short			 failtimes;		  //��ʾʧ�ܵĴ���
	char					 domain[128];	  //������"other"��ʾ��������������������
	char					 mail[128];	  //����
	string					 errstr;	  //�Զ˷��ش��󣬸�ʽΪstep 1-->550 5.1.1 <angel_178@sohu.com>: Recipient address rejected: User unknown in local recipient table���޴�λok
	time_t					 stime;			//���η���ʱ���
	_task_id				 task_id;         //����ID,��ʶΨһ���� 
	map<stdstring,stdstring> map_variable;    //�����������
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
	string					errstr;		//�Զ˷��ش���
};

/*
typedef struct tmp_mail_node
{
	char                     domain[64];      //������"other"��ʾ��������������������
	char                     mail[128];   //����
	map<stdstring,stdstring> map_variable;    //�����������
}_tmp_mail_node;
*/

struct domain_node
{
	unsigned int fail;       //����ʧ���� ��
	unsigned int total;      //�������� ��

	unsigned int fail_m;       //����ʧ�� ��
	unsigned int total_m;      //�������� ��

	unsigned int fail_q;       //����ʧ���� 15����
	unsigned int total_q;      //�������� 15���� 

	unsigned int fail_h;       //����ʧ���� Сʱ
	unsigned int total_h;      //�������� Сʱ 

	unsigned int fail_cur;       //��ǰ����ά�� ʧ����
	unsigned int total_cur;      //��ǰ����ά�� ����

	unsigned char flag_cur;      //��ǰ����ά�� ��1Ϊ�֣�2Ϊ�̣�3Ϊʱ��4Ϊ�գ�����Ϊ��

	unsigned int ability;    //��ǰ����ֵ����ֵͨ����һ�ֵ�fail/total������ʧ���ʸ���ĳֵʱ��ֵΪ0������Ϊ10(����������������)
	unsigned int oriability;    //��ʼ����ֵ����ֵͨ����һ�ֵ�fail/total������ʧ���ʸ���ĳֵʱ��ֵΪ0������Ϊ10(����������������)
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
	int ScanList(void);     //ɨ�跢�Ͷ��У���װeml�ļ�����session����
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
	char * m_req_tmgr;   //���͵��Ȼ�������
	char * m_rsp_tmgr;     //���յ��Ȼ�Ӧ����
	map<stdstring, domain_node>	m_map_domain;    //װ��������Ҫ����������domain��domain_node���������ϣ�ÿһ�ּ���һ��
//	map<unsigned int, mail_node *>	m_map_mail;   //ȫ������map��keyΪ������ˮ�ţ�mail_node�������䷢���������Ϊ������������ݣ���������Ӧ���д�룬��Ͷ�ݵ�session�Ự�и��£�ÿ10���ӱ���֮�����

	map<stdstring, _template_node> m_map_template; //����id���ʼ�ģ��Ķ�Ӧmap����װʱ��ѯ�����ѯ�������͸����Ȼ���ѯ
	vector<string> m_vec_helo;
	std::map<std::string, std::string> m_domain_map;
};

RFC_NAMESPACE_END

#endif	//APP_DELIVERENGINE_H_201108
