#include <strings.h> // for strncasecmp
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/sysinfo.h>
#include <signal.h>
#include <fstream>
#include "label_mgr.h"
#include "profile.h"
#include "threadpool.h"
#include "log.h"
#include <limits>
#include <sstream>
#include <algorithm>
#include <string.h>

namespace global
{
    mysql_inf_t mysql_conf;
    configure_t configure;
    Thread_Pool work_thread_queue;
    Thread_Pool scanlog_thread_queue;
    Msg_Queue<MYSQL*> mysql_queue(-1);
    set<int> working_time;

    int peer_shutdown_flag;
};

void cleanup_task_context(task_context_t* tctx, bool deleted)
{
    if ( tctx ){
        if (tctx->sockfd){
            close(tctx->sockfd);
            tctx->sockfd = 0;
        }
        tctx->epfd = -1;
        tctx->recvbuf.clear();
        //tctx->sendbuf.clear();
        if (tctx->sendbuf){
            delete tctx->sendbuf;
            tctx->sendbuf = NULL;
        }
        tctx->sb_bytes = 0;
        tctx->sb_transfrred = 0;
        tctx->request.method = HTTP_NULL;
        tctx->request.url.clear();
        tctx->request.version.clear();
        tctx->action.type = 0;

        if (tctx->mysql) {
            global::mysql_queue.put(tctx->mysql);
            tctx->mysql = NULL;
        }

        tctx->synlock.UnLock(); 

        if (deleted){
            delete tctx;
        }
    }
}

//OK
int check_email(const char* mail)
{
    const char* flag = NULL;
    const char* p = mail;
    while (isalnum(*p) || *p=='.' || *p=='_' || *p=='-') ++p;

    if (*p != '@' || p == mail)
        return -1;

    flag = p++; // '@'
    while (isalnum(*p) || *p=='-') ++p;
    if (*p != '.' || p == flag+1)
        return -1;

    flag = p++; // '.'
    while (isalnum(*p) || *p=='-') ++p;
    if (p == flag+1) 
        return -1;

    while ( *p != '\0')
    {
        if (*p != '.')
            return -1;

        flag = p++; // '.'
        while (isalnum(*p) || *p=='-') ++p;
        if (p == flag+1) 
            return -1;
    }

    return 0;
}

//OK
int parse_request_line(const char* line, http_request_t* req)
{
    const char* url = NULL;

    if (strncasecmp(line, "GET", 3) == 0){
        req->method = HTTP_GET;
        url = line + 3;
    }
    else if (strncasecmp(line, "POST", 4) == 0){
        req->method = HTTP_POST;
        url = line + 4;
    }
    else
        return -1;

    if ( !isblank(*url))
        return -1;

    while ( isblank(*url) )
        ++url;

    char* version = (char*)strchr(url, ' ');
    if ( !version )
        version = (char*)strchr(url, '\t');

    if ( !version )
        return -1;

    req->url.assign(url,  version-url);

    while ( isblank(*version) ) ++version;

    if (strncasecmp(version, "HTTP/",5) != 0)
        return -1;

    if ( !isdigit(version[5])
        || version[6] != '.'
        || !isdigit(version[7]))
        return -1;

    req->version.assign(version,8);

    char* next = version + 8;
    while ( *next != '\0' && *next!='\r' && *next!='\n'){
        if (!isblank(*next++))
            return -1;
    }

    return 0;
}

//OK
int parse_task_action(const char* url, task_action_t* ta)
{
    if ( !url || !ta )
        return -1;

    const char* param = NULL;
    if ( strncasecmp(url, "/insert?", 8) == 0 ){
        param = url + 8;
        ta->type = TA_INSERT;
    }
    else if (strncasecmp(url, "/query?", 7) == 0){
        param = url + 7;
        ta->type = TA_QUERY;
    }
    else
        return -1;

    string field,value;
    string* target = &field;

    do{
        switch(*param)
        {
        case '=':target = &value;
            break;
        case '&':
        case '\0':
            if (field.length() > 0){
                std::transform(field.begin(),field.end(),field.begin(),(int(*)(int))tolower);
                ta->param[field] = value;
            }
            field.clear();
            value.clear();
            target = &field;
            break;
        default:target->append(1,*param);
            break;
        }

        if (*param != '\0')
            param++;
        else 
            break;

    }while(1);

    return 0;
}

//OK
int sockselect(int fd, int flag, int timeout)
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

    timeval tv = {timeout, 0};
    return ::select(fd + 1, p[0], p[1], p[2], (timeout>-1? &tv:NULL));
}

//OK, nb is 'non-blocking' , is not 'niu bi'
int nb_connect( const char* host, int port, int timeout)
{
    sockaddr_in peer;
    bzero(&peer, sizeof(sockaddr_in));
    peer.sin_family = PF_INET;
    peer.sin_port = htons(port);
    peer.sin_addr.s_addr = inet_addr(host);

    int fd = -1;
    if((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        return -1;
    }

    int mode = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,&mode, sizeof(mode));
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK);

    switch (::connect(fd,(sockaddr*)&peer, sizeof(sockaddr_in)))
    {
    case -1:{
        if (errno != EINPROGRESS && errno != EWOULDBLOCK){
            close(fd);
            return -1;
        }
        sockselect(fd, SOCK_W, timeout);

        int errval = 0;
        socklen_t errlen = sizeof(errval);
        getsockopt(fd, SOL_SOCKET, SO_ERROR, &errval, &errlen);
        if (errval != 0){
            close(fd);
            return -1;
        }
        break;}
    default:
        break;
    }

    return fd;
}

