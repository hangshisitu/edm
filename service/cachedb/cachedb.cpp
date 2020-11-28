/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan				Date: 2011-08      */

#include "cachedb.h"
#include "mutex/mutex.h"
#include "base/formatstream.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <pthread.h>
#include <mcheck.h>
#include <string.h>
#include "ip_area.h"
#include <sys/time.h>
#include "berkeleydb.h"

using namespace std;

RFC_NAMESPACE_BEGIN
FileLog     fileLog;
ParseRequest parse;
pthread_mutex_t  mutex_task;
pthread_mutex_t  mutex_url;
typedef list<_tinfo_node *> LISTTASK;
typedef list<_url_node *> LISTURL;
typedef list<_tdomain_node*> LISTTDOMAIN;
LISTTASK list_task;
LISTURL  list_url;
LISTTDOMAIN list_task_domain;

SafeDBConnection m_dbConn;
DBEnvParams m_dbparams;
MysqlEnv	m_mysqlenv; 
SafeDBStatement m_dbStmt;
table_name_t g_tablename;
//IP_AREA_OBJ* obj;
vector<_ipline *> vec_ip;
map<string,unsigned int> map_region;
map<string,unsigned int> map_bigcity;

string CacheDbApp::m_database;

map<string,string> map_domain;
int dbiter;
int recviter;

string lbsvr_host = "127.0.0.1";
int lbsvr_port = 0;

#include "data_struct.h"

BerkeleyDB bdbCache;
BerkeleyDB bdbHaviorHistory;
FILE*      fdHaviorDaily = NULL;

string bdb_cache_file;
string havior_daily_file;
string bdb_havior_history_file;
string bdb_pici;

int terminal = 0;
int fuckexit = 0;

void show_handler(int sig)
{
    if(sig == SIGTERM)
    {
        fileLog.writeLog("this is a kill signal\n");
        terminal = 1;
    }
    else if (sig == SIGPIPE)
    {
        fileLog.writeLog("this is a sigpipe signal\n");
    }
    return;
}

