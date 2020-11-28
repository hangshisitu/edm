#ifndef __SMTPSVR_H__
#define __SMTPSVR_H__

#include "application/application.h"
#include "application/configuremgr.h"
#include "network/dnslookup.h"
#include <string>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <mysql/mysql.h>
#include "lock.h"


using namespace std;

RFC_NAMESPACE_BEGIN

#define SMTP_DOT  "."
#define SMTP_CRLF "\r\n"
#define RETRY_LIMIT_EXCEEDED     5
#define MAX_EPOLL_EVENTS    512
#define OBJECT_NUMBER       1024

#define DEFAULT_CLOCK_TICK      600
#define DEFAULT_DOMAIN_QUOTA    10000

#define QUOTA_DEFAULT_INTERVEL 10
#define QUOTA_DEFAULT_VALUE    10000

#define byte16_swap(A)  ((((u_short)(A) & 0xff00) >> 8) | (((u_short)(A) & 0x00ff) << 8))
#define byte32_swap(A)  ((((unsigned int)(A) & 0xff000000) >> 24) | \
    (((unsigned int)(A) & 0x00ff0000) >> 8) | \
    (((unsigned int)(A) & 0x0000ff00) << 8) | \
    (((unsigned int)(A) & 0x000000ff) << 24))

#define SMTP_REPLY_220   "220 Ready\r\n"
#define SMTP_REPLY_221   "221 Bye\r\n"
#define SMTP_REPLY_250   "250 OK\r\n"
#define SMTP_REPLY_354   "354 End with <CRLF>.<CRLF>\r\n"
#define SMTP_REPLY_502   "502 Command not implemented\r\n"
#define SMTP_REPLY_503   "503 Bad sequence of commands \r\n"
#define SMTP_REPLY_506   "506 Your ip is not allowed to connect to this server\r\n" //不允许该IP地址连接
#define SMTP_REPLY_507   "507 System exception\r\n"                      //系统异常
#define SMTP_REPLY_508   "508 Syntax: HELO/EHLO hostname\r\n"            //HELO/EHLO语法错误
#define SMTP_REPLY_509   "509 Invalid User\r\n"                          //无效用户
#define SMTP_REPLY_510   "510 Failed DNS Lookup\r\n"                       //MX查询失败
#define SMTP_REPLY_511   "511 Timeout while trying to connect to mail exchange server\r\n"
#define SMTP_REPLY_512   "512 Mailtext size limit exceeded\r\n"

#define SMTP_REPLY_600   "600 Sender message rate limit exceeded\r\n"    //发送频率限制
#define SMTP_REPLY_601   "601 Connected rate limit exceeded\r\n"         //连接数超过限制
#define SMTP_REPLY_401   "401 Can not connect to mail exchange server\r\n"
#define SMTP_REPLY_402   "402 The connection to the server is closed\r\n"

//for AUTH LOGIN
#define SMTP_USERNAME    "334 VXNlcm5hbWU6\r\n" //Username:
#define SMTP_PASSWORD    "334 UGFzc3dvcmQ6\r\n" //Password:
#define SMTP_AUTH_MSG    "235 Delayed verification\r\n"

#pragma pack(1)
// HR = Hua Run
typedef struct{
	time_t point; //计时点
	int    rval;  //频率值
	int    sval;  //间隔值,秒
	int    total; //数量
} HR_rate_t;

typedef struct {
    HR_rate_t rt[3]; //q,h,d
    int       ab; //attached bytes
} HR_control_t;

#pragma pack()

typedef map<string, HR_control_t> HR_map_t;

typedef struct
{
	string auth_user;
	string auth_pswd;
} auth_node_t;

struct Smtp_Context
{
    int    command; 
    int    next_steps;
    
    int    client_handle;
    int    client_port;
    string client_host;

    int    remote_handle;
    int    remote_port;
    string remote_host;

    string smtp_helo; 
    string smtp_mail; 
    string smtp_rcpt; 
    string smtp_data;
    
    //for AUTH LOGIN
    string smtp_user;
    string smtp_pswd;
    //end AUTH LOGIN

    string real_sender;
    string msg_header;
    string rcpt_domain; 
    string reply_text;
    string x_mid;
    int    bytes_total;
    time_t point;
    int  ab_size;			///发送能力大小
    
    list<string> sendbuffer;
    list<string> list_history;
	//
	std::string	file_path;
	int cur_retry_count;
	long next_send_time;

    Smtp_Context(void)
    {
		reset();
    }
	void reset()
	{
		client_port = 0;
		client_handle = -1;
		remote_port = 0;;
		remote_handle = -1;
		command = 0;
		next_steps = 0;
		bytes_total = 0;
		cur_retry_count = -1;
		next_send_time = 0;
		file_path = "";
	}

    ~Smtp_Context(){clear();}

    void print(const char* text);
    void clear()
    {
        client_host.clear();
        client_port = 0;

        if (client_handle != -1){
            shutdown(client_handle,SHUT_RDWR);
            close(client_handle);
            client_handle = -1;
        }

        remote_host.clear();
        remote_port = 0;;
        if (remote_handle != -1){
            shutdown(remote_handle,SHUT_RDWR);
            close(remote_handle);
            remote_handle = -1;
        }

        command = 0; 
        next_steps = 0;

        smtp_helo.clear(); 
        smtp_mail.clear(); 
        smtp_rcpt.clear(); 
        smtp_data.clear();
        
        //for AUTH LOGIN
        smtp_user.clear();
        smtp_pswd.clear();
        //end AUTH LOGIN
        
        real_sender.clear();
        msg_header.clear();
        x_mid.clear();

        rcpt_domain.clear(); 
        reply_text.clear();
        bytes_total = 0;

        list_history.clear();
        point = time(0);
    }
};