//OK
int recv_until_edge( int handle, std::string & result, int length, int timeout)
{
    timeval tv = {timeout, 0};
    int remaining = length;
    char buffer[512];

    while (remaining > 0)
    {
        int bytes = ::recv(handle, buffer, MIN(remaining, sizeof(buffer)),0);
        switch (bytes)
        {
        case -1:
            if (errno==EINTR)
                break;
            else if (errno==EAGAIN || errno==EWOULDBLOCK)
            {
                if (timeout > 0){
                    fd_set rds;
                    FD_ZERO(&rds);
                    FD_SET(handle,&rds);
                    if (::select(handle + 1, &rds, NULL, NULL, &tv) > 0 ){
                        break;
                    }
                }
                return length - remaining;
            }
            return -1;
        case 0:
            global::peer_shutdown_flag = 1;
            LOG_MSG(("error.the peer has performed an orderly shutdown\n"));
            return length>remaining ? length-remaining:-1;
        default:
            remaining -= bytes;
            result.append(buffer,bytes);
            break;
        }
    }

    return length - remaining;
}

int recv_line_from_cache_or_socket(int sockfd, string& cache, string& line, int timeout)
{
    do {
        int crlf = cache.find(S_LF);
        if ( string::npos != crlf ){
            line = cache.substr(0,crlf);
            cache.erase(0,crlf + 1);
            return 0;
        }

        if (cache.length() > MAX_LENGTH){
            LOG_MSG(("error.Message line too large.\n"));
            return -1;
        }

        switch(recv_until_edge(sockfd, cache, MAX_LENGTH, 0))
        {
        case -1: LOG_MSG(("error.receive message fails.\n"));
            return -1;
        case 0:
            if (sockselect(sockfd, SOCK_R, timeout) < 1){
                LOG_MSG(("error.receive timeout.\n"));
                return -1;
            }
        default:
            break;
        }
    }while(1);
}

int get_http_length_and_remove_header(int sockfd, string& recvbuf, int timeout)
{
    string line;
    int length = 0;
    int num_rows = 32;

    do {
        if (0 != recv_line_from_cache_or_socket(sockfd, recvbuf, line, timeout)){
            return -1;
        }

        STD_TRIM(line, " \t\r");
        if (line.length() < 1)
            break;

        if (strncasecmp(line.data(), "Content-Length:", 15) == 0){
            sscanf(line.data() + 15, "%d", &length);
            LOG_MSG(("get length '%d' from '%s'\n", length, line.data()));
        }

        if (num_rows-- == 0) {
            LOG_MSG(("error.There is too much message-header in request.\n"));
            return -1;
        }
    }while(1);

    return length;
}

//OK
int send_until_edge( int handle, const void* buffer, int length, int timeout )
{
    if (handle < 1|| !buffer || length < 0)
        return -1;

    timeval tv = {timeout, 0};
    int transferrd = 0;
    while (length > transferrd )
    {
        int bytes = ::send(handle,(char*)buffer + transferrd,length - transferrd,0);
        switch (bytes)
        {
        case -1:
            if (errno==EINTR)
                break;
            else if(errno==EAGAIN || errno==EWOULDBLOCK){
                if (timeout > 0){
                    fd_set wrds;
                    FD_ZERO(&wrds);
                    FD_SET(handle,&wrds);
                    if (select(handle + 1, NULL, &wrds, NULL, &tv) > 0 ){
                        break;
                    }
                }
                return transferrd;
            }
            return -1;
        case 0:
            return transferrd>0?transferrd:-1;
        default:
            transferrd += bytes;
            break;
        }
    }
    return transferrd;
}

//OK
int query_taskid_date(task_context_t* tctx, const char* taskid, string &create_time)
{
    int corp_id, task_id, template_id;
    sscanf(taskid,"%d.%d.%d", &corp_id, &task_id, &template_id);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), 
        "select date_format(create_time,'%%Y%%m%%d') as create_time from task where corp_id=%d and task_id=%d and template_id=%d",
        corp_id, task_id, template_id);
    
    if (0 != mysql_query(tctx->mysql, buffer)) {
        LOG_MSG(("query_taskid_date.error1:%s\n",mysql_error(tctx->mysql)));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(tctx->mysql);
    if (mysql_num_rows(result) > 0){
        create_time = mysql_fetch_row(result)[0];
        mysql_free_result(result);
        LOG_MSG(("taskid %s in '%s', create_time = %s\n",taskid, global::mysql_conf.db.c_str(),create_time.c_str()));
        return 0;
    }

    mysql_free_result(result);
    return -1;
}

//检查任务导入历史
typedef struct {
    string taskid;
    char   action;
    string btime;
    string etime;
    int    number;
} label_history_t;

