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
#include "deliverhelper.h"
#include "profile.h"
#include "log.h"
#include <limits>
#include <sstream>
#include <algorithm>
#include <vector>
#include "berkeleydb.h"

#define VERSION  "1.0.1"

namespace global
{
    string                       bind;
    int                          port = 0;
    int                          socktimeout = 60;
    int                          work_nthread = 0;
    long                         log_size = 0;
    bool                         thread_running = true;
    string                       log_path;
    mysql_inf_t                  mysql_conf;
    std::vector<pthread_t>       tids;
    Sync_Queue<task_context_t*>  task_queue(5120);
    bdb_inf_t                    bdbinf;
    BerkeleyDB                   bdb_report;
    BerkeleyDB                   bdb_status;
    map<string,int>              tbquantity;
    SyncLock                     tbqlock;
};

void cleanup_task_queue()
{
    task_context_t* tctx;
    while (global::task_queue.get(tctx, 0) != -1){
        LOG_MSG(("cleanup %s:%d\n",tctx->host.c_str(), tctx->port));
        cleanup_task_context(tctx, true);
    }
}

void cleanup_task_context(task_context_t* tctx, bool deleted)
{
    if ( !tctx )
        return;

    tctx->serial = 0;
    tctx->jrtime = 0;
    if (tctx->sockfd){
        close(tctx->sockfd);
    }

    tctx->epfd = -1;
    tctx->port = 0;
    tctx->host.clear();
    tctx->recvbuf.clear();
    tctx->sendbuf.length = 0;
    tctx->sendbuf.transferred = 0;

    if (tctx->sendbuf.data){
        delete tctx->sendbuf.data;
        tctx->sendbuf.data = NULL;
    }

    tctx->request.method = HTTP_NULL;
    tctx->request.url.clear();
    tctx->request.version.clear();
    tctx->action.type = 0;
    tctx->mysqldb = NULL;
    tctx->synlock.UnLock();

    if (deleted){
        delete tctx;
    }
}

int parse_http_request_line(const char* line, http_request_t* req)
{
    const char* url = NULL;

    if (strncasecmp(line, "GET", 3) == 0)
    {
        req->method = HTTP_GET;
        url = line + 3;
    }
    else if (strncasecmp(line, "POST", 4) == 0)
    {
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
    if ( !version ){
        version = (char*)strchr(url, '\t');
    }
    if ( !version ){
        return -1;
    }

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
    while ( *next != '\0' && *next!='\r' && *next!='\n')
    {
        if (!isblank(*next++))
            return -1;
    }

    return 0;
}

int parse_task_action(const char* url, task_action_t* ta)
{
    if ( !url || !ta )
        return -1;

    const char* param = NULL;
    if (strncasecmp(url, "/do?", 4) == 0){
        param = url + 4;
        ta->type = TA_DO;
    }
    else
        return -1;

    string field,value;
    string* target = &field;
    do
    {
        switch(*param)
        {
        case '=':target = &value;
            break;
        case '&':
        case '\0':
            if (field.length() > 0){
                std::transform(field.begin(),field.end(),field.begin(),(int(*)(int))tolower);
                ta->param[field] = value;
                //LOG_MSG(("[%s] = [%s]\n",field.c_str(), value.c_str()));
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
            //LOG_MSG(("error.recv:%s\n", strerror(errno)));
            return -1;
        case 0:
            return length>remaining ? length-remaining:-1;
        default:
            remaining -= bytes;
            result.append(buffer,bytes);
            break;
        }
    }
    return length - remaining;
}

int send_until_edge( int handle, const void* buffer, int length, int timeout )
{
    if (handle < 1|| !buffer || length < 0)
        return -1;

    timeval tv = {timeout, 0};
    int transferred = 0;
    while (length > transferred )
    {
        int bytes = ::send(handle,(char*)buffer + transferred,length - transferred,0);
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
                return transferred;
            }
            //LOG_MSG(("error.send:%s\n", strerror(errno)));
            return -1;
        case 0:
            return transferred>0?transferred:-1;
        default:
            transferred += bytes;
            break;
        }
    }
    return transferred;
}

