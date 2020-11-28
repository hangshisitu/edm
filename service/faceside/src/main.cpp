#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <strings.h>
//
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
//
#include <string.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <string>
#include <fstream>
#include "config.h"
#include "socketopt.h"
#include "protocol.h"
#include "http_handler.h"
#include "task_handler.h"
#include "smtp_handler.h"
#include "syncqueue.h"
#include "pthreads.h"
#include "filesystem.h"
#include "log_msg.h"

using namespace std;

static SyncQueue<string> *pq1=0;
static SyncQueue<string> *pq2=0;

typedef struct {
    int n;
    time_t t;
}retries_t;

static void dw_smart_move(int flag, const char* source)
{
    tm mt;
    time_t tt = time(0);
    localtime_r(&tt, &mt);

    char subpath[32];
    snprintf(subpath, sizeof(subpath), 
        "/%04d%02d%02d/", 
        mt.tm_year+1900,mt.tm_mon+1,mt.tm_mday);

    string dst = (flag==SMTP_OK ? DW::backup_path:DW::fail_path);
    dst += subpath;
    mkdir(dst.c_str(), S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    dst += basename(source);
    rename(source, dst.c_str());

    return;
}

static FILE* FILE_pointer_take(FILE*& p, char* file, unsigned int size, time_t& t)
{
    time_t m = time(0);

    if (p == NULL || m-t > DW::report_interval){

        if (file && file[0]){
            string product = file;
            rename(file, (product + ".side").c_str() );
        }

        if (p){fclose(p);}

        snprintf(file, size, "%s/%u-%u", 
            DW::report_path.c_str(), 
            (unsigned int)m, 
            (unsigned int)pthread_self());
        
        t = m;
        p = fopen(file, "a");

        if (!p){
            LOG_MSG((LM_ERROR, "fopen error(%s), %s/%u-%u\n", 
                strerror(errno),
                DW::report_path.c_str(),
                (unsigned int)m, 
                (unsigned int)pthread_self()));
        }
    }

    return p;
}

static void fput_report_file(FILE*& out, int flag, const char* source)
{    
    int corp_id=0, task_id=0, temp_id=0;

    char* sub = (char*)strrchr(source, '-');

    if (!sub||sscanf(sub, "-%d.%d.%d", &corp_id,&task_id,&temp_id) != 3){
        LOG_MSG((LM_ERROR, "nnd, sscanf '%s'\n", source));
        return;
    }

    string rcpt;
    ifstream fs(source, ios_base::in);

    while (!fs.eof()) {
        getline(fs,rcpt,'\n');
        if ( strncasecmp(rcpt.c_str(), "RCPT TO:", 8) != 0 ){
            rcpt.clear();
        } else{
            rcpt = rcpt.substr(8);
            TRIMSTR(rcpt, "\t <>\r\n");
            break;
        }
    }

    fs.close();

    if (rcpt.length() < 1){
        LOG_MSG((LM_ERROR, "Not found 'RCPT TO:' in '%s'\n", source));
        //return;
    }

    if (out){
        fprintf(out, "%d.%d.%d|%s|%s\n", 
            corp_id,task_id,temp_id, rcpt.c_str(), SMTP_ERR_MSG(flag));
        fflush(out);
    }

    //non-trigger-plan
    if (task_id<100000000 || task_id>999999999)
        temp_id = 1;

    //[08:32:11] [192.168.21.66.02603][REPORT][s][1.3738.1020]cfyx01@126.com(ok)
    LOG_MSG((LM_INFO, "[REPORT][%c][%d.%d.%d]%s(%s)\n", 
        flag==SMTP_OK?'s':'f', 
        corp_id,task_id,temp_id, 
        rcpt.c_str(), 
        SMTP_ERR_MSG(flag)));

    return;
}

static void* get_modula_queue(void *arg)
{
    arg = NULL;
    string src;
    char prefix[64];

    while (pq1 == NULL)
        sleep(3);

    while ( pq1 && !pq1->get(src) )
    {
        if (access(src.c_str(), R_OK) != 0){
            LOG_MSG((LM_ERROR, "Can not read '%s'\n",src.c_str()));
            continue;
        }

        taskinf_t task;
        file_get_taskinf(src.c_str(), &task);

        timeval tv = {0,0};
        gettimeofday(&tv, NULL);

        snprintf(prefix, sizeof(prefix),"/%u%u-%u-", 
            (unsigned int)tv.tv_sec, 
            (unsigned int)tv.tv_usec,
            (unsigned int)pthread_self()); 

        string dst = DW::task_path + prefix + task.id;
        if ( 0 != taskinf_dump(&task, dst.c_str()))
            continue;

        pq2->put(dst);
        LOG_MSG((LM_INFO, "queue2.put '%s' %d\n",dst.c_str(),pq2->size()));
    }

    return PVOID_OK;
}

static void* post_email_queue(void *arg)
{
    arg = NULL;
    string src;
    int retrycnt = (int)DW::smtp_fail_retry;
    int interval = DW::smtp_retry_interval;
    map<string, retries_t> retry;
    map<string, retries_t>::iterator i;

    while (pq2 == NULL)
        sleep(3);

    char rfname[256] = {0};
    FILE* rfd = NULL;
    time_t rtime = time(0);

    do {
        src.clear();
        if (-1 == pq2->get(src, 5)) //-1 -2
            break;
            
        FILE* p = FILE_pointer_take(rfd, rfname, sizeof(rfname), rtime);

        if (src.length() > 0){

            int xflag = smtp_post(src.c_str(), 0);

            if (xflag != SMTP_RETRY){
                fput_report_file(p, xflag, src.c_str());
                dw_smart_move(xflag, src.c_str());
            }
            else if (DW::smtp_fail_retry > 0){
                retries_t r = {1, time(0)};
                retry[src] = r;
            }
        }
        
        time_t end = time(0);

        for (i=retry.begin(); i!=retry.end(); ){

            int xflag = 9999999;

            if (i->second.n > retrycnt ){
                xflag = SMTP_NRETRY;
            }
            else if (end-i->second.t > interval){
                xflag = smtp_post(i->first.c_str(),i->second.n);
            }
            else {
                ++i; 
                continue;
            }

            if (xflag != SMTP_RETRY){
                fput_report_file(p, xflag, i->first.c_str());
                dw_smart_move(xflag, i->first.c_str());
                retry.erase(i++);
            } 
            else {
                i->second.n++;
                i->second.t = end;
                ++i;
            }
        }

        sleep(DW::smtp_thread_send_interval);
    }
    while (pq2);

    return PVOID_OK;
}

static void epoll_accept(int listener, int kdpfd)
{
    sockaddr_in local;
    socklen_t addrlen = sizeof(local);

    for(;;) {
        memset(&local, 0x00, sizeof(local));

        int client = ::accept(listener,(sockaddr*)&local, &addrlen);
        if (client == -1){
            if ( errno != EAGAIN){
                LOG_MSG((LM_ERROR, "accept: %s\n", strerror(errno)));
            }
            return;
        }

        LOG_MSG((LM_INFO, "accepted %s:%d\n", 
            inet_ntoa(local.sin_addr), 
            ntohs(local.sin_port)));

        sockbuf_t* psb = new(std::nothrow) sockbuf_t;
        if ( !psb ){
            close(client);
            continue;
        }

        psb->sockfd = client;
        psb->buf = NULL;
        psb->size = MY_BUFSIZE;
        psb->offset = 0;
        sock_nonblock(psb->sockfd);

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.ptr = (void*)psb;

        if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, psb->sockfd, &ev) < 0) {
            LOG_MSG((LM_ERROR, "epoll_ctl:%s\n", strerror(errno)));
            sockbuf_destroy(psb);
            return;
        }
    }
    return;
}