int check_label_task_history(MYSQL* mysql, 
                             const char* taskid, 
                             char action, 
                             label_history_t* his)
{
    string sqlstr = " select btime,etime,number "
                    " from label_task_history "
                    " where taskid = '";
    sqlstr.append(taskid);
    sqlstr.append("' and action = ");
    sqlstr.append(1, action);

    if (0 != mysql_query(mysql, sqlstr.c_str())) {
        LOG_MSG(("check_label_task_history failed:%s\n", mysql_error(mysql) ));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(mysql);
    int num_rows = mysql_num_rows(result);

    if (num_rows != 1){
    	LOG_MSG(("check_label_task_history num_rows: %d\n",num_rows));
        mysql_free_result(result);
        return -1;
    }

    if (his) 
    {
    	MYSQL_ROW row = mysql_fetch_row(result);
        his->taskid = taskid;
        his->action = action;
        his->btime  = row[0];
        his->etime  = row[1];
        sscanf(row[2],"%d", &his->number);
    }
    mysql_free_result(result);
    return 0;
}

//'1.2.3','1', now(), now(),???
int insert_label_task_history(MYSQL* mysql, label_history_t* his)
{
    char sqlbuffer[512];
    snprintf(sqlbuffer, sizeof(sqlbuffer), 
             "insert into label_task_history "
             " (taskid, action, btime, etime, number) "
             " values ('%s',%c, now(), now(), %d)",
             his->taskid.c_str(), his->action, /*his->btime, his->etime,*/ his->number);
	
    if (0 != mysql_query(mysql, sqlbuffer)) {
        LOG_MSG(("insert_label_task_history failed:%s\n", mysql_error(mysql) ));
        return -1;
    }

    return 0;
}

int insert_label_task_history(MYSQL* mysql,  const char* taskid, char action, int number)
{
    char sqlbuffer[512];
    snprintf(sqlbuffer, sizeof(sqlbuffer), 
             "insert into label_task_history "
             " (taskid, action, btime, etime, number) "
             " values ('%s',%c, now(), now(), %d)", taskid, action, number);
   
    if (0 != mysql_query(mysql, sqlbuffer)){
        LOG_MSG(("insert_label_task_history failed:%s\n", mysql_error(mysql) ));
        return -1;
    }
    return 0;
}

//OK
int check_label_exist(MYSQL* mysql, string& label, int count)
{
    if (mysql == NULL){
        LOG_MSG(("%s mysql NULL\n",__FUNCTION__));
        return -1;
    }

    string scnt;
    std::stringstream ss;
    ss<<count;ss>>scnt;

    string sqlstr = "select * from (select count(label_id) c from label where label_id in (";
    sqlstr += label;
    sqlstr += ")) t where c=";
    sqlstr += scnt;

    //mysql_ping(mysql);

    if (0 != mysql_query(mysql, sqlstr.c_str())) {
        LOG_MSG(("check_label_exist failed:%s\n", mysql_error(mysql) ));
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(mysql);
    if (mysql_num_rows(result) > 0){
        mysql_free_result(result);
        return 0;
    }

    mysql_free_result(result);
    return -1;
}

//OK
int check_table_exist(MYSQL* mysql, string & table)
{
    string sqlstr = "select table_name from information_schema.tables ";
    sqlstr += (" where table_name ='" + table + "' and table_schema='" + global::mysql_conf.db + "'");

    //mysql_ping(mysql);
    if (0 != mysql_query(mysql, sqlstr.c_str())) {
        LOG_MSG(("check_table_exist:%s\n", mysql_error(mysql)));
        return 0;
    }
    MYSQL_RES *result = mysql_store_result(mysql);
    int rownum = mysql_num_rows(result);
    mysql_free_result(result);
    return rownum>0 ? 0:-1;
}

//OK
int create_label_table(MYSQL* mysql, string &table)
{
    // create table using demo
    string sqlstr = "CREATE TABLE " + table + " LIKE label_data";
    //mysql_ping(mysql);
    if (0 != mysql_query(mysql, sqlstr.c_str())){
        LOG_MSG(("create_label_table:%s\n", mysql_error(mysql)));
        return -1;
    }

    return 0;
}

//OK
//up_time 参数请注意,传入now()时,不带''，传入时间字符时请自行加'', 如: "'20130717'"
int insert_label_by_email(MYSQL* mysql, const char* email, set<string>& lbset, char action, int count, const char* up_time)
{
    char subsql[368];
    set<string>::iterator itlb;

    //mysql_ping(mysql);
    for (itlb=lbset.begin(); itlb!=lbset.end(); ++itlb)
    {
        string table = "label_data_" + *itlb; 
        switch(action)
        {
        case '1':
            snprintf(subsql, sizeof(subsql),
                "insert into %s (label_id,mailbox,open,click,score,create_time,update_time) "
                "values ( %s, '%s', %d, 0, %d, now(), %s ) "
                "on duplicate key update "
                "open=open+%d, score=score+%d, update_time=%s ",
                table.c_str(),
                itlb->c_str(), email, count,count, up_time,
                count,count,up_time);
            break;
        case '2':
            snprintf(subsql, sizeof(subsql),
                "insert into %s (label_id,mailbox,open,click,score,create_time,update_time) "
                "values ( %s, '%s', 0, %d, %d, now(), %s ) "
                "on duplicate key update "
                "click=click+%d, score=score+%d, update_time=%s ",
                table.c_str(),
                itlb->c_str(), email, count,count*2, up_time,
                count,count*2,up_time);
            break;
        default:
            continue;
        }

        //LOG_MSG(("insert.email.sql[%s]\n", subsql));

        if (0 != mysql_query(mysql, subsql)) {
            LOG_MSG(("insert into %s failed:%s\n", table.c_str(),mysql_error(mysql)));
            return -1;
        }
    }

    return 0;
}

void* insert_label_by_taskid(void* param)
{
    thread_data_t* pthrdt = (thread_data_t*)(param);
    thread_data_t thrdt;
    thrdt.dbsrc = pthrdt->dbsrc;
    thrdt.action = pthrdt->action;
    thrdt.btime = pthrdt->btime;
    thrdt.taskid = pthrdt->taskid;
    thrdt.lbstr = pthrdt->lbstr;
    thrdt.lbset.swap(pthrdt->lbset);
    delete pthrdt;

    while(1)
    {
        time_t wktime;
        time( &wktime );
        struct tm wktm;
        localtime_r( &wktime, &wktm );

    	if (global::working_time.find(wktm.tm_hour) 
    		!= global::working_time.end()){
    		break;
    	}

    	sleep(5*60);
    }
    
    LOG_MSG(("thread.begin.insert label, action:%c, taskid:%s, labels:%s\n", 
        thrdt.action, thrdt.taskid.c_str(), thrdt.lbstr.c_str()));

    MYSQL* mysql = NULL;
    //global::mysql_new_queue.get(mysql);

    LOG_MSG(("connect to %s:%d ...\n",global::configure.scanlog_host.c_str(), global::configure.scanlog_port));
    int handle = nb_connect(global::configure.scanlog_host.c_str(), global::configure.scanlog_port, global::configure.socktimeout);

    if (handle < 1){
        LOG_MSG(("connect scanlog failed,%s\n",strerror(errno)));
        return (void*)-1;
    }

    char reqbuffer[256];
    int reqlen = snprintf(reqbuffer, sizeof(reqbuffer), 
        "GET /%s?mode=detail&taskid=%s&begintime=%s HTTP/1.1\r\n\r\n", 
        (thrdt.action=='1'?"open":"click"),
        thrdt.taskid.c_str(), 
        thrdt.btime.substr(0,8).c_str());

    LOG_MSG((reqbuffer));

    int transfrred = send_until_edge(handle, (void*)reqbuffer,reqlen, global::configure.socktimeout);
    if (transfrred < 1){
        LOG_MSG(("scan.request failed:%s\n",strerror(errno)));
        close(handle);
        return (void*)-1;
    }

    LOG_MSG(("request length: %d bytes, transfrred length: %d bytes\n", reqlen, transfrred));

    string recvbuf;
    int length = get_http_length_and_remove_header(handle,recvbuf, global::configure.socktimeout * 100);
    LOG_MSG(("scan.Content-Length: %d\n", length));

    if (length < 0){
        close(handle);
        return (void*)-1;
    }

    int row_total = 0;
    int row_valid = 0;
    int row_succeed = 0;
    transfrred = 0;

    //MYSQL* mysql = NULL;
    global::mysql_queue.get(mysql);
    bool lastline = false;

    do {
        string line;
        if (0 != recv_line_from_cache_or_socket(handle, 
            recvbuf, 
            line, 
            global::configure.socktimeout * 100)){
                break;
        }

        if (line.length() < 1) {
        	LOG_MSG(("scanlog.null string\n"));
        	break;
        }
        
        transfrred += (line.length() + 1);
        if (length > 0 && transfrred >= length) {
        	lastline = true;
        	int extra = transfrred - length;
        	if (extra > 0){
        	    line.erase(line.length() - extra);
        	    LOG_MSG(("truncate => Content-Length:%d, transfrred:%d\n", length, transfrred));
        	}
        }

        int count = 0;
        char email[256], date[64], time[16];

        if (row_total++ < 1){
            LOG_MSG(("Title:%s\n", line.c_str()));
        }
        else {
            sscanf(line.data(),"%[^,],%[^,],%[^,],%d", email,date,time,&count);
            int a[6];

            string datetime = "'";
            datetime += date;
            STD_TRIM(datetime, " \t");
            datetime += " ";
            datetime += time;
            STD_TRIM(datetime, " \t");
            datetime += "'";

            if (check_email(email) != 0 
                || 6 != sscanf(datetime.data(), "'%4d-%2d-%2d %2d:%2d:%2d'", &a[0],&a[1],&a[2],&a[3],&a[4],&a[5])
                || count < 1){
                LOG_MSG(("scan.Row error:%s\n", line.c_str()));
                continue;
            }

            row_valid ++;
            int retval = insert_label_by_email(mysql,email,thrdt.lbset,thrdt.action, count, datetime.c_str());
            if (retval == 0){
                row_succeed++;
            }
        }
    }while(!lastline);

    //记录导入的任务,避免重复导入
    //insert_label_task_history(mysql,thrdt.taskid.c_str(),thrdt.action, row_succeed);

    global::mysql_queue.put(mysql);
    LOG_MSG(("thread.end.insert.Row : action=%c, taskid=%s, labels=%s, total=%d, valid=%d, succeed=%d\n", 
        thrdt.action, thrdt.taskid.c_str(), thrdt.lbstr.c_str(),row_total,row_valid,row_succeed));
    close(handle);
    return (void*)0;
}

//只取第一列row[0]
int get_query_result(MYSQL* mysql, const char* sqlstr, task_context_t* tctx)
{
    LOG_MSG(("sql:[%s]\n",sqlstr));

    //mysql_ping(mysql);
    if (0 != mysql_query(mysql, sqlstr)) {
        LOG_MSG(("get_query_result failed:%s\n",mysql_error(mysql)));
        return DT_DB_ERROR;
    }

    MYSQL_ROW row;
    MYSQL_RES *result = mysql_store_result(mysql);
    int num_rows = mysql_num_rows(result);
    LOG_MSG(("mysql_num_rows: %d\n",num_rows));

    if (num_rows < 1){
        mysql_free_result(result);
        return DT_HTTP_OK;
    }

    int row_defualt_length = 80;
    int buffer_length = 256 + num_rows * row_defualt_length;
    tctx->sendbuf = new(std::nothrow) char[buffer_length];
    if (tctx->sendbuf == NULL){
        LOG_MSG(("new(std::nothrow) failed\n"));
        mysql_free_result(result);
        return DT_ERROR;
    }

    tctx->sb_bytes = 0;
    tctx->sb_transfrred = 0;
    int bytes = snprintf(tctx->sendbuf + tctx->sb_bytes, buffer_length - tctx->sb_bytes, 
        "HTTP/1.1 200 OK\r\n"
        "Server: www.mtarget.cn\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: $$$$$$$$$$$$\r\n" //保留11个字节
        "Connection: close\r\n"
        "\r\n");
    tctx->sb_bytes += bytes;

    int remaining_rows = num_rows;
    int content_length = 0;
    while (row = mysql_fetch_row(result)) 
    {
        unsigned long row_lengths = mysql_fetch_lengths(result)[0] + 2;
        if (row_lengths > (buffer_length - tctx->sb_bytes))
        {
            buffer_length = buffer_length + remaining_rows * row_lengths * 2;
            char* newbuffer = new(std::nothrow) char[buffer_length];
            memmove(newbuffer, tctx->sendbuf,tctx->sb_bytes);
            delete tctx->sendbuf;
            tctx->sendbuf = newbuffer;
            LOG_MSG(("renew(std::nothrow): %d bytes.\n", buffer_length));
        }

        int bytes = snprintf(tctx->sendbuf + tctx->sb_bytes, buffer_length - tctx->sb_bytes, "%s\r\n", row[0]);
        content_length += bytes;
        tctx->sb_bytes += bytes;
        remaining_rows--;
    }

    char* pflag = (char*)strchr(tctx->sendbuf, '$');
    pflag += snprintf(pflag, 11, "%u",content_length); //在保留的11个字节处操作
    while (*pflag == '\0' || *pflag == '$') {
        *pflag = ' ';
        pflag++;
    }

    LOG_MSG(("Response.Content-Length: %d\n",content_length));
    mysql_free_result(result);
    return 0;
}

int query_label_detail(MYSQL* mysql, 
                       set<string>& lbset, 
                       string& btime,
                       string& etime, 
                       string& minscore, 
                       string& maxscore, 
                       task_context_t* tctx)
{
    //string sqlstr = "select label_id, mailbox, score, create_time, update_time from label_data_";
    string sqlstr = "select concat(label_id,',',mailbox, ',',score, ',',create_time, ',',update_time) x from label_data_";
    sqlstr += lbset.begin()->c_str();
    sqlstr += " where 1>0 ";
    
    if (btime.length() > 0){
        sqlstr += " and update_time>= '";
        sqlstr += btime;
        sqlstr += "' ";
    }

    if (etime.length() > 0){
        sqlstr += " and update_time <= '";
        sqlstr += etime;
        sqlstr += "' ";
    }

    if (minscore.length() > 0){
        sqlstr += " and score>= ";
        sqlstr += minscore;
    }

    if (maxscore.length() > 0){
        sqlstr += " and score <= ";
        sqlstr += maxscore;
    }

    return get_query_result(mysql, sqlstr.c_str(), tctx);
}

//返回格式: 标签ID,表记录数, 创建时间,更新时间
int query_label_count(MYSQL* mysql, set<string>& lbset, task_context_t* tctx)
{
    string tblist;
    for (set<string>::iterator ii=lbset.begin(); ii!=lbset.end(); ++ii)
    {
        tblist += "'";
        tblist += ("label_data_" + *ii);
        tblist += "',";
    }

    tblist += "' label_data_null'";

    //table_name like 'label_data_%'
    string sqlstr = "select concat(substr(table_name,12),',',table_rows,',',create_time,',',update_time) "
        "from information_schema.tables where table_name in (";
    sqlstr += tblist;
    sqlstr += ") and table_schema = '";
    sqlstr += global::mysql_conf.db;
    sqlstr += "'";
    return get_query_result(mysql, sqlstr.c_str(), tctx);
}

void* thread_task_process(void* param)
{
    task_context_t* tctx = (task_context_t*)param;
    //AutoGuard<SyncLock> tmp_lock(tctx->synlock);

    if (tctx->sendbuf){
        delete tctx->sendbuf;
        tctx->sendbuf = NULL;
    }

    int retval = task_process_imp(param);
    LOG_MSG(("retval: %d\n",retval));

    switch (retval)
    {
    case DT_PROCESS:
        break;
    default:
        tctx->sendbuf = new(std::nothrow) char[256];
        tctx->sb_transfrred = 0;
        tctx->sb_bytes = snprintf(tctx->sendbuf, 256, 
            "HTTP/1.1 %d OK\r\n"
            "Server: www.mtarget.cn\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%d\r\n", retval,retval);
        break;
    }

    if (tctx->request.method & HTTP_CLOSE) {
        cleanup_task_context(tctx);
        return (void*)0;
    }

    epoll_output(param);

    //epoll_event evt;
    //evt.events = EPOLLOUT|EPOLLET;
    //evt.data.ptr = param;
    //if (0 != epoll_ctl(tctx->epfd, EPOLL_CTL_MOD, tctx->sockfd, &evt)) {
    //    LOG_MSG(("error.epoll_ctl.epollout: %s\n",strerror(errno)));
    //    cleanup_task_context(tctx);
    //}
    return (void*)0;
}

void split_label(string& labels, set<string>& lbset)
{
    string label;
    for (int i=0; i<labels.length(); i++)
    {
        switch(labels[i])
        {
        case ',': lbset.insert(label);
            label.clear();
            break;
        default:label.append(1,labels[i]);
            break;
        }
    }

    if (label.length() > 0){
        lbset.insert(label);
    }
}

int task_process_imp(void* param)
{
    task_context_t* tctx = (task_context_t*)param;
    //AutoGuard<SyncLock> tmp_lock(tctx->synlock);

    map<string,string>::iterator itlok;
    itlok = tctx->action.param.find("label");
    if (itlok == tctx->action.param.end()){
        LOG_MSG(("label not exist [URL]\n"));
        return DT_LABEL_ERROR;
    }
    
    set<string> lbset;
    string lblist = itlok->second;
    split_label(lblist,lbset);
    if (lbset.size() < 1){
        LOG_MSG(("label is empty\n"));
        return DT_LABEL_ERROR;
    }

    //在cleanup_task_context中释放
    LOG_MSG(("get mysql handle...\n"));
    int num = global::mysql_queue.size();
    tctx->mysql = NULL;
    while(1)
    {
        global::mysql_queue.get(tctx->mysql);
        if (mysql_ping(tctx->mysql) == 0)
            break;
        mysql_close(tctx->mysql);
        delete tctx->mysql;
        tctx->mysql = NULL;
        init_mysql_queue(global::mysql_queue, &global::mysql_conf, 1);
    }while(num-- > 0);

    LOG_MSG(("get mysql handle...ok\n"));

    if (0 != check_label_exist(tctx->mysql,itlok->second, lbset.size())) {
        LOG_MSG(("label is not exist [DB]\n"));
        return DT_LABEL_ERROR;
    }

    set<string>::iterator itlb;
    for (itlb=lbset.begin(); itlb!=lbset.end(); ++itlb){
        string table = "label_data_" + *itlb;
        if (0 != check_table_exist(tctx->mysql, table)){
            LOG_MSG(("create label_data table:%s\n",table.c_str()));
            create_label_table(tctx->mysql, table);
        }
    }

    switch(tctx->action.type)
    {
    case TA_INSERT:{

        //按任务ID批量插入
        itlok = tctx->action.param.find("taskid");
        if (itlok != tctx->action.param.end())
        {
            string btime;
            int retval = query_taskid_date(tctx, itlok->second.c_str(), btime);
            if ( retval == -1){
                LOG_MSG(("taskid is not exist:%s\n", itlok->second.c_str()));
                return DT_TASKID_ERROR;
            }

            thread_data_t* taskopen = new thread_data_t;
            taskopen->dbsrc = retval;
            taskopen->action = '1';
            taskopen->taskid = itlok->second;
            taskopen->btime = btime;
            taskopen->lbstr = lblist;
            taskopen->lbset.insert(lbset.begin(), lbset.end());

            thread_data_t* taskclick = new thread_data_t;
            taskclick->dbsrc = retval;
            taskclick->action = '2';
            taskclick->taskid = itlok->second;
            taskclick->btime = btime;
            taskclick->lbstr = lblist;
            taskclick->lbset.insert(lbset.begin(), lbset.end());

            global::scanlog_thread_queue.run(insert_label_by_taskid,(void*)taskopen);
            global::scanlog_thread_queue.run(insert_label_by_taskid,(void*)taskclick);
            //return 之后tctx被销毁
            return DT_HTTP_OK;
        }

        //按email插入
        itlok = tctx->action.param.find("email");
        if (itlok != tctx->action.param.end())
        {
            if (check_email(itlok->second.c_str()) != 0){
                LOG_MSG(("email error:%s\n", itlok->second.c_str()));
                return DT_MAILBOX_ERROR;
            }

            string email = itlok->second;
            itlok = tctx->action.param.find("action");

            if (itlok == tctx->action.param.end() 
                || (itlok->second != "1" && itlok->second != "2")){
                LOG_MSG(("action error\n"));
                return DT_MISSING_FIELD;
            }

            if (0 != insert_label_by_email(tctx->mysql,email.c_str(), lbset,itlok->second[0])) {
                return DT_DB_ERROR;
            }

            return DT_HTTP_OK;
        } 

        LOG_MSG(("error.missing field\n"));
        return DT_MISSING_FIELD;
        break;}

    case TA_QUERY:{

        itlok = tctx->action.param.find("type");
        if (itlok == tctx->action.param.end()){
            LOG_MSG(("error.missing 'type'\n"));
            return DT_MISSING_FIELD;
        }

        if (strcasecmp(itlok->second.c_str(), "count") == 0)
        {
            return query_label_count(tctx->mysql,lbset,tctx);
        } 
        else if (strcasecmp(itlok->second.c_str(), "detail") == 0)
        {
            string btime,etime,minscore,maxscore;

            //btime
            itlok = tctx->action.param.find("btime");
            if (itlok != tctx->action.param.end()){
                if (itlok->second.length() != 8 || 
                    itlok->second.find_first_not_of("0123456789") != string::npos){
                    LOG_MSG(("btime error:%s\n",itlok->second.c_str()));
                    return DT_DATE_ERROR;
                }
                btime = itlok->second;
            }

            //etime
            itlok = tctx->action.param.find("etime");
            if (itlok != tctx->action.param.end()) {
                if (itlok->second.length() != 8 || 
                    itlok->second.find_first_not_of("0123456789") != string::npos){
                    LOG_MSG(("etime error:%s\n",itlok->second.c_str()));
                    return DT_DATE_ERROR;
                }
                etime = itlok->second;
            }

            //minscore
            itlok = tctx->action.param.find("minscore");
            if (itlok != tctx->action.param.end()) {
                if (itlok->second.find_first_not_of("0123456789") != string::npos){
                    LOG_MSG(("minscore error:%s\n",itlok->second.c_str()));
                    return DT_SCORE_ERROR;
                }
                minscore = itlok->second;
            }

            //maxscore
            itlok = tctx->action.param.find("maxscore");
            if (itlok != tctx->action.param.end()) {
                if (itlok->second.find_first_not_of("0123456789") != string::npos){
                    LOG_MSG(("maxscore error:%s\n",itlok->second.c_str()));
                    return DT_SCORE_ERROR;
                }
                maxscore = itlok->second;
            }

            return query_label_detail(tctx->mysql,
                lbset, 
                btime, 
                etime, 
                minscore, 
                maxscore,
                tctx);
        } 
        else
        {
            LOG_MSG(("error.query type '%s' unknow\n",itlok->second.c_str()));
            return DT_TYPE_ERROR;
        }

        return DT_PROCESS;
        break;};
    default:
        return -1;
    }
    return 0;
}

int epoll_accept( int svrfd, int epfd )
{
    sockaddr_in addr;
    socklen_t len = sizeof(sockaddr_in);

    do {
        memset(&addr, 0x00, sizeof(sockaddr_in));
        int fd = ::accept(svrfd,(sockaddr *)&addr, &len);
        if (fd == -1){
            break;
        }

        task_context_t* tctx = new(std::nothrow) task_context_t;
        if ( !tctx){
            LOG_MSG(("Allocate dynamic memory fails for:[%s:%d]\n",
                inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)));
            close(fd); 
            continue;
        }

        int mode = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,&mode, sizeof(mode));
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK);

        tctx->sockfd = fd;
        tctx->epfd = epfd;
        tctx->host = inet_ntoa(addr.sin_addr);
        tctx->port = ntohs(addr.sin_port);
        tctx->request.method = HTTP_NULL;
        tctx->action.type = 0;
        tctx->sendbuf = NULL;
        tctx->sb_bytes = 0;
        tctx->sb_transfrred = 0;
        tctx->mysql = NULL;

        LOG_TXT(("\n"));
        LOG_MSG(("Connect from: %s:%d\n", tctx->host.c_str(), tctx->port));

        epoll_event evt;
        evt.events = EPOLLIN|EPOLLET;
        evt.data.ptr = tctx;
        if (0 != epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &evt)){
            LOG_MSG(("error.epoll_ctl.epollin:%s\n",strerror(errno)));
            cleanup_task_context(tctx);
        }
    } while (1);

    return 0;
}