int query_context_result(MYSQL* mysql, const char* sqlstr, task_context_t* tctx)
{
    LOG_MSG(("sql:[%s]\n",sqlstr));

    if (0 != mysql_query(mysql, sqlstr)) {
        LOG_MSG(("mysql_query failed:%s\n",mysql_error(mysql)));
        return HTTP_503;
    }

    MYSQL_ROW row;
    MYSQL_RES *result = mysql_store_result(mysql);
    int num_rows = mysql_num_rows(result);
    LOG_MSG(("mysql_num_rows: %d\n",num_rows));

    if (num_rows < 1){
        mysql_free_result(result);
        return HTTP_404;
    }

    int default_row_len = 80;
    int buf_len = 256 + num_rows * default_row_len;
    tctx->sendbuf.length = 0;
    tctx->sendbuf.transferred = 0;
    tctx->sendbuf.data = new(std::nothrow) char[buf_len];
    if (tctx->sendbuf.data == NULL){
        LOG_MSG(("new(std::nothrow) failed\n"));
        mysql_free_result(result);
        return HTTP_503;
    }

    int bytes = snprintf(tctx->sendbuf.data, buf_len, 
        "HTTP/1.1 200 OK\r\n"
        "Server: Deliverhelper/%s\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: $$$$$$$$$$$$\r\n"
        "Connection: close\r\n"
        "\r\n",VERSION);

    tctx->sendbuf.length += bytes;
    int ctx_len = 0;
    int remaining_rows = num_rows;

    while (row = mysql_fetch_row(result)) 
    {
        unsigned long row_lengths = mysql_fetch_lengths(result)[0] + 2;
        if (row_lengths > (buf_len - tctx->sendbuf.length))
        {
            buf_len = buf_len + remaining_rows * row_lengths * 2;
            char* newbuffer = new(std::nothrow) char[buf_len];
            memmove(newbuffer, tctx->sendbuf.data,tctx->sendbuf.length);
            delete tctx->sendbuf.data;
            tctx->sendbuf.data = newbuffer;
            LOG_MSG(("renew(std::nothrow): %d bytes.\n", buf_len));
        }

        int bytes = snprintf(tctx->sendbuf.data + tctx->sendbuf.length, buf_len - tctx->sendbuf.length, "%s\r\n", row[0]);

        ctx_len += bytes;
        tctx->sendbuf.length += bytes;
        remaining_rows--;
    }

    char* pflag = (char*)strchr(tctx->sendbuf.data, '$');
    pflag += snprintf(pflag, 11, "%u",ctx_len);
    while (*pflag == '\0' || *pflag == '$') {
        *pflag = ' ';
        pflag++;
    }

    LOG_MSG(("response.Content-Length: %d\n",ctx_len));
    mysql_free_result(result);
    return HTTP_200;
}

void* thread_http_process(void* param)
{
    MYSQL* mysql_instance = NULL;
    make_mysql_instance(mysql_instance);

    while (global::thread_running)
    {
        task_context_t* tctx = NULL;
        if ( 0 != global::task_queue.get(tctx)){
            LOG_MSG(("task queue deactivated\n"));
            break;
        }

        tctx->mysqldb = make_mysql_instance(mysql_instance);;

        int retval = context_process(tctx);
        switch (retval)
        {
        case HTTP_200_NODATA:
        case HTTP_400:
        case HTTP_404:
        case HTTP_405:
        case HTTP_406:
        case HTTP_503:
            if (tctx->sendbuf.data)
                delete tctx->sendbuf.data;
    
            tctx->sendbuf.data = new(std::nothrow) char[256];
            tctx->sendbuf.transferred = 0;
            tctx->sendbuf.length = snprintf(tctx->sendbuf.data, 256, 
                "HTTP/1.1 %s\r\n"
                "Server: Deliverhelper/%s\r\n"
                "Content-Length: 0\r\n"
                "Connection: close\r\n"
                "\r\n", http_status[retval], VERSION);
            //LOG_MSG(("http.response:[%s]\n",http_status[retval]));
            //break;
        case HTTP_200:
            LOG_MSG(("http.response:[%s]\n",http_status[retval]));
            break;
        default:
            LOG_MSG(("http.retval:[%d]\n", retval));
            break;
        }
    
        epoll_event evt;
        evt.events = EPOLLOUT|EPOLLET;
        evt.data.ptr = tctx;
        epoll_ctl(tctx->epfd, EPOLL_CTL_MOD, tctx->sockfd, &evt);

        if (!global::mysql_conf.keepalive){
            mysql_close(mysql_instance);
            delete mysql_instance;
            mysql_instance = NULL;
        }
    }

    if (mysql_instance){
        mysql_close(mysql_instance);
        delete mysql_instance;
    }

    return (void*)0;
}

