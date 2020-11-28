/***********************************************************************************************************************
														���ļ��ǹ���base64���qp��Ľ��룬���������ļ��ϡ�
														qp���뽨��ʹ��decode_qp
***********************************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef _H_BaseQp

#define _H_BaseQp


#ifndef _H_IsHexChar
#define _H_IsHexChar
#define IsHexChar(c) ( ((c>='a' && c<='f' ) || (c>='A' && c<='F' ) || (c>='0' && c<='9' )) )
/***********************************************************************************************************************
���ܣ�
	һ���궨��ĺ������ж��ַ��Ƿ�Ϊ16���Ƶ�����
����:
	c��Ҫ�жϵ��ַ�
����:
	�����16���Ƶ��ַ��������0��������ǣ������0
***********************************************************************************************************************/
#endif











int decode_qp_ext(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize , size_t* aOutLen, char cEnd);
/***********************************************************************************************************************
���ܣ�
	��aIn���ַ�����qp���롣�����ʶ�������еġ�
����:
	aIn����Ҫ����Ĵ�ŵ��ڴ�
	aInLen����Ҫ������ַ��ĳ���
	aOut:��������aOut�ڴ���
	aOutSize��������ŵ��ڴ�Ĵ�С����aOut�Ĵ�С
	aOutLen:�����ĳ���
	cEnd������������ϵĽ����ַ���һ����\0
����:
	����������������-1���������뷵��0
ע�⣺
	aOut��aIn������ͬ�����ǲ���aOut��aIn�ĺ��棬�磺aOut������aIn+8�����ġ�aOutSize���ܴ���aOut�ڴ�Ĵ�С
	aOutLen�ǲ����������ַ��ĳ��ȣ�����˵aOut[aOutLen] = cEnd�ġ�
	�ڽ�ĳЩ����ʱ����bug
***********************************************************************************************************************/



int decode_qp_ext2(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize , size_t* aOutLen, char cEnd);
/***********************************************************************************************************************
���ܣ�
	��aIn���ַ�����qp���롣���ʶ�������С�
����:
	aIn����Ҫ����Ĵ�ŵ��ڴ�
	aInLen����Ҫ������ַ��ĳ���
	aOut:��������aOut�ڴ���
	aOutSize��������ŵ��ڴ�Ĵ�С����aOut�Ĵ�С
	aOutLen:�����ĳ���
	cEnd������������ϵĽ����ַ���һ����\0
����:
	����������������-1���������뷵��0
ע�⣺
	aOut��aIn������ͬ�����ǲ���aOut��aIn�ĺ��棬�磺aOut������aIn+8�����ġ�aOutSize���ܴ���aOut�ڴ�Ĵ�С
	aOutLen�ǲ����������ַ��ĳ��ȣ�����˵aOut[aOutLen] = cEnd�ġ�
***********************************************************************************************************************/



int decode_qp(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize , size_t* aOutLen);
/***********************************************************************************************************************
���ܣ�
	��aIn���ַ�����qp���롣�����ʶ�������С�����һ���������������̶�����Ϊ0��
����:
	aIn����Ҫ����Ĵ�ŵ��ڴ�
	aInLen����Ҫ������ַ��ĳ���
	aOut:��������aOut�ڴ���
	aOutSize��������ŵ��ڴ�Ĵ�С����aOut�Ĵ�С
	aOutLen:�����ĳ���
����:
	����������������-1���������뷵��0
ע�⣺
	aOut��aIn������ͬ�����ǲ���aOut��aIn�ĺ��棬�磺aOut������aIn+8�����ġ�aOutSize���ܴ���aOut�ڴ�Ĵ�С
	aOutLen�ǲ����������ַ��ĳ��ȣ�����˵aOut[aOutLen] = \0�ġ�
***********************************************************************************************************************/



int encode_base64_asm(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen);
/***********************************************************************************************************************
���ܣ�
	�û��д��base64������򡣣����룡�������ǽ��룩
����:
	aIn��Ҫ������ַ���
	aInLen:Ҫ������ַ����ĳ���
	aOut:�������õ��ڴ�
	aOutSize:�������ڵ��ڴ�Ĵ�С
	aOutLen�������ĳ���
����:
	����ڴ�鲻���ڻ��߳���Ϊ�㣬����-1�����aOutSize�����󣬷���-2���ɹ�����0
ע�⣺
	aOutSizeӦ�ô���((aInLen)/3)*4��
	aOut��aIn����һ���������Ǳ䳤�ģ����Ǳ�̵�
	û����ĩβ���㣬��Ҫ�Լ��õ�
***********************************************************************************************************************/