void* epoll_output(void* param)
{
    task_context_t* tctx = (task_context_t*)param;

    if (tctx->sendbuf)
    {
        bool endofwhile = false;
        while (!endofwhile && tctx->sb_transfrred < tctx->sb_bytes) 
        {
            int bytes = ::send(tctx->sockfd,
                tctx->sendbuf + tctx->sb_transfrred, 
                tctx->sb_bytes - tctx->sb_transfrred,
                0);

            switch (bytes)
            {
            case -1:
                if (errno==EINTR)
                    break;
                else if(errno==EAGAIN || errno==EWOULDBLOCK){
                    endofwhile = true;
                    break;
                    //return (void*)0;
                }
            case 0:
                LOG_MSG(("error.response,client:[%s:%d], transfrred: %d/%d\n",
                    tctx->host.c_str(), tctx->port,tctx->sb_transfrred,tctx->sb_bytes));
                cleanup_task_context(tctx);
                //return (void*)-1;
                endofwhile = true;
                break;
            default:
                tctx->sb_transfrred += bytes;
                break;
            }
        }

        LOG_MSG(("success.response,client:[%s:%d], transfrred: %d/%d\n",
            tctx->host.c_str(), tctx->port,tctx->sb_transfrred, tctx->sb_bytes));
    }

    cleanup_task_context(tctx);
    return (void*)0;
}

