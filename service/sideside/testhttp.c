#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "socket_tool.h"


const char* sName[] = 
{
    "jan.com", "feb.com", "mar.com", "apr.com", "may.com", "jun.com",
    "jul.com", "aug.com", "sep.com", "oct.com", "nov.com", "dec.com"
};



void *Thread_Session(void *sArg)
{
	char sIp[20];
	int nPort;
	int i; 
	int nSocket;
	char sMsg[1024];
	int k = 0;
	
	pthread_detach(pthread_self());
	memcpy(sIp, sArg, 20);
	memcpy(&nPort, sArg+20, 4);
	memcpy(&i, sArg+24, 4);
	
	free(sArg);
	
	nSocket = TcpConnect(sIp, nPort, NULL, 5, sMsg, 1000);
	
	if(nSocket < 0)
	{
		printf("[%d][%06d]FFFFF can not connect![%s]\n", time(NULL), i, sMsg);
		return (void*)0;	
	}
	
	snprintf(sMsg, 1024, "GET /test2.php?u=%s&e=%d@dd.com HTTP/1.1\r\nHOST: 127.0.0.1:80\r\nConnection: Keep-Alive\r\nUser-Agent: test\r\n\r\n", 
																								sName[i%12], i%12);
	k = SocketWrite(nSocket, 5, sMsg, strlen(sMsg));
	if(k != strlen(sMsg))
	{
		printf("[%d][%06d]FFFFF can write:%d,%d.\n", time(NULL), i, k, strlen(sMsg));
		close(nSocket);
		return (void*)0;	

	}
	printf("[%d][%06d]send msg success.\n", time(NULL), i);
	
	k = SocketReadDoubleLine(nSocket, 5, sMsg, 1024);
	if(k <= 0)
	{
		printf("[%d][%06d]FFFFF read fail.\n", time(NULL), i);
		close(nSocket);return (void*)0;	
	}
	printf("[%d][%06d]read msg success..\n", time(NULL), i);
	
	if(strstr(sMsg, sName[i%12]))
	{
		printf("[%d][%06d]session success..\n", time(NULL), i);
	}
	else
	{
		printf("%s", sMsg);
		printf("[%d][%06d]FFFFF can not find the domain.\n", time(NULL), i);
	}
	close(nSocket);
	return (void*)0;
}



int main(int argc, char *argv[])
{
	int i = 0;
	int nPort = atoi(argv[2]);
	int nnn = atoi(argv[3]);	
	
	while(1)
	{
		i = 0;
		while(i < nnn)
		{
			char *sArg = (char*)malloc(28);
			pthread_t nThreadId;
			
			snprintf(sArg, 20, "%s", argv[1]);
			memcpy(sArg+20, &nPort, 4);
			memcpy(sArg+24, &i, 4);
			if(pthread_create(&nThreadId, NULL, Thread_Session, sArg) != 0)
			{
				free(sArg);
				printf("can't create thread.\n");	
			}
			++i;
		}	
		sleep(1);
	}		
	
	return 0;
}






