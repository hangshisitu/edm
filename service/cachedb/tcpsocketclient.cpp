#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


int main(int argc,char *argv[])
{
    int fd,ret;
    struct sockaddr_in addr = {0};
    struct in_addr x;
    inet_aton("192.168.18.156",&x);
    addr.sin_family = AF_INET;
    addr.sin_addr = x;
    addr.sin_port = htons(5881);
    fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1)
    {
        printf("error:%s\n",strerror(errno));
        return fd;
    }
    ret = connect(fd,(sockaddr*)&addr,sizeof(addr));
    if(ret == -1)
    {
        printf("error:%s\n",strerror(errno));
        return fd;
    }
    char buf[100]={0};
	unsigned int len;
	unsigned int tmp = 6;
	tmp = htonl(tmp);
	char * p = buf;
	memcpy(p+4,&tmp,4);
	tmp = 2;
	tmp = htonl(tmp);
	memcpy(p+8,&tmp,4);

	tmp = 101;
	tmp = htonl(tmp);
	memcpy(p+12,&tmp,4);

	tmp = 93;
	tmp = htonl(tmp);
	memcpy(p+16,&tmp,4);

	tmp = 52;
	tmp = htonl(tmp);
	memcpy(p+20,&tmp,4);

	tmp = 100;
	tmp = htonl(tmp);
	memcpy(p+24,&tmp,4);

	tmp = 98;
	tmp = htonl(tmp);
	memcpy(p+28,&tmp,4);

	tmp = 90;
	tmp = htonl(tmp);
	memcpy(p+32,&tmp,4);

	tmp = 2;
	tmp = htonl(tmp);
	memcpy(p+36,&tmp,4);

	tmp = 3;
	tmp = htonl(tmp);
	memcpy(p+40,&tmp,4);

	memcpy(p+44,"qq.com",6);

	tmp = 200;
	tmp = htonl(tmp);
	memcpy(p+51,&tmp,4);

	tmp = 198;
	tmp = htonl(tmp);
	memcpy(p+55,&tmp,4);

	tmp = 190;
	tmp = htonl(tmp);
	memcpy(p+59,&tmp,4);

	tmp = 5;
	tmp = htonl(tmp);
	memcpy(p+63,&tmp,4);

	tmp = 6;
	tmp = htonl(tmp);
	memcpy(p+67,&tmp,4);

	memcpy(p+71,"sina.com",8);

	tmp = 76;
	tmp = htonl(tmp);
	memcpy(p,&tmp,4);

    ret = send(fd,buf,80,0);
    if(ret == -1)
    {
        printf("error:%s\n",strerror(errno));
        return fd;
    }
    write(STDOUT_FILENO,buf,ret);
    close(fd);
    return 0;
}