void* epoll_input(void* param)
{
    global::peer_shutdown_flag = 0;
    task_context_t* tctx = (task_context_t*)param;

    if (tctx->synlock.TryLock() != 0){
        LOG_MSG(("error:task runing.\n"));
        return (void*)0;
    }

    if (recv_until_edge(tctx->sockfd, tctx->recvbuf, MAX_LENGTH, 0) < 0){
        LOG_MSG(("error:receive fail(%d), %s\n", tctx->request.method, strerror(errno)));
        return (tctx->request.method == HTTP_NULL) ? (void*)-1:(void*)0;
    }

    int crlf = tctx->recvbuf.rfind(S_LF); //CRLF
    if ( string::npos == crlf ){
        if (tctx->recvbuf.length() > MAX_LENGTH){
            LOG_MSG(("error:request line too large.\n"));
            return (void*)-1;
        }

        tctx->synlock.UnLock();
        return (void*)0;
    }

    tctx->recvbuf[crlf] = '\0';
    LOG_MSG(("request:[%s]\n", tctx->recvbuf.c_str()));
    tctx->recvbuf[crlf] = C_LF;

    if ( 0 != parse_request_line(tctx->recvbuf.data(), &tctx->request)){
        LOG_MSG(("error:can't parse request message.\n"));
        return (void*)-1;
    }

    if (0 != parse_task_action(tctx->request.url.data(), &tctx->action)){
        LOG_MSG(("error:can't parse task action.\n"));
        return (void*)-1;
    }

    if (global::peer_shutdown_flag == 1){
        tctx->request.method |= HTTP_CLOSE;
    }

    epoll_ctl(tctx->epfd, EPOLL_CTL_DEL, tctx->sockfd, NULL);
    global::work_thread_queue.run(thread_task_process, tctx);
    return (void*)0;
}

