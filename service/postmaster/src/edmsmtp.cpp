/* author: D.Wei 2014.03 Richinfo */
#include <vector>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>//

#include <string.h>

#include "profile.h"
#include "guard_ptr.h"
#include "edmsmtp.h"

using namespace std;

namespace global
{
    int nprocs = get_nprocs_conf();
    string bind = "0.0.0.0";
    unsigned int port = 12580;
    int socktimeout = 60;
    int checkperiod = 5;
    string logfile = "./logs/app.log";
    char seque[32] = {0};
    bool terminal = false;
    unsigned int use_count = 0;
    int twid;
    vector<tw_part_t> twps;
    storage_t emlfilter;
};

void log_write(const char* format,...)
{
    if (global::logfile.length() < 1) 
        return;

    tm ss;
    timeval tv = {0,0};
    gettimeofday(&tv, NULL);
    localtime_r( &tv.tv_sec, &ss );

    char prifix[64];
    snprintf(prifix, sizeof(prifix), 
        "[%04d-%02d-%02d %02d:%02d:%02d.%06d][%ld] ", 
        ss.tm_year + 1900,ss.tm_mon + 1,ss.tm_mday,ss.
        tm_hour,ss.tm_min,ss.tm_sec,(int)tv.tv_usec,
        (long int)getpid());

    va_list vptr;
    va_start(vptr, format);
    printf(prifix);
    vprintf(format,vptr);
    va_end(vptr);
    fflush(0);
    return;
}

bool check_deleted(int fd)
{
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/fd/%d", getpid(), fd);

    char buf[512]; buf[0] = '\0';
    int retval = readlink(path, buf, sizeof(buf)-1);

    if (retval > 10) {
        buf[retval] = '\0';
        if (strncasecmp(buf+retval-10," (deleted)", 10) == 0)
            return true;
    }

    return false;
}

inline tw_part_t* get_twps_ptr(int id)
{
    return (id >= 0 
        && (size_t)id < global::twps.size()) 
        ? &global::twps[id] : NULL;
}

inline int twps_next_id(int twid)
{
    return (twid + 1) % global::twps.size();
}

inline int twps_prev_id(int twid)
{
    int previd = (twid - 1) % global::twps.size();

    if (previd < 0)
        previd += global::twps.size();

    return previd;
}

int tw_insert(tw_node_t* node,  tw_part_t* twp)
{
    if (!node ||!twp)
        return -1;

    switch(twp->size)
    {
    case 0:
        twp->head_ptr = twp->tail_ptr = node;
        node->prev_ptr = node->next_ptr = NULL;
        break;

    default:
        if (node == twp->tail_ptr){
            return -1;
        }

        node->prev_ptr = twp->tail_ptr;
        node->next_ptr = NULL;
        twp->tail_ptr->next_ptr = node;
        twp->tail_ptr = node;
        break;
    }

    twp->size++;
    return 0;
}

int tw_remove(tw_node_t* node, tw_part_t* twp)
{
    if (!node || !twp|| twp->size < 1)
        return -1;

    if (twp->size == 1
        && node == twp->head_ptr
        && node == twp->tail_ptr){
        twp->head_ptr = twp->tail_ptr = NULL;
    }
    else if (node == twp->head_ptr){
        twp->head_ptr = node->next_ptr;
        twp->head_ptr->prev_ptr = NULL;
    }
    else if (node == twp->tail_ptr){
        twp->tail_ptr = node->prev_ptr;
        twp->tail_ptr->next_ptr = NULL;
    }
    else{
        node->prev_ptr->next_ptr = node->next_ptr;
        node->next_ptr->prev_ptr = node->prev_ptr;
    }

    node->next_ptr = node->prev_ptr = NULL;
    twp->size--;
    return 0;
}

int tw_rset(tw_node_t* node)
{
    if (!node)
        return -1;

    int twid = twps_prev_id(global::twid);

    if (twid != node->twid){

        if (node->twid != -1){
            tw_remove(node, get_twps_ptr(node->twid));
        }

        node->twid = twid;
        tw_insert(node, get_twps_ptr(node->twid));
    }

    return 0;
}

int clean_edmsmtp_context(edmsmtp_ctx_t* ctx)
{
    if (!ctx)
        return -1;

    LOG_MSG(("(seq %d) closing connection\n\n", ctx->serial));

    tw_remove(ctx, get_twps_ptr(ctx->twid));

    if (ctx->sockfd != -1){
        close(ctx->sockfd);
        ctx->sockfd = -1;
    }
    
    if (ctx->fdev != dw_null){
        dw_fclose(ctx->fdev);
        ctx->fdev = dw_null;
    }

    if (ctx->rdbuf.buffer){
        delete [] ctx->rdbuf.buffer;
        ctx->rdbuf.buffer = NULL;
    }

    if (ctx->wtbuf.buffer){
        delete [] ctx->wtbuf.buffer;
        ctx->wtbuf.buffer = NULL;
    }

    delete ctx;
    global::use_count>0 ? global::use_count-- : 0;
    return 0;
}

