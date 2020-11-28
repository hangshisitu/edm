/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan             Date: 2011-08      */

#include "deliverengine.h"
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
#include <edcode.h>
#include <iostream>
#include <fstream>

RFC_NAMESPACE_BEGIN
FileLog     fileLog;
RequestMail req_mail;
MxMgr mxmgr;
map<unsigned int, struct tmp_node> m_map_mail_tmp;   //临时结果map，unsigned int和全量邮箱map对应，线程只需要写该队列，由主线程将发送情况同步到全量队列，同步后清空该队列
map<unsigned int, mail_node *>  m_map_mail;     //全量邮箱map，key为报文流水号，mail_node保存邮箱发送情况，作为能力计算的依据，申请邮箱应答后写入，在投递的session会话中更新，每10分钟报告之后清空

pthread_mutex_t * mutex;
int  m_dispfd;
char m_dispatchport[8];   //调度机port
char m_dispatchip[64];   //调度机IP
char m_ip[64];   //本地IP
char from[128];   //发件人域名
char is_auth[8];   //是否需要认证（至投递平台）
char auth_info[64];   //认证信息
string  m_sms_path; //sms file path
////deliverhelper
typedef struct{
    string bind;
    int    port;
} helper_t;
helper_t g_helpersvr;
string g_usraddr;
////

const char* sWeekName1[] =
{
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

const char* sMonthName1[] =
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void show_handler(int sig)
{
    if(sig == SIGTERM)
    {
        fileLog.writeLog("this is a kill signal\n");
    }
    else if (sig == SIGPIPE)
    {
        fileLog.writeLog("this is a sigpipe signal\n");
        CreateClient();
    }
    return;
}

/// 2013/11/11 add by weide for deliverhelper function
int check_inet_addr(const char* cp)
{
    if (!cp)
        return -1;

    int i=0, dot=0;
    char buf[4] = {0,0,0,0};
    while (*cp != '\0')
    {
        if (isdigit(*cp)){
            if (i > 3)
                return -1;
            buf[i++] = *cp;
        }
        else if (*cp == '.'){
            if (++dot > 3)
                return -1;
            if ( i<1|| atoi(buf)>255)
                return -1;
            i = buf[1] = buf[2] = 0;
        }
        else{
            return -1;
        }

        cp++;
    }

    return  (dot!=3 || i<1 || atoi(buf)>255) ? -1:0;
}

enum {SOCK_R = 0x01, SOCK_W = 0x02, SOCK_E = 0x04};

int sockselect(int fd, int flag, int seconds)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd,&fds);
    fd_set *p[3] = {0,0,0};

    if (flag & SOCK_R)
        p[0] = &fds;
    if (flag & SOCK_W)
        p[1] = &fds;
    if (flag & SOCK_E)
        p[2] = &fds;

    timeval tv = {seconds, 0};
    return ::select(fd + 1, p[0], p[1], p[2], (seconds>-1? &tv:NULL));
}

//nb = non-blocking
int nb_connect(string& remote, int port, int timeout, string& local)
{
    int mode = 1;
    sockaddr_in peer;

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&mode, sizeof(mode));
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK);

    switch ( check_inet_addr(local.c_str()) )
    {
    case 0:
        bzero(&peer, sizeof(peer));
        peer.sin_family = PF_INET;
        peer.sin_addr.s_addr = inet_addr(local.c_str());
        bind(sockfd, (sockaddr *) &peer, sizeof(sockaddr));
        break;
    default:
        //local.clear();
        break;
    }
    
    bzero(&peer, sizeof(peer));
    peer.sin_family = PF_INET;
    peer.sin_port = htons(port);
    peer.sin_addr.s_addr = inet_addr(remote.c_str());

    switch(connect(sockfd,(sockaddr*)&peer, sizeof(sockaddr_in)))
    {
    case -1:
        switch(errno){
        case EINPROGRESS:
        case EWOULDBLOCK:{
            if (sockselect(sockfd,SOCK_W,timeout) < 1){
                fileLog.writeLog(( FormatString("nb_connect.fail(1):%\n").arg(strerror(errno)).str()));
                close(sockfd);
                return -1;
            }
            int eval = 0;
            socklen_t len = sizeof(eval);
            int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &eval, &len);
            if (retval != 0 || eval != 0){
                close(sockfd);
                eval ? errno=eval:NULL;
                fileLog.writeLog(( FormatString("nb_connect.fail(2):%\n").arg(strerror(errno)).str()));
                return -1;
            }
            fileLog.writeLog("nb_connect.OK(1)\n");
        } break;
        default:
            fileLog.writeLog(( FormatString("nb_connect.fail(3):%\n").arg(strerror(errno)).str()));
            close(sockfd);
            return -1;
        }
        break;
    default:
        fileLog.writeLog("nb_connect.OK(0)\n");
        break;
    }
    
    return sockfd;
}

int nb_send(int sockfd, string& sbuf, int timeout)
{
    int transferred = 0;
    while (sbuf.length() > 0)
    {
        int bytes = ::send(sockfd, sbuf.data(), sbuf.length(), 0);
        switch (bytes){
        case -1:
            if (errno==EINTR)
                break;
            if (errno==EAGAIN || errno==EWOULDBLOCK){
                if(timeout>0 && sockselect(sockfd,SOCK_W,timeout)>0)
                    break;
                return transferred;
            }
        case 0:
            //fileLog.writeLog(( FormatString("nb_send.fail:%\n").arg(strerror(errno)).str()));
            return -1;
        default:
            transferred += bytes;
            sbuf.erase(0, bytes);
            break;
        }
    }

    return transferred;
}

int nb_recv(int sockfd, string& sbuf, int timeout)
{
    int transferred = -1;
    do{
        char buf[256] = {0};
        int bytes = ::recv(sockfd, buf, sizeof(buf)-1, 0);
        switch (bytes)
        {
        case -1:
            if (errno==EINTR)
                break;
            else if (errno==EAGAIN || errno==EWOULDBLOCK){
                if (timeout > 0 && sockselect(sockfd, SOCK_R, timeout) > 0)
                    break;
            }
        case 0:
            //fileLog.writeLog(( FormatString("nb_recv.fail:%\n").arg(strerror(errno)).str()));
            return transferred;
        default:
            transferred += bytes;
            sbuf.append(buf);
            break;
        }
    }while(1);
}

void take_network_addr(int sockfd, string& naddr)
{
    sockaddr_in peer;
    bzero(&peer, sizeof(peer));
    socklen_t len = sizeof(sockaddr_in);
    if (0 == getsockname(sockfd, (sockaddr*)&peer, &len))
        naddr = inet_ntoa(peer.sin_addr);
    else
        naddr = "127.0.0.1";
}

#define CRLF "\r\n"

int take_one_line(int sockfd, string& sbuf, int timeout, string& line)
{
    do{
        int offset = sbuf.find(CRLF);
        if (offset != string::npos){
            line = sbuf.substr(0, offset);
            sbuf.erase(0, offset + strlen(CRLF));
            return 0;
        }
        
        if(sbuf.length() > 10240){
            fileLog.writeLog("take_one_line: line too large\n");
            return -1;
        }

	    if (sockselect(sockfd, SOCK_R, timeout) < 1 || nb_recv(sockfd, sbuf, 0) < 1){
		    return -1;
		}
    }while(1);
}