//OK
int check_mailbox_syntax(const char* mail)
{
    const char* flag = NULL;
    const char* p = mail;

    while (isalnum(*p) 
          || *p=='.' 
          || *p=='_' 
          || *p=='-') 
        ++p;

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

void* bdb_query_report(const void* key, unsigned int kl, const void* data, unsigned int dl, void* param )
{
    if (!param){
        return (void*)-1;
    }

    string line;
    bdb_param_t* bdbpkt = (bdb_param_t*) param;

    int offset = 0;
    char* head = (char*)key; 
    //flag = T1
    if (kl < 3 || head[0] != 'T' || head[1] != '1' || head[2] != '|')
        return (void*)-1;

    //ip
    offset = 3;
    head = head + offset; 
    char* tail = (char*)memchr(head, '|', kl - offset);
    if (!tail){
        return (void*)-1;
    }

    if ( bdbpkt->ip.length() > 0 
        && (tail-head != bdbpkt->ip.length() 
        || strncasecmp(bdbpkt->ip.data(), head, bdbpkt->ip.length()) != 0)){
        return (void*)-1;
    }

    line.append(head, tail - head);
    line.append(",");

    //from
    offset += (tail + 1 - head);
    head = tail + 1;
    tail = (char*)memchr(head, '|', kl - offset);
    if (!tail){
        return (void*)-1;
    }

    if (bdbpkt->from.length() > 0 
        && (tail-head != bdbpkt->from.length() 
        || strncasecmp(bdbpkt->from.data(), head, bdbpkt->from.length()) != 0)){
        return (void*)-1;
    }

    line.append(head, tail - head);
    line.append(",");

    //rcpt
    offset += (tail + 1 - head);
    head = tail + 1;
    tail = (char*)memchr(head, '|', kl - offset);
    if (!tail){
        return (void*)-1;
    }

    if (bdbpkt->rcpt.length() > 0
        && (tail-head != bdbpkt->rcpt.length() 
        || strncasecmp(bdbpkt->rcpt.data(), head, bdbpkt->rcpt.length())) != 0){
        return (void*)-1;
    }

    line.append(head, tail - head);
    line.append(",");

    //date
    offset += (tail + 1 - head);
    head = tail + 1;
    tail = head + kl - offset;

    if (bdbpkt->date.length() > 0){
        int length = kl-offset;
        if (bdbpkt->date.length() < length)
            length = bdbpkt->date.length();

        if (strncasecmp(bdbpkt->date.data(), head, length) != 0)
            return (void*)-1;
    }

    line.append(head, tail - head);
    line.append(",");

    //success,fails
    if (dl != sizeof(unsigned int)*2){
        return (void*)-1;
    }

    unsigned int* psf = (unsigned int*)data;
    char sf[64] = {0};
    snprintf(sf, sizeof(sf), "%u,%u\r\n", psf[0],psf[1]);
    line.append(sf);
    
    bdbpkt->bytes += line.length();
    bdbpkt->linebuf.push_back(line);
    return (void*)0;
}

void* bdb_query_status(const void* key, unsigned int kl, const void* data, unsigned int dl, void* param )
{
    if (!param){
        return (void*)-1;
    }

    //T2|from|rcpt|seconds  chkpoint+numbers
    string from,rcpt,seconds,status;
    bdb_param2_t* bdbpkt = (bdb_param2_t*) param;

    int offset = 0;
    char* head = (char*)key; 
    //flag = T2
    if (kl < 3 || head[0] != 'T' || head[1] != '2' || head[2] != '|')
        return (void*)-1;

    //from
    offset = 3;
    head = head + offset; 
    char* tail = (char*)memchr(head, '|', kl - offset);
    if (!tail){
        return (void*)-1;
    }

    if (bdbpkt->from.length() > 0 
        && (tail-head != bdbpkt->from.length() 
        || strncasecmp(bdbpkt->from.data(), head, bdbpkt->from.length()) != 0)){
        return (void*)-1;
    }

    from.assign(head, tail - head);

    //rcpt
    offset += (tail + 1 - head);
    head = tail + 1;
    tail = (char*)memchr(head, '|', kl - offset);
    if (!tail){
        return (void*)-1;
    }

    if (bdbpkt->rcpt.length() > 0
        && (tail-head != bdbpkt->rcpt.length() 
        || strncasecmp(bdbpkt->rcpt.data(), head, bdbpkt->rcpt.length())) != 0){
        return (void*)-1;
    }

    rcpt.assign(head, tail - head);

    //seconds
    offset += (tail + 1 - head);
    head = tail + 1;
    tail = head + kl - offset;
    seconds.assign(head, tail - head); 

    //chkpoint numbers
    if (dl != sizeof(bdb_quantity_t)){
        return (void*)-1;
    }

    bdb_quantity_t* psf = (bdb_quantity_t*)data;

    string sqlbuf;
    sqlbuf += "select quantity, (if(from_domain = 'ANY', 2, 1)+if(rcpt_domain = 'OTHER', 20, 10)) lv"
              " from deliver_quantity"
              " where from_domain in('";
    sqlbuf += from;
    sqlbuf += "', 'any') and rcpt_domain in('";
    sqlbuf += rcpt;
    sqlbuf += "', 'other')"
              " and quantity > 0"
              " and seconds = ";
    sqlbuf += seconds;
    sqlbuf += " order by lv limit 1";

    list<vector<string> > records;
    if (0 != mysql_common_query(bdbpkt->mysql, sqlbuf.c_str(), records)){
        return 0;
    }

    if (records.size() < 1){
        return 0;
    }

    records.begin()->at(0);
    int quantity = atoi(records.begin()->at(0).c_str());

    char buf[128] = {0};
    snprintf(buf,sizeof(buf)-1, "%d/%s,", quantity, seconds.c_str());
    string line = from + "," + rcpt + ",";
    line += buf;

    status = psf->numbers > quantity ? "stop":"normal";
    if (bdbpkt->status.length() > 0
        && strcasecmp(bdbpkt->status.c_str(), status.c_str()) !=0){
        return 0;
    }

    line.append(status +"\r\n");
    bdbpkt->bytes += line.length();
    bdbpkt->linebuf.push_back(line);
    return (void*)0;

}


int try2forget_1001 (string &ip,task_context_t* tctx)
{
    char sqlbuf[256];
    if (tctx->action.param.size() != 2){
        LOG_MSG(("error.there is too much field in request-line\n"));
        return HTTP_400;
    }

    if ( check_inet_addr(ip.c_str()) != 0 ){
        LOG_MSG(("error:ip(%s) invalid\n", ip.c_str()));
        return HTTP_400;
    }

    snprintf(sqlbuf, sizeof(sqlbuf)-1,
        " select concat(domain,\",\",ability,\",\",dimension) "
        " from deliver_ability "
        " where ip = '%s'",
        ip.c_str());

    return query_context_result(tctx->mysqldb, sqlbuf, tctx);
}

int try2forget_1002 (string &ip, task_context_t* tctx)
{
    char sqlbuf[256];
    if (tctx->action.param.size() != 2){
        LOG_MSG(("error.there is too much field in request-line\n"));
        return HTTP_400;
    }

    if ( check_inet_addr(ip.c_str()) != 0 ){
        LOG_MSG(("error:ip(%s) invalid\n", ip.c_str()));
        return HTTP_400;
    }
    
    snprintf(sqlbuf, sizeof(sqlbuf)-1,
        " select concat(domain,\",\",transport) "
        " from deliver_transport "
        " where ip = '%s'",
        ip.c_str());

    return query_context_result(tctx->mysqldb, sqlbuf, tctx);
}

int try2forget_1003 (string &ip, task_context_t* tctx)
{
    char sqlbuf[256];
    if (tctx->action.param.size() != 2){
        LOG_MSG(("error.there is too much field in request-line\n"));
        return HTTP_400;
    }

    if ( check_inet_addr(ip.c_str()) != 0 ){
        LOG_MSG(("error:ip(%s) invalid\n", ip.c_str()));
        return HTTP_400;
    }

    snprintf(sqlbuf, sizeof(sqlbuf)-1,
        " select count(sent_by) from "
        " from deliver_setting "
        " where ip = '%s' and sent_by = 1",
        ip.c_str());

    return query_context_result(tctx->mysqldb, sqlbuf, tctx);
}

int try2forget_2001 (string &ip, task_context_t* tctx)
{
    if ( check_inet_addr(ip.c_str()) != 0 ){
        LOG_MSG(("error:ip(%s) invalid\n", ip.c_str()));
        return HTTP_400;
    }

    string from = tctx->action.param["from"];
    if (check_mailbox_syntax(("user@" + from).c_str()) != 0){
        LOG_MSG(("error:2001.from='%s' invalid.\n", from.c_str()));
        return HTTP_400;
    }
    
    string rcpt = tctx->action.param["rcpt"];
    if (check_mailbox_syntax(("user@" + rcpt).c_str()) != 0){
        LOG_MSG(("error:2001.rcpt='%s' invalid.\n", rcpt.c_str()));
        return HTTP_400;
    }

    string status = tctx->action.param["status"];
    
    if (status != "s" && status != "f"){
        LOG_MSG(("error:status='%s' invalid.\n", status.c_str()));
        return HTTP_400;
    }
    
    if (0 != insert_detail_records(ip, from, rcpt,status)){
        return HTTP_503;
    }
    
    add_deliver_quantity(tctx->mysqldb, from.c_str(), rcpt.c_str(), 1);
    return HTTP_200_NODATA;
}

int try2forget_2002 (string&ip, task_context_t* tctx)
{
    bdb_param_t bdbparam;
    bdbparam.ip    = ip;
    bdbparam.bytes = 0;

    if (ip.length()> 0 && check_inet_addr(ip.c_str()) != 0 ){
        LOG_MSG(("error:ip(%s) invalid\n", ip.c_str()));
        return HTTP_400;
    }

    map<string, string>::iterator itparam;

    string field = "from";
    itparam = tctx->action.param.find(field);
    if (itparam != tctx->action.param.end())
    {
        if (check_mailbox_syntax(("user@" + itparam->second).c_str()) != 0){
            LOG_MSG(("error:2002.from='%s' invalid.\n", itparam->second.c_str()));
            return HTTP_404;
        }
    
        bdbparam.from = itparam->second;
    }
    
    field = "rcpt";
    itparam = tctx->action.param.find(field);
    if (itparam != tctx->action.param.end())
    {
        if (check_mailbox_syntax(("user@" + itparam->second).c_str()) != 0){
            LOG_MSG(("error:2002.rcpt='%s' invalid.\n", itparam->second.c_str()));
            return HTTP_404;
        }
    
        bdbparam.rcpt = itparam->second;
    }
    
    field = "date";
    itparam = tctx->action.param.find(field);
    if (itparam != tctx->action.param.end()){
        if (itparam->second.length() > 8 || 
        	itparam->second.find_first_not_of("0123456789") != string::npos){
            LOG_MSG(("error:2002.date='%s' invalid.\n", itparam->second.c_str()));
            return HTTP_400;
        }
    
        bdbparam.date = itparam->second;
    }
    
    LOG_MSG(("2002.bdb.traversal.begin\n"));
    global::bdb_report.cursor_traversal(bdb_query_report, &bdbparam);
    LOG_MSG(("2002.bdb.traversal.end, record count %d.\n", bdbparam.linebuf.size()));
    
    if (bdbparam.linebuf.size() < 1){
        return HTTP_200_NODATA;
    }
    
    tctx->sendbuf.transferred = bdbparam.bytes + 256;
    tctx->sendbuf.data = new(std::nothrow) char[tctx->sendbuf.transferred];
    
    if (tctx->sendbuf.data == NULL){
        LOG_MSG(("%s.%d: new(std::nothrow) fails\n",__FUNCTION__,__LINE__));
        return HTTP_503;
    }
    
    int bytes = snprintf(tctx->sendbuf.data, bdbparam.bytes + 256, 
        "HTTP/1.1 200 OK\r\n"
        "Server: Deliverhelper/%s\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %u\r\n"
        "Connection: close\r\n"
        "\r\n", VERSION, bdbparam.bytes);
    
    tctx->sendbuf.length = bytes;
    
    list<string>::iterator itrar = bdbparam.linebuf.begin();
    for(; itrar!=bdbparam.linebuf.end(); ++itrar)
    {
        memcpy(tctx->sendbuf.data + tctx->sendbuf.length,
               itrar->data(),
               itrar->length());
        tctx->sendbuf.length += itrar->length();
    }
    
    tctx->sendbuf.transferred = 0;
    LOG_MSG(("response.Content-Length: %d\n",bdbparam.bytes));
    return HTTP_200;
}

int try2forget_2003 (string &ip, task_context_t* tctx)
{
    bdb_param2_t bdbparam;
    bdbparam.bytes = 0;
    bdbparam.mysql = tctx->mysqldb;

    map<string, string>::iterator itparam;

    string field = "from";
    itparam = tctx->action.param.find(field);
    if (itparam != tctx->action.param.end())
    {
        if (check_mailbox_syntax(("user@" + itparam->second).c_str()) != 0){
            LOG_MSG(("error:2003.from='%s' invalid.\n", itparam->second.c_str()));
            return HTTP_404;
        }
    
        bdbparam.from = itparam->second;
    }
    
    field = "rcpt";
    itparam = tctx->action.param.find(field);
    if (itparam != tctx->action.param.end())
    {
        if (strcasecmp(itparam->second.c_str(), "other") != 0 
            && check_mailbox_syntax(("user@" + itparam->second).c_str()) != 0){
            LOG_MSG(("error:2003.rcpt='%s' invalid.\n", itparam->second.c_str()));
            return HTTP_404;
        }
    
        bdbparam.rcpt = itparam->second;
    }
    
    field = "status";
    itparam = tctx->action.param.find(field);
    if (itparam != tctx->action.param.end()){
        if (strcasecmp(itparam->second.c_str(), "normal") != 0
            && strcasecmp(itparam->second.c_str(), "stop") != 0){
            LOG_MSG(("error:2003.status='%s' invalid.\n", itparam->second.c_str()));
            return HTTP_400;
        }
    
        bdbparam.status = itparam->second;
    }

    LOG_MSG(("2003.bdb.traversal.begin\n"));
    //load_tbquantity(tctx->mysqldb, global::tbquantity); 
    global::bdb_status.cursor_traversal(bdb_query_status, &bdbparam);
    LOG_MSG(("2003.bdb.traversal.end, record count %d.\n", bdbparam.linebuf.size()));
    
    if (bdbparam.linebuf.size() < 1){
        return HTTP_200_NODATA;
    }
    
    tctx->sendbuf.transferred = bdbparam.bytes + 256;
    tctx->sendbuf.data = new(std::nothrow) char[tctx->sendbuf.transferred];
    
    if (tctx->sendbuf.data == NULL){
        LOG_MSG(("%s.%d: new(std::nothrow) fails\n",__FUNCTION__,__LINE__));
        return HTTP_503;
    }
    
    int bytes = snprintf(tctx->sendbuf.data, bdbparam.bytes + 256, 
        "HTTP/1.1 200 OK\r\n"
        "Server: Deliverhelper/%s\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %u\r\n"
        "Connection: close\r\n"
        "\r\n", VERSION, bdbparam.bytes);
    
    tctx->sendbuf.length = bytes;
    
    list<string>::iterator itrar = bdbparam.linebuf.begin();
    for(; itrar!=bdbparam.linebuf.end(); ++itrar)
    {
        memcpy(tctx->sendbuf.data + tctx->sendbuf.length,
               itrar->data(),
               itrar->length());
        tctx->sendbuf.length += itrar->length();
    }
    
    tctx->sendbuf.transferred = 0;
    LOG_MSG(("response.Content-Length: %d\n",bdbparam.bytes));
    return HTTP_200;
}

int context_process(void* param)
{
    task_context_t* tctx = (task_context_t*)param;

    if (0 != parse_task_action(tctx->request.url.data(), &tctx->action)){
        LOG_MSG(("error:can't parse task action.\n"));
        return HTTP_400;
    }

    string command = tctx->action.param["cmd"];
    if (command.length() < 1){
        LOG_MSG(("error:command length is zero\n"));
        return HTTP_400;
    }
    
    string ip = tctx->action.param["ip"];

    char *endptr = NULL;    
    int nid = strtol(command.c_str(), &endptr, 10);

    if (*endptr != '\0'){
        LOG_MSG(("error.command '%s':%s]\n",command.c_str(),http_status[HTTP_405]));
        return HTTP_405;
    }

    switch (nid)
    {
    case 1001:
        return try2forget_1001(ip, tctx);
    case 1002:
        return try2forget_1002(ip, tctx);
    case 1003:
        return try2forget_1003(ip, tctx);
    case 2001:
        return try2forget_2001(ip, tctx);
    case 2002:
        return try2forget_2002(ip, tctx);
    case 2003:
        return try2forget_2003(ip, tctx);
    default:
        LOG_MSG(("default.error:'%s':%s]\n",command.c_str(),http_status[HTTP_405]));
        return HTTP_405;
    }
}

int mysql_common_query(MYSQL* mysql, const char* sqlbuf, list<vector<string> >& result)
{
    if (!mysql || !sqlbuf){
        return -1;
    }

    result.clear();
    LOG_MSG(("%s.sql:[%s]\n", __FUNCTION__, sqlbuf));

    if (0 != mysql_query(mysql, sqlbuf)) {
        LOG_MSG(("{%s} %s\n", __FUNCTION__, mysql_error(mysql)));
        return -1;
    }

    MYSQL_RES *dbres = mysql_store_result(mysql);

    int rn = mysql_num_rows(dbres);
    LOG_MSG(("mysql_num_rows:%d\n",rn));

    if (rn < 1){
        mysql_free_result(dbres);
        return 0;
    }

    MYSQL_ROW row = NULL;
    unsigned int fieldcount = mysql_num_fields(dbres);
    
    while (row = mysql_fetch_row(dbres))
    {
        vector<string> line;
        for(int i=0; i<fieldcount; i++){
            line.push_back(row[i]);
        }

        result.push_back(line);
    }

    mysql_free_result(dbres);
    return 0;
}

int add_deliver_quantity(MYSQL* mysql, const char* from,const char* rcpt, unsigned int numbers)
{
    string sqlbuf;
    sqlbuf += "select from_domain,rcpt_domain,quantity,seconds from ( "
              " select *, if(from_domain='ANY',2,1)+if(rcpt_domain='OTHER',20,10) lv "
              " from deliver_quantity"
              " where from_domain in('";
    sqlbuf += from;
    sqlbuf += "', 'ANY') and rcpt_domain in('";
    sqlbuf += rcpt;
    sqlbuf += "', 'OTHER')"
              " and quantity > 0"
              " and seconds > 0"
              " order by seconds, lv asc ) t"
              " group by seconds";

    list<vector<string> > records;
    if (0 != mysql_common_query(mysql, sqlbuf.c_str(), records)){
        return 0;
    }

    int retval = 0;
    list<vector<string> >::iterator itrds;
    for(itrds=records.begin(); itrds!=records.end(); ++itrds)
    {  
        string db_rcpt = (*itrds)[1];
        string db_seconds = (*itrds)[3];
        std::transform(db_rcpt.begin(),db_rcpt.end(),db_rcpt.begin(),(int(*)(int))tolower);

        string skey = "T2|";
        skey += from;
        skey += "|";
        skey += db_rcpt;
        skey += "|";
        skey += db_seconds;

        LOG_DEBUG(("%s.skey:%s\n", __FUNCTION__, skey.c_str()));

        bdb_quantity_t data;
        memset(&data, 0x00, sizeof(data));
        if (0 != global::bdb_status.get(skey.c_str(),&data, sizeof(data))){
            data.chkpoint = time(0);
            data.numbers = 0;
        }

        if (difftime(time(0),data.chkpoint) > atoi(db_seconds.c_str())){
            data.numbers = 0;
            data.chkpoint = time(0);
        }

        data.numbers += numbers;
        global::bdb_status.put(skey.c_str(),skey.length(),&data, sizeof(data));

        LOG_MSG(("DOMAIN.mark:%s->%s, %un/%ss\n", from, rcpt, data.numbers, db_seconds.c_str()));
    }

    return retval;

}

int insert_detail_records(string &ip, string& from, string& rcpt, string& status)
{
    unsigned int data[2];
    memset(data, 0x00, sizeof(data));
    
    time_t tt;
    tm ttm;
    time( &tt );
    localtime_r( &tt, &ttm);
    char date[32];
    snprintf(date,sizeof(date)-1, 
        "%04d%02d%02d",//"%02d%02d%02d"
        ttm.tm_year + 1900,ttm.tm_mon + 1,ttm.tm_mday/*,ttm.tm_hour,ttm.tm_min,ttm.tm_sec*/);
    
    string skey = "T1|" + ip + "|" + from + "|" + rcpt + "|";
    skey += date;
    global::bdb_report.get(skey.c_str(),data, sizeof(data));
    data[status=="s" ? 0:1]++;
    int retval = global::bdb_report.put(skey.c_str(),skey.length(),data, sizeof(data));
    
    LOG_MSG(("bdb.detail.put.%s:[key=%s][success=%d][fails=%d]\n",
        retval==0 ? "OK":"ERROR", skey.c_str(), data[0],data[1]));
    return retval;
}

int epoll_accept( int acceptor, int epollfd )
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);

    do {
        memset(&addr, 0x00, sizeof(addr));
        int sockfd = ::accept(acceptor,(sockaddr *)&addr, &len);
        if (sockfd == -1)
        {
            switch(errno)
            {
            case EAGAIN:
            //case EWOULDBLOCK:
                return 0;
            default:
                LOG_MSG(("error.accept:%s\n", strerror(errno)));
                return -1;
            }
        }

        task_context_t* tctx = new(std::nothrow) task_context_t;
        if ( !tctx){
            close(sockfd); 
            LOG_MSG(("allocate dynamic memory fails for:[%s:%d]\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)));
            continue;
        }

        int optval = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&optval, sizeof(optval));
        fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK);

        tctx->jrtime = time(0);
        tctx->serial = 0;
        tctx->sockfd = sockfd;
        tctx->epfd   = epollfd;
        tctx->host   = inet_ntoa(addr.sin_addr);
        tctx->port   = ntohs(addr.sin_port);
        tctx->request.method = HTTP_NULL;
        tctx->sendbuf.data = NULL;
        tctx->mysqldb = NULL;

        LOG_TXT(("\n"));
        LOG_MSG(("accepted:[%s:%d]\n", tctx->host.c_str(), tctx->port));

        epoll_event evt;
        evt.events = EPOLLIN|EPOLLET;
        evt.data.ptr = tctx;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &evt);
    } while (1);

    return 0;
}

