#ifndef __LABEL_MGR__
#define __LABEL_MGR__

#include <string>
#include <list>
#include <map>
#include <set>
#include "lock.h"
#include <mysql/mysql.h>  //g++ -lmysqlclient
#include "msg_queue.h"

using namespace std;

#define MIN(a,b)            (a>b?b:a)
#define MAX_LENGTH          10240
#define C_LF                '\n'
#define S_LF                "\n"
#define CRLF                "\r\n"

#define STD_TRIM(x,y) \
    x.erase(0, x.find_first_not_of(y));x.erase(x.find_last_not_of(y)+1)

enum {SOCK_R = 0x01, SOCK_W = 0x02, SOCK_E = 0x04};
enum {HTTP_NULL=0x00, HTTP_GET=0x01, HTTP_POST=0x02, HTTP_CLOSE=0x04};
enum {TA_INSERT = 0x01,TA_QUERY  = 0x02};
enum {DB_NEW = 1, DB_OLD = 2};

enum{
    DT_PROCESS       = 0,
    DT_HTTP_OK       = 200,
    DT_LABEL_ERROR   = 201,
    DT_MAILBOX_ERROR = 202,
    DT_TASKID_ERROR  = 203,
    DT_DATE_ERROR    = 204,
    DT_MISSING_FIELD = 205,
    DT_DB_ERROR      = 206,
    DT_TYPE_ERROR    = 207,
    DT_SCORE_ERROR   = 208,
    DT_ERROR         = 299
};

typedef struct
{
    int method;
    string url;
    string version;
}http_request_t;

typedef struct
{
    int type;
    map<string, string> param;
} task_action_t;

typedef struct
{
    int sockfd;
    int epfd;
    string host;
    int port;
    string recvbuf;
    int sb_bytes;
    int sb_transfrred;
    char* sendbuf;
    http_request_t request;
    task_action_t action;
    SyncLock synlock;

    MYSQL* mysql;
}task_context_t;

typedef struct  
{
    int    dbsrc;
    char   action;
    string btime;
    string taskid;
    string lbstr;
    set<string> lbset;
} thread_data_t;

typedef struct 
{
    string bind;
    int    port;
    int    socktimeout;
    int    mysql_queue_size;
    int    work_nthread;
    int    scanlog_nthread;
    string log_path;
    long   log_filesize;
    string scanlog_host;
    int    scanlog_port;
} configure_t;

typedef struct {
    string host;
    int    port;
    string user;
    string passwd;
    string db;
    string charset;
} mysql_inf_t;


void cleanup_task_context(task_context_t* tctx, bool deleted = true);
int load_configure(const char*file, configure_t* conf);
int parse_request_line(const char* line, http_request_t* req);
int parse_task_action(const char* url, task_action_t* ta);
void* thread_task_process(void* param);
int task_process_imp(void* param);

int sockselect(int fd, int flag, int timeout = 0);
int mksocket( const char* host, int port );
int nb_connect( const char* host, int port, int timeout );
int recv_until_edge( int handle, std::string & result, int length, int timeout = 0);
int send_until_edge( int handle, const void* buffer, int length ,int timeout = 0);

int epoll_accept( int svrfd, int epfd );
int epoll_engine(int handle);
void* epoll_output(void* param);
void* epoll_input(void* param);

int get_query_result(MYSQL* mysql, const char* sqlstr, task_context_t* tctx);
int check_email(const char* mail);
int query_taskid_date(task_context_t* tctx, const char* taskid, string &create_time);
int check_label_exist(MYSQL* mysql, string& label, int count);
int check_table_exist(MYSQL* mysql, string & table);
int create_label_table(MYSQL* mysql, string &table);
int insert_label_by_email(MYSQL* mysql, const char* email, set<string>& lbset, char action, int count=1, const char* up_time = "now()");
void* insert_label_by_taskid(void* param);
int query_label_detail(MYSQL* mysql, set<string>& lbset, string& btime, string& etime, string& minscore, string& maxscore, task_context_t* tctx);
int query_label_count(MYSQL* mysql, set<string>& lbset, task_context_t* tctx);
void split_label(string& labels, set<string>& lbset);
int recv_line_from_cache_or_socket(int sockfd, string& cache, string& line, int timeout);
int get_http_length_and_remove_header(int sockfd, string& recvbuf, int timeout);
int init_mysql_queue(Msg_Queue<MYSQL*>& mysqlqueue, mysql_inf_t* conf, int count);
#endif