static int epoll_input(void* param, int kdpfd)
{
    sockbuf_t* psb = (sockbuf_t*)param;

    if ( !psb->buf ) {
        psb->buf = new(std::nothrow) char[psb->size];
        if ( !psb->buf )
            return -1;
    }

    int transferred = 0;
    int src_offset = psb->offset;
    int bytes = recv_n(psb->sockfd, psb->buf + psb->offset, psb->size - psb->offset, &transferred);

    psb->offset += transferred;
    char *pCRLF = (char*)memchr(psb->buf + src_offset, '\n', psb->offset - src_offset);

    if ( !pCRLF ) {
        return (psb->offset+1 >= psb->size || bytes == 0) ? -1:0; 
    }

    int http_return_code = 404;
    *pCRLF = '\0';
    http_request_t request;

    LOG_MSG((LM_INFO,"request '%s'\n", psb->buf));

    do{
        if (parse_http_request_line(psb->buf, &request) != 0){
            LOG_MSG((LM_ERROR, "request.invalid\n"));
            break;
        }

        if (request.method != HTTP_GET){
            LOG_MSG((LM_ERROR, "request.mothod != GET\n"));
            break;
        }

        if (0 != strncasecmp("/?filePath=", request.url.c_str(), 11)){
            LOG_MSG((LM_ERROR, "request.path error\n"));
            break;
        }
        
        http_return_code = 200;
        string dst = DW::web_path + request.url.substr(11);
        pq1->put(dst);
        LOG_MSG((LM_INFO, "queue1.put '%s' %d\n", dst.c_str(), pq1->size() ));
    } while (0);

    tm gmt;
    time_t tt = time(0);
    gmtime_r(&tt, &gmt);

    bytes = snprintf( psb->buf, psb->size,
        "HTTP/1.1 %s\r\n"
        "Server: mtarget-side/1.1\r\n",
        http_return_code==200 ? "200 ":"404 ");

    bytes += strftime( psb->buf+bytes, psb->size-bytes,
        "Date: %a, %d %b %Y %H:%M:%S %Z\r\n"
        "Connection: close\r\n\r\n\0",
        &gmt);

    psb->offset = psb->size - bytes;
    memmove(psb->buf + psb->offset , psb->buf, bytes);

    struct epoll_event ev;
    ev.events = EPOLLOUT | EPOLLET;
    ev.data.ptr = (void*)psb;
    return epoll_ctl(kdpfd, EPOLL_CTL_MOD, psb->sockfd, &ev);
}