int tw_clear(tw_part_t* twp)
{
    if (!twp)
        return -1;

    int size = 0;
    tw_node_t* ptr = twp->head_ptr;

    while(ptr){
        size++;
        tw_node_t *next = ptr->next_ptr;
        clean_edmsmtp_context(ptr);
        ptr = next;
    }

    twp->size = 0;
    twp->head_ptr = twp->tail_ptr = NULL;
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

int check_mailbox_syntax(const char* mail)
{
    const char* flag = NULL;
    const char* p = mail;
    while (isalnum(*p) || *p=='.' || *p=='_' || *p=='-') ++p;
    //while (isprint(*p) && *p!=' ' && *p!='@') ++p;

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

void sock_nonblock(int sockfd)
{
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    ::fcntl(sockfd, F_SETFL, flags);

    flags = 1;

    setsockopt(sockfd, 
        SOL_SOCKET, 
        SO_REUSEADDR,
        &flags, 
        sizeof(flags));

    return;
}

int set_edmsmtp_response(edmsmtp_ctx_t *ctx,  
                         const char* msg, 
                         int flag = EPOLL_CTL_MOD)
{
    int len = strlen(msg);

    if (len > ctx->wtbuf.length){
        ctx->wtbuf.length = 
            RWBUF_SIZE > len ? RWBUF_SIZE : len;

        if (ctx->wtbuf.buffer != NULL)
            delete ctx->wtbuf.buffer;

        ctx->wtbuf.buffer = 
            new(std::nothrow) char[ctx->wtbuf.length + 1];
    }

    if (ctx->wtbuf.buffer == NULL){
        LOG_MSG(("(seq %d)error.new char[%d] (wbuf)\n", 
            ctx->serial, ctx->wtbuf.length + 1));
        return -1;
    }

    ctx->wtbuf.offset = ctx->wtbuf.length - len;
    memcpy(ctx->wtbuf.buffer+ctx->wtbuf.offset, msg, len);

    ctx->wtbuf.buffer[ctx->wtbuf.length] = 0;

    LOG_MSG(("(seq %d) <-%s", 
        ctx->serial, 
        ctx->wtbuf.buffer+ctx->wtbuf.offset));

    epoll_event evt;
    evt.events = EPOLLOUT|EPOLLET;
    evt.data.ptr = ctx;

    if (0 != epoll_ctl(ctx->epolfd, flag, ctx->sockfd, &evt)) {
        LOG_MSG(("(seq %d) error.epoll_ctl.epollin: %s\n", 
            ctx->serial,strerror(errno)));
        return -1;
    }

    return 0;
}

int where_mailbox(const char* keyword, int keylen, 
                  const char* ptr,     int len, 
                  string& mailbox)
{
    if (len < keylen || 
        strncasecmp(ptr,keyword, keylen) != 0){
        return -1;
    }

    ptr += keylen;
    len -= keylen; 

    while(len>0 && isblank(*ptr) ) { 
        ptr++; len--;
    }

    if (len<1 && *ptr!=':' )
        return -1;

    ptr++;len--;

    while(len>0 && isblank(*ptr) ) {
        ptr++; len--;
    }

    if ( *ptr == '<' ){
        ptr++;len--;
        while(len>0 && isblank(*ptr) ) {
            ptr++; len--;
        }
    }

    if (len < 1 )
        return -1;

    mailbox.assign(ptr, len);
    mailbox.erase(mailbox.find_last_not_of(" \t>")+1);
    return check_mailbox_syntax(mailbox.c_str());
}

int check_storage_address(unsigned long int address, int mask)
{
    std::map<unsigned long int, int>::iterator itptr;
    itptr = global::emlfilter.address.find(address);

    if ( itptr == global::emlfilter.address.end() )
        itptr = global::emlfilter.address.find(DEFAULT_ADDRESS);

    return (itptr!=global::emlfilter.address.end() 
        && itptr->second&mask) ? 0:-1;
}

int check_storage_domain(string& mailbox, int mask)
{
    std::map<string, int>::iterator itptr;
    itptr = global::emlfilter.domain.find( mailbox.substr(mailbox.find("@") + 1) );

    if ( itptr == global::emlfilter.domain.end() )
        itptr = global::emlfilter.domain.find(DEFAULT_DOMAIN);

    return ( itptr!=global::emlfilter.domain.end() 
        && itptr->second&mask ) ? 0:-1;
}

int check_storage_mailbox(string& mailbox, int mask)
{
    std::map<string, int>::iterator itptr;
    itptr = global::emlfilter.mailbox.find(mailbox);

    if ( itptr == global::emlfilter.mailbox.end() )
        itptr = global::emlfilter.mailbox.find(DEFAULT_MAILBOX);

    return ( itptr!=global::emlfilter.mailbox.end() 
        && itptr->second&mask ) ? 0:-1;
}

int is_storage_filter(edmsmtp_ctx_t* ctx)
{
    if ( global::emlfilter.filter.empty())
        return -1;

    return (check_storage_address(ctx->address, 1) == 0
        || check_storage_domain(ctx->cmds.mail, 1) == 0
        || check_storage_domain(ctx->cmds.rcpt, 2) == 0
        || check_storage_mailbox(ctx->cmds.mail, 1) == 0
        || check_storage_mailbox(ctx->cmds.rcpt, 2) == 0 ) ? 0:-1;
}

int start_storage(edmsmtp_ctx_t* ctx)
{
    if (ctx->fdev != dw_null){
        dw_fclose(ctx->fdev);
        ctx->fdev = dw_null;
    }

    int mode = is_storage_filter(ctx) == 0 ? 
        0x01 : (!global::emlfilter.backup.empty() ? 
        0x02 : 0x00);

    if ( mode == 0x00 )
        return -1;

    tm lt;
    time_t tt = time(0);
    localtime_r(&tt, &lt);

    static char path[64] = {0};
    static int sequeue = 0;
    sequeue = (sequeue % 999999) + 1;

    ctx->cmds.emlpath = 
        mode==0x01 ? global::emlfilter.filter:global::emlfilter.backup;

    snprintf(path, sizeof(path), "%04d%02d%02d/",
        lt.tm_year+1900,lt.tm_mon+1,lt.tm_mday);

    ctx->cmds.emlpath += path;
    mkdir(ctx->cmds.emlpath.c_str(), S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);

    snprintf(path, sizeof(path) , 
        "H%02d%02d%02d.P%05d.S%06d.e",
        lt.tm_hour,lt.tm_min,lt.tm_sec, getpid(), sequeue);

    ctx->cmds.emlpath += path;
    LOG_MSG(("(seq %d) (%s: %sml)\n", 
        ctx->serial, mode==0x01?"filter":"storage", ctx->cmds.emlpath.c_str()));

    ctx->fdev = dw_fopen(ctx->cmds.emlpath.c_str(), RW_FLAGS);

    if (ctx->fdev == dw_null ){
        LOG_MSG(("(seq %d) error.open '%s': %s)\n", 
            ctx->serial, ctx->cmds.emlpath.c_str(), strerror(errno)));
    }
    else {

        dw_fchmod(ctx->fdev, ctx->cmds.emlpath.c_str(), S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);

        string header = "Received: from " + ctx->cmds.helo +  "[";
        in_addr inaddr = {ctx->address};
        header += inet_ntoa(inaddr);
        header += ("] (account " + ctx->cmds.mail + ")\r\n");
        header += "\tby M.Target with SMTP for " + ctx->cmds.rcpt+";\r\n";
        strftime( path, sizeof(path), "\t%a, %d %b %Y %H:%M:%S %z (%Z)\r\n", &lt);
        header += path;

        dw_write(ctx->fdev, header.data(), header.size());
    }

    return 0;
}

int handler_request_command(edmsmtp_ctx_t *ctx, const char* buf, int len)
{
    const char* ptr = buf;

    switch (ctx->forword){

    case FW_AUTH:
        ctx->cmds.user.assign(ptr,len);
        ctx->forword = FW_PASS;
        return set_edmsmtp_response(ctx, ESMTP_P334);

    case FW_PASS:
        ctx->cmds.pass.assign(ptr,len);
        ctx->forword = FW_HELO|FW_EHLO|FW_AUTH|FW_MAIL|FW_RSET|FW_NOOP|FW_QUIT;
        return set_edmsmtp_response(ctx, ESMTP_235);

    default:
        break;
    }

    while (len>0 && isblank(*ptr)){
        ptr++;
        len--;
    }

    if (len < 1) {
        return 0;
    }else if (len < 4) {
        return set_edmsmtp_response(ctx, ESMTP_502);
    }

    if (strncasecmp(ptr,"HELO",4) == 0)
        ctx->status = FW_HELO;
    else if (strncasecmp(ptr,"EHLO",4) == 0)
        ctx->status = FW_EHLO;
    else if (strncasecmp(ptr,"EHLO",4) == 0)
        ctx->status = FW_EHLO;
    else if (strncasecmp(ptr,"AUTH",4) == 0)
        ctx->status = FW_AUTH;
    else if (strncasecmp(ptr,"MAIL",4) == 0)
        ctx->status = FW_MAIL;
    else if (strncasecmp(ptr,"RCPT",4) == 0)
        ctx->status = FW_RCPT;
    else if (strncasecmp(ptr,"DATA",4) == 0)
        ctx->status = FW_DATA;
    else if (strncasecmp(ptr,"RSET",4) == 0)
        ctx->status = FW_RSET;
    else if (strncasecmp(ptr,"NOOP",4) == 0)
        ctx->status = FW_NOOP;
    else if (strncasecmp(ptr,"QUIT",4) == 0)
        ctx->status = FW_QUIT;
    else{
        set_edmsmtp_response(ctx,ESMTP_502);
        return -1;
    }

    ptr += 4;
    len -= 4;

    if (len>0 && !isblank(*ptr)){
       set_edmsmtp_response(ctx, ESMTP_502);
       return -1;
    }

    while (len>0 && isblank(*ptr)){
        ptr++;
        len--;
    }

    switch(ctx->forword & ctx->status)
    {
    case FW_HELO:
    case FW_EHLO:
        if (len < 1){
            set_edmsmtp_response(ctx,ESMTP_H501);
            return -1;
        }

        ctx->cmds.helo.assign(ptr, len);
        ctx->forword = FW_HELO|FW_EHLO|FW_AUTH|FW_MAIL|FW_RSET|FW_NOOP|FW_QUIT;
        set_edmsmtp_response(ctx,ESMTP_250);
        break;

    case FW_AUTH:
        if (len<5 || strncasecmp(ptr,"LOGIN", 5)!=0){
            return set_edmsmtp_response(ctx, ESMTP_501);
        }

        for (int i=5; i<len ;i++ ){
            if (!isblank(ptr[i]) ){
                return set_edmsmtp_response(ctx, ESMTP_501);
            }
        }

        ctx->forword = FW_AUTH;
        set_edmsmtp_response(ctx, ESMTP_U334);
        break;

    case FW_MAIL:
        if ( 0 != where_mailbox("FROM", 4, ptr, len, ctx->cmds.mail)){
            return set_edmsmtp_response(ctx, ESMTP_M501);
        }

        std::transform(ctx->cmds.mail.begin(),
            ctx->cmds.mail.end(),
            ctx->cmds.mail.begin(),
            (int(*)(int))tolower);
        ctx->forword = FW_HELO|FW_EHLO|FW_MAIL|FW_RCPT|FW_RSET|FW_NOOP|FW_QUIT;
        set_edmsmtp_response(ctx, ESMTP_250);
        break;

    case FW_RCPT:
        if ( 0 != where_mailbox("TO", 2, ptr, len, ctx->cmds.rcpt)){
            return set_edmsmtp_response(ctx, ESMTP_R501);
        }

        std::transform(ctx->cmds.rcpt.begin(),
            ctx->cmds.rcpt.end(),
            ctx->cmds.rcpt.begin(),
            (int(*)(int))tolower);

        ctx->forword = FW_HELO|FW_EHLO|FW_MAIL|FW_RCPT|FW_DATA|FW_RSET|FW_NOOP|FW_QUIT;
        set_edmsmtp_response(ctx, ESMTP_250);
        break;

    case FW_DATA:
        start_storage(ctx);
        ctx->follow = EDMSMTP_HEADER;
        ctx->forword = FW_DATA;
        ctx->cmds.size = 0;
        set_edmsmtp_response(ctx, ESMTP_354);
    break;

    case FW_RSET:
        ctx->cmds.user.clear();
        ctx->cmds.pass.clear();
        ctx->cmds.mail.clear();
        ctx->cmds.rcpt.clear();
        ctx->forword = FW_HELO|FW_EHLO|FW_AUTH|FW_MAIL|FW_RSET|FW_NOOP|FW_QUIT;
        set_edmsmtp_response(ctx, ESMTP_250);
        break;

    case FW_NOOP:
        set_edmsmtp_response(ctx, ESMTP_250);
        break;

    case FW_QUIT:
        ctx->forword = FW_EXIT;
        set_edmsmtp_response(ctx,ESMTP_221);
        break;

    default:
        set_edmsmtp_response(ctx,ESMTP_503);
        break;
    }

    return 0;
}

int handler_request_data(edmsmtp_ctx_t *ctx, const char* buf, int len)
{
    int flags = ctx->follow;

    switch(len)
    {
    case 1:
        if (buf[0] == '\n'&& flags == EDMSMTP_HEADER)
            flags = EDMSMTP_BODY;
        break;

    case 2:
        if (buf[0] == '\r'&& buf[1] == '\n'&& flags == EDMSMTP_HEADER)
            flags = EDMSMTP_BODY;
        else if (buf[0] == '.'&& buf[1] == '\n')
            flags = EDMSMTP_END;
        break;

    case 3:
        if (buf[0] == '.'&& buf[1] == '\r'&& buf[2] == '\n')
            flags = EDMSMTP_END;
        break;

    default:
        break;
    }

    switch (flags)
    {
    case EDMSMTP_END:
        LOG_MSG(("(seq %d) (data size: %d bytes)\n", ctx->serial,ctx->cmds.size));

        if (ctx->fdev != dw_null){
            dw_fclose(ctx->fdev);
            ctx->fdev = dw_null;
            ::rename(ctx->cmds.emlpath.c_str(), (ctx->cmds.emlpath + "ml").c_str());
        }

        ctx->rdbuf.offset = 0;
        ctx->follow = EDMSMTP_END;
        ctx->forword = FW_HELO|FW_EHLO|FW_MAIL|FW_RSET|FW_NOOP|FW_QUIT;
        set_edmsmtp_response(ctx, ESMTP_250);
        break;

    case EDMSMTP_HEADER:
        //break;
    case EDMSMTP_BODY:
        if ( flags==EDMSMTP_BODY && ctx->follow==EDMSMTP_HEADER)
            ctx->follow = EDMSMTP_BODY;
        //break;
    default:
        ctx->cmds.size += len;
        if (ctx->fdev != dw_null)
            dw_write(ctx->fdev, buf, len);
        break;
    }

    return 0;
}

int make_sock_acceptor(const char* local, int port)
{
    int handle = socket(PF_INET, SOCK_STREAM, 0);

    if(handle == -1){
        perror("socket");
        return -1;
    }

    sock_nonblock(handle);

    sockaddr_in host;
    bzero(&host, sizeof(host));
    host.sin_family = PF_INET;
    host.sin_port = htons(port);
    host.sin_addr.s_addr = inet_addr(local);//INADDR_ANY;

    if (bind(handle, (sockaddr *) &host, sizeof(sockaddr)) == -1){
        close(handle); perror("bind");
        return -1;
    }

    if (listen(handle, 32) == -1){
        close(handle); perror("listen");
        return -1;
    }

    return handle;
}

int clean_sockrd_buffer(int fd)
{
    int transferred = 0;
    char buf [256];

    while(1)
    {
        int bytes = recv(fd, buf, sizeof(buf), 0);

        switch(bytes)
        {
        case -1:
            if (errno == EINTR)
                break;
            else if (errno==EAGAIN || errno==EWOULDBLOCK)
                return 0;

            LOG_MSG(("error.recv2:%s\n", strerror(errno)));
            return -1;

        case 0:
            LOG_MSG(("Client closed2 connection\n"));
            return -1;

        default:
            transferred += bytes;
            break;
        }
    }

    if (transferred > 0){
        LOG_MSG(("clean socket buffer: %d bytes\n", transferred));
    }

    return 0;
}

int recv_n( int fd, void* buf, int len, int* transferred)
{
    int offset = 0;

    transferred ? *transferred=0 : true;

    while (offset < len)
    {
        int bytes = ::recv(fd, (char*)buf+offset, len-offset, 0);

        switch (bytes)
        {
        case -1:
            if (errno==EINTR)
                break;
            else if (errno==EAGAIN
                || errno==EWOULDBLOCK)
                return offset;

            LOG_MSG(("error.recv: %s\n", strerror(errno)));
            return -1;

        case 0:
            LOG_MSG(("Client closed connection.\n"));
            return 0;

        default:
            offset += bytes;
            transferred ? *transferred=offset : true;
            break;
        }
    }

    return offset;
}

int epoll_input(epoll_event *evt)
{
    edmsmtp_ctx_t* ctx = (edmsmtp_ctx_t*)evt->data.ptr;

    do {

        if (ctx->rdbuf.offset >= ctx->rdbuf.length ){
            LOG_MSG(("(seq %d) error.buffer overflow, %u bytes\n", 
                ctx->serial, ctx->rdbuf.offset));
            return -1;
        }

        int offset = 0;
        int transferred = 0;

        int retval = recv_n( ctx->sockfd,
                            ctx->rdbuf.buffer + ctx->rdbuf.offset,
                            ctx->rdbuf.length - ctx->rdbuf.offset,
                            &transferred);

        if (retval < 1){
            return (errno==EAGAIN && ctx->forword!=FW_EXIT ) ? 0 : -1;
        }

        ctx->rdbuf.offset += transferred;

        while (offset < ctx->rdbuf.offset)
        {
            char* ptr = (char*)memchr(ctx->rdbuf.buffer + offset,
                                      '\n', ctx->rdbuf.offset - offset);

            if (!ptr) 
                break;

            switch(ctx->forword)
            {
            case FW_EXIT:
                return -1;

            case FW_DATA:
                handler_request_data(ctx, 
                    ctx->rdbuf.buffer + offset, 
                    ptr-ctx->rdbuf.buffer-offset+1);

                offset = (ptr - ctx->rdbuf.buffer + 1);
                break;

            default:
                *ptr = '\0';
                while(ptr > ctx->rdbuf.buffer+offset) {
                    char* n = ptr - 1;
                    if ( *n!='\r' && !isblank(*n) && *n!='\n' )
                        break;
                    *ptr-- = '\0';
                }

                LOG_MSG(("(seq %d) ->%s\n", 
                    ctx->serial, (ctx->forword==FW_PASS) ? 
                    "(password hidden)":ctx->rdbuf.buffer+offset));

                handler_request_command(ctx, 
                    ctx->rdbuf.buffer+offset, 
                    ptr-ctx->rdbuf.buffer-offset);

                ctx->rdbuf.offset = 0;
                break;
            }
        }

        if (ctx->wtbuf.length > 0 && ctx->wtbuf.offset < ctx->wtbuf.length) {
            //clean_sockrd_buffer(ctx->sockfd);
            return 0;
        }

        if (ctx->rdbuf.offset > offset){

            memmove(ctx->rdbuf.buffer,
                    ctx->rdbuf.buffer + offset,
                    ctx->rdbuf.offset-offset);
            ctx->rdbuf.offset -= offset;
        }
        else {
            ctx->rdbuf.offset = 0;
        }
    }while(1);

    return 0;
}

int epoll_output(epoll_event * evt)
{
    edmsmtp_ctx_t* ctx = (edmsmtp_ctx_t*)evt->data.ptr;

    while(ctx->wtbuf.offset < ctx->wtbuf.length)
    {
        int bytes = send(ctx->sockfd,
                         (char*)ctx->wtbuf.buffer + ctx->wtbuf.offset,
                         ctx->wtbuf.length - ctx->wtbuf.offset, 0);

        switch(bytes)
        {
        case -1:
            if ( errno == EINTR)
                break;
            else if (errno==EAGAIN || errno==EWOULDBLOCK)
                return 0;

        case 0:
            LOG_MSG(("(seq %d) error.send:%s\n", ctx->serial, strerror(errno)));
            return -1;

        default:
            ctx->wtbuf.offset += bytes;
            LOG_DEBUG(("%s: seq %d, twid %d, bytes %d\n",
                __FUNCTION__, ctx->serial, ctx->twid, bytes));
            break;
        }
    }

    if (ctx->forword == FW_EXIT){
        return -1;
    }

    epoll_event nevt;
    nevt.events = EPOLLIN|EPOLLET;
    nevt.data.ptr = ctx;
    if (0 != epoll_ctl(ctx->epolfd, EPOLL_CTL_MOD, ctx->sockfd, &nevt)) {
        LOG_MSG(("(seq %d) error.epoll_ctl.epollin:%s\n", ctx->serial, strerror(errno)));
        return -1;
    }
    return 0;
}

inline int get_next_serial()
{
    static int serial = 0;
    return (serial = (serial + 1) % 100000000);
}

edmsmtp_ctx_t* Create_edmsmtp_context()
{
    edmsmtp_ctx_t* ctx = new(std::nothrow) edmsmtp_ctx_t;
    if (ctx == NULL)
        return NULL;

    ctx->twid = -1;
    ctx->actime = time(0);
    ctx->status = FW_NULL;
    ctx->forword = FW_HELO|FW_EHLO|FW_RSET|FW_NOOP|FW_QUIT;
    ctx->rdbuf.offset = 0;
    ctx->rdbuf.length = RWBUF_SIZE;
    ctx->rdbuf.buffer = new(std::nothrow) char[RWBUF_SIZE];

    if (!ctx->rdbuf.buffer){
        delete ctx;
        return NULL;
    }

    memset(&ctx->wtbuf, 0x00, sizeof(ctx->wtbuf));
    tw_rset(ctx);
    global::use_count++;
    return ctx;
}

void epoll_accept(int acceptor, int epfd)
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);

    do {
        memset(&addr, 0x00, sizeof(addr));

        int sockfd = ::accept(acceptor,(sockaddr *)&addr, &len);
        if (sockfd == -1)
            return;

        sock_nonblock(sockfd);
        int serial = get_next_serial();

        LOG_MSG(("(seq %d) accept connection %s:%d\n",
            serial, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)));

        edmsmtp_ctx_t* ctx = Create_edmsmtp_context();

        if (ctx == NULL){
            LOG_MSG(("(seq %d) Allocate memory fails at %s\n", serial, __FUNCTION__));
            close(sockfd);
            continue;
        }

        ctx->fdev = dw_null;
        ctx->serial = serial;
        ctx->sockfd = sockfd;
        ctx->epolfd = epfd;
        ctx->address = addr.sin_addr.s_addr;

        set_edmsmtp_response(ctx, ESMTP_220, EPOLL_CTL_ADD);
    } while (1);

    return;
}

