#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "socketopt.h"

void sock_nonblock(int sockfd)
{
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    if (!(flags & O_NONBLOCK)){
        flags |= O_NONBLOCK;
        ::fcntl(sockfd, F_SETFL, flags);
    }

    flags = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,&flags, sizeof(flags));
}

int listen_start(const char* local, int port)
{
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);

    if(sockfd == -1){
        return -1;
    }

    sock_nonblock(sockfd);

    sockaddr_in host;
    bzero(&host, sizeof(host));
    host.sin_family = PF_INET;
    host.sin_port = htons(port);
    host.sin_addr.s_addr = inet_addr(local);

    if (bind(sockfd, (sockaddr *) &host, sizeof(sockaddr)) == -1){
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, 128) == -1){
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int sock_nbconnect(const char* svr, int port, unsigned int timeout)
{
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
        return -1;

    sockaddr_in host;
    bzero(&host, sizeof(host));
    host.sin_family = PF_INET;
    host.sin_port = htons(port);
    host.sin_addr.s_addr = inet_addr(svr);
    
    if (0 != nbconnect(sockfd, (sockaddr*)&host, sizeof(host), timeout)){
        close(sockfd);
        sockfd = -1;
    }

    return  sockfd;
}

int nbconnect(int sockfd, const sockaddr *serv_addr, socklen_t addrlen, unsigned int timeout)
{
    sock_nonblock(sockfd);

    switch (::connect(sockfd, serv_addr, addrlen ))
    {
    case -1:{
        if (errno != EINPROGRESS)
            return -1; 

        fd_set wrds;
        FD_ZERO(&wrds);
        FD_SET(sockfd,&wrds);
        timeval ttl = {timeout, 0};

        if (select(sockfd+1,0,&wrds,0,&ttl) < 1)
            return -1;

        if (!FD_ISSET(sockfd, &wrds))
            return -1;

        int error = 0;
        socklen_t len = sizeof(error);
        int code = getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&error,&len);

        if (code < 0 || error != 0)
            return -1;
        break;}
    default:
        break;
    }

    return 0;
}

int recv_n( int fd, void* buf, int len, int* transferred)
{
    int offset = 0;
    transferred ? *transferred=0 : 0;

    while (offset < len)
    {
        int bytes = ::recv(fd, (char*)buf+offset, len-offset, 0);

        switch (bytes){
        case -1:
            if (errno==EINTR)
                break;
            else if (errno==EAGAIN)
                return offset;
        case 0:
            return bytes;

        default:
            offset += bytes;
            transferred ? *transferred=offset : 0;
            break;
        }
    }

    return offset;
}

int send_n( int fd, void* buf, int len, int* transferred)
{
    int offset = 0;
    transferred ? *transferred=0 : 0;

    while (offset < len)
    {
        int bytes = ::send(fd, (char*)buf+offset, len-offset, 0);

        switch (bytes){
        case -1:
            if (errno==EINTR)
                break;
            else if (errno==EAGAIN)
                return offset;
        case 0:
            return bytes;
        default:
            offset += bytes;
            transferred ? *transferred=offset : 0;
            break;
        }
    }

    return 0;
}

int send_until_edge( int fd, const void* buf, int len, int timeout )
{
    timeval tv = {timeout, 0};
    int transferred = 0;
    while (len > transferred )
    {
        int bytes = ::send(fd,
            (char*)buf + transferred,
            len - transferred,0);

        switch (bytes){
        case -1:
            if (errno==EINTR)
                break;
            else if(errno==EAGAIN){
                if (timeout > 0){
                    fd_set wrds;
                    FD_ZERO(&wrds);
                    FD_SET(fd,&wrds);
                    if (select(fd + 1, NULL, &wrds, NULL, &tv) > 0 )
                        break;
                }
                return transferred;
            }
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