int epoll_output(void* param)
{
    task_context_t* tctx = (task_context_t*)param;

    if (tctx->sendbuf.data)
    {
        while (tctx->sendbuf.transferred < tctx->sendbuf.length) 
        {
            int bytes = ::send(tctx->sockfd,
                tctx->sendbuf.data + tctx->sendbuf.transferred, 
                tctx->sendbuf.length - tctx->sendbuf.transferred,
                0);

            switch (bytes)
            {
            case -1:
                if (errno == EINTR)
                    break;
                else if(errno == EAGAIN || errno == EWOULDBLOCK)
                    return 0;
            case 0:
                LOG_MSG(("error.epoll_output:%s\n", strerror(errno)));
                LOG_MSG(("error.response,client:[%s:%d], transferred: %d/%d\n",
                    tctx->host.c_str(), 
                    tctx->port,
                    tctx->sendbuf.transferred,
                    tctx->sendbuf.length));
                LOG_MSG(("closing client %s:%d\n", tctx->host.c_str(), tctx->port));
                cleanup_task_context(tctx);
                return -1;
            default:
                tctx->sendbuf.transferred += bytes;
                break;
            }
        }

        LOG_MSG(("success.response,client:[%s:%d], transferred: %d/%d\n",
            tctx->host.c_str(), 
            tctx->port,
            tctx->sendbuf.transferred, 
            tctx->sendbuf.length));
    }

    LOG_MSG(("closing client %s:%d\n", tctx->host.c_str(), tctx->port));
    cleanup_task_context(tctx);
    return 0;
}