int decode_base64(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen);
/***********************************************************************************************************************
���ܣ�
	base64���롣��ĩβ����
����:
	aIn��Ҫ������ַ���
	aInLen:Ҫ������ַ����ĳ���
	aOut:�������õ��ڴ�
	aOutSize:�������ڵ��ڴ�Ĵ�С
	aOutLen�������ĳ���	
����:
	�ɹ�����0	��ʧ�ܷ���-1
�޸ģ�
	��ĩβ��Ϊ\0������һ��:aOut[outpos] = 0;
ע�⣺
	���룬������aIn��aOut��ͬ
	Ҫ��aInĩβ�Ķ����\r��\n��Щȥ��������˵�����ж�����ַ�
***********************************************************************************************************************/



int decode_base64_asm(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen);
/***********************************************************************************************************************
���ܣ�
	����base64���롣û��ĩβ����
����:
	aIn��Ҫ������ַ���
	aInLen:Ҫ������ַ����ĳ���
	aOut:�������õ��ڴ�
	aOutSize:�������ڵ��ڴ�Ĵ�С
	aOutLen�������ĳ���	
����:
	����ڴ�鲻���ڻ��߳���Ϊ�㣬����-1�����aOutSize�����󣬷���-2���ɹ�����0
ע�⣺
	���룬������aIn��aOut��ͬ
	Ҫ��aInĩβ�Ķ����\r��\n��Щȥ��������˵�����ж�����ַ�
***********************************************************************************************************************/



int encode_base64(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen);
/***********************************************************************************************************************
���ܣ�
	base64���룬ĩβ����ġ�
����:
	aIn��Ҫ������ַ���
	aInLen:Ҫ������ַ����ĳ���
	aOut:�������õ��ڴ�
	aOutSize:�������ڵ��ڴ�Ĵ�С
	aOutLen�������ĳ���
����:
	�ɹ�����0��ʧ�ܷ���-1
ע�⣺
	���룬aIn��aOut������ͬ��aOutSizeҪ����
***********************************************************************************************************************/



int decode_qp_asm(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen,char cEnd);
/***********************************************************************************************************************
���ܣ�
	����qp���룬��ĩβ��ΪcEnd
����:
	aIn��Ҫ������ַ���
	aInLen:Ҫ������ַ����ĳ���
	aOut:�������õ��ڴ�
	aOutSize:�������ڵ��ڴ�Ĵ�С
	aOutLen�������ĳ���
	cEnd�������ַ�
����:
	�ɹ�����0��ʧ�ܷ���-1
ע�⣺
	���룬aIn��aOut������ͬ��
	�ڽ������ĵ�ʱ������bug
***********************************************************************************************************************/



int encode_qp_asm(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen); 
/***********************************************************************************************************************
���ܣ�
	����qp����
����:
	aIn��Ҫ������ַ���
	aInLen:Ҫ������ַ����ĳ���
	aOut:�������õ��ڴ�
	aOutSize:�������ڵ��ڴ�Ĵ�С
	aOutLen�������ĳ���
����:
	�ɹ�����0��ʧ�ܷ���-1
ע�⣺
	���룬aIn��aOut������ͬ��aOutSizeҪ����û������
	������bug����codedump
***********************************************************************************************************************/



int encode_qp(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen,int *nPreLineLeft);
/***********************************************************************************************************************
���ܣ�
	qp����
����:
	aIn��Ҫ������ַ���
	aInLen:Ҫ������ַ����ĳ���
	aOut:�������õ��ڴ�
	aOutSize:�������ڵ��ڴ�Ĵ�С
	aOutLen�������ĳ���
	aPreLineLeft:��һ�д���76���ַ�ʱ���ú�����Ӹ������С������Ƿ��Ѿ���һ������˶��ٸ��ַ���
����:
	�ɹ�����0��ʧ�ܷ���-1
ע�⣺
	���룬aIn��aOut������ͬ��aOutSizeҪ����û������
***********************************************************************************************************************/




void InsertBase64LineBreak(char *sLine, int szLine,char *sOut, int szOut,int *pnPreLineLeft,int *pnLen);
/***********************************************************************************************************************
���ܣ�
	ÿ76���ַ����Ὣ���������base64��Ӹ�����"\r\n"
����:
	sLine����Ҫ���뻻�е�base64��
	szLine��sLine�Ĵ�С
	sOut:������ŵ��ڴ�
	szOut:sOut�Ĵ�С
	pnPreLineLeft�����һ�������м����ַ�
	pnLen:�����Ĵ�С
����:
	û�з���
***********************************************************************************************************************/




#endif