static int epoll_output(void * param, int kdpfd)
{
    sockbuf_t* psb = (sockbuf_t*)param;

    while (psb->offset < psb->size)
    {
        int bytes = ::send(psb->sockfd, 
            psb->buf + psb->offset, 
            psb->size - psb->offset, 0);

        switch (bytes){
        case -1:
            if (errno==EINTR)
                break;
            else if (errno==EAGAIN)
                return 0;
        case 0:
            return -1;
        default:
            psb->offset += bytes;
            break;
        }
    }

    kdpfd = 0;
    return -1; //normal close
}

static int epoll_start(const char* host, int port)
{
    int listener = listen_start(host, port);
    if (listener == -1) {
        LOG_MSG((LM_ERROR, "epoll_start: %s\n", strerror(errno)));
        return listener;
    }

    int kdpfd = ::epoll_create(1024);

    struct epoll_event evs[1024];
    struct epoll_event evt;
    evt.events = EPOLLIN|EPOLLET;
    evt.data.ptr = (void*)&listener;

    epoll_ctl(kdpfd, EPOLL_CTL_ADD, listener, &evt);

    LOG_MSG((LM_INFO, "wait for an I/O event on epoll\n"));

    while ( true ){

        int nfds = epoll_wait(kdpfd, evs, 1024, 60*1000);
        if (nfds==-1 && errno!=EINTR)
            break;

        for (int i = 0; i < nfds; i++) {

            int errval = 0;

            if (evs[i].data.ptr == (void*)&listener)
                epoll_accept(listener, kdpfd);

            else if (evs[i].events & EPOLLERR 
                || evs[i].events & EPOLLHUP)
                errval = -1;

            else if(evs[i].events & EPOLLIN)
                errval = epoll_input(evs[i].data.ptr, kdpfd);

            else if (evs[i].events & EPOLLOUT)
                errval = epoll_output(evs[i].data.ptr, kdpfd);

            if ( errval != 0)
                sockbuf_destroy((sockbuf_t*)evs[i].data.ptr);
        }
    }

    close(kdpfd);
    close(listener);
    return 0;
}

void usage(const char* name)
{
    printf("Usage: %s -c=<file> -r -h\n"
           "Options:\n"
           " -c=<file> : set configuration file.\n"
           " -r        : recovery mode.\n"
           " -h        : print this help and exit.\n"
           ,name);
}

static int recovery = 0;
static char* conf = NULL;

int main(int argc, char* argv[])
{
    for (int a=1; a<argc; a++){
        if (!strncmp(argv[a],"-c=", 3))
            conf = argv[a] + 3;
        else if (!strcmp(argv[a], "-r"))
            recovery = 1;
        else if (!strcmp(argv[a], "-h")){
            usage(basename(argv[0]));
            exit(0);
        }
    }

    if (!conf){
        usage(basename(argv[0]));
        exit(0);
    }

    if (0 != ken_config(conf))
        return -1;

    system(("mkdir -p " 
        + DW::task_path + " " 
        + DW::backup_path + " " 
        + DW::fail_path).c_str());

    daemon(1, 0);

    Logger::instance()->open(
        DW::log_file.c_str(), 
        0xFFFFFFFF /*, DW::log_size*/);

    LOG_MSG((LM_INFO, 
        "%s-4acda1e9b7b18322ac401218ddcc31c7\n", 
        basename(argv[0])));

    Pthreads p1, p2;
    p1.start(DW::msg_threads_num);
    p2.start(DW::smtp_threads_num);

    for (int i=0; i<(int)DW::msg_threads_num; i++)
        p1.run(get_modula_queue, NULL);

    for (int i=0; i<(int)DW::smtp_threads_num; i++)
        p2.run(post_email_queue, NULL);

    pq1 = new(std::nothrow) SyncQueue<string>(DW::msg_queue_size);
    pq2 = new(std::nothrow) SyncQueue<string>(DW::smtp_queue_size);

    if (recovery){
        list<string> ls;
        getfiles(DW::task_path.c_str(), ls);
        list<string>::iterator i;
        for (i=ls.begin(); i!=ls.end(); ++i)
            pq2 && pq2->put(DW::task_path + *i);
    }

    epoll_start(DW::listen_address.c_str(), DW::listen_port);

    p1.stop();
    p2.stop();
    pq1->deactivate();
    pq2->deactivate();
    p1.wait();
    p2.wait();
    return 0;
}