int epoll_input(void* param)
{
    task_context_t* tctx = (task_context_t*)param;

    if (tctx->synlock.TryLock() != 0){
        return 0;
    }

    if (recv_until_edge(tctx->sockfd, tctx->recvbuf, MAX_LENGTH, 0) < 0){
        LOG_MSG(("error:receive fails %s\n", strerror(errno)));
        return -1;
    }

    int crlf = tctx->recvbuf.rfind(S_LF);
    if ( string::npos == crlf ){
        tctx->synlock.UnLock();
        if (tctx->recvbuf.length() > MAX_LENGTH){
            LOG_MSG(("error:request line too large.\n"));
            return -1;
        }

        return 0;
    }

    tctx->recvbuf[crlf] = '\0';
    LOG_MSG(("request =>%s\n", tctx->recvbuf.c_str()));
    tctx->recvbuf[crlf] = C_LF;

    if ( 0 != parse_http_request_line(tctx->recvbuf.data(), &tctx->request)){
        LOG_MSG(("error:can't parse http request message.\n"));
        return -1;
    }

    global::task_queue.put(tctx);
    return 0;
}

#ifndef EPOLL_SIZE
#define EPOLL_SIZE 1024
#endif

int epoll_engine_start(int handle)
{
    epoll_event evs[EPOLL_SIZE];
    epoll_event evt;
    evt.events = EPOLLIN|EPOLLET;
    evt.data.ptr = (void*)handle;

    int timeout = global::socktimeout>0 ? global::socktimeout:60;
    int epollfd = ::epoll_create(EPOLL_SIZE);
    epoll_ctl(epollfd, EPOLL_CTL_ADD, handle, &evt);

    while ( global::thread_running )
    {
        int rd = epoll_wait(epollfd, evs, EPOLL_SIZE, timeout*1000);

        if (rd < 0){
            LOG_MSG(("error.epoll_wait:%s\n",strerror(errno)));
            if (errno != EINTR)
                break;
        }
        else if (rd == 0){
            LOG_MSG(("no file descriptor became ready during %u seconds\n", timeout));
            if (timeout < 3600*12){
                timeout *= 2;
            }
            continue;
        }
        
        timeout = global::socktimeout;
        for (int i=0; i<rd; i++)
        {
            if (evs[i].data.ptr == (void*)handle){
                epoll_accept(handle, epollfd);
            }
            else if(evs[i].events & EPOLLIN){
                if (0 != epoll_input(evs[i].data.ptr)){
                    task_context_t* tctx = (task_context_t*)evs[i].data.ptr;
                    LOG_MSG(("closing client %s:%d\n", tctx->host.c_str(), tctx->port));
                    cleanup_task_context((task_context_t*)evs[i].data.ptr,true);
                }
            }
            else if (evs[i].events & EPOLLOUT){
                epoll_output(evs[i].data.ptr);
            }
        }
    }

    close(epollfd);
    return 0;
}

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

