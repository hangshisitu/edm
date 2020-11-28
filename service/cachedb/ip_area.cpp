#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "ip_area.h"




void load(const char * filename,vector<_ipline *> & ipv)
{
	struct stat sctFile;
	stat(filename, &sctFile);
	char * ipku = (char *)malloc(sctFile.st_size+1);
	char * line = ipku;
	char * p = ipku;
	FILE * pFile = fopen(filename, "r");
	fread(ipku, sctFile.st_size, 1, pFile);

	char area1[64];
	char area2[64];
	char b_ip[64];
	char e_ip[64];

	char tmp1[8];
	char tmp2[8];
	char tmp3[8];
	char tmp4[8];

	while(1) 
	{
		if(*p == '\n')
		{
			*p = '\0';
			sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]", b_ip, e_ip, area1, area2);

			sscanf(b_ip, "%[^.].%[^.].%[^.].%[^.]", tmp1, tmp2, tmp3, tmp4);
			_ipline * pl = new _ipline;

			memset(&pl->start,0,sizeof(pl->start));
			pl->start.ip1=atoi(tmp1);
			pl->start.ip2=atoi(tmp2);
			pl->start.ip3=atoi(tmp3);
			pl->start.ip4=atoi(tmp4);

			memset(&pl->end,0,sizeof(pl->end));
			sscanf(e_ip, "%[^.].%[^.].%[^.].%[^.]", tmp1, tmp2, tmp3, tmp4);
			pl->end.ip1=atoi(tmp1);
			pl->end.ip2=atoi(tmp2);
			pl->end.ip3=atoi(tmp3);
			pl->end.ip4=atoi(tmp4);
			if(pl->start.ip1 < pl->end.ip1) pl->flag = 1;
			else if(pl->start.ip2 < pl->end.ip2) pl->flag = 2;
			else if(pl->start.ip3 < pl->end.ip3) pl->flag = 3;
			else if(pl->start.ip4 < pl->end.ip4) pl->flag = 4;
			else pl->flag = 4;

			snprintf(pl->area1,sizeof(pl->area1),"%s",area1);
			snprintf(pl->area2,sizeof(pl->area2),"%s",area2);

			ipv.push_back(pl);
			line = p + 1;
			p++;
			continue;
		}
		if(*p == '\0')
			break;
		p++;
	}

	return;
}

int Comparison(_ipline * line,_ipnode & node)
{
	if(
			((line->flag == 1) && (node.ip1 >= line->start.ip1 && node.ip1 <= line->end.ip1)) ||
			((line->flag == 2) && (node.ip1 >= line->start.ip1 && node.ip1 <= line->end.ip1 && node.ip2 >= line->start.ip2 && node.ip2 <= line->end.ip2)) ||
			((line->flag == 3) && (node.ip1 >= line->start.ip1 && node.ip1 <= line->end.ip1 && node.ip2 >= line->start.ip2 && node.ip2 <= line->end.ip2 &&
				node.ip3 >= line->start.ip3 && node.ip3 <= line->end.ip3)) ||
			((line->flag == 4) && (node.ip1 >= line->start.ip1 && node.ip1 <= line->end.ip1 && node.ip2 >= line->start.ip2 && node.ip2 <= line->end.ip2 &&
				node.ip3 >= line->start.ip3 && node.ip3 <= line->end.ip3 && node.ip4 >= line->start.ip4 && node.ip4 <= line->end.ip4))
	  ) //正是需要查找的节点
	{
		return 0;
	}
	else if(node.ip1 < line->start.ip1 || 
			(node.ip1 >= line->start.ip1 && node.ip1 <= line->end.ip1 && node.ip2 < line->start.ip2) ||
			(node.ip1 >= line->start.ip1 && node.ip1 <= line->end.ip1 && node.ip2 >= line->start.ip2 && node.ip2 <= line->end.ip2 && node.ip3 < line->start.ip3) ||
			(node.ip1 >= line->start.ip1 && node.ip1 <= line->end.ip1 && node.ip2 >= line->start.ip2 && node.ip2 <= line->end.ip2 &&  node.ip3 >= line->start.ip3 && node.ip3 <= line->end.ip3 && node.ip4 < line->start.ip4) 
		   )
	{
		return 2;
	}
	else
		return 1;
}

int find_ip(const vector<_ipline *> & ipv,const char * ip, char * area)                      //ip 为待查ip
{
	if (ipv.empty() || NULL == ip || NULL == area) {
		return 0;
	}
	char tmp1[8];
	char tmp2[8];
	char tmp3[8];
	char tmp4[8];

	sscanf(ip,"%[^.].%[^.].%[^.].%[^.]", tmp1, tmp2, tmp3, tmp4);
	_ipnode node;
	node.ip1=(unsigned char )atoi(tmp1);
	node.ip2=(unsigned char )atoi(tmp2);
	node.ip3=(unsigned char )atoi(tmp3);
	node.ip4=(unsigned char )atoi(tmp4);

	int i_begin=0;                                  //查找开始索引
	int i_end=ipv.size();                   //查找结束索引
	int iret;

	while( i_end != i_begin + 1 )
	{
		if((iret = Comparison(ipv[(i_begin + i_end)/2],node)) == 2)             //往上找
		{
			i_end = (i_begin+i_end)/2;
		}
		else if (iret == 1)       // 往下找             
		{
			i_begin = (i_begin+i_end)/2;
		}
		else                                    //找到
		{
			strcpy(area,ipv[(i_begin+i_end)/2]->area1);
			return 1;
		}
	}
	return 0;
}
