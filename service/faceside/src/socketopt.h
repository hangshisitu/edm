#ifndef _SOCKETOPT_H_
#define _SOCKETOPT_H_

extern void sock_nonblock(int sockfd);
extern int listen_start(const char* local, int port);
extern int sock_nbconnect(const char* svr, int port, unsigned int timeout);
extern int nbconnect(int sockfd, const sockaddr *, socklen_t , unsigned int );
extern int recv_n(int fd, void* buf, int len, int* );
extern int send_n(int fd, void* buf, int len, int* );
extern int send_until_edge( int fd, const void* buf, int len, int timeout );

#endif