int create_socket( const char* host, int port)
{
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        perror("create socket");
        return -1;
    }

    int mode = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&mode, sizeof(mode));
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK);

    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);
    
    if (addr.sin_addr.s_addr == INADDR_NONE){
        printf("'%s' is a invalid address\n",host);
        close(sockfd);
        return -1;
    }

    if (bind(sockfd, (sockaddr *) &addr, sizeof(sockaddr)) == -1 || listen(sockfd, 128) == -1){
        close(sockfd);
        perror("socket:");
        return -1;
    }

    return sockfd;
}

MYSQL* make_mysql_instance(MYSQL*& mysql)
{
    char value = 1;
    if (mysql)
        return mysql;

    mysql = new MYSQL;
    mysql_init(mysql);
    mysql_options(mysql, MYSQL_SET_CHARSET_NAME, global::mysql_conf.charset.c_str());
#ifdef MYSQL_OPT_RECONNECT
    mysql_options(mysql, MYSQL_OPT_RECONNECT, (char *)&value);
#endif

    if (NULL == mysql_real_connect(mysql,global::mysql_conf.host.c_str(),
        global::mysql_conf.user.c_str(),
        global::mysql_conf.passwd.c_str(),
        global::mysql_conf.dbname.c_str(),
        global::mysql_conf.port,NULL, 0)){
        LOG_MSG(("mysql_real_connect fails:%s\n", mysql_error(mysql)));
        mysql_close(mysql);
        delete mysql; 
        return NULL;
    }
#ifdef MYSQL_OPT_RECONNECT
    mysql_options(mysql, MYSQL_OPT_RECONNECT, (char *)&value);
#endif
    return mysql;
}

