/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan Date: 2011-11-08      */

#ifndef APP_NEWCODE_H_201108
#define APP_NEWCODE_H_201108

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include "deliverengine.h"
//#include <vector>

RFC_NAMESPACE_BEGIN

using namespace std;

typedef struct node{
	string str;
	bool   flag;	//该部分是否需要加密
}_node;

const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="; 
char* base64_encode(const char* data, int data_len); 
char *base64_decode(const char* data, int data_len); 
char * emlbase64(char * eml);
char * spliteml(string str);
char * oldeml(char * eml);
static char find_pos(char ch); 



RFC_NAMESPACE_END

#endif
