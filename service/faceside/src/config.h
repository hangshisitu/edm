#ifndef _CONFIG_H_
#define _CONFIG_H_

#define MY_BUFSIZE          1024
#define MODULA_BUFSIZE      1024*10
#define MAILLIST_BUFSIZE    1024
#define VAR_FLAG            "$"
#define FILTER_STR          "\t \r\n"
#define BOUNDARY            "----=_NextPart"
#define PVOID_OK            (void*)0
#define PVOID_ERROR         (void*)-1 

#define SMTP_OK             0
#define SMTP_FAIL           -1
#define SMTP_RETRY          -2
#define SMTP_DNS_FAIL       -3
#define SMTP_NONE_EXIST     -4
#define SMTP_SPAM           -5
#define SMTP_NRETRY         -6

#define SMTP_ERR_MSG(x) (x==SMTP_OK?"ok":\
x==SMTP_DNS_FAIL?"domain not exist":\
x==SMTP_SPAM?"spam":\
x==SMTP_NONE_EXIST?"not found":\
x==SMTP_RETRY?"retry":"fail")

#include <string>
using namespace std;

struct DW {
    static string       log_file;
    static unsigned int log_size;
    static unsigned int msg_threads_num;
    static unsigned int smtp_threads_num;
    static unsigned int msg_queue_size;
    static unsigned int smtp_queue_size;
    static string       listen_address;
    static unsigned int listen_port;
    static string       smtp_address;
    static unsigned int smtp_port;
    static unsigned int smtp_timeout;
    static unsigned int smtp_fail_retry;
    static unsigned int smtp_retry_interval;
    static unsigned int smtp_thread_send_interval;
    static string       smtp_default_helo;
    static string       smtp_default_reply;
    static string       web_path;
    static string       task_path;
    static string       backup_path;
    static string       fail_path;
    static unsigned int report_interval;
    static string       report_path;
    static string       sms_path;
} ;

extern int ken_config(const char* file);
extern void ken_print();

#endif