int fork_process(int handle)
{
    int epfd = ::epoll_create(EPOLL_SIZE);

    epoll_event evs[EPOLL_SIZE];
    epoll_event evt;
    evt.events = EPOLLIN|EPOLLET;
    evt.data.ptr = (void*)handle;
    epoll_ctl(epfd, EPOLL_CTL_ADD, handle, &evt);

    int timeout = global::checkperiod;
    time_t tvstart = time(0);

    global::twid = 0;
    global::use_count = 0;
    for(int i=0; i<global::socktimeout/global::checkperiod + 1; i++)
    {
        tw_part_t twp;
        memset(&twp, 0x00, sizeof(twp));
        global::twps.push_back(twp);
    }

    while ( !global::terminal )
    {
        int rd = epoll_wait(epfd, evs, EPOLL_SIZE, timeout*1000);

        if (rd == -1){

            LOG_MSG(("error.epoll_wait: %s\n",strerror(errno)));

            if (errno != EINTR)
                break;
        }
        else if (rd == 0)
        {
            LOG_DEBUG(("no socket became ready during %u seconds, "
                       "user count %d\n", timeout, global::use_count));

            if (global::use_count < 1 ){
                if (timeout < 3600*12)
                    timeout *= 2;
            }

            tvstart = 0;
            goto check_timeout;
        }

        timeout = global::checkperiod;
        LOG_DEBUG(("user count %d\n", global::use_count));

        for (int i = 0; i < rd; i++)
        {
            int retval = -999;
            if (evs[i].data.ptr == (void*)handle){
                epoll_accept(handle,epfd);
            }
            else if (evs[i].events & EPOLLERR
                || evs[i].events & EPOLLHUP){
                retval = -1;
            }
            else if(evs[i].events & EPOLLIN){
                retval = epoll_input(&evs[i]); // -1 or 0
            }
            else if (evs[i].events & EPOLLOUT){
                retval = epoll_output(&evs[i]); // -1 or 0
            }

            edmsmtp_ctx_t* ctx = (edmsmtp_ctx_t*)(evs[i].data.ptr);
            switch(retval)
            {
            case 0: tw_rset(ctx);
                break;
            case -1: clean_edmsmtp_context(ctx);
                break;
            default:
                break;
            }
        }

check_timeout:
        if (time(0) - tvstart > global::checkperiod){
            tvstart = time(0);
            tw_clear(get_twps_ptr(global::twid));
            global::twid = twps_next_id(global::twid);
        }
    }

    close(epfd);
    return 0;
}