enum {HTTP_HEADER=0x01, HTTP_CONTENT=0x02};

int http_query(string& request, list<string>& response, int flag)
{
    if (request.length() < 1){
        fileLog.writeLog("your mother, request is empty\n");
        return -1;
    }

    int timeout = 30;
    int sockfd = nb_connect(g_helpersvr.bind, g_helpersvr.port, timeout, g_usraddr);
    if (sockfd < 0){
        fileLog.writeLog("http_query.nb_connect.fail.0\n");
        return -1;
    }

    int bytes = request.length();
    if (bytes != nb_send(sockfd, request, timeout)){
        close(sockfd);
        fileLog.writeLog("http_query.nb_send.fail.1\n");
        return -1;
    }

    string buffer;
    string line;
    int length = -1;
    int total = 0;
    do{
        //http-header
        if (0 != take_one_line(sockfd, buffer, timeout*2, line)){
            fileLog.writeLog(( FormatString(
                "http_query.take_one_line.fail.1: %\n").arg(strerror(errno)).str() ));
            close(sockfd);
        	return -1;
        }
        
        if (flag & HTTP_HEADER){
            response.push_back(line);
            total += (line.length() + 2);
        }

        if (strncasecmp(line.data(), "Content-Length:", 15) == 0){
            sscanf(line.data() + 15, "%d", &length);
        }
    }while(line.length() > 0);

    bytes = 0;
    while(length > bytes)
    {
        if (0 != take_one_line(sockfd, buffer, timeout, line)){
            fileLog.writeLog(( FormatString(
                "http_query.take_one_line.fail.2: %\n").arg(strerror(errno)).str() ));
        	break;
        }
        
        if (flag & HTTP_CONTENT){
            response.push_back(line);
            total += (line.length() + 2);
        }
        bytes += (line.length() + 2);
    }

    close(sockfd);
    return total;
}

int take_ability_info(map<stdstring, domain_node>& dmNode)
{
    string sbuf;
    sbuf += ("GET /do?cmd=1001&ip=" + g_usraddr + " HTTP/1.1\r\n");
    sbuf += "User-Agent: deliverengine\r\n";
    fileLog.writeLog(("take.ability.request:" + sbuf).c_str());
    
    list<string> response;
    if (http_query(sbuf, response, HTTP_CONTENT) < 0){
        fileLog.writeLog("take.ability.fail.1\n");
        return -1;
    }
    
    map<stdstring, domain_node> newNode;
    list<string>::iterator line;
    for(line=response.begin(); line!=response.end(); ++line)
    {
        domain_node dmBlk;
        memset(&dmBlk, 0x00, sizeof(dmBlk));

        int offset = line->find(",");
        string domain = line->substr(0, offset);
        line->erase(0,offset + 1);
        
        offset = line->find(",");
        string ability = line->substr(0, offset);
        line->erase(0,offset + 1);
        
        map<string, domain_node>::iterator item;
        item = dmNode.find(domain);
        if (item != dmNode.end()){
            dmBlk = item->second;
        }

        dmBlk.ability = atoi(ability.c_str());
        dmBlk.oriability = dmBlk.ability;
        dmBlk.flag_cur = line->length()>0 ? line->at(0):'q';

        switch(dmBlk.flag_cur){
        case MUNITE:
        case QUATOR:
        case HOUR:
        case DAY:
            newNode[domain] = dmBlk;
            fileLog.writeLog(( FormatString("ability -> % = %|% (total:%, fail:%)\n").
                arg(domain).arg(dmBlk.ability).arg(dmBlk.flag_cur).arg(dmBlk.total_cur).arg(dmBlk.fail_cur).str()));            
        	break;
        default: 
            break;
        }
    }
    dmNode.clear();
    dmNode.swap(newNode);
    return 0;
}

#include <stdio.h>
#include <map>
#include <string>
#include <vector>

using namespace std;

int dw_split(const char* line, char ch, vector<string> &result)
{
    char* start = (char*)line;
    while(1) {
        char* p = strchr(start, ch);
        if (!p) {
            result.push_back(start);
            return 0;
        }
        result.push_back(string(start, p-start));
        start = p + 1;
    }
    return 0;
}

int dw_replace(string& text, const char* optr, const char* nptr)
{
    int num = 0;
    if (text.length() < 1 || !optr || !nptr)
        return num;

    string::size_type offset = 0;
    string::size_type idx = string::npos;
    int olen = strlen(optr);
    int nlen = strlen(nptr);
    while(offset < text.length()){
        idx = text.find(optr, offset);
        if (idx == string::npos)
            break;
        text.erase(idx, olen);
        text.insert(idx, nptr);
        offset = idx + nlen;
        num++;
    }

    return num;
}

typedef vector<string> column_t;
typedef map<string, column_t> tables_t; 
#define BOUNDARY_B "$"
#define BOUNDARY_E "$"
#define TR_CLASS "__many__"

//交易日期,交易记账日期,交易金额符号,交易金额,交易描述,卡号末四位,交易地点
//2015-01-01|2015-02-02,2015-01-03|2015-02-04,RMB|RMB,1024|2048,TMAIL|JINGDONG,1111|2222, GZ|SZ

int table_first_step(string& vars, string& values, tables_t & tables)
{
    vector<string> data;
    if (vars.length()<1 || values.find("|")==string::npos)
        return -1;

    dw_split(values.c_str(), '|', data);
    string v = BOUNDARY_B + vars + BOUNDARY_E;
    //tables[v].insert(tables[v].end(), data.begin(), data.end());
    tables[v] = data;
    return 0;
}

// <tr class="__many__"><td>$birthday$</td><td>$age$</td><td>$email$</td></tr> 
void table_second_step(string& html, tables_t & tables)
{
    if (html.length()<1 || tables.size()<1)
        return;

    int offset = 0;
    while (offset < html.length()){
        const char* haystack = html.c_str();
        char* begin = (char*)strcasestr(haystack + offset, "<tr ");
        if (!begin)
            break;

        char *b = begin + 4;
        while ( isblank(*b) )
            ++b;

        if (0 != strncasecmp(b, "class=", 6)){
            offset = b - haystack;
            continue;
        }

        b += 6;
        if (*b!='\'' && *b!='\"'){
            offset = b - haystack;
            continue;
        }
 
        string name;
        ++b;
        while ( *b!='\0' && *b!='\'' && *b!='\"'){
            name.append(1, *b);
            ++b;
        }

        if (name.length()<1 || name!=TR_CLASS){
            offset = b - haystack;
            continue;
        }

        if (!(b=strchr(b, '>')))
            break;

        ++b;
        while ( isblank(*b) )
            ++b;

        char* endtr = strcasestr(b, "</tr>");
        if (!endtr)
            break;
        
        endtr += 5;
        string example(begin, endtr - begin);
        string trs;
        for (int pos=0; ; pos++) {
            int count = 0;
            string tr = "\r\n" + example;
            tables_t::iterator itr = tables.begin();
            for ( ; itr!=tables.end(); ++itr){
                if (itr->second.size() > pos)
                    count += dw_replace(tr, itr->first.c_str(), itr->second[pos].c_str());
            }

            if (count < 1)
                break;
            trs += tr;
        }

        if (trs.length() > 0){
            html.erase(begin-haystack, example.length());
            html.insert(begin-haystack, trs.c_str());
            endtr = begin + trs.length();
        }

        offset = endtr - haystack;
    }
}

