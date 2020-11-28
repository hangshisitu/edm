#include "smtpsvr.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>
#include <algorithm>
#include <functional>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <strings.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "log.h"
#include <signal.h>
#include <sys/time.h>
#include "base/datetime.h"
#include "charset/mimeparser.h"

#ifdef DEBUG
#   define LOG_DEBUG LOG_MSG
#else
#   define LOG_DEBUG 
#endif

#ifdef _SMTP_CACHE
#   define USING_BUFFER 1
#else
#   define USING_BUFFER 0
#endif

#define SMTP_354       "354 End with <CRLF>.<CRLF>\r\n"
#define SMTP_250	   "250 OK\r\n"
#define SEND_ONCE_VERSION		1

MAIN_APP(SmtpSvr)

RFC_NAMESPACE_BEGIN

#define STMPCMD2STR(x)\
    (x==HELO?"HELO":\
    x==EHLO?"EHLO":\
    x==MAIL?"MAIL":\
    x==RCPT?"RCPT":\
    x==DATA?"DATA":\
    x==RSET?"RSET":\
    x==NOOP?"NOOP":\
    x==QUIT?"QUIT":"ZERO")

#define LOG_ERROR(e,s) LOG_MSG(("[error] %s:%s\n", s,strerror(e)))

static string mysql_host;
static int mysql_port;
static string mysql_user;
static string mysql_passwd;
static string mysql_dbname;
static string mysql_charset;
static MYSQL* MysqlConnect(MYSQL*& mysql);
static string g_default_helo;
static int g_send_retry;
static int g_recv_retry;
static string g_mail_save_dir;
static int g_send_time_interval;
static int g_max_fail_count;
static int g_queue_limit_size = 10000;



typedef struct {
    size_t      len;
    u_char     *data;
} ngx_str_t;

