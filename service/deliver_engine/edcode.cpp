/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: liuan Date: 2011-11-28		*/


#include "edcode.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>


RFC_NAMESPACE_BEGIN
extern FileLog fileLog;
int flag = 0;
/*
int main(int argc, char* argv[]) 
{     
	emlbase64(argv[1]);
	return 0; 
} 
*/

char * spliteml(string str)
{
	char * p = (char *)str.c_str();
	char * q;
	if((q=strstr(p,"boundary=")) != NULL)
	{
//		printf("test 1111111111\n");
		if((q=strstr(q+15,"----=_NextPart")) != NULL)
		{
//			printf("test 222222222\n");
			*q = 0;
		}	
	}
//	printf("test p %s\n",p);
	string out=p;
	p=q+1;
	while((q=strstr(p+15,"------=_NextPart")) != NULL)
	{
		*q = 0;
	//	printf("test p1 %s\n",p);
		char * m = strstr(p,"base64");
		if(m != NULL) *(m+6)=0;
		else
			return NULL;
	//	printf("test p1.1 %s\n",p);
	//	printf("test p1.2 %s\n",m+8);
		char * encode = base64_encode(m+8, strlen(m+8));
		int len = strlen(encode);

		char * pp = new char[len/76+len+2048];
		memset(pp,0,len/76+len+2048);

		int i= 0;
		int j= 0;
		while(*(encode+i))
		{
			if(i!=0 && i%76 == 0)
			{
				*(pp+j) = '\n';
				j++;
				*(pp+j) = *(encode+i);
				i++;
				j++;
				continue;
			}
			*(pp+j) = *(encode+i);
			i++;
			j++;
		}
		free(encode);
	//	printf("test p1.3 %s\n",pp);
		char * tmp = (char *)malloc(strlen(p)+strlen(pp)+11);
		snprintf(tmp,strlen(p)+strlen(pp)+11,"-%s\r\n\r\n%s\r\n\r\n",p,pp);

		string s_tmp=tmp;
		delete [] pp;
		free(tmp);

		out = out+s_tmp;	
	//	printf("test p1.4 %s\n",tmp);
		p=q+1;
	}
	out = out + "------=_NextPart--\r\n\r\n";
//	printf("test p1.5 %s\n",(char *)out.c_str());
	char * pout = new char [out.size()+1];
        memset(pout,0,out.size()+1);
        strcpy(pout, (char *)out.c_str());
        return pout;
}

char * emlbase64(char * src)
{
	char * a = NULL;
	if((a = strstr(src,"\r\n\r\n")) != NULL)
	{
//		printf("test 1\n");
		*a = 0;
		flag =1;
	}
	else if((a = strstr(src,"\n\n")) != NULL)
	{
//		printf("test 2\n");
		*a = 0;
		flag =2;
	}

	if(strstr(src,"boundary=\"") == NULL)
	{
		if(flag == 1) *a = '\r';
		if(flag == 2) *a = '\n';
		char * p = oldeml(src);
		return p;
	}
	else
	{
		if(flag == 1) *a = '\r';
		if(flag == 2) *a = '\n';
		char * p = spliteml(src);
		return p;
	}
	return 0;
}

char * oldeml(char * src)
{
	char * context = new char [500*1024];
	memset(context,0,500*1024);
	strcpy(context,src);

	char * p = NULL;
	int k=0;
	if((p = strstr(context,"\n\n")) != NULL)
	{
		k=2;
		while(*(p+k) == '\n')
		{
			k = k+1;
		}
	}
	else if((p = strstr(context,"\r\n\r\n")) != NULL)
	{
		k=4;
		while(*(p+k) == '\r')
		{
			k = k+2;
		}
	}
	if(k == 0)
	{
	//	printf("this is not a eml file\n");
		delete []context;
		return NULL;
	}

	char * encode = base64_encode(p+k, strlen(p+k));

	char * pp = new char[500*1024];

	memset(pp,0,500*1024);
	int i= 0;
	int j= 0;
	while(*(encode+i))
	{
		if(i!=0 && i%76 == 0)
		{
			*(pp+j) = '\n';
			j++;
			*(pp+j) = *(encode+i);
			i++;
			j++;
			continue;
		}
		*(pp+j) = *(encode+i);
		i++;
		j++;
	}

	memset(p+k,0,strlen(p+k));
	memcpy(p+k,pp,strlen(pp));
	delete[] pp;
	free(encode);
	return context;

}