int take_transport_info()
{
    return 0;
}

int take_deliverengine_setting()
{
    return 0;
}

int deliverhelper_report(string& ip, string& from, string& rcpt, char status)
{
    string sbuf;
    sbuf += ("GET /do?cmd=2001&ip=" + g_usraddr);
    sbuf += ("&from=" + from);
    sbuf += ("&rcpt=" + rcpt);
    sbuf += "&status=";
    sbuf.append(1, status);
    sbuf += " HTTP/1.1\r\nUser-Agent: deliverengine\r\n";
    fileLog.writeLog(("deliverhelper.request:" + sbuf).c_str());
    
    list<string> response;
    if (http_query(sbuf, response, HTTP_HEADER) < 0){
        fileLog.writeLog("deliverhelper.report.fail\n");
        return -1;
    }

    if (response.size() > 0){
        fileLog.writeLog(( FormatString("deliverhelper.response: %\n").arg(*response.begin()).str() ));
    }
    return 0;
}

void * delivery(void * param)
{
    pthread_t pid;
    _thread_param * t_param = (_thread_param *)param;
    fileLog.writeLog(( FormatString("thread 1 [%] [%] [%] [%] [%]\n").arg(t_param->seqno).arg(t_param->task_id).arg(t_param->ip).arg(t_param->port).arg(t_param->mail).str() ));
    EmlSender emlsender;
    int port = atoi(t_param->port.c_str());
    emlsender.Set(t_param->mail_text.c_str(),  t_param->mail.c_str(), t_param->ip.c_str(), port,t_param->random, t_param->sender, t_param->same_flag, t_param->task_id, t_param->helo);
    unsigned char s_flag;
    char errstr[256];
    memset(errstr,0,sizeof(errstr));
    int retval = emlsender.Send2Domain(s_flag,t_param->vip,t_param->mail_text.c_str(),t_param->mail_text.size(),t_param->from.c_str(),t_param->from_seq,t_param->vec_helo,t_param->mail_from,t_param->from_flag,errstr,t_param->is_auth,t_param->auth_info);

    ///// deliverhelper 报告成功失败
    if (g_helpersvr.bind.length() > 0){
        string ip = m_ip;
        string from = t_param->vec_helo.rbegin()->c_str();
        string rcpt = t_param->mail.substr(t_param->mail.find("@") + 1);
        char status = retval==0 ? 's':'f';
        deliverhelper_report(ip, from, rcpt, status);
    }
    ///// 2.9.1
    if (retval == 0 && t_param->sms == 1 && m_sms_path.length()>0){
        string cellphone = t_param->mail.substr(0,t_param->mail.find("@")); 
        string domain = t_param->mail.substr(t_param->mail.find("@")); 
        if (strcasecmp("@139.com", domain.c_str()) == 0
           && cellphone.length() == 11
           && cellphone.at(0) == '1'
           && cellphone.find_first_not_of("1234567890") == string::npos){
           char text[256];
           timeval tv;
           timerclear(&tv);
           gettimeofday(&tv,NULL);
           snprintf(text, sizeof(text),"%s_%ld%ld_%lu",cellphone.c_str(),tv.tv_sec,tv.tv_usec,pthread_self());
           //cellphone ,t_param->subject
           string oldpath = m_sms_path+text; 
           FILE* f = fopen(oldpath.c_str(),"w");
           if (f){
               fprintf(f, "%s|%s",cellphone.c_str(),t_param->subject.c_str());
               fclose(f);
               rename(oldpath.c_str(), (oldpath+".o").c_str());
               fileLog.writeLog(( FormatString("SMS file=[%], text=[%|%]\n").arg(oldpath+".o").arg(cellphone).arg(t_param->subject).str()));
           }
			else{
				char buf[1024]={0};
				strerror_r(errno, buf, sizeof(buf));
				fileLog.writeLog(( FormatString("SMS open fail:%-%\n").arg(errno).arg(buf).str()));
			}

        }
    }

    char c_flag[2]={0};
    sprintf(c_flag,"%d",s_flag);
    fileLog.writeLog(( FormatString("thread 2 [%] [%] [%] [%] [%]\n").arg(t_param->task_id).arg(c_flag).arg(t_param->seqno).arg(t_param->mail).arg(pid).str()));
    //加锁
    if(pthread_mutex_lock(mutex) != 0)
    {
        fileLog.writeLog( "thread pthread_mutex_lock fail\n");
        t_param->vec_helo.clear();
        delete t_param;
        pthread_exit(NULL);
    }
    
    //剔除连接超时的MX记录
    if (strncasecmp(errstr,"step 1-->connectfail:", 21) == 0) {
        string tmpdomain = t_param->mail.substr(t_param->mail.find("@")+1);
        fileLog.writeLog(( FormatString("erase mx record from cache, [%][%:%]\n").arg(tmpdomain).arg(t_param->ip).arg(port).str()));
        //mxmgr.eraseMXRecord(tmpdomain, t_param->ip,port);
    }

    fileLog.writeLog(( FormatString("thread 3 [%] [%] [%]\n").arg(c_flag).arg(t_param->mail).arg(pid).str()));

    m_map_mail_tmp[t_param->seqno].flag = s_flag;
    m_map_mail_tmp[t_param->seqno].stime= time(NULL);
    m_map_mail_tmp[t_param->seqno].errstr= errstr;
    fileLog.writeLog("thread 4 \n");

    if(pthread_mutex_unlock(mutex) != 0)
    {
        fileLog.writeLog( "thread pthread_mutex_unlock fail\n");
        t_param->vec_helo.clear();
        delete t_param;
        pthread_exit(NULL);
    }
    fileLog.writeLog("thread 5 \n");
    t_param->vec_helo.clear();
    delete t_param;
    pthread_exit(NULL);
}

DeliverEngineApp::DeliverEngineApp(void) : Application("deliverengine")
{
    memset(m_dispatchip,0,sizeof(m_dispatchip));
    memset(m_dispatchport,0,sizeof(m_dispatchport));
    memset(m_ip,0,sizeof(m_ip));
    memset(from,0,sizeof(from));
    m_req_tmgr = NULL;
    m_rsp_tmgr = NULL;
    m_req_tmgr = new char [REQ_MSG_MAX_LEN];
    m_map_mail.clear(); 
    m_map_domain.clear();   
}

