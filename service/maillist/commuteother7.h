#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>



#ifndef _H_commuteother
#define _H_commuteother


int ReceivePackage(int nSocket, char **sMsg, int *nMsgSize, int *nMsgLeng, int nTimeOut);


int SendPackage(int nSocket, char *sResult, int nResultLeng, int nTimeOut);

//int ReadTaskFromJava(int nSocket, char *sFileName, int nFileNameSize);

int ReadTaskFromJava(int nSocket, char *sFileName, int nFileNameSize, pthread_mutex_t *pLock, int nTimeOut, 
										char *sDir1, char *sDir2);

#endif