int epoll_engine(int handle)
{
    int epfd = ::epoll_create(1024);

    epoll_event evs[1024];
    epoll_event evt;
    evt.events = EPOLLIN|EPOLLET;
    evt.data.ptr = (void*)handle;

    epoll_ctl(epfd, EPOLL_CTL_ADD, handle, &evt);

    int epwait = 1800 * 1000;
    while ( 1 )
    {
        int rd = epoll_wait(epfd, evs, 256, epwait);

        if (rd < 0){
            if (errno != EINTR) {
                LOG_MSG(("error.epoll_wait:%s\n",strerror(errno)));
                break;
            }
        }
        else if (rd == 0){
            LOG_MSG(("No file descriptor became ready during the %d seconds\n", epwait/1000));
            continue;
        }

        for (int i=0; i<rd; i++)
        {
            if (evs[i].data.ptr == (void*)handle){
                epoll_accept(handle, epfd);
            }
            else if(evs[i].events & EPOLLIN)
            {
                task_context_t* tctx = (task_context_t*)evs[i].data.ptr;
                if ( (void*)0 != epoll_input(tctx)){
                    LOG_MSG(("closing client:[%s:%d]\n",tctx->host.c_str(), tctx->port));
                    cleanup_task_context(tctx);
                }
            }
            else if (evs[i].events & EPOLLOUT){
                //task_context_t* tctx = (task_context_t*)evs[i].data.ptr;
                //epoll_output(tctx);
            }
        }
    }

    close(epfd);
    return 0;
}