bool DeliverEngineApp::daemon(void)
{
//  int a = semget(12382,1,IPC_CREAT);
//  int b = semctl(a,0,IPC_RMID);

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

bool DeliverEngineApp::onInit(int argc, char * argv[])
{
    ConfigureMgr conf;
    if(argc < 2 || !conf.loadConfFile(argv[1]))
    {
        printf("配置文件初始化失败\n");
        return false;
    }
    stdstring tmp;
    tmp = conf.getString("/iedm/application/dispatchsvrIP"); //maillist IP
    snprintf(m_dispatchip,strlen(tmp.c_str())+1,tmp.c_str());
    
    tmp = conf.getString("/iedm/application/DispatchSvrPort"); //maillist port
    snprintf(m_dispatchport,strlen(tmp.c_str())+1,tmp.c_str());

    tmp = conf.getString("/iedm/application/VIP");   //bind local address
    snprintf(m_ip,strlen(tmp.c_str())+1,tmp.c_str());

    tmp = conf.getString("/iedm/application/From");
    snprintf(from,strlen(tmp.c_str())+1,tmp.c_str());

    tmp = conf.getString("/iedm/application/IsAuth");  //using (ESMTP) AUTH LOGIN
    snprintf(is_auth,strlen(tmp.c_str())+1,tmp.c_str());

    tmp = conf.getString("/iedm/application/AuthInfo");
    snprintf(auth_info,strlen(tmp.c_str())+1,tmp.c_str());

    tmp = conf.getString("/iedm/application/CircleIterval"); // 向mailist请求邮箱的间隔
    m_circleiterval = atoi((char *)tmp.c_str());

    tmp = conf.getString("/iedm/application/TemplateIterval");  //定时清空模板间隔
    m_tempiterval = atoi((char *)tmp.c_str());

    tmp = conf.getString("/iedm/application/ReportIterval");   //向maillist 发送报告间隔
    m_reportiterval= atoi((char *)tmp.c_str());

    m_sms_path = conf.getString("/iedm/application/SmsFilePath");   //

    tmp = conf.getString("/iedm/application/DeliverEngineLogPath");
    fileLog.init(tmp, 10*1024*1024);
    fileLog.writeLog("log start ok\n");

    //////deliverhelper///////////
    g_helpersvr.port = 0;
    g_helpersvr.bind = conf.getString("/iedm/deliverhelper/port");
    g_helpersvr.port = atoi(g_helpersvr.bind.c_str());
    g_helpersvr.bind = conf.getString("/iedm/deliverhelper/bind");
    fileLog.writeLog(( FormatString("deliverhelper server -> %:%\n").arg(g_helpersvr.bind).arg(g_helpersvr.port).str()));   
    /////////////////

/*
    FILE * fd = fopen("../task","r");
        char filec[300*1024];
        memset(filec,0,300*1024);
        fread(filec,1,300*1024,fd);
    string a = filec;
    _template_node node;
    Protemplate(node,a);    
*/

    struct sigaction act, oldact;
    act.sa_handler = show_handler;
    act.sa_flags = 0;
	memset(&act, 0 ,sizeof(act));
	memset(&oldact, 0, sizeof(oldact));
    sigaction(SIGPIPE, &act, &oldact);
    sigaction(SIGTERM, &act, &oldact);

    daemon();

    /*
    struct rlimit rlim_new;
    struct rlimit rlim;
    rlim_new.rlim_cur = rlim_new.rlim_max = 128;                    //设置栈大小为128
    if (setrlimit(RLIMIT_STACK, &rlim_new)!=0)
    {
        fileLog.writeLog("set 128 stack size fail\n");
    }

    if (getrlimit(RLIMIT_STACK, &rlim)==0)
    {
        fileLog.writeLog(( FormatString("rlim %  %\n").arg(rlim.rlim_cur).arg(rlim.rlim_max).str() ));  
    }
    */

    CreateClient(); 

    //////
    g_usraddr = m_ip;
    if ( 0 != check_inet_addr(g_usraddr.c_str()))
        take_network_addr(m_dispfd, g_usraddr);
    //////

    domain_node dn;
    typeConfPairList listAbility;
    conf.getSubItemPairList("/iedm/ability", listAbility);

    char key[16];       //能力值
    char value[16];     //调整能力值的时间维度

    typeConfPairList::const_iterator it ;
    for ( it=listAbility.begin(); it != listAbility.end(); ++it )
    {
        memset(key,0,16);
        memset(value,0,16);
        sscanf((char *)it->second.c_str(), "%[^|]|%[^|]", key,value);   
        dn.fail=0;
        dn.total=0;

        dn.fail_m=0;
        dn.total_m=0;

        dn.fail_h=0;
        dn.total_h=0;

        dn.fail_q=0;
        dn.total_q=0;

        dn.fail_cur=0;
        dn.total_cur=0;

        dn.ability=atoi(key);
        dn.oriability=atoi(key); 

        dn.flag_cur = value[0];
        m_map_domain[it->first] = dn;
    }

    conf.getSubItemPairList("/iedm/HELO", listAbility);
    m_vec_helo.clear();
    for ( it = listAbility.begin(); it != listAbility.end(); ++it )
    {
        m_vec_helo.push_back(it->second);
    }
	//domain_map
	typeConfPairList listDomainMap;
	conf.getSubItemPairList("/iedm/domain_map", listDomainMap);
	for (it = listDomainMap.begin(); it != listDomainMap.end(); ++it) {
		m_domain_map[it->first] = it->second;
	}
	//
    mxmgr.onInit(conf);
    mutex = new pthread_mutex_t;
    pthread_mutex_init(mutex, NULL);

    m_iterval = time(NULL);

    m_iterday = time(NULL);
    m_iter = time(NULL);
    m_iterhour= time(NULL);
    m_iterquater= time(NULL);
    m_itermunite= time(NULL);


    return true;
}

int DeliverEngineApp::onRun(void)
{
    while(1)
    {
        MailListRequest();
        //if(Response() == -1)
		stdstring rsp_data;
		int iret = req_mail.ParseRsp(rsp_data, m_dispfd, m_map_mail);
        if (iret == -1)
        {
            fileLog.writeLog("Response fail, reconnect\n");
            close(m_dispfd);
            CreateClient();
        }
	if (m_map_mail.size() > 0) {
	//扫描任务发送 
	ScanList();
        UpdateList();
	//返回结果至maillist
        SendReport();
	UpdateTotle();
	}
	        //
	sleep(m_circleiterval);
    }

    return 0;
}

void DeliverEngineApp::UpdateTotle()
{
    map<stdstring, domain_node>::iterator it = m_map_domain.begin();
    if(time(NULL) - m_itermunite>60)                //出分结果
    {
        while(it!=m_map_domain.end())
        {
            fileLog.writeLog(( FormatString("------------------munites info:domain[%] fail[%] total[%] ability[%]------------------\n").arg(it->first).arg(it->second.fail_m).arg(it->second.total_m).arg(it->second.ability).str()));  
            it->second.fail_m = 0;
            it->second.total_m = 0;
            m_itermunite = time(NULL);
            it++;
        }
    }
    if(time(NULL) - m_iterquater> 60*15)            //15分钟结果
    {
        while(it!=m_map_domain.end())
        {
            fileLog.writeLog(( FormatString("------------------15 munites info:domain[%] fail[%] total[%] ability[%]------------------\n").arg(it->first).arg(it->second.fail_q).arg(it->second.total_q).arg(it->second.ability).str()));   
            it->second.fail_q = 0;
            it->second.total_q = 0;
            m_iterquater = time(NULL);
            it++;
        }
    }
    if(time(NULL) - m_iterhour> 60*60)          //1小时结果
    {
        while(it!=m_map_domain.end())
        {
            fileLog.writeLog(( FormatString("------------------1 hour info:domain[%] fail[%] total[%] ability[%]------------------\n").arg(it->first).arg(it->second.fail_h).arg(it->second.total_h).arg(it->second.ability).str()));   
            it->second.fail_h = 0;
            it->second.total_h = 0;
            m_iterhour = time(NULL);
            it++;
        }
    }
    if(time(NULL) - m_iterday> 24*60*60)            //1天结果
    {
        while(it!=m_map_domain.end())
        {
            fileLog.writeLog(( FormatString("------------------1 day info:domain[%] fail[%] total[%] ability[%]------------------\n").arg(it->first).arg(it->second.fail).arg(it->second.total).arg(it->second.ability).str()));
            it->second.fail= 0;
            it->second.fail= 0;
            m_iterday = time(NULL);
            it++;
        }
    }
    if(time(NULL) - m_iter > m_tempiterval)                    //定时清空模板
    {
        m_map_template.clear();
        m_iter = time(NULL);
        fileLog.writeLog(( FormatString("clear templdate mem\n").arg(m_tempiterval).str()));
    }

    return;
}

int DeliverEngineApp::UpdateList()
{
    //fileLog.writeLog("begin to UpdateList\n");
    if(pthread_mutex_lock(mutex) != 0)
    {
         fileLog.writeLog( "UpdateList pthread_mutex_lock fail\n");
        return -1;
    }
    //fileLog.writeLog("begin to UpdateList 1\n");
    if(m_map_mail_tmp.size() == 0)
    {
         fileLog.writeLog( "has no send result need to update\n");
        if(pthread_mutex_unlock(mutex) != 0)
        {
            fileLog.writeLog( "UpdateList pthread_mutex_unlock fail\n");
            return -1;
        }
        return 0;
    }

    map<unsigned int, struct tmp_node>::iterator iter = m_map_mail_tmp.begin();

    while(1)
    {
        if(iter == m_map_mail_tmp.end())
        {
            break;
        }
        if(m_map_mail.find(iter->first) == m_map_mail.end())
        {
            fileLog.writeLog((FormatString("UpdateList 1 [%]\n").arg(iter->first).str()));
            iter++;
            continue;
        }
        if(iter->second.flag == 1)  //成功更新标志位status
        {
            m_map_mail[iter->first]->status = 1;            
            m_map_mail[iter->first]->stime = iter->second.stime;
        }
        else
        {
            m_map_mail[iter->first]->status = iter->second.flag;
            m_map_mail[iter->first]->stime = iter->second.stime;
            m_map_mail[iter->first]->failtimes++;
        }
        m_map_mail[iter->first]->errstr=iter->second.errstr;
        iter++;
    }

    m_map_mail_tmp.clear();
    if(pthread_mutex_unlock(mutex) != 0)
    {
        fileLog.writeLog( "UpdateList pthread_mutex_unlock fail\n");
        return -1;
    }
    //fileLog.writeLog("end to UpdateList\n");
    return 0;
}

int DeliverEngineApp::Protemplate(_template_node & node,string & template_str)
{
    memset(node.mail_from,0,sizeof(node.mail_from));
    char * p = (char *)template_str.c_str();
    char *p1 = NULL;
    char *p2 = NULL;
    char *p3 = NULL;
    char *p4 = NULL;
    char *p5 = NULL;
    node.sms = template_str.find("$SMS$99")!=string::npos?1:0;
    node.modula_type_eml = template_str.find("$MODULA_TYPE_EML$")!=string::npos?1:0;

    if((p1=strstr(p,"$MAIL_FROM$")) == NULL || 
        (p2=strstr(p,"$FLAG$")) == NULL || 
        (p3=strstr(p,"$SAMEFROM$")) == NULL || 
        (p4=strstr(p,"$FROM$")) == NULL || 
        (p5=strstr(p,"$HELO$")) == NULL)
    {
        return -1;  
    }
    else
    {
        p1 += 11;
        if(*p1 == '\r')
        {
            strcpy(node.mail_from,"123");
        }
        else
        {
            p = p1;
            while(1)
            {
                if(*p == '\r')
                {
                    *p = 0;
                    break;
                }
                p++;
            }
            strcpy(node.mail_from,p1);
        }
        p = p2+6;
        node.from_flag = *p-48;

        if(*(p3 + 10) == '1')
        {
            node.same_flag = 1; 
        }
        else
        {
            node.same_flag = 0; 
        }
        
        p = p4;
        while(1)
        {
            if(*p == '\r')
            {
                *p = 0;
                break;
            }
            p++;
        }

        strcpy(node.sender, p4 + 6);

        while(1)
        {
            if(*p == '\r')
            {
                *p = 0;
                break;
            }
            p++;
        }
        strcpy(node.helo, p5 + 6);
    }

    node.templatestr = p+2;
    int flag = node.from_flag;

    fileLog.writeLog(( FormatString("template size mailfrom[%] flag[%] sameflag[%] sender[%] helo[%]\n").arg(node.mail_from).arg(flag).arg(node.same_flag).arg(node.sender).arg(node.helo).str()));

    return 0;
}

void base64_replace(string &text)
{
    const char* bgptr = "<$BASE64>";
    const char* edptr = "</$BASE64>";

    string::size_type offset = 0;
    string::size_type p_start = 0;
    string::size_type p_end = string::npos;

    while(offset < text.length())
    {
        p_start = text.find(bgptr, offset);

        if (p_start == string::npos){
            return;
        }

        p_end = text.find(edptr, p_start + strlen(bgptr));

        if (p_end == string::npos){
            return;
        }

        string result = text.substr(p_start + strlen(bgptr), p_end - p_start - strlen(bgptr));
        char* pstr = base64_encode(result.data(), result.length());

        if (pstr)
        {
        	fileLog.writeLog(( FormatString("base64 url : [%]=>[%]\n").arg(result).arg(pstr).str()));
	        text.erase(p_start, p_end - p_start + strlen(edptr));
	        text.insert(p_start, pstr);
	        free(pstr);
        }

        offset = (p_start + result.length());
    }

    return;
}

void dw_line_wrap(string& text, int pos, char* str)
{
    if (!str || !(*str) || pos<1)
        return;
    int offset = 0;
    int L = strlen(str);
    while (offset+pos < (int)text.size()){
        text.insert(offset + pos, str);
        offset += (pos + L);
    }
}

void Split_by(const char* qwert,int len, vector<string> &vtr_qxlkjsd, char by)
{
    string qxlkjsd;
    vtr_qxlkjsd.clear();

    for (int i=0; i<len; i++)
    {
        if (!::isblank(qwert[i]))
        {
            if (qwert[i] == by)
            {
                if (qxlkjsd.length() > 0)
                {
                    vtr_qxlkjsd.push_back(qxlkjsd);
                    qxlkjsd.clear();
                }
            }
            else
            {
                qxlkjsd.append(1,qwert[i]);
            }
        }
    }

    if (qxlkjsd.length() > 0)
    {
        vtr_qxlkjsd.push_back(qxlkjsd);
    }
}

int DeliverEngineApp::ScanList(void)
{
    //UpdateList();
    //fileLog.writeLog("begin to ScanList\n");

    map<unsigned int, mail_node *>::iterator it = m_map_mail.begin(); 
    map<stdstring, _template_node>::iterator temp_it  = m_map_template.begin(); 

    if(m_map_mail.size() == 0)
    {
         //fileLog.writeLog("send list is empty, has no send\n");
        return 0;
    }

    int circle=0;
    static unsigned int from_seq = 0;
    //fileLog.writeLog(( FormatString("template size [%] \n").arg(m_map_template.size()).str()));

    while(1)
    {
         string mail_text = "";    //邮件正文
        stdstring str_template;
        if(it == m_map_mail.end())
        {
            break;
        }
        if(it->second->status == 4 && (time(NULL) - it->second->stime) < 120)           //软失败120s重试
        {
            fileLog.writeLog(( FormatString("60 s later,resend [%] [%]\n").arg(it->first).arg(it->second->mail).str() ));
            it++;
            continue;           
        }
        if(it->second->status != 1 && it->second->status != 2 && it->second->status != 5 && it->second->status != 6)  
        {
            char td[100]={0};
            string mail_from="";
            string sender="";
            string helo="";
            unsigned char from_flag;
            bool same_flag;
            int random = -1;
            int sms = 0;
            int modula_type_eml = 0;
            sprintf(td,"%u.%u.%u",it->second->task_id.time,it->second->task_id.m_id,it->second->task_id.p_id);
            if((temp_it = m_map_template.find(td)) == m_map_template.end())
            {
                if(CheckTemplate(it->second->task_id,str_template) != 0)
                {
                    fileLog.writeLog(("CheckTemplate error\n"));
                    it->second->status = 5;
                    it->second->errstr = "499 can't get template";
                    it->second->stime = time(NULL);
                    it->second->failtimes++;
                    it++;
                    continue;
                }

                _template_node node;
                if(Protemplate(node, str_template) != 0)
                {
                    fileLog.writeLog(("Protemplate error\n"));
                    it++;
                    continue;
                }

                //使用多发件人
                vector<string> vtrs;
                string oldsender = node.mail_from;
                Split_by(oldsender.data(),oldsender.length(),vtrs, ';');
                if (vtrs.size() > 1) {
                    mail_from = vtrs[rand() % vtrs.size()];
                    fileLog.writeLog(( FormatString("1.Using Duo sender {%} from {%}\n").arg(mail_from).arg(oldsender).str()));
                }
                else {
                    mail_from = node.mail_from;
                }

                sender = node.sender;

                MakeMailText((char *)(node.templatestr.c_str()),
                                     it->second->map_variable,
                                     mail_text,
                                     it->second->mail,
                                     random,
                                     (char*)sender.c_str()/*node.sender*/);
                                     
                fileLog.writeLog(( FormatString("check template id [%][%][%][%] \n")
                                .arg(it->second->task_id.time)
                                .arg(it->second->task_id.m_id)
                                .arg(it->second->task_id.p_id)
                                .arg(it->second->mail).str() ));

                m_map_template.insert(map<stdstring,_template_node>::value_type(td,node));
                from_flag = node.from_flag;
                same_flag = node.same_flag;
                helo      = node.helo;
                sms       = node.sms;
                modula_type_eml = node.modula_type_eml;
            }
            else
            {    
                //使用多发件人
                vector<string> vtrs;
                string oldsender = temp_it->second.mail_from;
                Split_by(oldsender.data(),oldsender.length(),vtrs, ';');
                if (vtrs.size() > 1) {
                    mail_from= vtrs[rand() % vtrs.size()];
                    fileLog.writeLog(( FormatString("2.Using Duo sender {%} from {%}\n").arg(mail_from).arg(oldsender).str()));
                }
                else {
                    mail_from = temp_it->second.mail_from;
                }
                
                sender = temp_it->second.sender;
                
                MakeMailText((char * )(temp_it->second.templatestr.c_str()),
                              it->second->map_variable,
                              mail_text,
                              it->second->mail,
                              random,
                              (char*)sender.c_str()/*temp_it->second.sender*/);
                fileLog.writeLog(( FormatString("template id [%][%][%][%] \n")
                                  .arg(it->second->task_id.time)
                                  .arg(it->second->task_id.m_id)
                                  .arg(it->second->task_id.p_id)
                                  .arg(it->second->mail).str() ));

                //mail_from = temp_it->second.mail_from;
                from_flag = temp_it->second.from_flag;
                same_flag = temp_it->second.same_flag;
                //sender    =   temp_it->second.sender;
                helo      = temp_it->second.helo;
                sms       = temp_it->second.sms;
                modula_type_eml = temp_it->second.modula_type_eml;
            }

            if (from_flag == 1)
            { //发件人随机数替换
                char buf[16] = {0};
                snprintf(buf,sizeof(buf), "%04d", random>0?random:rand()%10000);

                if (mail_from.find("$iedm_ran$") != string::npos){
                    StrReplace(mail_from, (char *)mail_from.c_str(), "$iedm_ran$", buf, 0);
                }
                else {
                    size_t pos = mail_from.find("@");
                    if (pos != string::npos){
                        mail_from.insert(pos,buf);
                    }
                }
                
                if (same_flag == 1){
                    sender = mail_from;
                }

                fileLog.writeLog(( FormatString("random replace: sender=[%] mail_from=[%]\n").arg(sender).arg(mail_from).str()));
            }
            
            StrReplace(mail_text, (char *)mail_text.c_str(), "$iedm_from_01$", sender.c_str(), 0);
            base64_replace(mail_text);

            
            /*
            //V2.8.1 2014-10-14 wEide
            <!--\n
            <RICHINFO>\n
            <WEB TYPE="QVGA"></WEB>\n
            </RICHINFO>\n
            -->
            */

            char *border_up = (char*)"<WEB TYPE=\"QVGA\">";
            char *border_down = (char*)"</WEB>";
            std::size_t pos_up = mail_text.find(border_up);
            if (pos_up != string::npos){
                pos_up += strlen(border_up);
                std::size_t pos_down = mail_text.find(border_down, pos_up);
                if (pos_down != string::npos){
                    string qvga_str = mail_text.substr (pos_up, pos_down-pos_up);
                    char *qvga_base64_str = base64_encode(qvga_str.c_str(), qvga_str.length());
                    qvga_str.assign(qvga_base64_str);
                    dw_line_wrap(qvga_str, 76, (char*)"\n");
                    mail_text.erase (pos_up, pos_down-pos_up);
                    mail_text.insert (pos_up, qvga_str);
                    delete [] qvga_base64_str;
                }
            }

            char * text = NULL;
            if (modula_type_eml == 0){
                text = emlbase64((char *)mail_text.c_str());
                if(text == NULL)
                {
                    fileLog.writeLog(("base64 error\n"));
                    it++;
                    continue;
                }
            }
			string subject;
			string fkstr = "Subject: =?utf-8?B?";
			string::size_type position = mail_text.find(fkstr.c_str());
			if (position != string::npos){
				subject = mail_text.substr(position+fkstr.length(),mail_text.find("?=",position+fkstr.length())-position-fkstr.length());
				char* p = base64_decode(subject.c_str(),subject.length()); 
				subject = p;
				if (p) delete [] p; 
			}

            stdstring strIP;
            rfc_uint_16 nPort = 0;
			std::string real_domain = it->second->domain;
			std::map<std::string, std::string>::iterator domain_map_it = m_domain_map.find(real_domain);
			if (domain_map_it != m_domain_map.end()) {
				real_domain = domain_map_it->second;
			}
            if (DNS_LOOKUP_OK != mxmgr.getMXRecord(real_domain, strIP, nPort, from_seq)) {
				fileLog.writeLog((FormatString("getMXRecord: % error\n").arg(real_domain).str()));
                it->second->status = 5;
                it->second->errstr = "550 Mailbox not found,Domain not exist";
                it->second->stime = time(NULL);
                it->second->failtimes++;
                it++;
                continue;
            }

            char port[8]  = {0};
            sprintf(port, "%d", nPort);

            //起线程发送    
            pthread_attr_t attr;
            pthread_t tid;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

            //参数
            _thread_param  * param =  new _thread_param;        //在线程中释放

            param->seqno     = it->first;
            param->ip        = strIP;
            param->port      = port;
//          param->ip        = "127.0.0.1";
//          param->port      = "25";
            param->mail_text = modula_type_eml?mail_text.c_str():text;
            param->mail      = it->second->mail;
            param->vip       = m_ip;
            param->from      = from;            //本地配置的mail from
            param->is_auth   = is_auth;         //
            param->auth_info = auth_info;           //
            param->mail_from = mail_from;
            param->from_flag = from_flag;
            param->same_flag = same_flag;
            param->random    = random;
            param->sender    = sender;
            param->helo      = helo;
            param->sms       = sms;
            param->subject   = subject;
            param->vec_helo.clear();
            for(vector<string>::iterator it_helo = m_vec_helo.begin();it_helo!=m_vec_helo.end();it_helo++)  
            {
                param->vec_helo.push_back(*it_helo);
            }

            if(from_seq++ == 1024*1024*1024)
                from_seq = 0;

            param->from_seq  = from_seq;
            snprintf(param->task_id,sizeof(param->task_id),"%u.%u.%u",it->second->task_id.time,it->second->task_id.m_id,it->second->task_id.p_id); 

            if (text)
                delete []text;

            fileLog.writeLog(( FormatString("thread [%] [%] [%] [%]\n").arg(param->seqno).arg(param->ip).arg(param->port).arg(param->mail).str() ));
            if(pthread_create(&tid, &attr, (void* (*)(void*)) delivery, (void *) param) != 0)
            {
                fileLog.writeLog(( FormatString("thread start fail [%]\n").arg(strerror(errno)).str() ));
                param->vec_helo.clear();
                delete param;
                break;
            }
            
            it->second->status = 2;   //工作中，请勿重发
            fileLog.writeLog("thread start success\n");
            circle++;
        }
        it++;
    }

    fileLog.writeLog(( FormatString("a circle send [%] mail\n").arg(circle).str() ));
    return 0;
}

int DeliverEngineApp::MakeMailText(char * template_text, 
                                  map<stdstring,stdstring> &map_variable, 
                                  string & mail_text,
                                  const char * mail, 
                                  int & random, 
                                  char * sender)
{
    map<stdstring,stdstring>::iterator it = map_variable.begin();   
    mail_text = template_text;
    char old[128];
    tables_t tables;
    while(1)
    {
        if(map_variable.size() == 0)
        {
            fileLog.writeLog( "this mail delivery has no variable\n");
            break;
        }

        if(it == map_variable.end())
        {
            break;
        }

        if(strlen(it->first.c_str()) == 0)
        {
            it++;
            continue;
        }

        memset(old,0,sizeof(old));
        sprintf(old, "$%s$", it->first.c_str());
        fileLog.writeLog((FormatString("template key [%] [%]\n").arg(it->first.c_str()).arg(it->second.c_str()).str()));
        if(strcmp(it->first.c_str(),"iedm_ran") == 0)
        {
            random = atoi(it->second.c_str());
        }
        // V2.9.0 
        string var(it->first),value(it->second);
        if(0!=table_first_step(var, value, tables) && strstr(template_text, it->first.c_str()) != NULL)  //找到则替换
        {
            StrReplace(mail_text, (char *)mail_text.c_str(), old, it->second.c_str(), strlen(template_text));
        }

        it++;
    }

    //V2.9.0
    table_second_step(mail_text, tables);

    //add 2013/03/25
    /* 在函数外部替换
    string rand_sender = sender;
    if (random >0) {
        char buf[16] = {0};
        snprintf(buf,sizeof(buf), "%d", random);
        StrReplace(rand_sender, (char *)rand_sender.c_str(), "$iedm_ran$", buf, 0);
    }
    StrReplace(mail_text, (char *)mail_text.c_str(), "$iedm_from_01$", rand_sender.c_str(), 0);
    */
    
    memset(old,0,128);
    sprintf(old, "%s", "$iedm_to$");
    if(strstr(template_text, "$iedm_to$") != NULL) //$iedm_to$替换
    {
        StrReplace(mail_text, (char *)mail_text.c_str(), old, mail, strlen(template_text));
    }

    memset(old,0,128);
    sprintf(old, "%s", "$iedm2_to$");
    char * encode = base64_encode(mail, strlen(mail));
    char base_mail[256];
    snprintf(base_mail, sizeof(base_mail), "%s", encode);
    free(encode);

    if(strstr(template_text, "$iedm2_to$") != NULL) //$iedm2_to$替换
    {
        StrReplace(mail_text, (char *)mail_text.c_str(), old, base_mail, strlen(template_text));
    }

    if(strstr(template_text, "$iedm_time$") != NULL) //$iedm_time$替换
    {
        snprintf(old, sizeof(old), "%s", "$iedm_time$");
        char sTime2[64];
        time_t tTime = time(NULL);
        struct tm tmTime;
        localtime_r(&tTime, &tmTime);
        snprintf(sTime2, sizeof(sTime2), "%s, %02d %s %d %02d:%02d:%02d %s", sWeekName1[tmTime.tm_wday], tmTime.tm_mday, sMonthName1[tmTime.tm_mon], tmTime.tm_year+1900, tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec, "+0800");

        StrReplace(mail_text, (char *)mail_text.c_str(), old, sTime2, strlen(template_text));
    }


    //二级域名随机串
    /*
    char newsender[128];
    char oldsender[128];
    char name[64];
    char domain[64];
    memset(newsender, 0, sizeof(newsender));
    memset(oldsender, 0, sizeof(oldsender));
    memset(name, 0, sizeof(name));
    memset(domain, 0, sizeof(domain));

    strcpy(newsender,sender);
    strcpy(oldsender,sender);

    char * p = strchr(oldsender,'@');
    if(GetRandomDomain(newsender,random) == 1)
    {
        fileLog.writeLog((FormatString("need to replace two stage domain[%] [%]\n").arg(p).arg(newsender).str()));
        StrReplace(mail_text, (char *)mail_text.c_str(), p, newsender, strlen(template_text));

        sscanf(sender, "%[^@]@%[^@]", name,domain);
        snprintf(sender,129,"%s%s",name,newsender);
    }
    */
//  exit(0);

    return 0;
}

int DeliverEngineApp::GetRandomDomain(char * newsender, const int random)
{
    if(strchr(newsender,'@') == NULL)
    {
        return -1;
    }

    char name[64];
    char domain[64];
    memset(name,0,sizeof(name));
    memset(domain,0,sizeof(domain));
    sscanf(newsender, "%[^@]@%[^@]", name,domain);

    char * p = NULL;
    if((p=strchr(domain,'.')) != NULL && strchr(p+1,'.') != NULL)   //二级域名
    {
        *p = '\0';
        srand((int)time(0));
        snprintf(newsender,129,"@%s%d.%s",domain, random(10),p+1);
    
        return 1;
    }

    return 0;   
}

void DeliverEngineApp::StrReplace(string & dest, char *src, const char *oldstr, const char *newstr, int len)
{
    if(strcmp(oldstr, newstr)==0)
    {
        dest = src;
        return;
    }

    char *needle;
    char *tmp;
    char *tmp1;
    dest = src;

    while((needle = (char*)strstr(dest.c_str(), oldstr)) != NULL)
    {
        tmp1 = (char *)dest.c_str();
        tmp=(char*)malloc(strlen(tmp1)+(strlen(newstr)-strlen(oldstr))+1);
        strncpy(tmp, (char *)dest.c_str(), needle-tmp1);
        tmp[needle-tmp1]='\0';

        strcat(tmp, newstr);
        strcat(tmp, needle+strlen(oldstr));

        dest = tmp;
        free(tmp);
    }

    return ;
}

int DeliverEngineApp::Response(void)
{
    fd_set ReadSet;

    FD_ZERO(&ReadSet);
    FD_SET(m_dispfd, &ReadSet);
    struct timeval timeout;
    timeout.tv_sec = 60;
    timeout.tv_usec= 0;
    int ret;
    unsigned int length;
    unsigned int h_length;
    char c_len[5];
    if((ret = select(m_dispfd+1,&ReadSet,NULL,NULL,&timeout)) == -1)
    {
        fileLog.writeLog(( FormatString("[%] select fail\n").arg(errno).str() ));
        return -1;
    }
    else if(ret == 0) 
    {
        //fileLog.writeLog("select time out\n");
        return 0;
    }
    else
    {
        //fileLog.writeLog("have data to read\n");
    }

    memset(c_len,0,sizeof(c_len));
    ret = recv(m_dispfd, c_len, 4, 0);
    if(ret  != 4)
    {
        fileLog.writeLog(( FormatString("recvrsp length fail [%] [%]\n").arg(ret).arg(strerror(errno)).str() ));
        return -1;
    }
    memcpy(&length, c_len, 4);
    h_length = ntohl(length);
    //fileLog.writeLog(( FormatString("recv msg length [%]\n").arg(h_length).str() ));

    if(h_length >= 1024*1024)           //超过这个数，就认定是垃圾连接
    {
        fileLog.writeLog(( FormatString("invalid msg length [%] \n").arg(h_length).str() ));
        return -1;
    }

    char * rsp_msg = new char [h_length+1];
    memset(rsp_msg,0,h_length+1);
    unsigned int i = 0;
    unsigned int j = h_length;
    while(1)
    {
        if(i == h_length)
            break;

        if((ret = select(m_dispfd+1,&ReadSet,NULL,NULL,&timeout)) == -1)
        {
            fileLog.writeLog(( FormatString("[%] select fail\n").arg(errno).str() ));
            delete [] rsp_msg;
            return -1;
        }
        else if(ret == 0)
        {
            //fileLog.writeLog("recv msg body select time out\n");
            delete [] rsp_msg;
            return -1;
        }
        else
        {
            //fileLog.writeLog("recv msg have data to read\n");
        }

        ret = recv(m_dispfd, rsp_msg + i, j, 0);
        if(ret == 0)
        {
            fileLog.writeLog("perr close\n");
            delete [] rsp_msg;
            return -1;
        }

        j -= ret;
        i += ret;
    }
    fileLog.writeLog(( FormatString("recv msg length real [%] should [%]\n").arg(i).arg(h_length).str() ));

    //解析应答，加入队列
    req_mail.ParseMailListRsp(rsp_msg, m_map_mail);     
    //fileLog.writeLog("ParseMailListRsp end\n");

    delete [] rsp_msg;
    return 0;
}

int DeliverEngineApp::CheckTemplate(_task_id & taskid, stdstring & str_template)
{
    int i = 0;
    while(1)
    {
	int iret = req_mail.TemplateSession(taskid,m_dispfd,str_template, m_map_mail);
     if((iret != 0))
        {
            if(i < 3)
            {
                fileLog.writeLog("check template fail,reconnect\n");
                close(m_dispfd);
                CreateClient();
                i++;
                continue;
            }
            else
            {
                fileLog.writeLog("check template 3 times fail,reconnect\n");
                return -1;
            }
        }
        else
        {
            break;
        }
    }
    fileLog.writeLog("check template ok\n");
    return 0;   
}

//发送调度机请求
int DeliverEngineApp::MailListRequest(void)
{
    memset(m_req_tmgr,0, REQ_MSG_MAX_LEN);
    if (g_helpersvr.bind.length() > 0){
        //take_ability_info(m_map_domain); //add by weide 2013/11/13
    }
    req_mail.MailListReq(m_map_mail, m_map_domain, m_req_tmgr, m_dispfd);
    return 0;
}

int DeliverEngineApp::SendReport(void)
{
    if(time(NULL) - m_iterval >= m_reportiterval) 
    {
	if(m_map_mail.size() == 0) {
            return 0;
        }
        vector<unsigned int> vec;
	int iret = req_mail.SendReport(m_map_mail, m_dispfd, vec);
	//fileLog.writeLog("begin to sendreport\n");
        if(iret == -1)
        {
            m_iterval = time(NULL);
            fileLog.writeLog("sendreport fail, can't clear mem,reconnect\n");
            close(m_dispfd);
            CreateClient();
            return -1;
        }
	//fileLog.writeLog("end to sendreport\n");
        m_iterval = time(NULL);
        vector<unsigned int>::iterator iter = vec.begin();
        for(;iter != vec.end();iter++)
        {
            if(m_map_mail.find(*iter) != m_map_mail.end())
            {
                m_map_mail[*iter]->map_variable.clear();
                delete m_map_mail[*iter];
                  m_map_mail.erase(*iter);
            }
        }
    }

    return 0;   
}

int CreateClient(void)
{
    int ret;
    struct sockaddr_in addr = {0};
    struct sockaddr_in sctClient = {0};
    struct in_addr x;
    inet_aton(m_dispatchip,&x);
    addr.sin_family = AF_INET;
    addr.sin_addr = x;
    addr.sin_port = htons((short )atoi(m_dispatchport));

    m_dispfd = socket(AF_INET,SOCK_STREAM,0);
    if(m_dispfd == -1)
    {
        printf("error:%s\n",strerror(errno));
        return m_dispfd;
    }

    memset(&sctClient, 0x0, sizeof(sctClient));
    sctClient.sin_family = AF_INET;
    sctClient.sin_addr.s_addr = inet_addr(m_ip);
    if(bind(m_dispfd, (struct sockaddr *)&sctClient, sizeof(sctClient)) < 0)
    {
        fileLog.writeLog(( FormatString("bind ip fail error [%] str [%]\n").arg(strerror(errno)).arg(errno).str() ));
        close(m_dispfd);
        return -1;
    }
    fileLog.writeLog(( FormatString("bind ip [%] ok\n").arg(m_ip).str() ));

    while(1)
    {
        ret = connect(m_dispfd,(sockaddr*)&addr,sizeof(addr));
        if(ret != 0)
        {
            fileLog.writeLog(( FormatString("connect fail error [%] str [%]\n").arg(strerror(errno)).arg(errno).str() ));
            sleep(3);
            continue;
        }
        if(ret == 0)
        {
            fileLog.writeLog("connect ok\n");
            break;
        }
    }

    ret = fcntl(m_dispfd, F_GETFL, 0);
    fcntl(m_dispfd, F_SETFL, ret | O_NONBLOCK);

    return 0;
}

RFC_NAMESPACE_END

MAIN_APP(DeliverEngineApp)