void signal_handle(int sign)
{
    switch (sign)
    {
    case SIGTERM:
        global::terminal = true;
        LOG_MSG(("Kill - terminate process\n"));
        break;

    case SIGUSR1:
        if (global::logfile.length() > 0 && check_deleted(0))
        {
            int fd = open(global::logfile.c_str(), 
                O_APPEND|O_CREAT|O_RDWR,
                S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);

            if (fd > 0){
                dup2(fd, 0);dup2(fd, 1);dup2(fd, 2);
                close(fd);
            }
        }
        LOG_MSG(("signal: check logger\n"));
        break;

    default:
        break;
    }
    return;
}

int dw_storage_inf(key_map_t& target, std::map<string, int>& filter)
{
    key_map_t::iterator iter;

    for (iter=target.begin(); iter!=target.end(); ++iter)
    {
        if (iter->second.back().length() != 1 
            || iter->second.back().find_first_not_of("0123") != string::npos)
            continue;

        string zcvf = iter->first;
        std::transform(zcvf.begin(),zcvf.end(),zcvf.begin(),(int(*)(int))tolower);
        filter[zcvf] = atoi(iter->second.back().c_str());
    }

    return 0;
}

int dw_get_options(const char* filename)
{
    CProfile prof;

    if (0 != prof.open(filename)){
        return -1;
    }

    string cfval;

    /* define/nprocs */
    prof.get<int>("define", "nprocs", global::nprocs);

    if (global::nprocs < 1 ){
        global::nprocs = get_nprocs_conf();
    }

    /* define/bind */
    prof.get("define", "bind", global::bind);

    if (check_inet_addr(global::bind.c_str()) != 0){
        printf("error [define].bind\n");
        return -1;
    }

    /* define/port */
    prof.get("define", "port", cfval);

    if (cfval.length() > 0 && cfval.length() < 6){

        if( cfval.find_first_not_of("0123456789") != string::npos){
            printf("error [define].port\n");
            return -1;
        }

        global::port = atoi(cfval.c_str());
    }

    if (global::port < 1 || global::port > 0xffff){
        printf("error [define].port\n");
        return -1;
    }

    /* define/socktimout */
    if (0 == prof.get("define", "socktimeout", cfval)){

        if (cfval.find_first_not_of("0123456789") != string::npos){
            printf("error [define].socktimeout\n");
            return -1;
        }

        global::socktimeout = atoi(cfval.c_str());
    }

    /* define/checkperiod */
    if (0 == prof.get("define", "checkperiod", cfval))
    {
        if (cfval.find_first_not_of("0123456789") != string::npos){
            printf("error [define].checkperiod\n");
            return -1;
        }

        global::checkperiod = atoi(cfval.c_str());
    }

    if (global::socktimeout < 1 || global::checkperiod < 1
        || global::checkperiod > global::socktimeout){

        printf("require: socktimeout > 0 "
               "and checkperiod > 0 "
               "and checkperiod(%d) <= socktimeout(%d)\n",
               global::checkperiod, global::socktimeout);

        return -1;
    }

    /* define/log */
    prof.get("define", "log", global::logfile);

    if ( global::logfile.length() > 0){

        char* dirc = strdup(global::logfile.c_str());
        char* basec = strdup(global::logfile.c_str());

        AutoFree df(dirc);AutoFree bf(basec);
        char* dname = dirname(dirc);
        char* bname = basename(basec);

        struct stat sb;
        stat(dname, &sb);

        if (!S_ISDIR(sb.st_mode)){
            printf("error [define].log\n");
            return -1;
        }

        global::logfile = dname;
        global::logfile += "/";
        global::logfile += bname;
    }

    /* storage/backup */
    prof.get("storage", "backup", global::emlfilter.backup);

    struct stat sb;
    stat(global::emlfilter.backup.c_str(), &sb);
    if (!S_ISDIR(sb.st_mode))
        global::emlfilter.backup.clear();

    if (global::emlfilter.backup.length() > 0
        && *global::emlfilter.backup.rbegin() != '/'){
        global::emlfilter.backup += "/";
    }

    /* storage/filter */
    prof.get("storage", "filter", global::emlfilter.filter);

    do {
        stat(global::emlfilter.filter.c_str(), &sb);
        if (!S_ISDIR(sb.st_mode)){
            global::emlfilter.filter.clear();
            break;
        }

        if ( *global::emlfilter.filter.rbegin() != '/'){
            global::emlfilter.filter += "/";
        }

        key_map_t target;
        key_map_t::iterator iter;

        prof.get_pair("filter/domain", target);
        dw_storage_inf(target, global::emlfilter.domain);

        target.clear();
        prof.get_pair("filter/mailbox", target);
        dw_storage_inf(target, global::emlfilter.mailbox);

        target.clear();
        prof.get_pair("filter/address", target);
        for (iter=target.begin(); iter!=target.end(); ++iter)
        {
            if (iter->second.back().length() != 1 
                || iter->second.back().find_first_not_of("0123") != string::npos)
                continue;

            if (iter->first == "default"){
                global::emlfilter.address[DEFAULT_ADDRESS] = atoi(iter->second.back().c_str());
            } else {
                in_addr ipv4;
                if (inet_aton(iter->first.c_str(),&ipv4)) {
                    global::emlfilter.address[ipv4.s_addr] = atoi(iter->second.back().c_str());
                }
            }
        }
    }
    while (0);

    return 0;
}