void stop_pthread_pool()
{
    global::thread_running = false;
    for (int i=0; i<global::tids.size(); i++){
        pthread_join(global::tids[i], NULL);
    }
    LOG_MSG(("%s exit\n",__FUNCTION__));
}

int start_pthread_pool(int nthreads)
{
    for (int i = 0; i < nthreads; ++i)
    {
        pthread_t pid;
        pthread_attr_t attr = {0};
        pthread_attr_init (&attr);
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);
        switch (pthread_create(&pid, &attr, &thread_http_process, NULL))
        {
        case 0:
            global::tids.push_back(pid);
            break;
        default:
            LOG_MSG(("Failed in pthread_create:%s\n", strerror(errno)));
            break;
        }
    }

    if ( global::tids.size() != nthreads){
          LOG_MSG(("Unable to create enough threads,""need=%d, real=%d\n",nthreads, global::tids.size()));         
    }

    return global::tids.size();
}

int read_configure(const char*file)
{
    CProfile prof;
    if (prof.open(file) != 0){
        printf("load profile fails:%s\n",file);
        return -1;
    }

    if (prof.get("main", "bind", global::bind) != 0){
        global::bind = "0.0.0.0";
    }

    if (check_inet_addr(global::bind.c_str()) != 0){
        printf("'%s' is a invalid address\n",global::bind.c_str());
        return -1;
    }

    prof.get<int>("main", "port", global::port);
    if (global::port < 1 || global::port > 65535){
        printf("'%d' is a invalid port\n",global::port);
        return -1;
    }

    prof.get<int>("main", "work_nthread", global::work_nthread);
    if (global::work_nthread < 1 ){
        global::work_nthread = get_nprocs_conf() * 2;
    }

    prof.get<int>("main", "socktimeout", global::socktimeout);
    prof.get<long>("logfile", "size",global::log_size);
    prof.get("logfile", "path", global::log_path);

    if (0 != prof.get("mysql", "host", global::mysql_conf.host)
        || 0 != prof.get<int>("mysql", "port", global::mysql_conf.port)
        || 0 != prof.get("mysql", "user", global::mysql_conf.user)
        || 0 != prof.get("mysql", "passwd", global::mysql_conf.passwd)
        || 0 != prof.get("mysql", "dbname", global::mysql_conf.dbname)
        || 0 != prof.get("mysql", "charset", global::mysql_conf.charset)
        || 0 != prof.get<int>("mysql", "keepalive", global::mysql_conf.keepalive)
        )
    {
        printf("get mysql config fails\n");
        return -1;
    }
    
    prof.get("berkeleydb", "report_file", global::bdbinf.report_file);
    prof.get("berkeleydb", "status_file", global::bdbinf.status_file);

    global::bdbinf.pagesize = global::bdbinf.cachesize = -1;
    prof.get<unsigned int>("berkeleydb", "pagesize", global::bdbinf.pagesize);
    prof.get<unsigned int>("berkeleydb", "cachesize",global::bdbinf.cachesize);

    return 0;
}