void
ngx_encode_base64(ngx_str_t *dst, ngx_str_t *src)
{
    u_char         *d, *s;
    size_t          len;
    static u_char   basis64[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    len = src->len;
    s = src->data;
    d = dst->data;

    while (len > 2) {
        *d++ = basis64[(s[0] >> 2) & 0x3f];
        *d++ = basis64[((s[0] & 3) << 4) | (s[1] >> 4)];
        *d++ = basis64[((s[1] & 0x0f) << 2) | (s[2] >> 6)];
        *d++ = basis64[s[2] & 0x3f];

        s += 3;
        len -= 3;
    }

    if (len) {
        *d++ = basis64[(s[0] >> 2) & 0x3f];

        if (len == 1) {
            *d++ = basis64[(s[0] & 3) << 4];
            *d++ = '=';

        } else {
            *d++ = basis64[((s[0] & 3) << 4) | (s[1] >> 4)];
            *d++ = basis64[(s[1] & 0x0f) << 2];
        }

        *d++ = '=';
    }

    dst->len = d - dst->data;
}

int ngx_decode_base64_internal(ngx_str_t *dst, ngx_str_t *src, const u_char *basis)
{
    size_t          len;
    u_char         *d, *s;

    for (len = 0; len < src->len; len++) {
        if (src->data[len] == '=') {
            break;
        }

        if (basis[src->data[len]] == 77) {
            return -1;
        }
    }

    if (len % 4 == 1) {
        return -1;
    }

    s = src->data;
    d = dst->data;

    while (len > 3) {
        *d++ = (u_char) (basis[s[0]] << 2 | basis[s[1]] >> 4);
        *d++ = (u_char) (basis[s[1]] << 4 | basis[s[2]] >> 2);
        *d++ = (u_char) (basis[s[2]] << 6 | basis[s[3]]);

        s += 4;
        len -= 4;
    }

    if (len > 1) {
        *d++ = (u_char) (basis[s[0]] << 2 | basis[s[1]] >> 4);
    }

    if (len > 2) {
        *d++ = (u_char) (basis[s[1]] << 4 | basis[s[2]] >> 2);
    }

    dst->len = d - dst->data;
    return 0;
}

int ngx_decode_base64(ngx_str_t *dst, ngx_str_t *src)
{
    static u_char   basis64[] = {
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77, 77, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
        77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 77,
        77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,

        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
    };

    return ngx_decode_base64_internal(dst, src, basis64);
}

void Smtp_Context::print(const char* text)
{
    if (text == NULL){
        text = "Unknow";
    }

//    LOG_MSG(("[history]:\n"));
//    for (list<string>::iterator it = list_history.begin();it != list_history.end();++it)
//    {
//        LOG_MSG((" -> %s\n",it->data()));
//    }

    if (*reply_text.rbegin() != '\n'){
        reply_text.append("\n");
    }

    LOG_MSG(("Result=%s,Mail=%s(%s),Rcpt=%s,Client=%s:%d,Remote=%s:%d,X-Mid=%s,Respond=%s",
        text,
        smtp_mail.c_str(),
        real_sender.c_str(),
        smtp_rcpt.c_str(),
        client_host.c_str(),client_port,
        remote_host.c_str(),remote_port,
        x_mid.c_str(),
        reply_text.data()
        ));
}

SmtpSvr* SmtpSvr::smtpsvr_instance = NULL;
SyncLock SmtpSvr::m_thrcnt_lock;
int      SmtpSvr::m_thread_count = 0;

SmtpSvr::SmtpSvr( void )
:Application("smtpsvr")
, mydb(NULL)
, _db_client_handle(NULL)
{
    smtpsvr_instance = this;
}

SmtpSvr::~SmtpSvr()
{
    smtpsvr_instance = NULL;
    pthread_rwlock_destroy(&m_rwlock);
	if (_client_control_infomap) {
		delete _client_control_infomap;
		_client_control_infomap = NULL;
	}
	if (mydb) {
		mysql_close(mydb);
		delete mydb;
		mydb = NULL;
	}
	if (_db_client_handle) {
		mysql_close(_db_client_handle);
		delete _db_client_handle;
		_db_client_handle = NULL;
	}
}

#define CONF_IEDM       "iedm.conf"
#define CONF_SMTPSVR    "smtpsvr.conf"

#define SIZE_B 1
#define SIZE_K (SIZE_B*1024) 
#define SIZE_M (SIZE_K*1024)
#define SIZE_G (SIZE_M*1024)

static string last_ip_str_list;
static string last_white_client;
static string cfg_path;
//
bool SmtpSvr::onInit( int argc, char * argv[] )
{
    srand(unsigned(time(0)));
    pthread_rwlock_init(&m_rwlock,NULL);

    /************************************************************************/
    /*  默认配置文件  1.smtpsvr.conf  2.iedm.conf                           */
    /************************************************************************/
    string file;
    if (argc > 1 && access(argv[1],R_OK) == 0)
        file = argv[1];
    else if (access(CONF_SMTPSVR,R_OK) == 0)
        file = CONF_SMTPSVR;
    else if (access(CONF_IEDM,R_OK) == 0)
        file = CONF_IEDM;
    else
    {
        printf("load configure file failed\n");
        return false;
    }
    
    ConfigureMgr conf_mgr;
    if(!conf_mgr.loadConfFile(file.c_str()))
    {
        printf("load configure file failed: %s\n",file.c_str());
        return false;
    }
    cfg_path = file;

    /************************************************************************/
    /* 获取配置文件名不包含后缀                                             */
    /************************************************************************/
    m_fuck_header = ::basename(file.c_str());
    m_fuck_header.erase(m_fuck_header.find_last_of(".") + 1);
    m_fuck_header.erase(m_fuck_header.find_last_not_of(".") + 1);

    m_logpath = conf_mgr.getString("logs/", m_fuck_header + "/smtpsvr/log/path");
    unsigned int logfilesize = conf_mgr.getInt(64 * SIZE_M, m_fuck_header + "/smtpsvr/log/file_size");
    system(("mkdir -p " + m_logpath).c_str());
    Log::instance()->set_path(m_logpath.c_str());
    Log::instance()->set_maxfilesize(logfilesize);

    m_dns_resolver.setDNSServerList();
    m_myhost = conf_mgr.getString("0.0.0.0", m_fuck_header + "/smtpsvr/local_host");
    m_listen_port      = conf_mgr.getInt(25,  m_fuck_header + "/smtpsvr/listen_port");
    m_socket_ttl       = conf_mgr.getInt(60,  m_fuck_header + "/smtpsvr/socket_ttl");
    m_max_threads      = conf_mgr.getInt(64,  m_fuck_header + "/smtpsvr/max_threads");
    m_query_dns_ttl    = conf_mgr.getInt(3600,m_fuck_header + "/smtpsvr/dns_ttl");
	m_ignore_auth = conf_mgr.getInt(0, m_fuck_header + "/smtpsvr/ignore_auth");
    m_control_interval = conf_mgr.getInt(m_fuck_header + "/smtpsvr/control/interval");
    m_control_number   = conf_mgr.getInt(m_fuck_header + "/smtpsvr/control/number");
	//-------------------------
	g_send_retry = conf_mgr.getInt(3, m_fuck_header + "/smtpsvr/control/send_retry");
	if ((g_send_retry < 1 || g_send_retry > 10)) {
		LOG_MSG(("invalid : /smtpsvr/control => send_retry:%d\n", g_send_retry));
		g_send_retry = 3;
	}
	g_recv_retry = conf_mgr.getInt(5, m_fuck_header + "/smtpsvr/control/recv_retry");
	if ((g_recv_retry < 3 || g_recv_retry > 20)) {
		LOG_MSG(("invalid : /smtpsvr/control => recv_retry:%d\n", g_recv_retry));
		g_recv_retry = RETRY_LIMIT_EXCEEDED;
	}
	g_send_time_interval = conf_mgr.getInt(60 * 15, m_fuck_header + "/smtpsvr/control/send_time_interval");;
	if ((g_send_time_interval < 5 && g_send_time_interval > 3600 * 24 * 7)) {
		LOG_MSG(("invalid : /smtpsvr/control => send_time_interval:%d\n", g_send_time_interval));
		g_send_time_interval = 60 * 15;
	}
	g_max_fail_count  = conf_mgr.getInt(6, m_fuck_header + "/smtpsvr/control/fail_count");
	if ((g_max_fail_count > ((3600 * 24 * 30) / g_send_time_interval))) {
		LOG_MSG(("invalid : /smtpsvr/control => fail_count:%d\n", g_max_fail_count));
		g_max_fail_count = 6;
	}
	LOG_MSG(("send mail control:send_retry:%d,recv_retry:%d,send_time_interval:%d,fail_count:%d\n", g_send_retry, g_recv_retry, g_send_time_interval, g_max_fail_count));
	g_mail_save_dir = conf_mgr.getString("data/", m_fuck_header + "/smtpsvr/control/save_dir");
	if (*g_mail_save_dir.rbegin() != '/') {
		g_mail_save_dir.push_back('/');
	}
	struct stat stat_info;
	if (lstat(g_mail_save_dir.c_str(), &stat_info) < 0) {
 	 	if (errno == ENOENT) {
	 		if (mkdir(g_mail_save_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
				LOG_MSG(("mkdir fail:%s\n", g_mail_save_dir.c_str()));
 			 	return false;
 			} 
		}
		else {
			LOG_MSG(("bad path:%s\n", g_mail_save_dir.c_str()));
			return false;
		}
	}
	else if (false == S_ISDIR(stat_info.st_mode)) {
		LOG_MSG(("is not dir:%s\n", g_mail_save_dir.c_str()));
		return false;
	}
	LOG_MSG(("save_dir:%s\n", g_mail_save_dir.c_str()));
	//-------------------------

    // mysql
    mysql_host = conf_mgr.getString("", m_fuck_header + "/smtpsvr/mysql_host");
    mysql_port = conf_mgr.getInt(m_fuck_header + "/smtpsvr/mysql_port");
    mysql_user = conf_mgr.getString("", m_fuck_header + "/smtpsvr/mysql_user");
    mysql_passwd = conf_mgr.getString("", m_fuck_header + "/smtpsvr/mysql_passwd");
    mysql_dbname = conf_mgr.getString("", m_fuck_header + "/smtpsvr/mysql_dbname");
    mysql_charset = conf_mgr.getString("", m_fuck_header + "/smtpsvr/mysql_charset");

    if (m_control_interval < 1 || m_control_number < 1){
        LOG_MSG(("error: /smtpsvr/control => interval or number\n"));
        return false;
    }
    
    g_default_helo = conf_mgr.getString("", m_fuck_header + "/smtpsvr/smtp_helo");

    LOG_MSG(("log path: %s\n",m_logpath.c_str()));
    LOG_MSG(("logfilesize: %u\n",logfilesize));
    LOG_MSG(("load configure file: %s\n",file.c_str()));
    LOG_MSG(("/smtpsvr/smtp_helo: %s\n",g_default_helo.c_str()));
    m_control_total = 0;

    m_quota_default_interval = QUOTA_DEFAULT_INTERVEL;
    m_quota_default_value    = QUOTA_DEFAULT_VALUE; 
    // 
    load_delivery_quota(conf_mgr);
    load_mail_eXchange(conf_mgr);
    reflush_white_list(&conf_mgr);
    //auth
    typeConfPairList lp;
    conf_mgr.getSubItemPairList(m_fuck_header + "/smtpsvr/auth", lp );
    for ( typeConfPairList::const_iterator it = lp.begin(); it != lp.end(); ++it ) {
        auth_node_t alt;
        alt.auth_user = it->first;
        alt.auth_pswd = it->second;
        m_auth_list.push_back(alt);
    }

	//domain_map
	typeConfPairList domin_list;
	conf_mgr.getSubItemPairList(m_fuck_header + "/smtpsvr/domain_map", domin_list);
	for (typeConfPairList::const_iterator it = domin_list.begin(); it != domin_list.end(); ++it) {
		m_domain_map[it->first] = it->second;
		LOG_MSG(("/smtpsvr/domain_map:(%s -> %s)\n", it->first.c_str(), it->second.c_str()));
	}
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGINT,  SIG_IGN);
    //maskSignal();
    //unmaskSignal();

    LOG_MSG(("listen port:%d\n",m_listen_port));
    return true;
}

static int event_sleep(unsigned long mSec)
{
	struct timeval tv;
	tv.tv_sec = mSec / 1000;
	tv.tv_usec = (mSec % 1000) * 1000;
	int ret_code;
	do {
		ret_code = select(0, NULL, NULL, NULL, &tv);
	} while (ret_code < 0 && errno == EINTR);

	return ret_code;
}//

bool SmtpSvr::g_stop_send = false;
pthread_mutex_t SmtpSvr::retry_mail_queue_lock = PTHREAD_MUTEX_INITIALIZER;
std::list<Smtp_Context*> SmtpSvr::keep_retry_queue;
//
void* SmtpSvr::timeRetrySendMail(void*)
{
	long wait_time = 0, now_time = 0;
	std::list<Smtp_Context*>::iterator task_it;
	std::list<Smtp_Context*> out_tasks;
	LOG_MSG(("timeRetrySendMail thread start\n"));
	while (!g_stop_send) {
	 	wait_time = (1000 * 60 * 5);		//不做任何事情时，等待5分钟
		if (getRetryTasks(out_tasks) > 0) {
			now_time = time(NULL);
			for (task_it = out_tasks.begin(); task_it != out_tasks.end();) {
				if (0 == SmtpSvr::instance()->connect_mail_eXchange((*task_it))) {
					(*task_it)->print("RetryOK");
					if (0 == delRetryTask((*task_it))) {
						task_it = out_tasks.erase(task_it);
						LOG_MSG(("del retry taskmail, success!\n"));
	 					continue;
					}
	 				LOG_MSG(("del retry taskmail, fail\n"));
				}
				else {
					if (g_max_fail_count > (*task_it)->cur_retry_count++) {
						(*task_it)->next_send_time = now_time + g_send_time_interval;
					}
					else {
						(*task_it)->print("RetryFailed");
						if (0 == delRetryTask((*task_it))) {
							task_it = out_tasks.erase(task_it);
							LOG_MSG(("del retry taskmail, success!\n"));
	 	 					continue;
						}
	 	 				LOG_MSG( ("del retry taskmail, fail\n"));
	 	 			}
				}
	 	 		++task_it;
			}//for
			if (!out_tasks.empty()) {
				if (keep_retry_queue.size() < g_queue_limit_size) { 
					addRetryTasks(out_tasks);
				}else {
					for (task_it = out_tasks.begin(); task_it !=  out_tasks.end(); ++task_it) {
						saveRetryTask((*task_it));			//TODO:这里仅更新失败次数、下次发送时间等信息，确重写文件
						delete (*task_it); 
					}
				 	out_tasks.clear();
				} 
			}
	 	 	wait_time = (1000 * g_send_time_interval);
		}
		else if(keep_retry_queue.size() < 1){
			//扫描目录
			std::list<Smtp_Context*> load_tasks;
			int load_size = loadTasksFromDir(g_mail_save_dir.c_str(), load_tasks);
			if (load_size > 0) {
				LOG_MSG(("load task from dir:%s, num:%d,%d\n", g_mail_save_dir.c_str(), load_size, load_tasks.size()));
				addRetryTasks(load_tasks);
		 	 	wait_time = 1000 * g_send_time_interval;
		 	} 
		}
		event_sleep(wait_time);
	}//while
	//cleanup
	for (task_it = keep_retry_queue.begin(); task_it != keep_retry_queue.end(); ++task_it) {
		delete (*task_it);
	}
	keep_retry_queue.clear();
	LOG_MSG( ("timeRetrySendMail thread end\n"));

	return NULL;
}

int SmtpSvr::addRetryTask(Smtp_Context* context)
{
	if (NULL == context) { 
		return -1;
	}
	saveRetryTask(context);
	pthread_mutex_lock(&retry_mail_queue_lock);
	keep_retry_queue.push_back(context);
	pthread_mutex_unlock(&retry_mail_queue_lock);

	return 0;
}//

int SmtpSvr::addRetryTasks(const std::list<Smtp_Context*>& tasks)
{
	pthread_mutex_lock(&retry_mail_queue_lock);
	keep_retry_queue.insert(keep_retry_queue.end(), tasks.begin(), tasks.end());
	pthread_mutex_unlock(&retry_mail_queue_lock);

	//LOG_MSG( ("now queue size:%d\n", keep_retry_queue.size()));
	return 0;
}//

int SmtpSvr::getRetryTasks(std::list<Smtp_Context*>& out_tasks)
{
	if (keep_retry_queue.empty()) {
		return 0;
	}else {
	 	out_tasks.clear();
		long now_time = time(NULL);
		std::list<Smtp_Context*>::iterator task_it;
		int cur_queue_size = keep_retry_queue.size();
		//此时，遍历链表，同时间不会发生移除操作
		if (cur_queue_size > g_queue_limit_size) {
			task_it = keep_retry_queue.begin();
	  		std::advance(task_it, (g_queue_limit_size - cur_queue_size));
		}
		else {
	 		for (task_it = keep_retry_queue.begin(); task_it != keep_retry_queue.end(); ++task_it) {
	 			if ((*task_it)->next_send_time > now_time) {
					break;
	 			}
	 		}//for
		}
		if (task_it == keep_retry_queue.begin()) {
			return 0;
		}
		pthread_mutex_lock(&retry_mail_queue_lock);
		out_tasks.splice(out_tasks.begin(), keep_retry_queue, keep_retry_queue.begin(), task_it);
	 	pthread_mutex_unlock(&retry_mail_queue_lock);
	}  

	return out_tasks.size();
}

static int readStringDataFromFile(std::fstream& finput, int& size, std::string& out_data)
{	
	size = 0; out_data = ""; 
	if (finput.eof()) {
		return -1;
	}
	char data_buffer[1024] = { 0 };
	int read_size = 0, once_size = 0;
	finput.read((char*)&size, sizeof(size));
	if (!finput.good()) {
		return -1;
	}
	while (read_size < size && !finput.eof()) {
	  	finput.read(data_buffer, std::min(1024, size - read_size));
		once_size = finput.gcount();
		if (once_size < 1) break;
		out_data.append(data_buffer, once_size);
	 	read_size += once_size;
	}
	
	return read_size;
}

static bool writeStringDataIntoFile(std::fstream& fout, const std::string& in_data)
{
	int data_size = 0;
	data_size = in_data.size();
	fout.write((char*)&data_size, sizeof(data_size));
	fout.write(in_data.data(), data_size);

	return fout.good();
}

static void test_loadfile(const char* filepath, int mail_data_size)
{
 	Smtp_Context* cxx = SmtpSvr::loadTaskFromFile(filepath);
	if (NULL == cxx) {
		LOG_MSG(("load mail fail:%s\n", filepath));
	}
 	else {
  		cxx->print("test_load");
		LOG_MSG(("load mail size:%d, %d\n", mail_data_size, cxx->sendbuffer.begin()->size()));
		std::fstream ff("ddd.o", ios_base::out | ios_base::binary);
		ff.write(cxx->sendbuffer.begin()->data(), cxx->sendbuffer.begin()->size());
		ff.close();
	 	delete cxx;
	} 
}

int SmtpSvr::saveRetryTask(Smtp_Context* context)
{
	if (NULL == context) { 
		return -1;
	}
	tm lt;
	time_t tt = time(0); localtime_r(&tt, &lt);
	char path[256] = { 0 };
	static int sequeue = 0;

        if (context->file_path.empty()) {
		sequeue = (sequeue % 999999) + 1;
		struct stat stat_info;
		snprintf(path, sizeof(path), "%s%04d%02d%02d/", g_mail_save_dir.c_str(), lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday);
		mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		//if ((lstat(path, &stat_info) < 0 && errno == ENOENT) || (false == S_ISDIR(stat_info.st_mode))) {
		//}
		context->file_path = path;
		snprintf(path, sizeof(path), "H%02d%02d%02d.P%05d.S%06d", lt.tm_hour, lt.tm_min, lt.tm_sec, getpid(), sequeue);
		context->file_path += path;
	}
	std::fstream fout(context->file_path.c_str(), ios_base::out | ios_base::binary);

	if (fout.good()) {  
		fout.write((char*)&context->cur_retry_count,  sizeof(context->cur_retry_count));
		fout.write((char*)&context->next_send_time,  sizeof(context->next_send_time));
		fout.write((char*)&context->bytes_total,  sizeof(context->bytes_total));
		fout.write((char*)&context->ab_size,  sizeof(context->ab_size));
		fout.write((char*)&context->point,  sizeof(context->point));
		writeStringDataIntoFile(fout, context->smtp_helo);
		writeStringDataIntoFile(fout, context->smtp_mail);
		writeStringDataIntoFile(fout, context->smtp_rcpt);
		writeStringDataIntoFile(fout, context->smtp_user);
		writeStringDataIntoFile(fout, context->smtp_pswd);
		writeStringDataIntoFile(fout, context->real_sender);
		writeStringDataIntoFile(fout, context->msg_header);
		writeStringDataIntoFile(fout, context->rcpt_domain);
		writeStringDataIntoFile(fout, context->x_mid);
		int mail_data_size = 0;
		fout.write((char*)&mail_data_size,  sizeof(mail_data_size));
		for (list<string>::iterator its = context->sendbuffer.begin(); its != context->sendbuffer.end(); ++its) {
	 		fout.write(its->data() , its->size());
		 	mail_data_size += its->size();
		}
		//seekg()是设置读位置，seekp是设置写位置
		fout.seekp(0 - (mail_data_size + sizeof(mail_data_size)), ios_base::cur);
		fout.write((char*)&mail_data_size,  sizeof(mail_data_size));
		fout.close();
		//test_loadfile(context->file_path.c_str(), mail_data_size);
	}
	else {
		LOG_MSG(("save mail fail:%s\n", context->file_path.c_str()));
	} 
}//

int SmtpSvr::delRetryTask(Smtp_Context* context)
{
	if (NULL == context) {
		return -1;
	}
	if (0 != remove(context->file_path.c_str())) {
		LOG_MSG(("remove task file:%s, fail:%s\n", context->file_path.c_str(), strerror(errno)));
	}
	delete context;
	context = NULL;

	return 0;
}//

int SmtpSvr::loadTasksFromDir(const char *c_dir_path, std::list<Smtp_Context*>& tasks)
{
	if (NULL == c_dir_path) { 
		LOG_MSG(("invalid directory\n"));
		return -1;
	}
	int task_num = 0;
	DIR *dir_ent = NULL;
	struct dirent *entry = NULL;
	char file_path[512] = { 0 };
	if ((dir_ent = opendir(c_dir_path)) == NULL) {
		LOG_MSG(("cannot open directory: %s\n", c_dir_path));
		return -1;
	}
	//chdir(dir);
	while ((entry = readdir(dir_ent)) != NULL) {
		if (entry->d_type & DT_DIR) {
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) {
				continue;
			}
			snprintf(file_path, 511, "%s/%s", c_dir_path, entry->d_name);
			int sub_tasks = loadTasksFromDir(file_path, tasks);
			if (sub_tasks > 0) {
				task_num += sub_tasks;
			} 
		}
		else {
			snprintf(file_path, 511, "%s/%s", c_dir_path, entry->d_name);
		 	Smtp_Context* context = loadTaskFromFile(file_path);
			if (context) {
				tasks.push_back(context);
				++task_num;
			}
		} 
	}
	closedir(dir_ent);
	if (0 == task_num && strcmp(c_dir_path, g_mail_save_dir.c_str())) {
		if (remove(c_dir_path) != 0) {
			LOG_MSG(("remove task dir:%s, fail:%s\n", c_dir_path, strerror(errno)));
		} 
	}

	return task_num; 
}//