void logger_start(int i)
{
    snprintf(global::seque,sizeof(global::seque), "%02d", i + 1);
    string fullpath = "/dev/null";

    if (global::logfile.length() > 0){
        global::logfile += ".";
        global::logfile += global::seque;
        fullpath = global::logfile;
    }

    int fd = open(fullpath.c_str(),
        O_APPEND|O_CREAT|O_RDWR,
        S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);

    if (fd > 0){
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
    }
}

int main(int argc, char * argv[])
{
    string file = "./etc/define.conf";

    switch(argc){
    case 1: break;
    case 2: file = argv[1];
        break;
    default: 
        printf("Usage : %s [CONFIG-FILE] "
               "(default:%s)\n\n", 
               basename(argv[0]), file.c_str());
        return -1;
    }

    if ( dw_get_options(file.c_str()) != 0){
        printf("Don't Keng Die!\n\n");
        return -1;
    }

    int sockfd = make_sock_acceptor(global::bind.c_str(), global::port);
    if ( sockfd < 1){
        return -1;
    }

    ::signal(SIGPIPE, SIG_IGN);
    ::signal(SIGCLD, SIG_IGN);
    ::signal(SIGUSR1, signal_handle); /* 日志处理信号 */
    ::signal(SIGTERM, signal_handle); /* 关闭信号 */

    int numofprocs = 0;
    for (int i=0; i<global::nprocs; i++)
    {
        pid_t pid;
        if((pid = ::fork()) < 0){
            perror("fork failed");
            continue;
        }

        if (pid == 0) {
            ::setsid();
            //::umask(0);
            logger_start(i);
            LOG_MSG(("%s start\n", basename(argv[0])));
            fork_process(sockfd);
            goto forkend;
        }

        numofprocs++;
    }

    printf("worker processes:%d, listening %s:%u\n",
        numofprocs, global::bind.c_str(),global::port);

forkend:
    close(sockfd);
    return 0;
}