void signal_handler(int sign)
{
    switch (sign)
    {
    case SIGTERM:
        LOG_MSG(("catching the signal: SIGTERM(%d)\n",sign));
        global::thread_running = false;
        global::task_queue.deactivate();
        break;
    default:
        break;
    }
    return;
}

int main(int argc, char * argv[])
{
    if (argc < 2){
        printf("Usage : %s [PROFILE]\n(version:%s)\n\n",::basename(argv[0]), VERSION);
        return -1;
    }

    if (read_configure(argv[1]) != 0) {
        return -1;
    }

    int handle = create_socket(global::bind.c_str(), global::port);
    if (handle < 1){
        return -1;
    }
    
    ::daemon(1, 0);
    ::signal(SIGPIPE, SIG_IGN);
    ::signal(SIGCLD, SIG_IGN);
    ::signal(SIGTERM, signal_handler);

    if (0 != mysql_library_init(0, NULL, NULL)){
        printf( "mysql_library_init fails\n");
        goto quit;
    }

    if (0 != global::bdb_report.open(global::bdbinf.report_file.c_str(),
                     global::bdbinf.pagesize,
                     global::bdbinf.cachesize)){
        printf("berkelay db open fails\n");
        goto quit;
    }

    if (0 != global::bdb_status.open(global::bdbinf.status_file.c_str(),
                     global::bdbinf.pagesize,
                     global::bdbinf.cachesize)){
        printf("berkelay db open fails\n");
        goto quit;
    }

    Log::instance()->set_path(global::log_path.c_str());
    Log::instance()->set_maxfilesize(global::log_size);

    LOG_MSG(("version:%s start => bind %s:%d\n", VERSION, global::bind.c_str(), global::port));

    start_pthread_pool(global::work_nthread);
    epoll_engine_start(handle);
    stop_pthread_pool();
    cleanup_task_queue();
quit:
    mysql_server_end();
    close(handle);
    LOG_MSG(("%s exit\n",argv[0]));
    return 0;
}

