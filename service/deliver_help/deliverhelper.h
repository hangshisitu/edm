#ifndef __DELIVER_HELPER__
#define __DELIVER_HELPER__

#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include "lock.h"
#include <mysql/mysql.h>
#include "sync_queue.h"

using namespace std;

#define MIN(a,b)            (a>b?b:a)
#define MAX_LENGTH          1024000
#define C_LF                '\n'
#define S_LF                "\n"
#define CRLF                "\r\n"

#define STD_TRIM(x,y) \
    x.erase(0, x.find_first_not_of(y));x.erase(x.find_last_not_of(y)+1)

enum {
    SOCK_R = 0x01,
    SOCK_W = 0x02,
    SOCK_E = 0x04
};

enum {
    HTTP_NULL = 0,
    HTTP_GET  = 1,
    HTTP_POST = 2
};

enum {TA_DO  = 0x01};

enum{
    HTTP_200 = 0,
    HTTP_200_NODATA,
    HTTP_400,
    HTTP_404,
    HTTP_405,
    HTTP_406,
    HTTP_503
};

char* http_status[] = {
    "200 OK",
    "200 OK",
    "400 Bad Request",
    "404 Not Found",
    "405 Command not implemented",
    "406 Not allowed",
    "503 Service Unavailable"
};

typedef struct
{
    int    method;
    string url;
    string version;
}http_request_t;

typedef struct
{
    int type;
    map<string, string> param;
} task_action_t;

typedef struct{
    int   length;
    int   transferred;
    char* data;
}buffer_t;

typedef struct
{
    time_t         jrtime; //接入(jr)时间
    unsigned int   serial;
    int            sockfd;
    int            epfd;
    unsigned short port;
    string         host;
    string         recvbuf;
    buffer_t       sendbuf;
    http_request_t request;
    task_action_t  action;
    SyncLock       synlock;
    MYSQL*         mysqldb;
}task_context_t;

typedef struct {
    unsigned int pagesize;
    unsigned int cachesize;
    string       report_file;
    string       status_file;
} bdb_inf_t;

typedef struct {
    string host;
    int    port;
    string user;
    string passwd;
    string dbname;
    string charset;
    int    keepalive;
} mysql_inf_t;

typedef struct{
    string ip;
    string from;
    string rcpt;
    string date;
    unsigned int bytes;
    list<string> linebuf;
}bdb_param_t;

typedef struct{
    MYSQL* mysql;
    string from;
    string rcpt;
    string status;
    unsigned int bytes;
    list<string> linebuf;
}bdb_param2_t;

typedef struct 
{
    time_t       chkpoint;
    unsigned int numbers;
}bdb_quantity_t;

typedef struct 
{
    int quantity;
    int seconds;
}db_quantity_t;

typedef vector<db_quantity_t> vt_quantity_t; 

void  cleanup_task_context(task_context_t* tctx, bool deleted = true);
int   read_configure(const char*file);
int   parse_http_request_line(const char* line, http_request_t* req);
int   parse_task_action(const char* url, task_action_t* ta);
void* thread_http_process(void* param);
int   context_process(void* param);
void  cleanup_task_queue();

int sockselect(int fd, int flag, int timeout = 0);
int create_socket( const char* host, int port );
int recv_until_edge( int handle, std::string & result, int length, int timeout = 0);
int send_until_edge( int handle, const void* buffer, int length ,int timeout = 0);

int check_inet_addr(const char* cp);
int epoll_accept( int svrfd, int epfd );
int epoll_engine_start(int handle);
int epoll_output(void* param);
int epoll_input(void* param);

void stop_pthread_pool();
int  start_pthread_pool(int nthreads);

int    query_context_result(MYSQL* mysql, const char* sqlstr, task_context_t* tctx);
MYSQL* make_mysql_instance(MYSQL*& mysql);
void   signal_handler(int sign);
void*  bdb_query_report(const void* key, unsigned int kl, const void* data, unsigned int dl, void* param );
void*  bdb_query_status(const void* key, unsigned int kl, const void* data, unsigned int dl, void* param );
int    mysql_common_query(MYSQL* mysql, const char* sqlbuf, list<vector<string> >& result);
int    insert_detail_records(string &ip, string& from, string& rcpt, string& status);
int    add_deliver_quantity(MYSQL* mysql,const char* from,const char* rcpt, unsigned int numbers);
int    check_mailbox_syntax(const char* mail);

int try2forget_1001 (string &ip,task_context_t* tctx);
int try2forget_1002 (string &ip, task_context_t* tctx);
int try2forget_1003 (string &ip, task_context_t* tctx);
int try2forget_2001 (string &ip, task_context_t* tctx);
int try2forget_2002 (string&ip, task_context_t* tctx);
int try2forget_2003 (string &ip, task_context_t* tctx);


#endif