void * DBprocess(void * param)
{
    //add 2014/02
    if (bdbCache.open(bdb_cache_file.c_str(),0,0) != 0){
        fileLog.writeLog("Open bdb_cache_file fail\n");
        exit(0);
    }

    if (bdbHaviorHistory.open(bdb_havior_history_file.c_str(),0,0) != 0){
        fileLog.writeLog("Open bdb_havior_history_file fail\n");
        exit(0);
    }

    while(terminal != 1)
    {
        _tinfo_node * node_t=NULL; 
        _url_node * node_u=NULL; 
        _tdomain_node * node_td=NULL; 

        bool flag=false;

        try
        {
            while(terminal != 1)
            {
                //add 2014/03
                time_t tt = time(0);
                struct tm *tms = localtime(&tt);

                char haviorFile[256];
                snprintf(haviorFile, sizeof(haviorFile), 
                    "%s.%d%02d%02d", havior_daily_file.c_str(),tms->tm_year + 1900, tms->tm_mon + 1, tms->tm_mday); 

                fdHaviorDaily = fopen(haviorFile, "a");
                if ( fdHaviorDaily == NULL) {
                    fileLog.writeLog( FormatString("open havior file fail: %").arg(strerror(errno)).str() );
                }

                while(1)
                {
                    if(pthread_mutex_lock(&mutex_task) != 0)
                    {
                        fileLog.writeLog((FormatString("mutex lock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
                    }

                    if(list_task.size() == 0)
                    {
                        fileLog.writeLog("task list size is 0, see url list\n");
                        if(pthread_mutex_unlock(&mutex_task) != 0)
                        {
                            fileLog.writeLog((FormatString("mutex unlock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
                        }	

                        break;
                    }

                    node_t = list_task.front();
                    list_task.pop_front();

                    if(pthread_mutex_unlock(&mutex_task) != 0)
                    {
                        fileLog.writeLog((FormatString("mutex unlock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
                    }

                    DBUpdateTask(node_t);
                    delete node_t;
                    node_t = NULL;
                }

                while(1)
                {
                    if(pthread_mutex_lock(&mutex_task) != 0)
                    {
                        fileLog.writeLog((FormatString("mutex lock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
                    }

                    if(list_task_domain.size() == 0)
                    {
                        fileLog.writeLog("task domain list size is 0, see url list\n");
                        if(pthread_mutex_unlock(&mutex_task) != 0)
                        {
                            fileLog.writeLog((FormatString("mutex unlock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
                        }

                        break;
                    }

                    node_td = list_task_domain.front();
                    list_task_domain.pop_front();

                    if(pthread_mutex_unlock(&mutex_task) != 0)
                    {
                        fileLog.writeLog((FormatString("mutex unlock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
                    }

                    DBUpdateTaskDomain(node_td);
                    delete node_td;
                    node_td = NULL;
                }

                //·ÖÅú´ÎÌá½» weide
                time_t timestart = time(0);

                while(1)
                {
                    if(pthread_mutex_lock(&mutex_url) != 0)
                    {
                        fileLog.writeLog((FormatString("mutex lock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
                    }
                    flag=true;

                    if(list_url.size() == 0)
                    {
                        fileLog.writeLog("url list is 0, sleep 5 s later...........\n");
                        //·¢Êý¾Ý¿âÐÄÌø°ü
                        string heart_sql = "select user()";
                        m_dbStmt = m_dbConn->getStatement(heart_sql);
                        m_dbStmt->execSQL();
                        if(pthread_mutex_unlock(&mutex_url) != 0)
                        {
                            fileLog.writeLog((FormatString("mutex unlock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
                        }
                        flag=false;
                        break;
                    }

                    node_u = list_url.front();
                    fileLog.writeLog((FormatString("url list [%.%.%] [%] size[%]\n").arg(node_u->task_id).arg(node_u->template_id).arg(node_u->corp_id).arg(node_u->seq_no).arg(list_url.size()).str()));
                    list_url.pop_front();
                    //fileLog.writeLog((FormatString("url list 1 [%.%.%] [%] size[%]\n").arg(node_u->task_id).arg(node_u->template_id).arg(node_u->corp_id).arg(node_u->seq_no).arg(list_url.size()).str()));

                    if(pthread_mutex_unlock(&mutex_url) != 0)
                    {
                        fileLog.writeLog((FormatString("mutex unlock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
                    }

                    DBUpdateUrl(node_u);

                    fileLog.writeLog((FormatString("url list 1 [%.%.%] [%] size[%]\n").arg(node_u->task_id).arg(node_u->template_id).arg(node_u->corp_id).arg(node_u->seq_no).arg(list_url.size()).str()));

                    delete node_u;
                    node_u = NULL;

                    if (bdb_pici == "1" && time(0) - timestart > dbiter) {
                        timestart = time(0);
                        fileLog.writeLog("fen pi commit begin\n");
                        commit_bdbcache_to_mysql();
                    }
                }

                //add 2014/02 
                commit_bdbcache_to_mysql();

                if (fdHaviorDaily) {
                    fclose(fdHaviorDaily);
                    fdHaviorDaily = NULL;
                }

                sleep(dbiter);
            }
        }
        catch(DBException & e)
        {
            if(node_u)
            {
                fileLog.writeLog((FormatString("db fail DBException 1 : [%] [%]\n").arg(e.what()).arg(node_u->seq_no).str()));
                delete node_u;	
            }
            if(node_t)
            {
                fileLog.writeLog((FormatString("db fail DBException 2 : [%]\n").arg(e.what()).str()));
                delete node_t;
            }

            if(node_td)
            {
                fileLog.writeLog((FormatString("db fail DBException 3 : [%]\n").arg(e.what()).str()));
                delete node_td;
            }

            if(flag)
            {
                if(pthread_mutex_unlock(&mutex_url) != 0)
                {
                    fileLog.writeLog((FormatString("mutex unlock fail [%]  [%]\n").arg(errno).arg(strerror(errno)).str()));
                }
            }

            //			string errorstr = "MYSQL error:2006 (MySQL server has gone away), debug info:select * from tb_corp_task_info where Task_id='594' and template_id='207' and Corp_id='113'";
            if(strstr(e.what(),"MYSQL error:2006") != NULL)
            {
                fileLog.writeLog((FormatString("mysql has gone away,then reconnect db %\n").arg(e.what()).str()));
                DataBase::getInstance().initDatebase();
                fileLog.writeLog((FormatString("mysql has gone away,reconnected db aaaaaaa %\n").arg(e.what()).str()));
                try{
                    m_dbConn = DataBase::getInstance().getConnection(m_dbparams);
                    fileLog.writeLog((FormatString("mysql has gone away,reconnected db %\n").arg(e.what()).str()));
                }catch(...){
                    fileLog.writeLog((FormatString("mysql has gone away,reconnected failed\n")).str());
                }
            }
            sleep(dbiter);
            continue;
        }
    }

    fuckexit = 1;
    fileLog.writeLog("DBprocess exit\n");
    pthread_exit(NULL);
}

CacheDbApp::CacheDbApp(void) : Application("cachedbapp")
{
    memset(m_cachedbsvrport,0,sizeof(m_cachedbsvrport));
}

bool CacheDbApp::daemon(void)
{
    pid_t pid;
    if((pid=fork()) < 0)
    {
        printf("fork fail \n");	
        return false;
    }
    else if(pid > 0)
    {
        printf("this is parent process\n");	
        exit(0);
    }
    else
    {
        printf("this is child process\n");
        return true;
    }
}

bool CacheDbApp::onInit(int argc, char * argv[])
{
    daemon();
    ConfigureMgr conf;
    if(argc < 2 || !conf.loadConfFile(argv[1]))
    {
        printf("ÅäÖÃÎÄ¼þ³õÊ¼»¯Ê§°Ü\n");
        return false;
    }
    stdstring tmp;

    tmp = conf.getString("/iedm/application/CachedbSvrPort");
    snprintf(m_cachedbsvrport,strlen(tmp.c_str())+1,tmp.c_str());
    tmp = conf.getString("/iedm/application/CacheDbLogPath");
    fileLog.init(tmp, 10*1024*1024);
    fileLog.writeLog("log start ok\n");

    if(CreateServer() == -1)
        exit(0);

    // add 2014/03/03
    bdb_cache_file  = conf.getString("/iedm/datasources/webadmin/bdb_cache");
    bdb_havior_history_file = conf.getString("/iedm/datasources/webadmin/bdb_havior_history");
    havior_daily_file = conf.getString("/iedm/datasources/webadmin/havior_daily");
    bdb_pici = conf.getString("/iedm/datasources/webadmin/bdb_pici");

    string dbserver = conf.getString("/iedm/datasources/webadmin/Server");
    string dbuser   = conf.getString("/iedm/datasources/webadmin/User");
    string password = conf.getString("/iedm/datasources/webadmin/Password");
    CacheDbApp::m_database = conf.getString("/iedm/datasources/webadmin/Database");
    string charset  = conf.getString("/iedm/datasources/webadmin/Charset");
    string port     = conf.getString("/iedm/datasources/webadmin/Port");

    g_tablename.tb_region_info = conf.getString("/iedm/table/tb_region_info");
    g_tablename.tb_domain_map = conf.getString("/iedm/table/tb_domain_map");
    g_tablename.tb_stat_task_unsubscribe = conf.getString("/iedm/table/tb_stat_task_unsubscribe");
    g_tablename.tb_task = conf.getString("/iedm/table/tb_task");
    g_tablename.tb_triger = conf.getString("/iedm/table/tb_triger");
    g_tablename.tb_result = conf.getString("/iedm/table/tb_result");
    g_tablename.tb_result_by_domain = conf.getString("/iedm/table/tb_result_by_domain");
    g_tablename.tb_result_by_hour = conf.getString("/iedm/table/tb_result_by_hour");
    g_tablename.tb_result_by_region = conf.getString("/iedm/table/tb_result_by_region");
    g_tablename.tb_result_by_url = conf.getString("/iedm/table/tb_result_by_url");
    g_tablename.tb_stat_havior = conf.getString("/iedm/table/tb_stat_havior");

    lbsvr_host = conf.getString("/iedm/application/lbsvr_host");
    lbsvr_port = conf.getInt("/iedm/application/lbsvr_port");

    fileLog.writeLog(( FormatString("LabelServer: [%:%]\n").arg(lbsvr_host).arg(lbsvr_port).str() ));

    m_dbparams.setEnvParam("Server",dbserver);
    m_dbparams.setEnvParam("Port",port);
    m_dbparams.setEnvParam("User",dbuser);
    m_dbparams.setEnvParam("Password",password);
    m_dbparams.setEnvParam("Database",CacheDbApp::m_database);
    m_dbparams.setEnvParam("Charset",charset);
    m_dbparams.setEnvParam("Driver","mysql");

    pthread_mutex_init(&mutex_url, NULL);
    pthread_mutex_init(&mutex_task, NULL);
    DataBase::getInstance().initDatebase();
    m_dbConn = DataBase::getInstance().getConnection(m_dbparams);

    //	char out[56];
    // ipµØÖ·ÐÅÏ¢  ==¡·0.0.0.0|0.255.255.255|IANA±£ÁôµØÖ·|CZ88.NET
    tmp = conf.getString("/iedm/application/ipdatapath");
    vec_ip.clear();
    load((char *)tmp.c_str(),vec_ip);

    string s_dbiter  = conf.getString("/iedm/application/Dbiterval");
    string s_recviter= conf.getString("/iedm/application/Recviterval");
    m_nginxdomain = conf.getString("/iedm/application/nginxdomain");
    dbiter=atoi((char *)s_dbiter.c_str());
    recviter=atoi((char *)s_recviter.c_str());

    string select = "select region_id , province_name from " + g_tablename.tb_region_info + "  group by 2";
    m_dbStmt = m_dbConn->getStatement(select);
    m_dbStmt->execSQL();

    int num = m_dbStmt->getRecordCount();
    char region[11]={0};
    char province[32]={0};
    map_region.clear();
    map_bigcity.clear();

    //µØÇø±àºÅ, Ãû³Æ
    for(;num!=0;num--)
    {
        m_dbStmt->fetch();

        snprintf(region,sizeof(region),"%s",(char *)m_dbStmt->getField(0).getFieldData());
        snprintf(province,sizeof(province),"%s",(char *)m_dbStmt->getField(1).getFieldData());
        if(strcmp(province,"ÄÚÃÉ¹Å×ÔÖÎÇø") == 0)
            snprintf(province,sizeof(province),"%s","ÄÚÃÉ¹Å");
        if(strcmp(province,"ÄþÏÄ»Ø×å×ÔÖÎÇø") == 0)
            snprintf(province,sizeof(province),"%s","ÄþÏÄ");
        if(strcmp(province,"¹ãÎ÷×³×å×ÔÖÎÇø") == 0)
            snprintf(province,sizeof(province),"%s","¹ãÎ÷");
        if(strcmp(province,"ÐÂ½®Î¬Îá¶û×ÔÖÎÇø") == 0)
            snprintf(province,sizeof(province),"%s","ÐÂ½®");
        if(strcmp(province,"Î÷²Ø×ÔÖÎÇø") == 0)
            snprintf(province,sizeof(province),"%s","Î÷²Ø");

        if(atoi(region) > 1000000)				//×÷ÎªÊ¡·ÝÒ»Ñù£¬µ¥¶ÀÁÐ³öµÄ³ÇÊÐ
        {
            map_bigcity[province] = atoi(region);
            fileLog.writeLog(( FormatString("zone bigcity map [%]-->[%]\n").arg(province).arg(atoi(region)).str() ));
            continue;
        }

        map_region[province]=atoi(region);
        fileLog.writeLog(( FormatString("zone map [%]-->[%]\n").arg(province).arg(atoi(region)).str() ));
    }


    //ÓòÃûÍ³Ò»Ó³Éä , ÓòÃû -> Ó³ÉäÓòÃû vip.qq.com->qq.com foxmail.com->qq.com
    select = "select domain, map_domain from " + g_tablename.tb_domain_map;
    m_dbStmt = m_dbConn->getStatement(select);
    m_dbStmt->execSQL();

    num = m_dbStmt->getRecordCount();
    for(;num!=0;num--)
    {
        m_dbStmt->fetch();

        map_domain[(char * )m_dbStmt->getField(0).getFieldData()] = (char *)m_dbStmt->getField(1).getFieldData();
        fileLog.writeLog(( FormatString("domain map [%]-->[%]\n").arg((char *)m_dbStmt->getField(0).getFieldData()).arg((char *)m_dbStmt->getField(1).getFieldData()).str() ));
    }

    struct sigaction act, oldact;
    act.sa_handler = show_handler;
    act.sa_flags = 0;
    //sigaction(SIGPIPE, &act, &oldact);
    //sigaction(SIGTERM, &act, &oldact);

    signal(SIGPIPE, show_handler);
    signal(SIGTERM, show_handler);

    return true;
}

int CacheDbApp::onRun(void)
{
    //ÆðÏß³Ì·¢ËÍ    
    pthread_attr_t attr;
    pthread_t tid;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    if(pthread_create(&tid, &attr, (void* (*)(void*)) DBprocess, NULL) != 0)
    {
        fileLog.writeLog(( FormatString("thread start fail [%]\n").arg(strerror(errno)).str() ));	
    }

    while(terminal != 1)
    {
        ReceiveRequest();	
        sleep(recviter);
    }

    close(m_listener);

    while( fuckexit != 1) {
        sleep(recviter);
    }

    fileLog.writeLog("onRun exit\n");
    return 0;
}

int CacheDbApp::CreateServer()
{
    struct sockaddr_in my_addr;
    int flag=1,len=sizeof(int); 
    if((m_listener = socket(PF_INET, SOCK_STREAM, 0)) == -1)  
    {  
        fileLog.writeLog((FormatString("socket create fail errno [%] strerror [%]\n").arg(errno).arg(strerror(errno)).str()));
        return -1;
    }  
    else  
    {  
        fileLog.writeLog("socket create ok\n");  
    }  

    if(fcntl(m_listener, F_SETFL, fcntl(m_listener, F_GETFD, 0)|O_NONBLOCK) == -1)  
    {  
        fileLog.writeLog((FormatString("set nonblock fail errno [%] strerror [%]\n").arg(errno).arg(strerror(errno)).str()));
        return -1;  
    }  

    if(setsockopt(m_listener, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1) 
    { 
        fileLog.writeLog((FormatString("set SO_REUSEADDR fail errno [%] strerror [%]\n").arg(errno).arg(strerror(errno)).str()));
        return -1;
    } 


    bzero(&my_addr, sizeof(my_addr));  
    my_addr.sin_family = PF_INET;  
    my_addr.sin_port = htons(atoi(m_cachedbsvrport));  
    my_addr.sin_addr.s_addr = INADDR_ANY;  

    if (bind(m_listener, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1)   
    {  
        fileLog.writeLog((FormatString("socket bind errno [%] strerror [%]\n").arg(errno).arg(strerror(errno)).str()));
        return -1;
    }   
    else  
    {  
        fileLog.writeLog("bind ip ok\n");  
    }  

    if (listen(m_listener, 5) == -1)   
    {  
        fileLog.writeLog((FormatString("listen fail errno [%] strerror [%]\n").arg(errno).arg(strerror(errno)).str()));
        return -1;
    }  
    else  
    {  
        fileLog.writeLog("listen ok\n");  
    }  

    m_kdpfd = epoll_create(MAXEPOLLSIZE);
    m_ev.events = EPOLLIN | EPOLLET;
    m_ev.data.fd = m_listener;
    if (epoll_ctl(m_kdpfd, EPOLL_CTL_ADD, m_listener, &m_ev) < 0)
    {
        fileLog.writeLog((FormatString("listen epoll add fail errno [%] strerror [%]\n").arg(errno).arg(strerror(errno)).str()));
        return -1;
    }
    else
    {
        fileLog.writeLog("add listen socket to epoll ok\n");
    }
    m_curfds = 1;
    return 0;
}

int CacheDbApp::ReceiveRequest()
{
    int nfds;
    int new_fd;
    int n;
    socklen_t len;

    struct sockaddr_in cli_addr;
    //	unsigned int jump = 0;
    char ip[128];
    while (terminal != 1)   
    {  
        /* µÈ´ýÓÐÊÂ¼þ·¢Éú */  
        fileLog.writeLog("waite begin......\n");
        nfds = epoll_wait(m_kdpfd, m_events, 1280/*m_curfds*/, 3000);  
        if (nfds == -1)  
        {  
            fileLog.writeLog((FormatString("epoll_wait fail errno [%] strerror [%]\n").arg(errno).arg(strerror(errno)).str()));
            break;  
        }  
        if (nfds == 0)
        {
            fileLog.writeLog((FormatString("no even happened current link %\n").arg(m_curfds).str()));
            break;
        }
        //fileLog.writeLog("waite end.......\n");
        fileLog.writeLog((FormatString("waite end ..... nfds=%\n").arg(nfds).str()));
        /* ´¦ÀíËùÓÐÊÂ¼þ */  
        for (n = 0; n < nfds; ++n)  
        {  
            if (m_events[n].data.fd == m_listener)   
            {  
                while(1)
                {
                    len = sizeof(struct sockaddr);
                    new_fd = accept(m_listener, (struct sockaddr *) &cli_addr,&len);  
                    if (new_fd < 0)   
                    {  
                        fileLog.writeLog((FormatString("accept fail errno [%] strerror [%]\n").arg(errno).arg(strerror(errno)).str()));
                        break;  
                    }   
                    else  
                    {  
                        fileLog.writeLog((FormatString("connect from  [%:%] £¬socket is [%]\n").arg(inet_ntoa(cli_addr.sin_addr)).arg(ntohs(cli_addr.sin_port)).arg(new_fd).str()));
                    }  

                    snprintf(ip,sizeof(ip),"%s:%d",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));

                    if(fcntl(new_fd, F_SETFL, fcntl(new_fd, F_GETFD, 0)|O_NONBLOCK) == -1)
                    {
                        fileLog.writeLog((FormatString("set nonblock fail errno [%] strerror [%]\n").arg(errno).arg(strerror(errno)).str()));
                        return -1;
                    }
                    m_ev.events = EPOLLIN | EPOLLET;  
                    m_ev.data.fd = new_fd;  
                    if (epoll_ctl(m_kdpfd, EPOLL_CTL_ADD, new_fd, &m_ev) < 0)  
                    {  
                        fprintf(stderr, "°Ñ socket '%d' ¼ÓÈë epoll Ê§°Ü£¡%s/n",  
                            new_fd, strerror(errno));  
                        return -1;  
                    }  
                    m_curfds++;  
                }
            }   
            else if(m_events[n].events & EPOLLIN)  //ÓÐ¿É¶ÁÊý¾Ý
            {  
                //¶ÁÍêcloseÁ¬½Ó
                HandleMessage(m_events[n].data.fd,ip);
                epoll_ctl(m_kdpfd, EPOLL_CTL_DEL, m_events[n].data.fd,&m_ev);  
                close(m_events[n].data.fd);
                m_curfds--; 
            }  
        }  
    }
    return 0;
}

//Êý¾Ý°ü¸ñÊ½ | 4 bytes | packet data |
int CacheDbApp::HandleMessage(int fd,const char * ip)
{
    char c_len[5];
    int ret,h_length;
    int i = 0;
    memset(c_len,0,5);
    if(recv(fd, c_len, 4, 0) != 4)
    {
        fileLog.writeLog(( FormatString("recv length fail [%] [%]\n").arg(errno).arg(strerror(errno)).str() ));
        return -1;
    }
    memcpy(&h_length, c_len, 4);
    h_length = ntohl(h_length);
    fileLog.writeLog(( FormatString("recv msg length [%]\n").arg(h_length).str() ));

    unsigned int j = h_length;
    if(j >=1024*1024)
    {
        fileLog.writeLog(( FormatString("length invalid errno[%] str[%]\n").arg(errno).arg(strerror(errno)).str() ));
        return -1;
    }

    char * rsp_msg = new char [h_length+1];
    memset(rsp_msg,0,h_length+1);
    fd_set ReadSet;
    FD_ZERO(&ReadSet);
    FD_SET(fd, &ReadSet);
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec= 0;
    int hdata = 0;
    while(1)
    {
        if(i == h_length)
            break;

        if((ret = select(fd+1,&ReadSet,NULL,NULL,&timeout)) == -1)
        {
            fileLog.writeLog(( FormatString("[%] select fail\n").arg(errno).str() ));
            delete []rsp_msg;
            return -1;
        }
        else if(ret == 0)
        {
            fileLog.writeLog("recv msg body select time out\n");
            break;
        }
        else
        {
            hdata++;
            fileLog.writeLog("recv msg have data to read\n");
        }

        ret = recv(fd, rsp_msg + i, j, 0);
        if(ret == 0)
        {
            fileLog.writeLog(( FormatString("[%] has no data to read\n").arg(errno).str() ));
            delete []rsp_msg;
            return -1;
        }

        j -= ret;
        i += ret;
    }	

    fileLog.writeLog(( FormatString("recv msg length real [%] should [%] \n").arg(i).arg(h_length).str() ));
    parse.ParseMessage(rsp_msg,m_task_map, m_url_map,ip, (char *)m_nginxdomain.c_str());
    parse.Response(fd);
    delete [] rsp_msg;
    return 0;
}

int CheckHistory(_url_node * un,  struct tm * lt, int flag)
{
    unsigned char havior;
    if(flag == 1)   //[1;9H
    {
        havior = 2;
    }
    else if(flag == 2)  //[1;9H
    {
        havior = 1;
    }

    char tablename[32];
    char lasttablename[32];
    snprintf(tablename,sizeof(tablename),"%s_%d%02d",g_tablename.tb_stat_havior.c_str(),1900+lt->tm_year,1+lt->tm_mon);
    if(1+lt->tm_mon == 1) 
        snprintf(lasttablename,sizeof(lasttablename),"%s_%d%02d",g_tablename.tb_stat_havior.c_str(),1889+lt->tm_year,12);
    else
        snprintf(lasttablename,sizeof(lasttablename),"%s_%d%02d",g_tablename.tb_stat_havior.c_str(), 1900+lt->tm_year,lt->tm_mon);

    //¼ì²éµ±ÔÂ±í£¬Ã»ÓÐ¾ÍÖØ½¨
    string sql = "SELECT table_name FROM information_schema.TABLES WHERE table_name =:tablename and TABLE_SCHEMA='" + CacheDbApp::m_database +"'";
    m_dbStmt = m_dbConn->getStatement(sql);
    m_dbStmt->getParam("tablename").assignString(tablename,strlen(tablename));
    m_dbStmt->execSQL();
    if(m_dbStmt->getRecordCount() == 0)
    {
        string tmp1="create table ";
        string tmp2=tablename;
        string tmp3=" (Corp_id int not null,Task_id int not null, Template_id int not null,email char (64),havior TINYINT,osid TINYINT,browserid TINYINT,slangid TINYINT,open TINYINT default 0,click TINYINT default 0,updateTime datetime, PRIMARY KEY  (Task_id, Template_id, email,havior)) ENGINE=MyISAM DEFAULT CHARSET=utf8";

        string sql=tmp1+tmp2+tmp3;
        m_dbStmt = m_dbConn->getStatement(sql);
        //		m_dbStmt->getParam("tablename").assignString(tablename,strlen(tablename));
        m_dbStmt->execSQL();

        fileLog.writeLog(( FormatString("Create the month table:%\n").arg(tablename).str() ));
    }

    //¼ì²éÉÏÔÂ±í£¬Ã»ÓÐ¾ÍÖØ½¨
    sql = "SELECT table_name FROM information_schema.TABLES WHERE table_name =:lasttablename and TABLE_SCHEMA='" + CacheDbApp::m_database +"'";
    m_dbStmt = m_dbConn->getStatement(sql);
    m_dbStmt->getParam("lasttablename").assignString(lasttablename,strlen(lasttablename));
    m_dbStmt->execSQL();
    if(m_dbStmt->getRecordCount() == 0)
    {
        string tmp1="create table ";
        string tmp2=lasttablename;
        string tmp3=" (Corp_id int not null,Task_id int not null, Template_id int not null,email char (64),havior TINYINT,osid TINYINT,browserid TINYINT,slangid TINYINT,open TINYINT default 0,click TINYINT default 0,updateTime datetime, PRIMARY KEY  (Task_id, Template_id, email,havior)) ENGINE=MyISAM DEFAULT CHARSET=utf8";

        string sql=tmp1+tmp2+tmp3;
        m_dbStmt = m_dbConn->getStatement(sql);
        //		m_dbStmt->getParam("lasttablename").assignString(lasttablename,strlen(lasttablename));
        m_dbStmt->execSQL();

        fileLog.writeLog(( FormatString("Create last month table:%\n").arg(lasttablename).str() ));
    }


    string tmp1="select havior from ";
    string tmp3=" where task_id=:task_id and Template_id=:template_id and email=:email and havior=:havior";
    string tmp2=tablename;
    sql = tmp1+tmp2+tmp3;
    m_dbStmt = m_dbConn->getStatement(sql);
    m_dbStmt->getParam("task_id").assignInteger(un->task_id);
    m_dbStmt->getParam("template_id").assignInteger(un->template_id);
    m_dbStmt->getParam("email").assignString(un->mail,strlen(un->mail));
    m_dbStmt->getParam("havior").assignInteger(havior);
    m_dbStmt->execSQL();
    //unsigned char havior;

    if(m_dbStmt->getRecordCount() == 1){
        return 2;
    }
    else	//µ±ÔÂÎÞ¼ÇÂ¼Ðè²éÉÏÔÂÔÂ±í
    {
        string tmp1="select havior from ";
        string tmp2=lasttablename;
        string tmp3=" where task_id=:task_id and template_id=:template_id and email=:email";

        sql=tmp1+tmp2+tmp3;
        m_dbStmt = m_dbConn->getStatement(sql);
        m_dbStmt->getParam("task_id").assignInteger(un->task_id);
        m_dbStmt->getParam("template_id").assignInteger(un->template_id);
        m_dbStmt->getParam("email").assignString(un->mail,strlen(un->mail));
        m_dbStmt->execSQL();

        if(m_dbStmt->getRecordCount() == 1)
        {
            m_dbStmt->fetch();
            if((char *)m_dbStmt->getField(0).getFieldData() != NULL)
            {
                havior = atoi((char *)m_dbStmt->getField(0).getFieldData());
            }
            if(flag == 1)   //µã»÷ 
            {
                if(havior == 1) //´ò¿ª
                {
                    return 3;           //ÐèÒª²åÈëÐÂ¼ÍÂ¼£¬ÇÒ¸üÐÂuserÊý
                }
                else if(havior == 2)    //µã»÷
                {
                    return 4;       //²åÈëÐÂ¼ÍÂ¼£¬²»ÐèÒª¸üÐÂuserÊý
                }
            }
            else if(flag == 2)  //´ò¿ª
            {
                return 4;       //²åÈëÐÂ¼ÍÂ¼£¬²»ÐèÒª¸üÐÂuserÊý
            }
        }
        else		//ÈÔÎÞ¼ÇÂ¼ÐèÒª²åÈëÒ»ÌõÐÂ¼ÇÂ¼
        {
            return 3;		//²åÈëÐÂ¼ÍÂ¼£¬ÐèÒª¸üÐÂuserÊý
        }
    }
    return 0;
}

unsigned int GetRegionId(const char * area, unsigned int & region_id)
{
    region_id = 1;
    map<string,unsigned int>::iterator it = map_region.begin();
    for(;it!=map_region.end();it++)										
    {
        if(strstr(area, (char *)it->first.c_str()) != NULL)
        {
            region_id = it->second;
        }
    }

    for(it=map_bigcity.begin();it!=map_bigcity.end();it++)
    {
        if(strstr(area, (char *)it->first.c_str()) != NULL)
        {
            region_id = it->second;
        }
    }

    if(region_id == 1)
        region_id = 999;	

    return 0;
}

// add 2014/02
char* strlower( char *str )
{
    char *p = str;
    while (*p != '\0'){
        if(*p >= 'A' && *p <= 'Z')
            *p = (*p) + 0x20;
        p++;
    }
    return str;
}

int DBUpdateUrl(_url_node * un)
{
    /* add by weide 2014/02  */
    result_t result_record;
    memset(&result_record, 0x00, sizeof(result_record) );

    char keybuf[1024] = {0};
    snprintf(keybuf, sizeof(keybuf)-1, BDB_RESULT"%d,%d,%d", un->corp_id, un->task_id, un->template_id);

    string result_key = keybuf;
    /* end */

    int iret;
    bool is_upusernum = false;

    string sql ="select task_id from " + g_tablename.tb_task 
        + " where Task_id=:task_id and template_id=:template_id and Corp_id=:corp_id";

    m_dbStmt = m_dbConn->getStatement(sql);
    m_dbStmt->getParam("task_id").assignInteger(un->task_id);
    m_dbStmt->getParam("template_id").assignInteger(un->template_id);
    m_dbStmt->getParam("corp_id").assignInteger(un->corp_id);
    m_dbStmt->execSQL();

    time_t timep;
    time(&timep);
    struct tm * lt = localtime(&timep);
    DateTime dt;
    dt.setDateTime(1900 + lt->tm_year, 1+lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);

    if((un->task_id > 20120700 && un->task_id < 20200000) 
        || (un->task_id > 1020120800 && un->task_id < 1020200000)
        || (un->task_id > 100000000 && un->task_id <  1000000000))
    {
        fileLog.writeLog((FormatString("auto task [%.%.%] \n").
            arg(un->task_id).arg(un->template_id).arg(un->corp_id).str()));
    }
    else 
    {
        if(m_dbStmt->getRecordCount() != 1) {
            fileLog.writeLog((FormatString("no record in % [%.%.%] [%] [%]\n")
                .arg(g_tablename.tb_task)
                .arg(un->task_id)
                .arg(un->template_id)
                .arg(un->corp_id)
                .arg(un->seq_no)
                .arg(iret).str()));
            return -1;
        }
    }

    if((un->tag == 3 || un->tag == 4)) //ÍË¶©¸üÐÂlist±í£¬ÒÔ¼°ÍË¶©ÈËÊý
    {
        sql ="select * from " + g_tablename.tb_stat_task_unsubscribe 
            + " where corp_id=:corp_id and task_id=:task_id and template_id=:template_id and email=:email";

        m_dbStmt = m_dbConn->getStatement(sql);
        m_dbStmt->getParam("corp_id").assignInteger(un->corp_id);
        m_dbStmt->getParam("task_id").assignInteger(un->task_id);
        m_dbStmt->getParam("template_id").assignInteger(un->template_id);
        m_dbStmt->getParam("email").assignString(un->mail,strlen(un->mail));
        m_dbStmt->execSQL();

        if((iret=m_dbStmt->getRecordCount()) == 0)
        {
            fileLog.writeLog((FormatString("no record in % [%] [%]\n").
                arg(g_tablename.tb_stat_task_unsubscribe).arg(un->mail).arg(un->corp_id).str()));

            sql = "insert into " + g_tablename.tb_stat_task_unsubscribe 
                + "(corp_id,task_id,template_id,email) values(:corp_id,:task_id,:template_id,:email)";

            m_dbStmt = m_dbConn->getStatement(sql);
            m_dbStmt->getParam("corp_id").assignInteger(un->corp_id);
            m_dbStmt->getParam("task_id").assignInteger(un->task_id);
            m_dbStmt->getParam("template_id").assignInteger(un->template_id);
            m_dbStmt->getParam("email").assignString(un->mail,strlen(un->mail));
            m_dbStmt->execSQL();

            // add 2014/02
            result_record.unsubscribe_count += 1;  //g_tablename.tb_result
            Region_Update(un,3,1);
			Domain_Update(un,3,1);
        }
    } //result

    if(un->tag == 5)		//×ª·¢Ö±½Ó¸üÐÂ×ª·¢Êý
    {
        //add 2014/02
        result_record.forward_count += 1;  //g_tablename.tb_result

        Region_Update(un,4,1);
        Domain_Update(un,4,1);
    }

    if(un->tag == 2)                //´ò¿ªÓÊ¼þ±ê¼Ç,´ò¿ªÊý+1
    {
        // add 2014/02
        result_record.read_count += 1;

        fileLog.writeLog((FormatString("update open record [%.%.%] [%]\n").
            arg(un->task_id).arg(un->template_id).arg(un->corp_id).arg(un->seq_no).str()));

        strlower(un->mail);
        snprintf(keybuf, sizeof(keybuf), 
            "%d,%d,%d,'%s',1", un->corp_id, un->task_id, un->template_id, un->mail);

        iret = find_havior_history(keybuf, true) == 0 ? 2:1;

        stat_havior_t stat_record;
        memset (&stat_record, 0x00, sizeof(stat_record) );

        stat_record.corp_id = un->corp_id;
        stat_record.task_id = un->task_id;
        stat_record.template_id = un->template_id;
        snprintf(stat_record.email,sizeof(stat_record.email)-1,un->mail);
        stat_record.havior = 1;
        stat_record.osid = un->osid;
        stat_record.browserid = un->browserid;
        stat_record.slangid = un->slangid;
        stat_record.open = 1;
        stat_record.click = 0;
        stat_record.updatetime = time(0);
        stat_record.url = NULL;//strdup(un->url);

        struct tm *tms = localtime(&stat_record.updatetime);
        snprintf(keybuf, sizeof(keybuf), "%s_%04d%02d", g_tablename.tb_stat_havior.c_str(),tms->tm_year + 1900, tms->tm_mon + 1);

        update_cache_stat_havior(keybuf, &stat_record);
        if (stat_record.url)
            free(stat_record.url);

        if(iret == 1){
            is_upusernum = true;
            result_record.read_user_count += 1;
        }

        Region_Update(un,2,is_upusernum);
        Domain_Update(un,2,is_upusernum);
        Hour_Update(un,2);

        fileLog.writeLog((FormatString("open mail update db ok [%.%.%] [%], ret[%]\n").
            arg(un->task_id).arg(un->template_id).arg(un->corp_id).arg(un->seq_no).arg(iret).str()));
    }

    else if(un->tag == 1)               //µã»÷ÓÊ¼þ±ê¼Ç,µã»÷Êý+1
    {
        result_record.click_count += 1;

        fileLog.writeLog((FormatString("update click record [%.%.%] [%]\n").
            arg(un->task_id).arg(un->template_id).arg(un->corp_id).arg(un->seq_no).str()));

        // add 2014/02
        strlower(un->mail);
        snprintf(keybuf, sizeof(keybuf)-1, 
            "%d,%d,%d,'%s',2", un->corp_id, un->task_id, un->template_id, un->mail);

        iret = find_havior_history(keybuf, true) == 0 ? 2:1;

        stat_havior_t stat_record;
        memset (&stat_record, 0x00, sizeof(stat_record) );

        stat_record.corp_id = un->corp_id;
        stat_record.task_id = un->task_id;
        stat_record.template_id = un->template_id;
        snprintf(stat_record.email,sizeof(stat_record.email)-1,un->mail);
        stat_record.havior = 2;
        stat_record.osid = un->osid;
        stat_record.browserid = un->browserid;
        stat_record.slangid = un->slangid;
        stat_record.open = 0;
        stat_record.click = 1;
        stat_record.updatetime = time(0);
        stat_record.url = strdup(un->url);

        struct tm *tms = localtime(&stat_record.updatetime);
        snprintf(keybuf, sizeof(keybuf), "%s_%04d%02d", g_tablename.tb_stat_havior.c_str(),tms->tm_year + 1900, tms->tm_mon + 1);

        update_cache_stat_havior(keybuf, &stat_record);
        if (stat_record.url)
            free(stat_record.url);

        if(iret == 1){
            is_upusernum = true;
            result_record.click_user_count += 1;
        }

        Region_Update(un,1,is_upusernum);
        Domain_Update(un,1,is_upusernum);
        Hour_Update(un,1);	

        //url add 2014/02
        strlower(un->url);
        snprintf(keybuf, sizeof(keybuf)-1, BDB_URL
            "%d,%d,%d,'%s'", un->corp_id, un->task_id, un->template_id, un->url);

        url_result_t url_record;
        url_record.click_count = 1;
        update_cache_url_result(keybuf, &url_record);

        fileLog.writeLog((FormatString("click mail update db ok [%.%.%] [%], ret[%]\n").
            arg(un->task_id).arg(un->template_id).arg(un->corp_id).arg(un->seq_no).arg(iret).str()));
    }

    //add 2014/02
    update_cache_result(result_key.c_str(), &result_record);
    return 0;
}

//flag 1 µã»÷ 2 ´ò¿ª 3 ÍË¶© 4 ×ª·¢
//is_upusernum 1 ÓÃ»§ÊýÀÛ¼Ó 0 ²»ÐèÀÛ¼Ó
int Domain_Update(_url_node * un,int flag,bool is_upusernum)
{
    string sql;
    char domain[64];
    char domain_tmp[64];
    memset(domain,0,64);
    memset(domain_tmp,0,64);
    char tmp[64];
    sscanf(un->mail,"%[^@]@%[^@]",tmp,domain_tmp);
    snprintf(domain,sizeof(domain),"%s",domain_tmp);

   /* delete for HuaRun
    map<string,string>::iterator it = map_domain.begin();
    for(;it != map_domain.end(); it++){
        if(strcasecmp(it->first.c_str(),domain_tmp) == 0){
            snprintf(domain,sizeof(domain),"%s",(char *)map_domain[it->first].c_str());
            break;
        }
    }
    if(it == map_domain.end()){
        snprintf(domain,sizeof(domain),"%s","other");	
    }
    */

    domain_result_t rst;
    memset(&rst, 0x00, sizeof(rst) );

    char keybuf[1024];
    snprintf(keybuf, sizeof(keybuf)-1, BDB_DOMAIN
        "%d,%d,%d,'%s'", un->corp_id, un->task_id, un->template_id, strlower(domain));

    if(flag == 2){
        rst.read_count += 1;
        rst.read_user_count += (is_upusernum ? 1:0);
    }
    if(flag == 1){
        rst.click_count += 1;
        rst.click_user_count += (is_upusernum ? 1:0);
    }
    if(flag == 3){
        rst.unsubscribe_count += 1;
    }
    if(flag == 4){
        rst.forward_count += 1;
    }

    update_cache_domain_result(keybuf, &rst);

    fileLog.writeLog((FormatString("Domain_Update db ok [%.%.%] [%]\n")
        .arg(un->task_id)
        .arg(un->template_id)
        .arg(un->corp_id)
        .arg(domain).str()));
    return 0;	
}

int Hour_Update(_url_node * un, int flag)
{
    hour_result_t rst;
    memset(&rst, 0x00, sizeof(rst) );

    char keybuf[1024];
    snprintf(keybuf, sizeof(keybuf)-1, BDB_HOUR"%d,%d,%d,%d", un->corp_id, un->task_id, un->template_id, un->hour);

    if(flag == 2){		//´ò¿ª
        rst.read_count += 1;
    }
    if(flag == 1){       //µã»÷
        rst.click_count += 1;
    }

    update_cache_hour_result(keybuf, &rst);

    return 0;
}


//flag 1 µã»÷ 2 ´ò¿ª 3 ÍË¶© 4 ×ª·¢
//is_upusernum 1 ÓÃ»§ÊýÀÛ¼Ó 0 ²»ÐèÀÛ¼Ó
int Region_Update(_url_node * un,int flag,bool is_upusernum)
{
    unsigned int region_id;
    GetRegionId(un->area, region_id);

    region_result_t rst;
    memset(&rst, 0x00, sizeof(rst));

    char keybuf[1024];
    snprintf(keybuf, sizeof(keybuf)-1, BDB_REGION
        "%d,%d,%d,%d", un->corp_id, un->task_id, un->template_id, region_id);

    if(flag == 2){
        rst.read_count += 1;
        rst.read_user_count += (is_upusernum ? 1:0);
    }
    if(flag == 1){
        rst.click_count += 1;
        rst.click_user_count += (is_upusernum ? 1:0);
    }
    if(flag == 3){
        rst.unsubscribe_count += 1;
    }
    if(flag == 4){
        rst.forward_count += 1;
    }

    fileLog.writeLog(( FormatString("% [%.%.%] [%] update ok\n")
        .arg(g_tablename.tb_result_by_region)
        .arg(un->task_id)
        .arg(un->template_id)
        .arg(un->corp_id)
        .arg(region_id).str()));

    update_cache_region_result(keybuf, &rst);
    return 0;	
}

int DBUpdateTaskDomain(_tdomain_node * td)
{
    string sql ="select * from " + g_tablename.tb_task + " where Task_id=:task_id and template_id=:template_id and Corp_id=:corp_id";

    m_dbStmt = m_dbConn->getStatement(sql);
    m_dbStmt->getParam("task_id").assignInteger(td->task_id);
    m_dbStmt->getParam("template_id").assignInteger(td->template_id);
    m_dbStmt->getParam("corp_id").assignInteger(td->corp_id);
    m_dbStmt->execSQL();

    if((td->task_id > 20120700 && td->task_id < 20200000) || (td->task_id > 1020120800 && td->task_id < 1020200000) || (td->task_id > 100000000 && td->task_id < 1000000000))
    {
        //		td->template_id = 0;
        //		td->corp_id = 0;
        fileLog.writeLog((FormatString("auto task [%.%.%] \n").arg(td->task_id).arg(td->template_id).arg(td->corp_id).str()));
    }
    else
    {
        if(m_dbStmt->getRecordCount() != 1)
        {
            fileLog.writeLog((FormatString("DBUpdateTaskDomain no record in % [%.%.%] \n").arg(g_tablename.tb_task).arg(td->task_id).arg(td->template_id).arg(td->corp_id).str()));
            return -1;
        }
    }

    sql = "select * from " + g_tablename.tb_result_by_domain + " where Task_id=:task_id and template_id=:template_id and Corp_id=:corp_id and email_domain=:email_domain";

    m_dbStmt = m_dbConn->getStatement(sql);
    m_dbStmt->getParam("task_id").assignInteger(td->task_id);
    m_dbStmt->getParam("template_id").assignInteger(td->template_id);
    m_dbStmt->getParam("corp_id").assignInteger(td->corp_id);
    m_dbStmt->getParam("email_domain").assignString(td->domain,strlen(td->domain));
    m_dbStmt->execSQL();

    if(m_dbStmt->getRecordCount() == 0)					// Ã»ÓÐÊý¾Ý¾Í²åÈë
    {
        if(td->none_exist_count != 0 || td->none_exist_count1 != 0 || td->dns_fail_count != 0 || td->spam_garbage_count != 0)
        {
            sql = "insert into " + g_tablename.tb_result_by_domain + " (corp_id,task_id,template_id,email_domain,email_count,sent_count,soft_bounce_count,hard_bounce_count,reach_count,"
                "none_exist_count,none_exist_count1,dns_fail_count,spam_garbage_count) values (:corp_id,:task_id,:template_id,:email_domain,:email_count,:sent_count,"
                ":soft_bounce_count,:hard_bounce_count,:reach_count,:none_exist_count,:none_exist_count1,:dns_fail_count,:spam_garbage_count)";
        }
        else
        {
            sql = "insert into " + g_tablename.tb_result_by_domain + " (corp_id,task_id,template_id,email_domain,email_count,sent_count,soft_bounce_count,hard_bounce_count,reach_count) values (:corp_id,:task_id,:template_id,:email_domain,:email_count,:sent_count,:soft_bounce_count,:hard_bounce_count,:reach_count)";
        }

        m_dbStmt = m_dbConn->getStatement(sql);
        m_dbStmt->getParam("task_id").assignInteger(td->task_id);
        m_dbStmt->getParam("template_id").assignInteger(td->template_id);
        m_dbStmt->getParam("corp_id").assignInteger(td->corp_id);
        m_dbStmt->getParam("email_domain").assignString(td->domain,strlen(td->domain));
        m_dbStmt->getParam("email_count").assignInteger(td->total);
        m_dbStmt->getParam("sent_count").assignInteger(td->send);
        m_dbStmt->getParam("soft_bounce_count").assignInteger(td->soft_bounce_count);
        m_dbStmt->getParam("hard_bounce_count").assignInteger(td->hard_bounce_count);
        m_dbStmt->getParam("reach_count").assignInteger(td->suc_send);
        if(td->none_exist_count != 0 || td->none_exist_count1 != 0 || td->dns_fail_count != 0 || td->spam_garbage_count != 0)
        {
            m_dbStmt->getParam("none_exist_count").assignInteger(td->none_exist_count);
            m_dbStmt->getParam("none_exist_count1").assignInteger(td->none_exist_count1);
            m_dbStmt->getParam("dns_fail_count").assignInteger(td->dns_fail_count);
            m_dbStmt->getParam("spam_garbage_count").assignInteger(td->spam_garbage_count);
        }
        m_dbStmt->execSQL();
    }
    else
    {
        if(td->none_exist_count != 0 || td->none_exist_count1 != 0 || td->dns_fail_count != 0 || td->spam_garbage_count != 0)
        {
            sql = "update " + g_tablename.tb_result_by_domain + " set email_count=:email_count,sent_count=:sent_count,soft_bounce_count=:soft_bounce_count,"
                "hard_bounce_count=:hard_bounce_count,reach_count=:reach_count,none_exist_count=:none_exist_count,none_exist_count1=:none_exist_count1,"
                "dns_fail_count=:dns_fail_count,spam_garbage_count=:spam_garbage_count where corp_id=:corp_id and task_id=:task_id and template_id=:template_id and email_domain=:email_domain";
        }
        else
        {
            sql = "update " + g_tablename.tb_result_by_domain + " set email_count=:email_count,sent_count=:sent_count,soft_bounce_count=:soft_bounce_count,hard_bounce_count=:hard_bounce_count,reach_count=:reach_count where corp_id=:corp_id and task_id=:task_id and template_id=:template_id and email_domain=:email_domain";
        }

        m_dbStmt = m_dbConn->getStatement(sql);
        m_dbStmt->getParam("task_id").assignInteger(td->task_id);
        m_dbStmt->getParam("template_id").assignInteger(td->template_id);
        m_dbStmt->getParam("corp_id").assignInteger(td->corp_id);
        m_dbStmt->getParam("email_domain").assignString(td->domain,strlen(td->domain));
        m_dbStmt->getParam("email_count").assignInteger(td->total);
        m_dbStmt->getParam("sent_count").assignInteger(td->send);
        m_dbStmt->getParam("soft_bounce_count").assignInteger(td->soft_bounce_count);
        m_dbStmt->getParam("hard_bounce_count").assignInteger(td->hard_bounce_count);
        m_dbStmt->getParam("reach_count").assignInteger(td->suc_send);
        if(td->none_exist_count != 0 || td->none_exist_count1 != 0 || td->dns_fail_count != 0 || td->spam_garbage_count != 0)
        {
            m_dbStmt->getParam("none_exist_count").assignInteger(td->none_exist_count);
            m_dbStmt->getParam("none_exist_count1").assignInteger(td->none_exist_count1);
            m_dbStmt->getParam("dns_fail_count").assignInteger(td->dns_fail_count);
            m_dbStmt->getParam("spam_garbage_count").assignInteger(td->spam_garbage_count);
        }

        m_dbStmt->execSQL();
    }
    fileLog.writeLog((FormatString("DBUpdateTaskDomain record ok in % [%.%.%.%] % % % % %\n").arg(g_tablename.tb_result_by_domain).arg(td->task_id).arg(td->template_id).arg(td->corp_id).arg(td->domain).arg(td->total).arg(td->send).arg(td->suc_send).arg(td->soft_bounce_count).arg(td->hard_bounce_count).str()));

    return 0;
}

int DBUpdateTask(_tinfo_node * tn)	
{
    string sql ="select * from " + g_tablename.tb_task + " where Task_id=:task_id and template_id=:template_id and Corp_id=:corp_id";

    m_dbStmt = m_dbConn->getStatement(sql);
    m_dbStmt->getParam("task_id").assignInteger(tn->task_id);
    m_dbStmt->getParam("template_id").assignInteger(tn->template_id);
    m_dbStmt->getParam("corp_id").assignInteger(tn->corp_id);
    m_dbStmt->execSQL();

    if((tn->task_id > 20120700 && tn->task_id < 20200000) || (tn->task_id > 1020120800 && tn->task_id < 1020200000) || (tn->task_id > 100000000 && tn->task_id < 1000000000))
    {
        //		tn->template_id = 0;
        //		tn->corp_id = 0;
        fileLog.writeLog((FormatString("auto task [%.%.%] \n").arg(tn->task_id).arg(tn->template_id).arg(tn->corp_id).str()));
    }
    else
    {
        if(m_dbStmt->getRecordCount() != 1)
        {
            fileLog.writeLog((FormatString("DBUpdateTask no record in % [%.%.%] \n").arg(g_tablename.tb_task).arg(tn->task_id).arg(tn->template_id).arg(tn->corp_id).str()));
            return -1;
        }
    }

    string sel_strsql = "select * from " + g_tablename.tb_result + " where Task_id = :task_id and template_id = :template_id and Corp_id=:corp_id ";
    m_dbStmt = m_dbConn->getStatement(sel_strsql);
    m_dbStmt->getParam("task_id").assignInteger(tn->task_id);
    m_dbStmt->getParam("template_id").assignInteger(tn->template_id);
    m_dbStmt->getParam("corp_id").assignInteger(tn->corp_id);

    m_dbStmt->execSQL();
    time_t timep; 
    time(&timep);
    struct tm * lt = localtime(&timep);
    DateTime dt;
    dt.setDateTime(1900 + lt->tm_year, 1+lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);

    if(m_dbStmt->getRecordCount() == 0)   //Ã»ÓÐ¼ÇÂ¼¾Í²åÈë
    {
        if(tn->hard_bounce_count !=0 || tn->soft_bounce_count != 0 || tn->none_exist_count != 0 || tn->none_exist_count1 != 0 || tn->dns_fail_count != 0 || tn->spam_garbage_count != 0)
        {
            string insertsql = "insert into " + g_tablename.tb_result + " (Task_id,template_id,Corp_id,Begin_send_time,email_count,Sent_count,reach_count,soft_bounce_count,hard_bounce_count,none_exist_count,none_exist_count1,dns_fail_count,spam_garbage_count) values (:task_id,:template_id,:corp_id,:begin_send_time,:email_count,:sent_count,:reach_count,:soft_bounce_count,:hard_bounce_count,:none_exist_count,:none_exist_count1,:dns_fail_count,:spam_garbage_count)";	

            m_dbStmt = m_dbConn->getStatement(insertsql);
            m_dbStmt->getParam("task_id").assignInteger(tn->task_id);
            m_dbStmt->getParam("template_id").assignInteger(tn->template_id);
            m_dbStmt->getParam("corp_id").assignInteger(tn->corp_id);

            m_dbStmt->getParam("begin_send_time").assignDateTime(dt);
            m_dbStmt->getParam("email_count").assignInteger(tn->total);
            m_dbStmt->getParam("sent_count").assignInteger(tn->send);
            m_dbStmt->getParam("reach_count").assignInteger(tn->suc_send);
            m_dbStmt->getParam("soft_bounce_count").assignInteger(tn->soft_bounce_count);
            m_dbStmt->getParam("hard_bounce_count").assignInteger(tn->hard_bounce_count);
            m_dbStmt->getParam("none_exist_count").assignInteger(tn->none_exist_count);
            m_dbStmt->getParam("none_exist_count1").assignInteger(tn->none_exist_count1);
            m_dbStmt->getParam("dns_fail_count").assignInteger(tn->dns_fail_count);
            m_dbStmt->getParam("spam_garbage_count").assignInteger(tn->spam_garbage_count);

            m_dbStmt->execSQL();
            fileLog.writeLog(( FormatString("insert ok task_id[%] template_id[%] corp_id[%] sent_count[%] reach_count[%] email_count[%]\n").arg(tn->task_id).arg(tn->template_id).arg(tn->corp_id).arg(tn->send).arg(tn->suc_send).arg(tn->total).str()));
        }
        else
        {
            string insertsql = "insert into " + g_tablename.tb_result + " (Task_id,template_id,Corp_id,Begin_send_time,email_count,Sent_count,reach_count,soft_bounce_count,hard_bounce_count) values (:task_id,:template_id,:corp_id,:begin_send_time,:email_count,:sent_count,:reach_count,:soft_bounce_count,:hard_bounce_count)";
            m_dbStmt = m_dbConn->getStatement(insertsql);
            m_dbStmt->getParam("task_id").assignInteger(tn->task_id);
            m_dbStmt->getParam("template_id").assignInteger(tn->template_id);
            m_dbStmt->getParam("corp_id").assignInteger(tn->corp_id);

            m_dbStmt->getParam("begin_send_time").assignDateTime(dt);
            m_dbStmt->getParam("email_count").assignInteger(tn->total);
            m_dbStmt->getParam("sent_count").assignInteger(tn->send);
            m_dbStmt->getParam("reach_count").assignInteger(tn->suc_send);
            m_dbStmt->getParam("soft_bounce_count").assignInteger(tn->soft_bounce_count);
            m_dbStmt->getParam("hard_bounce_count").assignInteger(tn->hard_bounce_count);

            m_dbStmt->execSQL();
            fileLog.writeLog(( FormatString("insert ok task_id[%] template_id[%] corp_id[%] sent_count[%] reach_count[%] email_count[%]\n").arg(tn->task_id).arg(tn->template_id).arg(tn->corp_id).arg(tn->send).arg(tn->suc_send).arg(tn->total).str()));
        }

    }	
    else								//ÓÐ¾Í¸üÐÂ
    {
        if(tn->hard_bounce_count != 0 || tn->soft_bounce_count != 0 || tn->none_exist_count != 0 || tn->none_exist_count1 != 0 || tn->dns_fail_count != 0 || tn->spam_garbage_count != 0)
        {
            string updatesql = "";
            if(tn->send_flag == 0)
            {
                updatesql   = "update " + g_tablename.tb_result + " set End_send_time=:end_send_time, email_count=:email_count, sent_count=:sent_count, reach_count=:reach_count, "
                    "soft_bounce_count=:soft_bounce_count,hard_bounce_count=:hard_bounce_count,none_exist_count=:none_exist_count,none_exist_count1=:none_exist_count1, "
                    "dns_fail_count=:dns_fail_count,spam_garbage_count=:spam_garbage_count where Task_id = :task_id and template_id = :template_id and Corp_id=:corp_id";
            }
            else
            {
                updatesql="update " + g_tablename.tb_result + " set email_count=:email_count, sent_count=:sent_count, reach_count=:reach_count, soft_bounce_count=:soft_bounce_count,"
                    "hard_bounce_count=:hard_bounce_count,none_exist_count=:none_exist_count,none_exist_count1=:none_exist_count1,dns_fail_count=:dns_fail_count,"
                    "spam_garbage_count=:spam_garbage_count where Task_id=:task_id and template_id=:template_id and Corp_id=:corp_id";
            }

            m_dbStmt = m_dbConn->getStatement(updatesql);
            if(tn->send_flag == 0) m_dbStmt->getParam("end_send_time").assignDateTime(dt);
            m_dbStmt->getParam("email_count").assignInteger(tn->total);
            m_dbStmt->getParam("sent_count").assignInteger(tn->send);
            m_dbStmt->getParam("reach_count").assignInteger(tn->suc_send);
            m_dbStmt->getParam("task_id").assignInteger(tn->task_id);
            m_dbStmt->getParam("template_id").assignInteger(tn->template_id);
            m_dbStmt->getParam("corp_id").assignInteger(tn->corp_id);
            m_dbStmt->getParam("soft_bounce_count").assignInteger(tn->soft_bounce_count);
            m_dbStmt->getParam("hard_bounce_count").assignInteger(tn->hard_bounce_count);
            m_dbStmt->getParam("none_exist_count").assignInteger(tn->none_exist_count);
            m_dbStmt->getParam("none_exist_count1").assignInteger(tn->none_exist_count1);
            m_dbStmt->getParam("dns_fail_count").assignInteger(tn->dns_fail_count);
            m_dbStmt->getParam("spam_garbage_count").assignInteger(tn->spam_garbage_count);

            m_dbStmt->execSQL();
            fileLog.writeLog(( FormatString("update ok task_id[%] template_id[%] corp_id[%] sent_count[%] reach_count[%] email_count[%]\n").arg(tn->task_id).arg(tn->template_id).arg(tn->corp_id).arg(tn->send).arg(tn->suc_send).arg(tn->total).str()));	
        }
        else
        {
            string updatesql = "";
            if(tn->send_flag == 0)
            {
                updatesql	= "update " + g_tablename.tb_result + " set End_send_time=:end_send_time, email_count=:email_count, sent_count=:sent_count, reach_count=:reach_count, "
                    "soft_bounce_count=:soft_bounce_count,hard_bounce_count=:hard_bounce_count  where Task_id = :task_id and template_id = :template_id and Corp_id=:corp_id";
            }
            else 
            {
                updatesql="update " + g_tablename.tb_result + " set email_count=:email_count, sent_count=:sent_count, reach_count=:reach_count, soft_bounce_count=:soft_bounce_count,"
                    "hard_bounce_count=:hard_bounce_count where Task_id=:task_id and template_id=:template_id and Corp_id=:corp_id";
            }

            m_dbStmt = m_dbConn->getStatement(updatesql);
            if(tn->send_flag == 0) m_dbStmt->getParam("end_send_time").assignDateTime(dt);
            m_dbStmt->getParam("email_count").assignInteger(tn->total);
            m_dbStmt->getParam("sent_count").assignInteger(tn->send);
            m_dbStmt->getParam("reach_count").assignInteger(tn->suc_send);
            m_dbStmt->getParam("task_id").assignInteger(tn->task_id);
            m_dbStmt->getParam("template_id").assignInteger(tn->template_id);
            m_dbStmt->getParam("corp_id").assignInteger(tn->corp_id);
            m_dbStmt->getParam("soft_bounce_count").assignInteger(tn->soft_bounce_count);
            m_dbStmt->getParam("hard_bounce_count").assignInteger(tn->hard_bounce_count);

            m_dbStmt->execSQL();
            fileLog.writeLog(( FormatString("update ok task_id[%] template_id[%] corp_id[%] sent_count[%] reach_count[%] email_count[%]\n").arg(tn->task_id).arg(tn->template_id).arg(tn->corp_id).arg(tn->send).arg(tn->suc_send).arg(tn->total).str()));
        }
    }


    string updateinfo;
    if(tn->send_flag == 0)
    {
        updateinfo="update " + g_tablename.tb_task + " set Task_status=27 where Task_id=:task_id and template_id=:template_id and Corp_id=:corp_id";
    }
    else
    {
        updateinfo="update " + g_tablename.tb_task + " set Task_status=24 where Task_id=:task_id and template_id=:template_id and Corp_id=:corp_id and task_status is NULL";
    }

    m_dbStmt = m_dbConn->getStatement(updateinfo);
    m_dbStmt->getParam("task_id").assignInteger(tn->task_id);
    m_dbStmt->getParam("template_id").assignInteger(tn->template_id);
    m_dbStmt->getParam("corp_id").assignInteger(tn->corp_id);
    m_dbStmt->execSQL();

    if(m_dbStmt->getRecordCount())
    {
        if(tn->send_flag == 0)
            fileLog.writeLog(( FormatString("update % task[%] template[%] corp[%] Task_status 27\n").arg(g_tablename.tb_task).arg(tn->task_id).arg(tn->template_id).arg(tn->corp_id).str()));

        else
            fileLog.writeLog(( FormatString("update % task[%] template[%] corp[%] Task_status 24\n").arg(g_tablename.tb_task).arg(tn->task_id).arg(tn->template_id).arg(tn->corp_id).str()));
    }
    return 0;
}


RFC_NAMESPACE_END

MAIN_APP(CacheDbApp)