Smtp_Context* SmtpSvr::loadTaskFromFile(const char* c_file_path)
{
	long now_time = time(NULL); 
	Smtp_Context *context = NULL;
	std::fstream finput(c_file_path, ios_base::in | ios_base::binary);
	if (finput.good()) {
		context = new Smtp_Context;
		context->file_path = c_file_path;
		finput.read((char*)&context->cur_retry_count, sizeof(context->cur_retry_count));
		finput.read((char*)&context->next_send_time, sizeof(context->next_send_time));
		if (context->next_send_time < now_time) {
			context->next_send_time = now_time;
		}
		finput.read((char*)&context->bytes_total, sizeof(context->bytes_total));
		finput.read((char*)&context->ab_size, sizeof(context->ab_size));
		finput.read((char*)&context->point, sizeof(context->point));
		int line_size = 0, out_size = 0;
		out_size = readStringDataFromFile(finput, line_size, context->smtp_helo);
		assert(out_size == line_size);
		out_size = readStringDataFromFile(finput, line_size, context->smtp_mail);
		assert(out_size == line_size);
		out_size = readStringDataFromFile(finput, line_size, context->smtp_rcpt);
		assert(out_size == line_size);
		out_size = readStringDataFromFile(finput, line_size, context->smtp_user);
		assert(out_size == line_size);
		out_size = readStringDataFromFile(finput, line_size, context->smtp_pswd);
		assert(out_size == line_size);
		out_size = readStringDataFromFile(finput, line_size, context->real_sender);
		assert(out_size == line_size);
		out_size = readStringDataFromFile(finput, line_size, context->msg_header);
		assert(out_size == line_size);
		out_size = readStringDataFromFile(finput, line_size, context->rcpt_domain);
		assert(out_size == line_size);
		out_size = readStringDataFromFile(finput, line_size, context->x_mid);
		assert(out_size == line_size);
		std::string mail_data;
		out_size = readStringDataFromFile(finput, line_size, mail_data);
		assert(out_size == line_size);
		context->sendbuffer.push_back(mail_data);
	   	finput.close(); 
	}
	else {
		LOG_MSG(("load mail fail:%s\n", c_file_path));
	}    

	return context;
}

void SmtpSvr::loadControlInfoFromDB(HR_map_t& client_control_infomap)
{
	MYSQL*& db_client_handle = this->_db_client_handle;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row = NULL;
	try {
		do {
			if (!(db_client_handle = MysqlConnect(db_client_handle))) {
				break;
			}
			string sqlstr = "select size,quarter,hour,day,ip from smtp ";
			if (0 != mysql_query(db_client_handle, sqlstr.c_str())) {
				LOG_MSG(("mysql_query[%s]: %s\n", sqlstr.c_str(), mysql_error(db_client_handle)));
				break;
			}
			result = mysql_store_result(db_client_handle);
			if (mysql_num_rows(result) < 1 || mysql_num_fields(result) < 1) {
				break;
			}
			HR_control_t control_data;
			std::string client_ip;
			while ((row = mysql_fetch_row(result))) {
				control_data.ab = atoi(row[0]) * 1024;			//邮件大小
				control_data.rt[0].total = atoi(row[1]);
				control_data.rt[1].total = atoi(row[2]);
				control_data.rt[2].total = atoi(row[3]);
				client_ip = row[4];
				client_control_infomap.insert(std::make_pair(client_ip, control_data));
				//LOG_MSG(("client_ip:[%s]\n", client_ip.c_str()));
			}
		} while (0);
		//clean
		if (result) {
			mysql_free_result(result);
		}
		//改为长连接
		/*if (db_client_handle) {
			mysql_close(db_client_handle);
		}*/
	}
	catch (...) {
		LOG_MSG(("load db config fail\n"));
	}
}//

void SmtpSvr::reflush_white_list(ConfigureMgr* cfg_mgr_in)
{
        std::string ip_str_list;
        if (cfg_mgr_in) {
		ip_str_list = cfg_mgr_in->getString(m_fuck_header + "/smtpsvr/ip_white_list");
	}else {
		ConfigureMgr conf_mgr;
    		if(!conf_mgr.loadConfFile(cfg_path.c_str())) {
   			ip_str_list = last_ip_str_list;	
		 }else {
			ip_str_list = conf_mgr.getString(m_fuck_header + "/smtpsvr/ip_white_list");
		}
	}
	//
	HR_map_t client_control_infomap;
	loadControlInfoFromDB(client_control_infomap);
	std::string white_client;
	map<std::string, HR_control_t>::iterator it, end_it = client_control_infomap.end();
	for (it = client_control_infomap.begin(); it != end_it; ++it) {
		white_client += (it->first + ";");
	}
 	if (ip_str_list != last_ip_str_list || white_client != last_white_client) {
		m_ipsegment.clear();			//安全，仅主线程访问
		load_ip_white_list(ip_str_list);
		load_ip_white_list(white_client);
		last_ip_str_list = ip_str_list; 
		last_white_client = white_client;
	}
}//

void SmtpSvr::doIdleProcess()
{
	static time_t last_time = 0, now_time = 0;
	now_time = time(NULL);
	//15分钟
	//if (now_time > (last_time + 15 * 60)) {
		reflush_white_list();
		last_time = now_time;
	//}
}//

int SmtpSvr::onRun( void )
{
    ::daemon(1,0); //linux => man daemon

//    if (m_dbConnstr.size()){
//        OracleDB::initialize(1);
//        if (0 != m_dbOrcl.connect(m_dbConnstr.c_str())){
//            LOG_MSG(("orcl::connect error '%s'\n", m_dbConnstr.c_str()));
//            m_dbConnstr.clear();
//        }
//    }
	do {
		//启动定时发送线程
		pthread_t pid_time_send;
		pthread_attr_t time_atrr;
		pthread_attr_init(&time_atrr);
		pthread_attr_setdetachstate(&time_atrr, PTHREAD_CREATE_DETACHED);
		int nret_code = pthread_create(&pid_time_send, &time_atrr, timeRetrySendMail, NULL);
		if (nret_code != 0) {
			LOG_ERROR(nret_code, "pthread_create time_retry_send_mail fail!\n");
			break;
		}
		pthread_attr_destroy(&time_atrr);
		//
		if (0 > (m_listen_handle = create_server_socket(m_myhost, m_listen_port))) {
			break;
		}
		Smtp_Context *pctx = SmtpSvr::alloc();
		pctx->client_handle = m_listen_handle;
		m_ep_handle = epoll_create(MAX_EPOLL_EVENTS);
		register_epollin_event(m_ep_handle, pctx);
		pthread_t ntid;
		epoll_event all_events[MAX_EPOLL_EVENTS];

		while (1) {
			int number = epoll_wait(m_ep_handle, all_events, MAX_EPOLL_EVENTS, 1000 * m_socket_ttl);
			if (number == -1) {
				if (errno == EINTR)
					continue;
				break;
			}
			else {
				doIdleProcess();
			}

			for (int n = 0; n<number; n++) {
				pctx = (Smtp_Context*)(all_events[n].data.ptr);
				if (pctx->client_handle == m_listen_handle) {
					this->poll4accept(m_listen_handle, m_ep_handle);
				}else if (all_events[n].events & EPOLLIN) {
					epoll_ctl(m_ep_handle, EPOLL_CTL_DEL, pctx->client_handle, &all_events[n]);
					AutoGuard<SyncLock> tmplock(SmtpSvr::m_thrcnt_lock);
					int perr = pthread_create(&ntid, NULL, SmtpSvr::thread_service, pctx);
					if (perr != 0) {
						LOG_ERROR(perr, "pthread_create");
						LOG_MSG(("close connection:[%s:%d]\n", pctx->client_host.c_str(), pctx->client_port));
						SmtpSvr::free(pctx);
						//LOG_MSG(("=============== end ===============\n\n"));
					}else {
						SmtpSvr::m_thread_count++;

					}
					//LOG_MSG(("thread count :[%d]\n",SmtpSvr::m_thread_count)); 
				}
			}
		}//main_event_loop
	} while (0);
	//cleanup
    close(m_listen_handle);
    LOG_MSG(("application terminal\n"));

    return 0;
}