//OK
int mksocket( const char* host, int port )
{
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        perror("create socket");
        return -1;
    }

    int mode = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,&mode, sizeof(mode));
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK);

    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);//INADDR_ANY;

    if (bind(fd, (sockaddr *) &addr, sizeof(sockaddr)) == -1
        || listen(fd, 128) == -1)
    {
        perror("socket:");
        return -1;
    }

    return fd;
}

//OK
int init_mysql_queue(Msg_Queue<MYSQL*>& mysqlqueue, mysql_inf_t* conf, int count)
{
    for (int i=0; i< count; i++)
    {
        char value = 1;
        MYSQL* mysql = new MYSQL;
        mysql_init(mysql);
        mysql_options(mysql, MYSQL_SET_CHARSET_NAME, conf->charset.c_str());
        mysql_options(mysql, MYSQL_OPT_RECONNECT, (char *)&value);

        if (NULL == mysql_real_connect(mysql,conf->host.c_str(),
                                       conf->user.c_str(),
                                       conf->passwd.c_str(),
                                       conf->db.c_str(),
                                       conf->port,NULL, 0)){
            LOG_MSG(("mysql_real_connect failed:%s\n", mysql_error(mysql)));
            continue;
        }

        mysql_options(mysql, MYSQL_OPT_RECONNECT, (char *)&value);
        mysqlqueue.put(mysql);
    }

    LOG_MSG(("mysql(%s) connect queue: %d\n",conf->db.c_str(),mysqlqueue.size()));
    return 0;
}

