#include "config.h"
#include "profile.h"
#include "log_msg.h"
#include "dwstring.h"
#include <sys/sysinfo.h>

string          DW::log_file            = "./logs/faceside.log";
unsigned int    DW::log_size            = 67108800;
unsigned int    DW::msg_threads_num     = get_nprocs_conf();
unsigned int    DW::smtp_threads_num    = get_nprocs_conf() * 2;
unsigned int    DW::msg_queue_size      = 1024;
unsigned int    DW::smtp_queue_size     = 2048;
string          DW::listen_address      = "0.0.0.0";
unsigned int    DW::listen_port         = 51001;
string          DW::smtp_address        = "127.0.0.1";
unsigned int    DW::smtp_port           = 25678;
unsigned int    DW::smtp_timeout        = 60;
unsigned int    DW::smtp_fail_retry     = 3;
unsigned int    DW::smtp_retry_interval = 120;
unsigned int    DW::smtp_thread_send_interval = 3;
string          DW::smtp_default_helo   = "mtargeting.net";
string          DW::smtp_default_reply  = "services@mtargeting.net";
string          DW::web_path            = "./web/";
string          DW::task_path           = "./task/";
string          DW::backup_path         = "./backup/";
string          DW::fail_path           = "./fail/";
unsigned int    DW::report_interval     = 300;
string          DW::report_path         = "./report/";
string          DW::sms_path            = "./sms/";

#define _XD_ "RICHINF0"

#define OUT_MSG(x)  printf x

int ken_config(const char* file)
{
    CProfile prof;
    if (prof.open(file) != 0){
        return -1;
    }

    prof.get(_XD_, "log_file", DW::log_file);
    prof.get<unsigned int>(_XD_, "log_size", DW::log_size);
    prof.get<unsigned int>(_XD_, "msg_threads_num",  DW::msg_threads_num);
    prof.get<unsigned int>(_XD_, "smtp_threads_num", DW::smtp_threads_num);
    prof.get<unsigned int>(_XD_, "msg_queue_size",   DW::msg_queue_size);
    prof.get<unsigned int>(_XD_, "smtp_queue_size",  DW::smtp_queue_size);
    prof.get(_XD_, "listen_address",  DW::listen_address);
    prof.get<unsigned int>(_XD_, "listen_port", DW::listen_port);

    if (dw_check_netaddr(DW::listen_address.c_str()) != 0){
        OUT_MSG(("listen_address '%s' is a invalid address\n",
            DW::listen_address.c_str()));
        return -1;
    }

    if (DW::listen_port < 1 || DW::listen_port > 65535){
        OUT_MSG(("listen_port '%d' is a invalid port\n",
            DW::listen_port));
        return -1;
    }

    prof.get(_XD_, "smtp_address", DW::smtp_address);
    prof.get<unsigned int>(_XD_, "smtp_port", DW::smtp_port);

    if (dw_check_netaddr(DW::smtp_address.c_str()) != 0){
        OUT_MSG(("smtp_address '%s' is a invalid address\n", 
            DW::smtp_address.c_str()));
        return -1;
    }

    if (DW::smtp_port < 1 || DW::smtp_port > 65535){
        OUT_MSG(("smtp_port '%d' is a invalid port\n",
            DW::smtp_port));
        return -1;
    }

    prof.get<unsigned int>(_XD_, "smtp_timeout",        DW::smtp_timeout);
    prof.get<unsigned int>(_XD_, "smtp_fail_retry",     DW::smtp_fail_retry);
    prof.get<unsigned int>(_XD_, "smtp_retry_interval", DW::smtp_retry_interval);
    prof.get<unsigned int>(_XD_, "smtp_thread_send_interval", DW::smtp_thread_send_interval);
    prof.get(_XD_, "smtp_default_helo",  DW::smtp_default_helo);
    prof.get(_XD_, "smtp_default_reply", DW::smtp_default_reply);

    if (dw_check_mailbox(DW::smtp_default_reply.c_str()) != 0){
        OUT_MSG(("smtp_default_reply '%s' bad address\n",
            DW::smtp_default_reply.c_str()));
        return -1;
    }

    prof.get(_XD_, "web_path",    DW::web_path);
    prof.get(_XD_, "task_path",   DW::task_path);
    prof.get(_XD_, "backup_path", DW::backup_path);
    prof.get(_XD_, "fail_path",   DW::fail_path);
    
    prof.get<unsigned int>(_XD_, "report_interval", DW::report_interval);

    if (DW::report_interval < 15 || DW::report_interval > 86400)
    {
        OUT_MSG(("report_interval '%d'  (15 - 86400)\n",
            DW::report_interval));
        return -1;
    }
    prof.get(_XD_, "report_path",   DW::report_path);
    prof.get(_XD_, "sms_path",   DW::sms_path);

    return 0;
}

void ken_print()
{
    LOG_MSG((LM_DEBUG, "log_file = %s\n", DW::log_file.c_str() ));
    LOG_MSG((LM_DEBUG, "log_size = %d\n", DW::log_size));
    LOG_MSG((LM_DEBUG, "msg_threads_num  = %d\n", DW::msg_threads_num));
    LOG_MSG((LM_DEBUG, "smtp_threads_num = %d\n", DW::smtp_threads_num));
    LOG_MSG((LM_DEBUG, "msg_queue_size   = %d\n", DW::msg_queue_size));
    LOG_MSG((LM_DEBUG, "smtp_queue_size  = %d\n", DW::smtp_queue_size));
    LOG_MSG((LM_DEBUG, "listen_address   = %s\n",  DW::listen_address.c_str()));
    LOG_MSG((LM_DEBUG, "listen_port  = %d\n", DW::listen_port));
    LOG_MSG((LM_DEBUG, "smtp_address = %s\n", DW::smtp_address.c_str()));
    LOG_MSG((LM_DEBUG, "smtp_port    = %d\n", DW::smtp_port));
    LOG_MSG((LM_DEBUG, "smtp_timeout        = %d\n", DW::smtp_timeout));
    LOG_MSG((LM_DEBUG, "smtp_fail_retry     = %d\n", DW::smtp_fail_retry));
    LOG_MSG((LM_DEBUG, "smtp_retry_interval = %d\n", DW::smtp_retry_interval));
    LOG_MSG((LM_DEBUG, "smtp_default_helo   = %s\n",  DW::smtp_default_helo.c_str()));
    LOG_MSG((LM_DEBUG, "smtp_default_reply  = %s\n", DW::smtp_default_reply.c_str()));
    LOG_MSG((LM_DEBUG, "web_path    = %s\n",  DW::web_path.c_str()));
    LOG_MSG((LM_DEBUG, "task_path   = %s\n", DW::task_path.c_str()));
    LOG_MSG((LM_DEBUG, "backup_path = %s\n", DW::backup_path.c_str()));
    LOG_MSG((LM_DEBUG, "fail_path   = %s\n", DW::fail_path.c_str()));
    LOG_MSG((LM_DEBUG, "report_interval   = %d\n", DW::report_interval));
}