int SmtpSvr::create_server_socket(string& host,  int port )
{
    int socket_handle = 0;
    if((socket_handle = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
    {
        LOG_ERROR(errno,"socket");
        return -1;
    }

    fcntl(socket_handle, 
        F_SETFL, 
        fcntl(socket_handle, F_GETFD, 0)|O_NONBLOCK);

    int reuse = 1;
    int length =sizeof(int);

    setsockopt(socket_handle, 
        SOL_SOCKET, 
        SO_REUSEADDR,
        &reuse, 
        length);

    struct sockaddr_in address;
    bzero(&address, sizeof(address));  
    address.sin_family      = PF_INET;  
    address.sin_port        = htons(port);  
    address.sin_addr.s_addr = inet_addr(host.c_str());

    if (bind(socket_handle, 
        (struct sockaddr *) &address, 
        sizeof(struct sockaddr)) == -1)
    {
        LOG_ERROR(errno,"bind");
        return -1;
    }

    if (listen(socket_handle, 64) == -1)   
    {
        LOG_ERROR(errno,"listen");
        return -1;
    }

    return socket_handle;
}


int SmtpSvr::register_epollin_event( int ep_handle, void* context )
{
    epoll_event evt;
    evt.events = EPOLLIN|EPOLLET;
    evt.data.ptr = context;

    Smtp_Context* p = (Smtp_Context*)context;

    fcntl(p->client_handle, 
        F_SETFL, 
        fcntl(p->client_handle, F_GETFD, 0)|O_NONBLOCK);

    linger lg_enable = {1,m_socket_ttl};
    if (0 != setsockopt(p->client_handle, 
        SOL_SOCKET, 
        SO_LINGER, 
        (char*)&lg_enable, 
        sizeof(lg_enable)))
    {
        LOG_MSG(("setsockeopt failed:%s\n",strerror(errno)));
    }

    if (epoll_ctl(ep_handle, EPOLL_CTL_ADD, p->client_handle, &evt) < 0)
    {
        LOG_ERROR(errno,"epoll_ctl(add)");
        return -1;
    }

    return 0;
}


int SmtpSvr::remove_epollin_event( int ep_handle, int socket_handle )
{
    static epoll_event ev;
    if (epoll_ctl(ep_handle, EPOLL_CTL_DEL, socket_handle, &ev) < 0)
    {
        LOG_ERROR(errno,"epoll_ctl(del)");
        return -1;
    }

    return 0;
}

int SmtpSvr::recv_n(int handle, void* buffer, int length,timeval* ttl/*=NULL*/)
{
    if (handle < 1){
        return -1;
    }

    timeval timeout;
    if (ttl != NULL){
        timeout.tv_sec = ttl->tv_sec;
        timeout.tv_usec = ttl->tv_usec;
        ttl = &timeout;
    }

    fd_set rdset;
    FD_ZERO(&rdset);
    FD_SET(handle, &rdset);
    select(handle+1, &rdset, NULL, NULL, ttl);
			    	
    long bytes_transferred = 0;
    memset(buffer,0x00,length);
    while (length > bytes_transferred )
    {
        int bytes = ::recv(handle, 
                           (char*)buffer + bytes_transferred ,
                           length - bytes_transferred,
                           0);
        switch (bytes)
        {
        case -1:
            if (errno == EINTR)
                continue;
            if (errno == EAGAIN)
			    return bytes_transferred;
        case 0:
            return bytes;
        default:
            bytes_transferred += bytes;
            break;
        }
    }
    return bytes_transferred;
}

int SmtpSvr::recv_a(int handle , string& data, timeval *ttl)
{
    if (handle < 1)
        return -1;

    timeval timeout;
    if (ttl != NULL){
        timeout.tv_sec = ttl->tv_sec;
        timeout.tv_usec = ttl->tv_usec;
        ttl = &timeout;
    }

    fd_set rdset;
    FD_ZERO(&rdset);
    FD_SET(handle, &rdset);
    select(handle+1, &rdset, NULL, NULL, ttl);

    char buffer[256];
    data.clear();

    while (1)
    {
        int bytes = ::recv(handle, buffer , sizeof(buffer), 0);

        switch (bytes)
        {
        case -1:
            if (errno == EINTR)
                continue;
            if (errno == EAGAIN )
			    return data.length();
        case 0:
            return bytes;
        default:
            data.append(buffer,bytes);
            break;
        }
    }
    return data.length();
}

int SmtpSvr::send_n(int handle, const void* buffer, int length,timeval *ttl/*=NULL*/)
{
    if (handle < 1){
        return -1;
    }

    long bytes_transferred = 0;
    while (length > bytes_transferred )
    {
        int bytes = ::send(handle, 
                          (char*)buffer + bytes_transferred ,
                          length - bytes_transferred,
                          0);
        switch (bytes)
        {
        case -1:
            if (errno == EAGAIN){
            	timeval timeout;
			    if (ttl != NULL){
			        timeout.tv_sec = ttl->tv_sec;
			        timeout.tv_usec = ttl->tv_usec;
			        ttl = &timeout;
			    }
			
			    fd_set evset;
			    FD_ZERO(&evset);
			    FD_SET(handle, &evset);
			    if (select(handle+1,NULL,&evset,NULL,ttl) < 1) {
			    	LOG_MSG(("send timeout:%s\n",strerror(errno)));
                    return bytes_transferred;
                }
                continue;
            }
            return -1;
        case 0:
            return 0;
        default:
            bytes_transferred += bytes;
            break;
        }
    }
    return bytes_transferred;
}

void SmtpSvr::poll4accept(int listen_handle, int ep_handle)
{
    int new_handle = -1;
    socklen_t length = sizeof(sockaddr_in);
    struct sockaddr_in from;

    int xyz=0; 
    char *ptext = NULL;

    do 
    {
        xyz++;
        time_t endpoint = time(0);

        if (m_control_total == 0)
            m_control_point = time(0);

        int interval = (int)difftime(endpoint,m_control_point);
        if (difftime(endpoint,m_control_point) > m_control_interval)
        {
            m_control_total = 0;
            m_control_point = time(0);
        }

        Smtp_Context* pctx = NULL;
        memset(&from, 0x00,sizeof(from));

        new_handle = accept(listen_handle,
            (struct sockaddr *)&from,
            &length);

        if (new_handle == -1){
            break;
        }

        string host = inet_ntoa(from.sin_addr);
        unsigned short port = ntohs(from.sin_port);

        try{
            if (check_ipsegment(from.sin_addr) != 0  
				//&& (_client_control_infomap && _client_control_infomap->end() == _client_control_infomap->find(host))
				){
                ptext = (char*)SMTP_REPLY_506;
                throw -1;
            }

            //LOG_DEBUG(("[smtpsvr/control]-%d difftime:%ds, number:%d, control: %d/%ds\n",
                //xyz,interval,m_control_total,m_control_number,m_control_interval));

            if (m_control_total++ >= m_control_number){
                LOG_MSG(("Connection exceed limit:[%s:%d] %d\n",host.c_str(),port, xyz));
                ptext = (char*)SMTP_REPLY_601;
                throw -1;
            }

            LOG_MSG(("connection from:[%s:%d] %d\n",host.c_str(),port,xyz));

            pctx = SmtpSvr::alloc();
            pctx->client_host    = host;
            pctx->client_port    = port;
            pctx->client_handle  = new_handle;
            pctx->command        = ZERO;
            pctx->next_steps     = HELO|EHLO|NOOP|RSET|QUIT;
            pctx->remote_handle  = -1;
            pctx->remote_port    = 0;
            pctx->bytes_total    = 0;
            pctx->point          = time(0);
            pctx->sendbuffer.clear();

            if (register_epollin_event(ep_handle,pctx) == -1)
            {
                pctx->client_handle = -1;
                ptext = (char*)SMTP_REPLY_507;
                throw -3;
            }

            SmtpSvr::send_n(new_handle, SMTP_REPLY_220, sizeof(SMTP_REPLY_220)-1,&SmtpSvr::timeval_zero);
        }
        catch (...)
        {
            SmtpSvr::send_n(new_handle,ptext, strlen(ptext),&SmtpSvr::timeval_zero);
            LOG_MSG(("rejectd -> Client-%d:[%s:%d], %s",xyz,host.c_str(),port,ptext));
            //LOG_MSG(("closing channel-%d:[%s:%d]\n\n",xyz,host.c_str(),port));

            if (new_handle != -1){
                shutdown(new_handle,SHUT_RDWR);
                close(new_handle);
            }

            if (pctx){
                SmtpSvr::free(pctx);
            }
        }

    } while (1);
}

void SmtpSvr::onSignal( int sign )
{
    LOG_MSG(("catch signal: [%d]\n",sign));
    switch (sign)
    {
    case SIGTERM:
    case SIGINT:
        break;
    default:
        break;
    }
}

void* SmtpSvr::thread_service( void * param )
{
    pthread_detach(pthread_self());

    Smtp_Context* pctx = (Smtp_Context*) param;

    string data;
    int retry_count = 0;
    int length = 1024;
    int nrecv_time_sum = g_recv_retry;
	bool send_success = false;

    char* buffer = new(std::nothrow) char[length];
    if (buffer == NULL){
        LOG_MSG(("[system error] alloc(%d) memory failed\n",length));
        goto _thread_end_;
    }
    while (retry_count < nrecv_time_sum && pctx->command != QUIT) {
        timeval ttl = {SmtpSvr::instance()->m_socket_ttl,0};
        int bytes = SmtpSvr::recv_n(pctx->client_handle, buffer, sizeof(buffer), &ttl);
        if (bytes < 1) {
            LOG_MSG(("Received from %s:%d failed:%s\n", pctx->client_host.c_str(),pctx->client_port,strerror(errno)));
            break;
        }

        size_t pos_index = string::npos;
        int pos_offset = data.length()>1 ? data.length()-2:0;
        data.append(buffer,bytes);
        while (data.length() > 0 && pctx->command != QUIT) {
			//接收到块尾
            pos_index = data.find(SMTP_CRLF,pos_offset);
            if (pos_index == string::npos){
                break;
            }

            string line = data.substr(0,pos_index);
            data.erase(0,pos_index+strlen(SMTP_CRLF));
            pos_offset = 0;

            if (0 != SmtpSvr::instance()->handle_line(line, pctx)){
                retry_count ++;
            }
            else{
                retry_count = 0;
            }

            if (retry_count >= nrecv_time_sum) {
                LOG_MSG(("retry limit exceeded , >%d\n",g_recv_retry));
                //pctx->print("failed");
                pctx->list_history.clear();
                break;
            }
        }
    }//recv_n
#if SEND_ONCE_VERSION
	close(pctx->client_handle);
	//
	if (pctx && pctx->cur_retry_count == 0) {
		for (int nretry = 0; !send_success && nretry < g_send_retry; ++nretry) {
			switch (SmtpSvr::instance()->connect_mail_eXchange(pctx)) {
			case 0:
				send_success = true;
				break;
			case -2:
			case -3:
				LOG_MSG(("send retry:dnslookup or connect fail!\n"));
				event_sleep(2 * 1000);			//等待2s
				break;
			default:
				pctx->list_history.clear();
			}
		}//for_retry
		pctx->print(send_success ? "OK" : "failed");
	}
#endif

_thread_end_:
    //LOG_MSG(("closing channel:[%s:%d]\n\n",pctx->client_host.c_str(),pctx->client_port));
	if (send_success || (NULL != pctx && pctx->cur_retry_count == -1)) {
		SmtpSvr::free(pctx);
	}
	else if (NULL != pctx){
		//
		//if (strncmp(ctx->reply_text.c_str(), "5", 1) != 0) {
		//}
		pctx->next_send_time = (long)time(NULL) + g_send_time_interval;
		addRetryTask(pctx);
	}
    if (buffer){
        delete [] buffer;
        buffer = NULL;
    }
    AutoGuard<SyncLock> tmplock(SmtpSvr::m_thrcnt_lock);
    SmtpSvr::m_thread_count--;
    //LOG_MSG(("=============== end ===============\n\n"));
    return 0;
}

int SmtpSvr::on_helo(string& line,Smtp_Context* ctx)
{
    const char* p = line.data() + 4;
    while (*p==' '|| *p=='\t')
        p++;

    if (*p == '\0')
    {
        ctx->reply_text = SMTP_REPLY_508;
        return -1;
    }
    
    line[0] = 'H';line[1] = 'E';
    ctx->smtp_helo = line;//HELO EHLO
    ctx->next_steps = HELO|EHLO|MAIL|RSET|NOOP|QUIT|AUTH;
    ctx->reply_text = SMTP_REPLY_250;
    return 0;
}

int SmtpSvr::on_mail(string& line,Smtp_Context* ctx)
{
    const char* p = line.data() + 5;
    while (*p==' '|| *p=='\t')
        p++;

    if (strncasecmp(p,"FROM",4) != 0)
    {
        LOG_MSG(("Command not implemented:[%s]\n",line.data()));
        ctx->reply_text = SMTP_REPLY_502;
        return -1;
    }

    p = p + 4; 
    while (*p==' '|| *p=='\t') 
        p++;

    if (*p != ':'){
        LOG_MSG(("Command not implemented:[%s]\n",line.data()));
        ctx->reply_text = SMTP_REPLY_502;
        return -1;
    }

    ctx->smtp_mail = ++p;
    ctx->smtp_mail.erase(0,ctx->smtp_mail.find_first_not_of(" \t<"));
    ctx->smtp_mail.erase(ctx->smtp_mail.find_last_not_of(" \t>")+1);

    std::transform(ctx->smtp_mail.begin(),
        ctx->smtp_mail.end(),
        ctx->smtp_mail.begin(),
        (int(*)(int))tolower);

    if ( !MimeParser::isValidEmailAddress( DwString(ctx->smtp_mail.data(), ctx->smtp_mail.size())))
    {
        LOG_MSG(("Invaild Mail user:[%s]\n",ctx->smtp_mail.data()));
        ctx->reply_text = SMTP_REPLY_509;
        return -1;
    }

    ctx->next_steps = HELO|EHLO|MAIL|RCPT|RSET|NOOP|QUIT;
    ctx->reply_text = SMTP_REPLY_250;
    return 0;
}

int SmtpSvr::on_rcpt(string& line,Smtp_Context* ctx)
{
    const char* p = line.data() + 5;
    while (*p==' '|| *p=='\t')
        p++;

    if (strncasecmp(p,"TO",2) != 0)
    {
        LOG_MSG(("Command not implemented:[%s]\n",line.data()));
        ctx->reply_text = SMTP_REPLY_502;
        return -1;
    }

    p = p + 2; 
    while (*p==' '|| *p=='\t') 
        p++;

    if (*p != ':')
    {
        LOG_MSG(("Command not implemented:[%s]\n",line.data()));
        ctx->reply_text = SMTP_REPLY_502;
        return -1;
    }

    ctx->smtp_rcpt = ++p;
    ctx->smtp_rcpt.erase(0,ctx->smtp_rcpt.find_first_not_of(" \t<"));
    ctx->smtp_rcpt.erase(ctx->smtp_rcpt.find_last_not_of(" \t>")+1);

    if ( !MimeParser::isValidEmailAddress( DwString(ctx->smtp_rcpt.data(), ctx->smtp_rcpt.size())))
    {
        LOG_MSG(("Invaild Rcpt user:[%s]\n",ctx->smtp_rcpt.data()));
        ctx->reply_text = SMTP_REPLY_509;
        return -1;
    }

    while (*p != '@' && *p != '\0') 
        p++;
    p++;

    string previous_domain = ctx->rcpt_domain;
    ctx->rcpt_domain = ctx->smtp_rcpt.substr(ctx->smtp_rcpt.find("@") + 1);

    std::transform(ctx->rcpt_domain.begin(),
        ctx->rcpt_domain.end(),
        ctx->rcpt_domain.begin(),
        (int(*)(int))tolower);

    /*if ( ctx->rcpt_domain != previous_domain)
    {
        if (ctx->remote_handle > 0){
            close(ctx->remote_handle);
            ctx->remote_handle = -1;
        }
    } */

    ctx->next_steps = HELO|EHLO|MAIL|RCPT|DATA|RSET|NOOP|QUIT;
    //ctx->smtp_rcpt = line;
    ctx->reply_text = SMTP_REPLY_250;
    return 0;
}

static MYSQL* MysqlConnect(MYSQL*& mysql)
{
	static int inires = mysql_library_init(0, NULL, NULL);
	std::string bad_info = "none";
	try {
		if (mysql) {
			if (mysql_ping(mysql) == 0) {
				return mysql;
			}
			mysql_close(mysql);
			delete mysql;
			mysql = NULL;
		}
		//
		if (mysql_host.length() < 1
			|| mysql_user.length() < 1
			|| mysql_passwd.length() < 1
			|| mysql_dbname.length() < 1)
		{
			return NULL;
		}
		//
		if (mysql_port > 65535 || mysql_port < 1) {
			mysql_port = 3306;
		}
		//
		do {
			mysql = new MYSQL;
			mysql_init(mysql);
			unsigned int timeout = 5;
			if (mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&timeout)) {
				break;
			}
			if (mysql_charset.length() && mysql_options(mysql, MYSQL_SET_CHARSET_NAME, mysql_charset.c_str())) {
				break;
			}
			char value = 1;
#ifdef MYSQL_OPT_RECONNECT
			if (mysql_options(mysql, MYSQL_OPT_RECONNECT, (char *)&value)) {
				break;
			}
#endif
			if (NULL == mysql_real_connect(mysql,
				mysql_host.c_str(),
				mysql_user.c_str(),
				mysql_passwd.c_str(),
				mysql_dbname.c_str(),
				mysql_port, NULL, 0))
			{
				mysql_close(mysql);
				break;
			}
			//双保险：版本不同，MYSQL_OPT_RECONNECT 设置生效
#ifdef MYSQL_OPT_RECONNECT
			if (mysql_options(mysql, MYSQL_OPT_RECONNECT, (char *)&value)) {
				mysql_close(mysql);
				break;
			}
#endif
			//mysql_autocommit(mysql, 0);
			return mysql;
		} while (0);
		//
		if (mysql) {
			delete mysql;
			mysql = NULL;
		}
		bad_info = "mysql_options fail";
	}
	catch (...) {
		if (mysql) {
			delete mysql;
			mysql = NULL;
		}
		bad_info = "mysql expcetion fail";
	}
	LOG_MSG(("MysqlConnect:[%s]\n", bad_info.c_str()));

	return NULL;
}

// q=10 h=20 d=30
int SmtpSvr::check_HR_control(Smtp_Context* ctx)
{
	int size = 0, quarter  = 0, hour = 0, day = 0;
	int quarter_count = 0, hour_count = 0, day_count = 0;
	int quarter_time = 0, hour_time = 0, day_time = 0;
	//int now_quarter = 0, now_hour = 0, now_day = 0;
	bool over_status = false;
	MYSQL_RES *result = NULL;
	AutoGuard<SyncLock> tmplock(m_hr_lock);
	//
	if (!(if (!(this->mydb = MysqlConnect(this->mydb))) {
		LOG_MSG(("mysql connect error\n"));
		return 0;  //通过
	}
	//mysql_query(this->mydb, "BEGIN");
	mysql_query(this->mydb, "START TRANSACTION");
	do {
		//string sqlstr = "select size, quarter, hour, day, quarter_count, hour_count, day_count,"
		//	" quarter_time, hour_time, day_time, CEIL((MINUTE(now()) / 15)) as now_quarter, HOUR(NOW()) as now_hour, DAYOFYEAR(NOW()) as now_day from smtp "
		//	" where instr( concat(';', ip, ';'), ';" + ctx->client_host + ";') > 0 "
		//	" for update";  //锁表串化
		string sqlstr = "select size, quarter, hour, day, quarter_count, hour_count, day_count,"
			" TIME_TO_SEC(TIMEDIFF(NOW(), quarter_time)) AS tq, TIME_TO_SEC(TIMEDIFF(NOW(), hour_time)) AS th, TIME_TO_SEC(TIMEDIFF(NOW(), day_time)) AS td from smtp "
			" where instr( concat(';', ip, ';'), ';" + ctx->client_host + ";') > 0 "
			" for update";  //锁表串化

		if (0 != mysql_query(this->mydb, sqlstr.c_str())) {
			LOG_MSG(("mysql_query[%s]: %s\n", sqlstr.c_str(), mysql_error(this->mydb)));
			break;
		}
		result = mysql_store_result(this->mydb);
		if (mysql_num_rows(result) < 1 || mysql_num_fields(result) < 1) {
			break;			//未限制
		}
		MYSQL_ROW row = NULL;
		if (row = mysql_fetch_row(result)) {
			ctx->ab_size = (row[0] ? atoi(row[0]) : 0) * 1024;			//邮件大小
			quarter = row[0] ? atoi(row[1]) : 0;
			hour = row[2] ? atoi(row[2]) : 0;
			day = row[3] ? atoi(row[3]) : 0;
			quarter_count = row[4] ? atoi(row[4]) : 0;
			hour_count = row[5] ? atoi(row[5]) : 0;
			day_count = row[6] ? atoi(row[6]) : 0;
			//
			quarter_time = row[7] ? atoi(row[7]) : 0;
			hour_time = row[8] ? atoi(row[8]) : 0;
			day_time = row[9] ? atoi(row[9]) : 0;
		}
		mysql_free_result(result); 
		result = NULL;
		//
		do {
			++day_count;
			++hour_count;
			++quarter_count;
		} while (0);
		if (day_time /60 /60 /24 > 1) {
			day_count = 1;
		}
		if (hour_time /60 /60 > 1) {
			hour_count = 1;
		}
		if (quarter_time /60 > 15) {
			quarter_count = 1;
		}
		if ((quarter < quarter_count || hour < hour_count || day < day_count)) {
			over_status = true;		//超限
			break;
		}
		//
		std::stringstream str_stream;
		str_stream << "update smtp"
			<< " set quarter_count = " << quarter_count
			<< ",hour_count = " << hour_count
			<< ",day_count = " << day_count;
		if (1 == day_count) {
			str_stream << ",day_time = NOW()";
		}
		if (1 == hour_count) {
			str_stream << ",hour_time = NOW()";
		}
		if (1 == quarter_count) {
			str_stream << ",quarter_time = NOW()";
		}
		str_stream << " where instr(concat(';', ip, ';'), ';" + ctx->client_host + ";') > 0";
		sqlstr = str_stream.str();
		if (0 != mysql_query(this->mydb, sqlstr.c_str())) {
			LOG_MSG(("mysql_query[%s]: %s\n", sqlstr.c_str(), mysql_error(this->mydb)));
	 		break;
		}
	 	if ((mysql_affected_rows(this->mydb)) < 1) {
			LOG_MSG(("update smtp info fail!"));
		}
	} while (0);
	mysql_commit(this->mydb);
	//
	if (result) {
		mysql_free_result(result);
	}

    return (over_status ? -1 : 0);
}

int SmtpSvr::on_data(string& line,Smtp_Context* ctx)
{

	//if (ctx->smtp_user.length() > 0 && ctx->smtp_pswd.length() > 0)
    {
		if (m_ignore_auth == 0 && m_auth_list.size() > 0)
        {
			int index = rand() % m_auth_list.size();
			LOG_MSG(("Use AUTH_LOGIN '%s'\n", m_auth_list[index].auth_user.c_str()));

			ngx_str_t src,dst;
    	    src.data = (u_char*)m_auth_list[index].auth_user.data();
    	    src.len = m_auth_list[index].auth_user.length();
    	    dst.data = new(std::nothrow) u_char[m_auth_list[index].auth_user.length() * 2];
    	    dst.len = m_auth_list[index].auth_user.length() * 2;
    	    
    	    memset(dst.data, 0x00, dst.len);
    	    ngx_encode_base64(&dst, &src);

    	    ctx->smtp_user.assign((char*)dst.data);
            ctx->smtp_pswd = m_auth_list[index].auth_pswd;
    	    delete [] dst.data;
		}
	}

    ctx->ab_size = -1; //check_HR_control函数读出数据中包含ab_size (某个IP发送邮件data的大小限制)
    if ( 0 != check_HR_control(ctx) || 0 != set_real_sender(ctx))
    {
        ctx->next_steps = HELO|EHLO|MAIL|RSET|NOOP|QUIT;
	    //LOG_MSG(("[history]:\n"));
	    //for (list<string>::iterator it = ctx->list_history.begin();it != ctx->list_history.end();++it)
	    //{
	       // LOG_MSG((" -> %s\n",it->data()));
	    //}
	    ctx->list_history.clear();
	    ctx->command = QUIT;
	    LOG_MSG(("check_HR_control Result=failed,Mail=%s(%s),Rcpt=%s,Client=%s:%d,Remote=?:?:?;?:?,X-Mid=?,Respond=%s",
	        ctx->smtp_mail.c_str(),
	        ctx->real_sender.c_str(),
	        ctx->smtp_rcpt.c_str(),
	        ctx->client_host.c_str(),ctx->client_port,
	        ctx->reply_text.data()
	        ));
        return -1;
    }
    
    ctx->sendbuffer.clear();
    ctx->bytes_total = 0;
    ctx->x_mid.clear();
#if SEND_ONCE_VERSION
	ctx->reply_text = SMTP_354;
#else
	for (int nretry = 0; nretry < g_send_retry; ++nretry) {
		switch (connect_mail_eXchange(ctx)) {
		case 0:
			ctx->next_steps = HELO | EHLO | MAIL | RSET | NOOP | QUIT;
			return 0;
			case -2:
			case -3:
				LOG_MSG(("send retry:dnslookup or connect fail!"));
				event_sleep(200);			//等待200ms
				break;
			default:
				ctx->print("failed");
				ctx->list_history.clear();
				return -1;
		}
	}//for_retry
#endif
}

int SmtpSvr::on_input(string& line, Smtp_Context* ctx)
{
    int bytes;
    int err_code = 0;
    timeval ttl = {m_socket_ttl,0};

    if (line.length() < 1){
        if (ctx->x_mid.length() < 1){
            ctx->x_mid = " ";
        }
    }

    if (ctx->x_mid.length() < 1) {
        if (strncasecmp(line.data(),"x-mid",5) == 0) {
            char* p= (char*)line.data() + 5;
            while (*p==' '|| *p=='\t') p++;
            if (*p++ == ':') {
                while (*p==' '|| *p=='\t') p++;
                ctx->x_mid = p;
            }
        }
    }

#if SEND_ONCE_VERSION
	////发送信头
	//if (ctx->msg_header.length() > 0) {
	//	bytes = ctx->msg_header.length();
	//	ctx->sendbuffer.push_back(ctx->msg_header);
	//	ctx->msg_header.clear();
	//}
	ctx->smtp_data = line + SMTP_CRLF;
	bytes = ctx->smtp_data.length();
	if (bytes > 0) {
		if (ctx->ab_size >= 0 && ctx->bytes_total > ctx->ab_size) {
			LOG_MSG(("512 mailtext size limit:%d\n", ctx->ab_size));
			ctx->reply_text = SMTP_REPLY_512;
			close(ctx->remote_handle);
			ctx->remote_handle = -1;
			return -1;
		}
		ctx->bytes_total += bytes;
		ctx->sendbuffer.push_back(ctx->smtp_data);
	}
	//数据块结束
	if (line == SMTP_DOT)	{
		ctx->reply_text = SMTP_250;
		ctx->cur_retry_count = 0;			//标记需要发送
		smtp_reply(ctx);
	}
	return 0;
#else
	//发送信头
    if (ctx->msg_header.length() > 0)
    {
    	if (USING_BUFFER) {
    		bytes = ctx->msg_header.length();
    		ctx->sendbuffer.push_back(ctx->msg_header);
    	}
    	else {
	        bytes = SmtpSvr::send_n(ctx->remote_handle,
	            ctx->msg_header.data(),
	            ctx->msg_header.length(),
	            &ttl);
        }

        ctx->msg_header.clear();
    }
    ctx->smtp_data = line + SMTP_CRLF;
    
    if (USING_BUFFER) {
    	bytes = ctx->smtp_data.length();
    	ctx->sendbuffer.push_back(ctx->smtp_data);
    }
    else {
	    bytes = SmtpSvr::send_n(ctx->remote_handle,
	        ctx->smtp_data.data(),
	        ctx->smtp_data.length(),
	        &ttl);
    }

    if (bytes > 0)
        ctx->bytes_total += bytes;
    
    //if (bytes < ctx->smtp_data.length()) {
        //LOG_MSG(("mei.fa.wan: transffred[%d] < mailtext[%d]\n",bytes, ctx->smtp_data.length()));
    //}

    if (line == SMTP_DOT)
    {
    	char* text = NULL;
        do {
	    	if (USING_BUFFER){
		    	if (ctx->ab_size >=0 && ctx->bytes_total > ctx->ab_size) {
		    		err_code = -1;
		    		text =(char*) "failed";
		    		LOG_MSG(("512 mailtext size limit:%d\n",ctx->ab_size));
		            ctx->reply_text = SMTP_REPLY_512;
		            close(ctx->remote_handle);
		            ctx->remote_handle = -1;
		            break;
		    	}
		    	else {
		    		for (list<string>::iterator its = ctx->sendbuffer.begin();its != ctx->sendbuffer.end(); ++its) {
		    			SmtpSvr::send_n(ctx->remote_handle,its->data(),its->length(),&ttl);
		    		}
		    		ctx->sendbuffer.clear();
		    	}
	    	}
    	    
	        timeval ttl2 = {m_socket_ttl,0}; //最后一步，等待2倍的时间	    	
	        SmtpSvr::recv_line(ctx->remote_handle,ctx->reply_text,&ttl2);
	        if (strncmp(ctx->reply_text.data(),"250",3) != 0){
	            text = (char*)"failed";
	            err_code = -1;
	            if (ctx->reply_text.length() < 1) {
	            	//LOG_MSG(("'DATA End' response failed\n"));
	            	ctx->reply_text = "512 DATA End failed\r\n";
	            }
	        } else{
	            text = (char*)"OK";
	        }
        }while(0);

	    //LOG_MSG(("Mail text length: %u\n",ctx->bytes_total));
	    smtp_reply(ctx);
	    ctx->print(text);
	    ctx->list_history.clear();      	
    }
    return err_code;
#endif
}

int SmtpSvr::on_rset(string& line,Smtp_Context* ctx)
{
    //ctx->smtp_helo.clear();
    ctx->smtp_mail.clear();
    ctx->smtp_rcpt.clear();
    ctx->msg_header.clear();
    //for AUTH LOGIN
    ctx->smtp_user.clear();
    ctx->smtp_pswd.clear();
    //end AUTH LOGIN
    ctx->x_mid.clear();
    ctx->sendbuffer.clear();
    ctx->next_steps = HELO|EHLO|MAIL|RSET|NOOP|QUIT;
    
    if (ctx->remote_handle > 0) {
        SmtpSvr::send_n(ctx->remote_handle,"RSET\r\n",6);
        SmtpSvr::recv_a(ctx->remote_handle,ctx->reply_text);
    }
    	
    ctx->reply_text = SMTP_REPLY_250;
    return 0;
}

int SmtpSvr::on_quit(string &line, Smtp_Context* ctx)
{
#if SEND_ONCE_VERSION
#else
    SmtpSvr::send_n(ctx->remote_handle,"QUIT\r\n",6);
    SmtpSvr::recv_a(ctx->remote_handle,ctx->reply_text);
#endif
    ctx->reply_text = SMTP_REPLY_221;
    ctx->next_steps = ZERO;
    LOG_MSG(("client [%s:%d] QUIT\n",ctx->client_host.c_str(),ctx->client_port));

    return 0;
}

int SmtpSvr::handle_line( string& line, Smtp_Context* ctx )
{
	//for AUTH LOGIN
	if (ctx->next_steps == AUTH) {
		ctx->list_history.push_back(line);
		ctx->smtp_user = line;
		ctx->reply_text = SMTP_PASSWORD;
		ctx->next_steps = PSWD;
        return smtp_reply(ctx);
	} 
	else if (ctx->next_steps == PSWD) {
		ctx->list_history.push_back("(password hidden)");
		ctx->smtp_pswd = line;
		ctx->reply_text = SMTP_AUTH_MSG;
		ctx->next_steps = HELO|EHLO|MAIL|RSET|NOOP|QUIT;;
        return smtp_reply(ctx);
	} 
	//end AUTH LOGIN

    switch (ctx->command)
    {
    case DATA:
        if (strncmp(ctx->reply_text.data(),"354",3) == 0){
            return on_input(line,ctx);
        }
        break;
    default:
        if (line.length() < 1){
            return -1;
        }
        break;
    }
	
    ctx->list_history.push_back(line);		//记录历史命令信息
    ctx->command = parse_type(line.c_str());

    if (ctx->command == ZERO)
    {
        LOG_MSG(("Command not implemented: %s\n",line.c_str()));
        ctx->reply_text = SMTP_REPLY_502;
        smtp_reply(ctx);
        return -1;
    }

    int err_code = 0;
    switch (ctx->command & ctx->next_steps)
    {
    case HELO:
    case EHLO:
        err_code = on_helo(line,ctx);
        break;

    //for AUTH LOGIN
    case AUTH:
        ctx->next_steps = AUTH;
        ctx->reply_text = SMTP_USERNAME;
        ctx->smtp_user.clear();
        ctx->smtp_pswd.clear();
    	break; 
    //end AUTH LOGIN
    	
    case MAIL:
        err_code = on_mail(line,ctx);
        break;

    case RCPT:
        err_code = on_rcpt(line,ctx);
        break;

    case DATA:
        err_code = on_data(line,ctx);
        break;

    case RSET:
        err_code = on_rset(line,ctx);
        break;

    case NOOP:
        ctx->reply_text = SMTP_REPLY_250;
        break;

    case QUIT:
        err_code = on_quit(line,ctx);
        break;

    default:
        err_code = -1;
        ctx->reply_text = SMTP_REPLY_503;
        break;
    }

    smtp_reply(ctx);
    return err_code;
}

int SmtpSvr::parse_type( const char* line )
{
    if (strncasecmp(line,"HELO",4) == 0)
        return HELO;
    else if (strncasecmp(line,"EHLO",4) == 0) 
        return EHLO;
    else if (strncasecmp(line,"MAIL ",5) == 0) 
        return MAIL;
    else if (strncasecmp(line,"RCPT ",5) == 0) 
        return RCPT;
    else if (strcasecmp(line,"DATA") == 0)  
        return DATA;
    else if (strcasecmp(line,"RSET") == 0)  
        return RSET;
    else if (strcasecmp(line,"NOOP") == 0)  
        return NOOP;
    else if (strcasecmp(line,"QUIT") == 0)  
        return QUIT;
    else if (strcasecmp(line,"AUTH LOGIN") == 0) 
    	return AUTH;
    else
        return ZERO;
}

int SmtpSvr::smtp_reply( Smtp_Context* context )
{
    return send(context->client_handle,
        context->reply_text.data(),
        context->reply_text.length(),
        0);
}

string SmtpSvr::split_string( string &src,const char *s )
{
    string ret;
    string::size_type pos = src.find(s);

    if (pos == string::npos){
        ret = src;
        src.clear();
    }
    else{
        ret = src.substr(0,pos);
        src.erase(0,pos+strlen(s));
    }

    return ret;
}

#define OTHER_QUOTA_KEY   "other"
#define OTHER_DEFAULT_KEY "default"

int SmtpSvr::load_delivery_quota(ConfigureMgr& confmgr)
{
    AutoGuard<SyncLock> ag_lock(m_quota_lock);
    typeConfPairList list_quota;
    confmgr.getSubItemPairList(m_fuck_header + "/smtpsvr/sender", list_quota );

    for ( typeConfPairList::const_iterator it = list_quota.begin(); it != list_quota.end(); ++it )
    {
        string minor_key = it->first;
        string second_str = it->second;

        std::transform(minor_key.begin(),minor_key.end(),minor_key.begin(),(int(*)(int))tolower);
        std::transform(second_str.begin(),second_str.end(),second_str.begin(),(int(*)(int))tolower);

        if (minor_key == OTHER_DEFAULT_KEY) //兼容
        {
            continue;
        }

        string major_key = split_string(second_str,"|");
        int interval = atoi(split_string(second_str,"|").c_str());
        int quota_value = atoi(split_string(second_str,"|").c_str());

        if (minor_key.length()<1|| major_key.length()<1 || interval<1 || quota_value<1)
        {
            LOG_MSG(("[/smtpsvr/sender] invaild configure:[%s=%s]\n",minor_key.c_str(),it->second.c_str()));
            continue;
        }

        Delivery_Quota t_quota;
        t_quota.interval = interval;
        t_quota.quota    = quota_value;
        t_quota.number   = 0;
        t_quota.tpoint   = time(NULL);

        domain_quota_map::iterator it_quota = m_delivery_quota.find(major_key);
        if (it_quota == m_delivery_quota.end())
        {
            sender_quota_map mpQuota;
            mpQuota.insert(pair<string,Delivery_Quota>(minor_key,t_quota));
            m_delivery_quota.insert(pair<string,sender_quota_map>(major_key,mpQuota));
        }
        else
        {
            it_quota->second.insert(pair<string,Delivery_Quota>(minor_key,t_quota));
        }
    }

    return 0;
}

string& trimstr(string & ss, const char* cls = " \t")
{
    ss.erase(0, ss.find_first_not_of(cls));
    ss.erase(ss.find_last_not_of(cls)+1);
    return ss;
}

int SmtpSvr::load_ip_white_list(const std::string& ip_str_list_in)
{
    std::string ip_str_list = ip_str_list_in;
    GlobalFunc::removeSpace(ip_str_list);
    ip_segment ipseg;

    if (ip_str_list.empty()) {
	return 0;
    }
    LOG_MSG(("[/smtpsvr/ip_white_list] configure:[%s]\n", ip_str_list.c_str()));
    while (ip_str_list.length() > 0) {
         string range;
        int index = ip_str_list.find_first_of(";,");
        if (index == string::npos){
            range = ip_str_list;
            ip_str_list.clear();
        }
        else{
            range = ip_str_list.substr(0,index);
            ip_str_list.erase(0,index+1);
        }

        if (range.find_first_not_of("0123456789./-") != string::npos){
            LOG_MSG(("[/smtpsvr/ip_white_list] invaild configure:[%s]\n",range.c_str()));
            continue;
        }

        int flag = range.find_first_not_of("0123456789.");

        if (string::npos == flag)
        {
            if (inet_aton(range.c_str(),&ipseg.ip_begin) != 0)
            {
                ipseg.ip_begin.s_addr = ntohl(ipseg.ip_begin.s_addr);
                ipseg.ip_end.s_addr = ipseg.ip_begin.s_addr;
                m_ipsegment.push_back(ipseg);
            }
            else
            {
                LOG_MSG(("[/smtpsvr/ip_white_list] invaild configure:[%s]\n",range.c_str()));
            }
        }
        else 
        {
            switch (range.at(flag))
            {
            case '-':
                if (inet_aton(split_string(range, "-").c_str(),&ipseg.ip_begin) != 0){
                    if (inet_aton(range.c_str(),&ipseg.ip_end) != 0){
                        ipseg.ip_begin.s_addr = ntohl(ipseg.ip_begin.s_addr);
                        ipseg.ip_end.s_addr = ntohl(ipseg.ip_end.s_addr);
                        m_ipsegment.push_back(ipseg);
                    }
                    else
                        LOG_MSG(("[/smtpsvr/ip_white_list] invaild configure:[%s]\n",range.c_str()));
                }
                else
                    LOG_MSG(("[/smtpsvr/ip_white_list] invaild configure:[%s]\n",range.c_str()));
                break;

            case '/':
                if (inet_aton(split_string(range, "/").c_str(),&ipseg.ip_begin) != 0)
                {
                    int submask = 0;
                    sscanf(range.c_str(),"%d",&submask);
                    if (submask >= 0 && submask <=32)
                    {
                        unsigned int uaddr = ntohl(ipseg.ip_begin.s_addr);
                        ipseg.ip_begin.s_addr = uaddr & 0xffffffff<<(32-submask);
                        ipseg.ip_end.s_addr = uaddr | 0xffffffff>>submask;
                        m_ipsegment.push_back(ipseg);
                    }
                    else
                        LOG_MSG(("[/smtpsvr/ip_white_list] invaild configure:[%s]\n",range.c_str()));
                }
                else
                    LOG_MSG(("[/smtpsvr/ip_white_list] invaild configure:[%s]\n",range.c_str()));
                break;
            default:
                LOG_MSG(("[/smtpsvr/ip_white_list] invaild configure:[%s]\n",range.c_str()));
                break;
            }
        }
    }//

    return 0;
}

int SmtpSvr::check_ipsegment( in_addr& ip )
{
    unsigned int uaddr = ntohl(ip.s_addr);
    for (int i=0; i<m_ipsegment.size(); i++){
        if ( uaddr >= m_ipsegment[i].ip_begin.s_addr 
            && uaddr <= m_ipsegment[i].ip_end.s_addr){
            return 0;
        }
    }

    return -1;
}

int SmtpSvr::load_mail_eXchange( ConfigureMgr& confmgr )
{
    typeConfPairList list_transport;
    confmgr.getSubItemPairList(m_fuck_header + "/smtpsvr/transport", list_transport );

    Mail_eXchange mxtransport;
    mxtransport.ttl = -1;

    for ( typeConfPairList::const_iterator it = list_transport.begin(); it != list_transport.end(); ++it )
    {
        string domain = it->first;
        string transport = it->second;

        if (transport.length() < 1 || transport.find_first_not_of("0123456789.:") != string::npos)
        {
            LOG_MSG(("[/smtpsvr/transport] invaild configure:%s=%s\n",it->first.c_str(),it->second.c_str()));
            continue;
        }

        std::transform(domain.begin(),domain.end(),domain.begin(),(int(*)(int))tolower);
        std::transform(transport.begin(),transport.end(),transport.begin(),(int(*)(int))tolower);
        GlobalFunc::removeSpace(transport);

        mxtransport.host = split_string(transport,":");
        if ( inet_addr(mxtransport.host.c_str()) < 1){
            LOG_MSG(("[/smtpsvr/transport] invaild configure:%s=%s\n",it->first.c_str(),it->second.c_str()));
            continue;
        }
        
        mxtransport.port = atoi(transport.c_str());
        if (mxtransport.port < 1 || mxtransport.port > 0xffff)
        {
            LOG_MSG(("[/smtpsvr/transport] invaild configure:%s=%s\n",it->first.c_str(),it->second.c_str()));
            continue;
        }

        mailx_map::iterator it = m_mail_eXchange.find(domain);
        if (it != m_mail_eXchange.end()){
            it->second.push_back(mxtransport);
        }
        else
        {
            mailx_vector vtr;
            vtr.push_back(mxtransport);
            m_mail_eXchange.insert(mailx_map::value_type(domain,vtr));
        }
    }

    return 0;
}

int SmtpSvr::query_mail_eXchange( const char* domain, string& host, int& port )
{
    AutoGuard<SyncLock> tmp_lock(m_mail_lock);

    mailx_map::iterator it_mailx = m_mail_eXchange.find(domain);
    if (it_mailx == m_mail_eXchange.end()){
        it_mailx = m_mail_eXchange.find("all");
    }

    if (it_mailx != m_mail_eXchange.end())
    {
        int index = rand() % it_mailx->second.size();

        if (it_mailx->second[index].ttl < 0 || difftime(time(NULL),it_mailx->second[index].ttl) < m_query_dns_ttl)
        {
            if (it_mailx->second[index].host == host && it_mailx->second[index].port == port)
            {
                host = it_mailx->second[(index+1) % it_mailx->second.size()].host;
                port = it_mailx->second[(index+1) % it_mailx->second.size()].port;
            }
            else
            {
                host = it_mailx->second[index].host;
                port = it_mailx->second[index].port;
            }

            return 0;
        }

        it_mailx->second.clear();
    }

    string domain_str = domain;
    DNSRecord dnsrecord;
    if (DNS_LOOKUP_NOTFOUND == m_dns_resolver.queryMXIPV4Record(domain_str,dnsrecord,128))
    {
        LOG_MSG(("dnslookup failed: [%s]\n",domain_str.c_str()));
        return -1;
    }

    Mail_eXchange mxtransport;
    mxtransport.port = 25;
    mxtransport.ttl = time(NULL);

    typeDNSEntryList::const_iterator it_dnslist = dnsrecord.getEntryList().begin();
    for ( ; it_dnslist != dnsrecord.getEntryList().end(); ++it_dnslist )
    {
        mxtransport.host = it_dnslist->getRecordResult();
        if ( mxtransport.host != "0.0.0.0" && mxtransport.host != "127.0.0.1" ){
            m_mail_eXchange[domain_str].push_back(mxtransport);
        }
    }
	//
    it_mailx = m_mail_eXchange.find(domain_str);
    if (it_mailx == m_mail_eXchange.end() || it_mailx->second.size() < 1) {
		//queryIPV4ARecord(const stdstring & strDomain, DNSRecord & dnsRecord)
		//note:主要针对mopen.163.com 这类没有mx记录的地址
		if (DNS_LOOKUP_NOTFOUND == m_dns_resolver.queryIPV4ARecord(domain_str, dnsrecord)) {
			LOG_MSG(("dnslookup ip4 which not had mx, failed: [%s]\n", domain_str.c_str()));
			return -1;
		}
		mxtransport.port = 25;
		mxtransport.ttl = time(NULL);
		typeDNSEntryList::const_iterator it_dnslist = dnsrecord.getEntryList().begin();
		for (; it_dnslist != dnsrecord.getEntryList().end(); ++it_dnslist) {
			mxtransport.host = it_dnslist->getRecordResult();
			if (mxtransport.host != "0.0.0.0" && mxtransport.host != "127.0.0.1") {
				m_mail_eXchange[domain_str].push_back(mxtransport);
			}
		}
		it_mailx = m_mail_eXchange.find(domain_str);
		if (it_mailx == m_mail_eXchange.end() || it_mailx->second.size() < 1) {
			LOG_MSG(("dnslookup empty: [%s]\n", domain_str.c_str()));
			return -1;
		}
    }
    //LOG_MSG(("dnslookup [%s]\n",domain_str.c_str()));
    
    int index = rand() % it_mailx->second.size();
    host = it_mailx->second[index].host;
    port = it_mailx->second[index].port;

    return 0;
}

int SmtpSvr::connect4( const char* host, int port, timeval* ttl)
{
    int handle = 0;
    char buffer[256] = {0};
    if((handle = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
    {
        LOG_ERROR(errno,"socket");
        return -1;
    }

    struct sockaddr_in address;
    bzero(&address, sizeof(address));  
    address.sin_family = PF_INET;  
    address.sin_addr.s_addr = inet_addr(SmtpSvr::instance()->m_myhost.c_str());

    if (bind(handle, (struct sockaddr *) &address, sizeof(struct sockaddr)) == -1){
        LOG_ERROR(errno,"bind");
        return -1;
    }
    
    fcntl(handle, F_SETFL, fcntl(handle, F_GETFD, 0)|O_NONBLOCK);

    int reuse = 1;
    setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

    struct sockaddr_in localsin;
    bzero(&localsin, sizeof(localsin));
    localsin.sin_family = PF_INET;  
    localsin.sin_port = htons(port);  
    localsin.sin_addr.s_addr = inet_addr(host);

    timeval timeout;
    if (ttl != NULL)
    {
        timeout.tv_sec = ttl->tv_sec;
        timeout.tv_usec = ttl->tv_usec;
        ttl = &timeout;
    }

    switch (::connect(handle,(sockaddr*)&localsin, sizeof(localsin)))
    {
    case -1:
        {
            if (errno != EINPROGRESS && errno != EWOULDBLOCK){
                LOG_MSG(("connect [%s:%d] failed:%s\n",host,port,strerror(errno)));
                return -1; //-1 失败
            }

            fd_set wrds;
            FD_ZERO(&wrds);
            FD_SET(handle,&wrds);

            switch(select(handle+1, NULL, &wrds, NULL,ttl))
            {
            case 0:
            case -1:
                strerror_r(errno,buffer,sizeof(buffer));
                LOG_MSG(("connect [%s:%d] timeout:%s\n",host,port,strerror(errno)));
                close(handle);
                return -2;//超时
            default:
                break;
            }

            int error = 0;
            socklen_t len = sizeof(error);
            getsockopt(handle,SOL_SOCKET,SO_ERROR,&error,&len);
            
            if (error != 0)
            {
                close(handle);
                return -1; //失败
            }
        }
        break;
    default:
        break;
    }

    return handle;
}

int SmtpSvr::recv_line(int handle, string& data, timeval *ttl)
{
    char buffer[256];
    string tmpdata;
    data.clear();

    timeval timeout;
    if (ttl != NULL){
        timeout.tv_sec = ttl->tv_sec;
        timeout.tv_usec = ttl->tv_usec;
        ttl = &timeout;
    }

	int recv_max_time = 3;
    fd_set rdset;
    FD_ZERO(&rdset);
    FD_SET(handle, &rdset);
    select(handle+1, &rdset, NULL, NULL, ttl);

    do 
    {
        memset(buffer, 0x00,sizeof(buffer));
        //int bytes = SmtpSvr::recv_n(handle,buffer,sizeof(buffer),ttl);
        int bytes = ::recv(handle,buffer,sizeof(buffer), 0);

        switch (bytes)
        {
        case -1:
            if (errno == EINTR)
                continue;
			if (errno == EAGAIN) {
				if (recv_max_time-- > 0 && (timeout.tv_sec > 0 || timeout.tv_usec > 0)) {
					fd_set rdset;
					FD_ZERO(&rdset);
					FD_SET(handle, &rdset);
					select(handle + 1, &rdset, NULL, NULL, ttl);
					continue;
				}
				else {
					return -1;
				}
			}
        case 0:
        	LOG_MSG(("smtp_recv failed:%s\n", strerror(errno)));
            return bytes;
        default: tmpdata.append(buffer,bytes);
            break;
        }
        
        while (tmpdata.length() > 0)
        {
            int poscrlf = tmpdata.find(SMTP_CRLF);
            if (poscrlf == string::npos)
                break;

            string line = tmpdata.substr(0,poscrlf);
            tmpdata.erase(0,poscrlf+strlen(SMTP_CRLF));
            
            if (line.length() < 3){
            	LOG_MSG(("smtp_recv line error1:%s\n", line.c_str()));
            	return -1;
            }

            if ( !isdigit(line[0]) || !isdigit(line[1]) || !isdigit(line[2])){
            	LOG_MSG(("smtp_recv line error2:%s\n", line.c_str()));
            	return -1;
            }
            
            if (line.length() > 3) {
                switch (line[3]){
                case ' ': break;
                case '-': continue;
                default: {
                	LOG_MSG(("smtp_recv line error3:%s\n", line.c_str()));
                	return -1;
                    }
                }
            }
            data += (line + SMTP_CRLF);
            return 0;
        }
    } while (1);

    return 0;
}

int SmtpSvr::connect_mail_eXchange( Smtp_Context* ctx )
{
    timeval ttl = {m_socket_ttl,0};
    int retry = 3;
    string text;
    
    bool using_auth_login = false;
	string smtp_domain;
	//LOG_MSG(("auth[%s-->%s]\n", ctx->smtp_user.c_str(), ctx->smtp_pswd.c_str()));
    if (0 == m_ignore_auth && ctx->smtp_user.length() > 0 && ctx->smtp_pswd.length() > 0){
    	using_auth_login = true;
		smtp_domain = ctx->real_sender.substr(ctx->real_sender.find("@") + 1);
    	LOG_MSG(("using_auth_login[%s-->%s]\n", smtp_domain.c_str(), ctx->real_sender.c_str()));
    }
    else {
    	smtp_domain = ctx->rcpt_domain;
    }
	std::map<std::string, std::string>::iterator domain_map_it = m_domain_map.find(smtp_domain);
	if (domain_map_it != m_domain_map.end()) {
		smtp_domain = domain_map_it->second;
	}
    if (ctx->remote_handle > 0) {
        close(ctx->remote_handle);
        ctx->remote_handle = -1;
    }
    if(ctx->remote_handle < 1) {
        if ( -1 == query_mail_eXchange(smtp_domain.c_str(),ctx->remote_host,ctx->remote_port)) {
            ctx->reply_text = SMTP_REPLY_510;
            //return -1;
			return -2;
        }
        ctx->remote_handle = SmtpSvr::connect4(ctx->remote_host.c_str(),ctx->remote_port,&ttl);
        if (ctx->remote_handle < 1) {
            LOG_MSG(("Connect failed:[%s][%s:%d]\n",smtp_domain.c_str(),ctx->remote_host.c_str(),ctx->remote_port));
            ctx->reply_text = ctx->remote_handle==-1 ? SMTP_REPLY_401:SMTP_REPLY_511;
            return -3;
        }

        //LOG_MSG(("Connect mail-eXchange :[%s][%s:%d]\n",smtp_domain.c_str(), ctx->remote_host.c_str(),ctx->remote_port));
        //220 ready
        SmtpSvr::recv_line(ctx->remote_handle,ctx->reply_text,&ttl);
		if (strncmp(ctx->reply_text.c_str(), "220", 3) != 0) {
			LOG_MSG(("request2 MX ready failed\n"));
			ctx->next_steps = HELO | EHLO | RSET | NOOP | QUIT;
			if (ctx->reply_text.length() < 1) {
				LOG_MSG(("set reply => %s", SMTP_REPLY_401));
				ctx->reply_text = SMTP_REPLY_401;
				close(ctx->remote_handle);
				ctx->remote_handle = -1;
			}
			else {
				LOG_MSG(("MX Ready <= %s", ctx->reply_text.c_str()));
			}
			return -1;
		}//end do welcome
    }

    //LOG_MSG(("Using Mail-eXchange :[%s][%s:%d]\n",smtp_domain.c_str(), ctx->remote_host.c_str(),ctx->remote_port));
    try
    {
        //helo/ehlo
        if (g_default_helo.length() > 0) {
        	//LOG_MSG(("ti huan : %s ==> HELO %s \n", ctx->smtp_helo.c_str(), g_default_helo.c_str()));
        	ctx->smtp_helo = "HELO " + g_default_helo;
        }

	    text = ctx->smtp_helo + SMTP_CRLF;
	    SmtpSvr::send_n(ctx->remote_handle,text.data(),text.length(),&ttl);
	    SmtpSvr::recv_line(ctx->remote_handle,ctx->reply_text,&ttl);
	    if (strncmp(ctx->reply_text.data(),"250",3) != 0) {
	        ctx->next_steps = HELO|EHLO|RSET|NOOP|QUIT;
	        LOG_MSG(("HELO/EHLO fail : %s",ctx->reply_text.data()));
	        //throw HELO;
	        //有小部分MX服务器只支持多次长链投递中只发送一次HELO/EHLO指令
	        //为兼容长连接中多次投递，该错误不再抛出异常
	    }
	    
	    //for AUTH LOGIN
	    if (using_auth_login) {
	    	//auth login
		    text = "AUTH LOGIN";
		    text +=SMTP_CRLF;
	
		    SmtpSvr::send_n(ctx->remote_handle,text.data(),text.length(),&ttl);
		    SmtpSvr::recv_line(ctx->remote_handle,ctx->reply_text,&ttl);
		    if (strncmp(ctx->reply_text.data(),"334",3) != 0) {
		        ctx->next_steps = HELO|EHLO|MAIL|RSET|NOOP|QUIT|AUTH;
		        throw AUTH;
		    }
		    
		    //username
		    text = ctx->smtp_user + SMTP_CRLF;
		    SmtpSvr::send_n(ctx->remote_handle,text.data(),text.length(),&ttl);
		    SmtpSvr::recv_line(ctx->remote_handle,ctx->reply_text,&ttl);
		    if (strncmp(ctx->reply_text.data(),"334",3) != 0) {
		        ctx->next_steps = HELO|EHLO|MAIL|RSET|NOOP|QUIT|AUTH;
		        throw AUTH;
		    }
		    	    
		    //password
		    text = ctx->smtp_pswd + SMTP_CRLF;
		    SmtpSvr::send_n(ctx->remote_handle,text.data(),text.length(),&ttl);
		    SmtpSvr::recv_line(ctx->remote_handle,ctx->reply_text,&ttl);
		    if (strncmp(ctx->reply_text.data(),"235",3) != 0) {
		        ctx->next_steps = HELO|EHLO|MAIL|RSET|NOOP|QUIT|AUTH;
		        throw AUTH;
		    }
	    }
	    //end AUTH LOGIN
	
	    //mail from
	    text = "MAIL FROM: <" + ctx->real_sender + ">"SMTP_CRLF;

	    SmtpSvr::send_n(ctx->remote_handle,text.data(),text.length(),&ttl);
	    SmtpSvr::recv_line(ctx->remote_handle,ctx->reply_text,&ttl);
	    if (strncmp(ctx->reply_text.data(),"2",1) != 0) {
	        ctx->next_steps = HELO|EHLO|MAIL|RSET|NOOP|QUIT;
	        throw MAIL;
	    }
	
	    //rcpt to
        text = "RCPT TO: <" + ctx->smtp_rcpt + ">"SMTP_CRLF;
	    SmtpSvr::send_n(ctx->remote_handle,text.data(),text.length(),&ttl);
	    SmtpSvr::recv_line(ctx->remote_handle,ctx->reply_text,&ttl);
	    if (strncmp(ctx->reply_text.data(),"2",1) != 0) {
	        ctx->next_steps = HELO|EHLO|MAIL|RCPT|RSET|NOOP|QUIT;
	        throw RCPT;
	    }
		
	    //data
	    text = "DATA\r\n";
	    SmtpSvr::send_n(ctx->remote_handle,text.data(),text.length(),&ttl);
	    SmtpSvr::recv_line(ctx->remote_handle,ctx->reply_text,&ttl);
	    if (strncmp(ctx->reply_text.data(),"354",3) != 0) {
	        ctx->next_steps = HELO|EHLO|MAIL|RSET|NOOP|QUIT;
            throw DATA;
	    }
#if SEND_ONCE_VERSION
		text = "SDATA\r\n";
		for (list<string>::iterator its = ctx->sendbuffer.begin(); its != ctx->sendbuffer.end(); ++its) {
			SmtpSvr::send_n(ctx->remote_handle, its->data(), its->length(), &ttl);
			//LOG_MSG(("mail data: => %s", its->data()));
		}
		timeval ttl2 = { m_socket_ttl * 2,0 }; //最后一步，等待2倍的时间	    	
		SmtpSvr::recv_line(ctx->remote_handle, ctx->reply_text, &ttl2);
		if (strncmp(ctx->reply_text.data(), "250", 3) != 0) {
			if (ctx->reply_text.length() < 1) {
				//LOG_MSG(("'DATA End' response failed\n"));
				ctx->reply_text = "512 DATA End failed\r\n";
			}
			throw DATA;
		}
		//
		text = "QUIT\r\n";
		SmtpSvr::send_n(ctx->remote_handle, text.data(), text.length(), &ttl);
		SmtpSvr::recv_a(ctx->remote_handle, ctx->reply_text);
		//cleanup
		close(ctx->remote_handle);
		ctx->remote_handle = -1;
		ctx->sendbuffer.clear();
		ctx->list_history.clear();

		return 0;
#endif
    }
    catch (...) {
        LOG_MSG(("send to MX => %s",text.data()));

        if (ctx->reply_text.length() < 1){
            LOG_MSG(("set reply => %s",SMTP_REPLY_402));
            ctx->reply_text = SMTP_REPLY_402;
            close(ctx->remote_handle);
            ctx->remote_handle = -1;
            LOG_MSG(("recv (0 bytes):%s\n", strerror(errno)));
        }
        else {
            LOG_MSG(("recv from MX<= %s",ctx->reply_text.data()));
        }
        return -1;
    }
#if 0
    string helo_host = ctx->smtp_helo.substr(5);
    helo_host.erase(0, helo_host.find_first_not_of(" \t"));
    helo_host.erase(helo_host.find_last_not_of(" \t\r\n")+1);

    timeval tv_sys;
    gettimeofday(&tv_sys,NULL);
    DateTime date(tv_sys.tv_sec);

    char buffer[128] = {0};
    snprintf(buffer, sizeof(buffer), "%u.%u.%s.%06d",
        getpid(),pthread_self(),
        date.formatDateTime("%Y%m%d%H%M%S").c_str(),tv_sys.tv_usec);

    ctx->msg_header = ("Received: from " + helo_host + "(unknown[" + ctx->client_host + "])\r\n");
    ctx->msg_header += ("\tby <SmtpSvr2> with SMTP id ");
    ctx->msg_header += buffer;
    ctx->msg_header += (";\r\n");
    ctx->msg_header += ("\t" + date.formatDateTime("%a, %d %b %Y %H:%M:%S %z (%Z)\r\n"));
    
    ctx->msg_header.clear();//delete 2013.10.30
	//LOG_MSG(("SMTP id: %s\n",buffer));
#endif

    return 0;
}

timeval SmtpSvr::timeval_zero = {0,0};

Smtp_Context* SmtpSvr::alloc()
{
    Smtp_Context* ctx = new (std::nothrow) Smtp_Context;
    if (ctx){
        ctx->clear();
    }

    return ctx;
}

void SmtpSvr::free( Smtp_Context *ctx )
{
    if (ctx){
        ctx->clear();
        delete ctx;
    }
}

/*
 *说明:
1.投递到规定的域,只能使用指定的发件人. 例如只能使用:a@qq.com投递
2.发件人投递到某个域的邮件在时间范围内有数量上的限制. 例如a@qq.com在1分钟内只能投递100封邮件到@richinfo.cn域
3.从[发件人列表中]选择一个作为邮件的投递者
4.检查发件人时间范围内投递数量是否超过限制
 **/
/************************************************************************/
/* 查找可用的发件人地址,发件人来自配置文件[smtpsvr/sender]              
   根据[发件人 和 投递的目标域] 做流量控制                              */
/************************************************************************/
int SmtpSvr::set_real_sender( Smtp_Context* ctx)
{
    AutoGuard<SyncLock> ag_lock(m_quota_lock);

    time_t tp_end = time(NULL);
    domain_quota_map::iterator it_s;

    /************************************************************************/
    /* 根据目标域名，从预定义的配置中查找是否有可用的发件人地址             */
    /************************************************************************/
    it_s = m_delivery_quota.find(ctx->rcpt_domain);

    if (it_s == m_delivery_quota.end())
    {
        it_s = m_delivery_quota.find("other");
    }

    if (it_s != m_delivery_quota.end())
    {
        sender_quota_map::iterator it_d = it_s->second.begin();

        /************************************************************************/
        /* 轮询检查发送量是否超额                                               */
        /************************************************************************/
        for (; it_d != it_s->second.end(); ++it_d)
        {
            if (it_d->second.number == 0){
                it_d->second.tpoint = tp_end;
            }

            int interval = int(difftime(tp_end,it_d->second.tpoint));
            if ( interval > it_d->second.interval * UNIT_CONVERSION_WITH_SECOND)
            {
                //投递计数重置
                it_d->second.number = 0;
                it_d->second.tpoint = tp_end;
            }

//            LOG_DEBUG(("[%s -> %s] difftime:%ds, number:%d, control: %d/%ds\n",
//                it_d->first.c_str(),
//                ctx->rcpt_domain.c_str(),
//                interval,
//                it_d->second.number, 
//                it_d->second.quota,
//                it_d->second.interval));

            if (it_d->second.number++ < it_d->second.quota)
            {
                ctx->real_sender = it_d->first;
                //LOG_MSG(("convert %s => %s\n",
                    //ctx->smtp_mail.c_str(),ctx->real_sender.c_str()));
                return 0;
            }
        }

        LOG_MSG(("Sender frequency limited:[%s]\n",ctx->rcpt_domain.c_str()));
        ctx->reply_text = SMTP_REPLY_600;
        return -1;
    }

    /************************************************************************/
    /* 根据目标域名，从缓存查找是否有可用的发件人地址                       */
    /************************************************************************/
    it_s = m_cache_quota.find(ctx->rcpt_domain);

    if (it_s == m_cache_quota.end())
    {
        //缓存新域名-新发件人
        ctx->real_sender = ctx->smtp_mail;

        Delivery_Quota t_quota;
        t_quota.interval = m_control_interval;//m_quota_default_interval;
        t_quota.quota    = m_control_number;//m_quota_default_value;
        t_quota.number   = 1;
        t_quota.tpoint   = time(NULL);

        sender_quota_map mpQuota;
        mpQuota.insert(pair<string,Delivery_Quota>(ctx->smtp_mail,t_quota));
        m_cache_quota.insert(pair<string,sender_quota_map>(ctx->rcpt_domain,mpQuota));

//        LOG_DEBUG(("[%s -> %s] difftime:0s, number:0, control: %d/%ds\n",
//            ctx->smtp_mail.c_str(),
//            ctx->rcpt_domain.c_str(),
//            t_quota.quota,
//            t_quota.interval));

        return 0;
    }

    sender_quota_map::iterator it_d;
    it_d = it_s->second.find(ctx->smtp_mail);

    if (it_d == it_s->second.end())
    {
        //缓存新发件人
        ctx->real_sender = ctx->smtp_mail;
        Delivery_Quota t_quota;
        t_quota.interval = m_control_interval;//m_quota_default_interval;
        t_quota.quota    = m_control_number;//m_quota_default_value;
        t_quota.number   = 1;
        t_quota.tpoint   = time(NULL);
        it_s->second.insert(pair<string,Delivery_Quota>(ctx->smtp_mail,t_quota));

        return 0;
    }

    if (it_d->second.number == 0){
        it_d->second.tpoint = tp_end;
    }

    int interval = int(difftime(tp_end,it_d->second.tpoint));

    if ( interval > it_d->second.interval * UNIT_CONVERSION_WITH_SECOND)
    {
        it_d->second.number = 0;
        it_d->second.tpoint = tp_end;
    }

//    LOG_DEBUG(("[%s -> %s] difftime:%ds, number:%d, control: %d/%ds\n",
//        it_d->first.c_str(),
//        ctx->rcpt_domain.c_str(),
//        interval,
//        it_d->second.number, 
//        it_d->second.quota,
//        it_d->second.interval));

    if (it_d->second.number++ >= it_d->second.quota)
    {
        LOG_MSG(("Sender frequency limited:[%s]\n",ctx->rcpt_domain.c_str()));
        ctx->reply_text = SMTP_REPLY_600;
        return -1;
    }

    ctx->real_sender = ctx->smtp_mail;
    return 0;
}

RFC_NAMESPACE_END