void free_mysql_queue(Msg_Queue<MYSQL*>& mysqlqueue)
{
    MYSQL* mysql = NULL;
    while (mysqlqueue.get(mysql, 0) == 0)
    {
        if (mysql){
            mysql_close(mysql);
            delete mysql;
            mysql = NULL;
        }
    }
}

//OK
int load_configure(const char*file, configure_t* conf)
{
    CProfile prof;
    if (conf==NULL || prof.open(file) != 0) {
        printf("load profile failed:%s\n",file);
        return -1;
    }

    //[main] -> bind,port
    if (prof.get("main", "bind", conf->bind) != 0 
    	|| prof.get<int>("main", "port", conf->port) != 0){
        LOG_MSG(("get bind or port failed\n"));
        return -1;
    }

    //[main] -> mysql_queue_size
    conf->mysql_queue_size = 0;
    prof.get<int>("main", "mysql_queue_size", conf->mysql_queue_size);
    if (conf->mysql_queue_size < 1 ){
        conf->mysql_queue_size = get_nprocs_conf() * 2;
    }

    //[main] -> work_nthread
    conf->work_nthread = 0;
    prof.get<int>("main", "work_nthread", conf->work_nthread);
    if (conf->work_nthread < 1 ){
        conf->work_nthread = get_nprocs_conf() * 2;
    }

    //[main] -> scanlog_nthread
    conf->scanlog_nthread = 0;
    prof.get<int>("main", "scanlog_nthread", conf->scanlog_nthread);
    if (conf->scanlog_nthread < 1 ){
        conf->scanlog_nthread = get_nprocs_conf() * 2;
    }

    //[main] -> socktimeout
    conf->socktimeout = -1;
    prof.get<int>("main", "socktimeout", conf->socktimeout);
    if (conf->socktimeout < 0){
        conf->socktimeout = 30;
    }

    //scanlog -> working_time
    string lparam;
    if (0 != prof.get("scanlog", "working_time", lparam)){
        LOG_MSG(("get scanlog working_time failed\n"));
        return -1;
    }
    
    if (lparam.length() < 0
    	|| lparam.find_first_not_of("0123456789,") != string::npos){
    	LOG_MSG(("scanlog.working_time error:%s\n",lparam.c_str()));
    	return -1;
    }

    string hour;
    for(int i=0; i<lparam.length(); i++)
    {
        switch(lparam[i]){
        case ',':
        	if(hour.length()> 0){
        		int h = atoi(hour.c_str());
        		if (h >=0 && h<=24){
        		    global::working_time.insert(h);
        		    hour.clear();
        	    }
        	}
            break;
        default:hour.append(1,lparam[i]);
        	break;
        }
    }
    if(hour.length()> 0){
    	int h = atoi(hour.c_str());
    	if (h >=0 && h<=24){
            global::working_time.insert(h);
    	}
    }

    //[scanlog-server]
    char szparam[256];
    if (0 != prof.get("scanlog", "server", lparam)){
        LOG_MSG(("get scanlog->server failed\n"));
        return -1;
    }

    memset(szparam, 0x00,sizeof szparam);
    sscanf(lparam.data(), "%[^:]:%d", szparam, &conf->scanlog_port);
    conf->scanlog_host = szparam;


    //[log] -> filesize
    conf->log_filesize = 0;
    prof.get<long>("log", "filesize", conf->log_filesize);
    
    //[log] -> path
    prof.get("log", "path", conf->log_path);
    if (conf->log_path.length() > 0 && access(conf->log_path.c_str(), F_OK) != 0) {
    	char bufpath[512] = {0};
    	snprintf(bufpath, sizeof(bufpath)-1, "mkdir -p '%s'", conf->log_path.c_str());
    	LOG_MSG(("make log path: [%s]\n", bufpath));
    	system(bufpath);
    }

    //[mysql]
    if (0 != prof.get("mysql", "host", global::mysql_conf.host)
        || 0 != prof.get<int>("mysql", "port", global::mysql_conf.port)
        || 0 != prof.get("mysql", "user", global::mysql_conf.user)
        || 0 != prof.get("mysql", "passwd", global::mysql_conf.passwd)
        || 0 != prof.get("mysql", "db", global::mysql_conf.db)
        || 0 != prof.get("mysql", "charset", global::mysql_conf.charset))
    {
        LOG_MSG(("get mysql config failed\n"));
        return -1;
    }

    return 0;
}

int main(int argc, char * argv[])
{
    if (argc < 2){
        printf("Usage : %s [PROFILE]\n",::basename(argv[0]));
        return -1;
    }

    int handle = -1;
    if (load_configure(argv[1], &global::configure) != 0) {
        return -1;
    }

    ::signal(SIGPIPE, SIG_IGN);
    ::signal(SIGCLD, SIG_IGN);

    handle = mksocket(global::configure.bind.c_str(), global::configure.port);
    if (handle < 1){
        goto __main_quit__;
    }
    
    ::daemon(1, 0);
    Log::instance()->set_path(global::configure.log_path.c_str());
    Log::instance()->set_maxfilesize(global::configure.log_filesize);

    LOG_MSG(("startup => bind:%s port:%d\n",global::configure.bind.c_str(), global::configure.port));

    if (0 != mysql_library_init(0, NULL, NULL)){
        LOG_MSG(( "mysql_library_init failed\n"));
        goto __main_quit__;
    }

    init_mysql_queue(global::mysql_queue, &global::mysql_conf, global::configure.mysql_queue_size);

    global::work_thread_queue.start(global::configure.work_nthread);
    global::scanlog_thread_queue.start(global::configure.scanlog_nthread);

    LOG_MSG(("work thread queue: %d, scanlog thread queue:%d\n",
        global::configure.work_nthread,global::configure.scanlog_nthread));

    epoll_engine(handle);

    global::work_thread_queue.stop();
    global::scanlog_thread_queue.stop();
    global::work_thread_queue.wait();
    global::scanlog_thread_queue.wait();

__main_quit__:
    free_mysql_queue(global::mysql_queue);
    mysql_server_end();
    close(handle);
    return 0;
}