struct ip_segment
{
    in_addr ip_begin;
    in_addr ip_end;
};

#define UNIT_CONVERSION_WITH_SECOND   (1)
struct Delivery_Quota
{
    int     interval; //配置文件定义,单位是分钟
    int     quota;    //单位内允许投递的数量
    int     number;   //已投递的数量
    time_t  tpoint;   //计时点

    Delivery_Quota():interval(0),quota(0),number(0),tpoint(-1){}
};

//ttl=-1是配置文件定义的, 其他的是通过MX查询得到的
struct Mail_eXchange
{
    string host;
    int    port;
    time_t ttl;

    Mail_eXchange():port(0),ttl(-1){}
};

enum SmtpCommand
{
    ZERO = 0x00,
    HELO = 0x01,
    EHLO = 0x02,
    MAIL = 0x04,
    RCPT = 0x08,
    DATA = 0x10,
    RSET = 0x20,
    NOOP = 0x40,
    QUIT = 0x80,
    AUTH = 0x100,
    PSWD = 0x200
};

class SmtpSvr:
    public RFC_NAMESPACE_NAME::Application
{
public:
    SmtpSvr(void);

    virtual ~SmtpSvr();

    static SmtpSvr* instance() {return smtpsvr_instance;};

    static timeval timeval_zero;

	static bool g_stop_send;
	static pthread_mutex_t retry_mail_queue_lock;
	static std::list<Smtp_Context*> keep_retry_queue;
	static void* timeRetrySendMail(void*);
	static int addRetryTask(Smtp_Context*);
	static int addRetryTasks(const std::list<Smtp_Context*>&);
	static int getRetryTasks(std::list<Smtp_Context*>&);
	static int saveRetryTask(Smtp_Context*);
	static int delRetryTask(Smtp_Context*);
	static int loadTasksFromDir(const char *c_dir_path, std::list<Smtp_Context*>&);
	static Smtp_Context* loadTaskFromFile(const char* c_file_path);

protected:

    virtual bool onInit(int argc, char * argv[]);

    virtual int onRun(void);

    virtual void onSignal(int);

	void doIdleProcess();
    void loadControlInfoFromDB(HR_map_t& control_map);
    int check_HR_control(Smtp_Context* ctx);
	pthread_mutex_t _control_data_lock;
	HR_map_t *_client_control_infomap;
    
    int create_server_socket(string&,int);

    int register_epollin_event(int, void*);

    int remove_epollin_event(int , int);

    void poll4accept(int, int);

    int handle_line(string&, Smtp_Context*);

    int parse_type(const char* );

    int smtp_reply(Smtp_Context* );

    int on_helo(string& ,Smtp_Context* );

    int on_mail(string& ,Smtp_Context* );

    int on_rcpt(string& ,Smtp_Context* );

    int on_data(string& ,Smtp_Context* );

    int on_input(string& ,Smtp_Context* );

    int on_rset(string& ,Smtp_Context* );

    int on_quit(string& ,Smtp_Context* );

    int load_delivery_quota(ConfigureMgr& );

    int load_ip_white_list(const std::string& ip_str_list_in);
	void reflush_white_list(ConfigureMgr* cfg_mng_in = NULL);

    int load_mail_eXchange(ConfigureMgr& );

    int check_ipsegment(in_addr& );

    int set_real_sender(Smtp_Context*);

	int connect_mail_eXchange(Smtp_Context*);
    int query_mail_eXchange(const char* , string& , int& );

    string split_string( string &,const char *s );

    //static functions
    static int connect4(const char* , int ,timeval* ttl=NULL);

    static int recv_n(int , void* , int ,timeval *ttl=NULL);

    static int recv_a(int , string&, timeval *ttl=NULL);
    static int recv_line(int , string&, timeval *ttl=NULL);

    static int send_n(int , const void* , int , timeval *ttl=NULL);


    static SmtpSvr* smtpsvr_instance;

    static void* thread_service(void * param);
    static void* hr_service(void * param);

    static Smtp_Context* alloc();

    static void free(Smtp_Context *ctx);

private:
    string m_myhost;
    string m_logpath;
    string m_fuck_header;

    int m_ep_handle;
    int m_listen_handle;
    int m_listen_port;
    int m_socket_ttl;
    int m_max_threads;
    int m_query_dns_ttl;

    SyncLock m_quota_lock;
    SyncLock m_mail_lock;
    SyncLock m_hr_lock;

    pthread_rwlock_t m_rwlock;

    static int m_thread_count;
    static SyncLock m_thrcnt_lock;

    typedef map<string, Delivery_Quota> sender_quota_map;
    typedef map<string, sender_quota_map>  domain_quota_map;

    domain_quota_map m_delivery_quota;
    domain_quota_map m_cache_quota;
    vector<ip_segment> m_ipsegment;

    int m_quota_default_interval;
    int m_quota_default_value; 

    typedef vector<Mail_eXchange> mailx_vector;
    typedef map<string, mailx_vector > mailx_map;

    mailx_map m_mail_eXchange;

    DNSResolver m_dns_resolver;

    time_t m_control_point;
    int m_control_interval;
    int m_control_number;
    int m_control_total;
	int m_ignore_auth;
    
    vector<auth_node_t> m_auth_list;
	std::map<std::string, std::string> m_domain_map;
    MYSQL* mydb;
	MYSQL* _db_client_handle;
};

RFC_NAMESPACE_END

#endif

