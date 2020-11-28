#ifndef __HOVERLEES_IP_AREA_H
#define __HOVERLEES_IP_AREA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

typedef struct ipnode
{
	unsigned char ip1;
	unsigned char ip2;
	unsigned char ip3;
	unsigned char ip4;
}_ipnode;

typedef struct ipline
{
	_ipnode start;
	_ipnode end;
	char area1[64];
	char area2[64];
	unsigned char flag;
}_ipline;

int find_ip(const vector<_ipline *> & ipv,const char * ip, char * area);
int Comparison(_ipline * line,_ipnode & node);
void load(const char * filename,vector<_ipline *> & ipv);

#endif