char *base64_encode(const char* data, int data_len) 
{     //int data_len = strlen(data);     
	int prepare = 0;     
	int ret_len;     
	int temp = 0;     
	char *ret = NULL;     
	char *f = NULL;     
	int tmp = 0;     
	char changed[4];     
	int i = 0;     
	ret_len = data_len / 3;     
	temp = data_len % 3;     
	if (temp > 0)     
	{         
		ret_len += 1;     
	}     
	ret_len = ret_len*4 + 1;     
	ret = (char *)malloc(ret_len);           
	if ( ret == NULL)     
	{         
		printf("No enough memory.\n");         
		exit(0);     
	}     
	memset(ret, 0, ret_len);     
	f = ret;     
	while (tmp < data_len)     
	{         
		temp = 0;         
		prepare = 0;         
		memset(changed, '\0', 4);         
		while (temp < 3)         
		{             
			//printf("tmp = %d\n", tmp);             
			if (tmp >= data_len)             
			{                 
				break;             
			}             
			prepare = ((prepare << 8) | (data[tmp] & 0xFF));             
			tmp++;             
			temp++;         
		}         
		prepare = (prepare<<((3-temp)*8));         
		//printf("before for : temp = %d, prepare = %d\n", temp, prepare);         
		for (i = 0; i < 4 ;i++ )         
		{             
			if (temp < i)             
			{                 
				changed[i] = 0x40;             
			}             
			else             
			{                 
				changed[i] = (prepare>>((3-i)*6)) & 0x3F;             
			}             
			*f = base[changed[i]];             
			//printf("%.2X", changed[i]);             
			f++;         
		}     
	}     
	*f = '\0';           
	return ret;       
} 
/* */ 

static char find_pos(char ch)   
{     
	char *ptr = (char*)strrchr(base, ch);
	//the last position (the only) in base[]     
	return (ptr - base); 
} 
/* */ 

char *base64_decode(const char *data, int data_len) 
{     
	int ret_len = (data_len / 4) * 3;     
	int equal_count = 0;     
	char *ret = NULL;     
	char *f = NULL;     
	int tmp = 0;     
	int temp = 0;     
	char need[3];     
	int prepare = 0;     
	int i = 0;     
	if (*(data + data_len - 1) == '=')     
	{         
		equal_count += 1;     
	}     
	if (*(data + data_len - 2) == '=')     
	{         
		equal_count += 1;     
	}     
	if (*(data + data_len - 3) == '=')     
	{
		//seems impossible         
		equal_count += 1;     
	}     
	switch (equal_count)     
	{     
		case 0:         
			ret_len += 4;//3 + 1 [1 for NULL]         
			break;     
		case 1:         
			ret_len += 4;//Ceil((6*3)/8)+1         
			break;     
		case 2:         
			ret_len += 3;//Ceil((6*2)/8)+1         
			break;     
		case 3:         
			ret_len += 2;//Ceil((6*1)/8)+1         
			break;     
	}     
	ret = (char *)malloc(ret_len);     
	if (ret == NULL)     
	{         
		printf("No enough memory.\n");         
		exit(0);     
	}     
	memset(ret, 0, ret_len);     
	f = ret;     
	while (tmp < (data_len - equal_count))     
	{         
		temp = 0;         
		prepare = 0;         
		memset(need, 0, 4);         
		while (temp < 4)         
		{             
			if (tmp >= (data_len - equal_count))             
			{                 
				break;             
			}             
			prepare = (prepare << 6) | (find_pos(data[tmp]));             
			temp++;             
			tmp++;         
		}         
		prepare = prepare << ((4-temp) * 6);         
		for (i=0; i<3 ;i++ )         
		{             
			if (i == temp)             
			{                 
				break;             
			}             
			*f = (char)((prepare>>((2-i)*8)) & 0xFF);             
			f++;         
		}     
	}     
	*f = '\0';     
	return ret; 
}

RFC_NAMESPACE_END
