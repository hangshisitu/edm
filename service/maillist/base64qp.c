#include "base64qp.h"


#define DW_EOL_LF
//#define DW_EOL_CRLF

#if defined(DW_EOL_CRLF)
#   define DW_EOL  "\r\n"
#elif defined(DW_EOL_LF)
#   define DW_EOL  "\n"
#else
#   error "Must define DW_EOL_CRLF, DW_EOL_LF"
#endif
//#define DW_EOL  "\n"
#define MAXLINE  76
#define MAXLINE_BASE64_PRIVATE 256

#ifdef __cplusplus
inline int isbase64(int a) {
    return ('A' <= a && a <= 'Z')
        || ('a' <= a && a <= 'z')
        || ('0' <= a && a <= '9')
        || a == '+' || a == '/';
}
#else
#define isbase64(a) (  ('A' <= (a) && (a) <= 'Z') || ('a' <= (a) && (a) <= 'z') || ('0' <= (a) && (a) <= '9')||  (a) == '+' || (a) == '/'  )
#endif

static char base64tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz0123456789+/";
//static char hextab[] = "0123456789ABCDEF";

char base64idx[128] = {
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377',    62,'\377','\377','\377',    63,
        52,    53,    54,    55,    56,    57,    58,    59,
        60,    61,'\377','\377','\377','\377','\377','\377',
    '\377',     0,     1,     2,     3,     4,     5,     6,
         7,     8,     9,    10,    11,    12,    13,    14,
        15,    16,    17,    18,    19,    20,    21,    22,
        23,    24,    25,'\377','\377','\377','\377','\377',
    '\377',    26,    27,    28,    29,    30,    31,    32,
        33,    34,    35,    36,    37,    38,    39,    40,
        41,    42,    43,    44,    45,    46,    47,    48,
        49,    50,    51,'\377','\377','\377','\377','\377'
};

char EncodeTable[64] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,0x77, 0x78, 0x79, 0x7A, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2B, 0x2F};

/*
char Base64DecodeTable[256] = {
0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFE, 0x3F,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF,
0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};*/

#ifndef __LP64__
char DecodeTable[256] = {
0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFE, 0x3F,
0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF,
0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
#else
char DecodeTable[256] = {
0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x01, 0x01, 0x02, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xF8, 0x01, 0x01, 0x01, 0xFC,
0xD0, 0xD4, 0xD8, 0xDC, 0xE0, 0xE4, 0xE8, 0xEC, 0xF0, 0xF4, 0x01, 0x01, 0x01, 0x02, 0x01, 0x01,
0x01, 0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38,
0x3C, 0x40, 0x44, 0x48, 0x4C, 0x50, 0x54, 0x58, 0x5C, 0x60, 0x64, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x68, 0x6C, 0x70, 0x74, 0x78, 0x7C, 0x80, 0x84, 0x88, 0x8C, 0x90, 0x94, 0x98, 0x9C, 0xA0,
0xA4, 0xA8, 0xAC, 0xB0, 0xB4, 0xB8, 0xBC, 0xC0, 0xC4, 0xC8, 0xCC, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};
#endif

short int QPEncodeTable[128] = {
0x3830, 0x3831, 0x3832, 0x3833, 0x3834, 0x3835, 0x3836, 0x3837, 0x3838, 0x3839, 0x3841, 0x3842, 0x3843, 0x3844, 0x3845, 0x3846,
0x3930, 0x3931, 0x3932, 0x3933, 0x3934, 0x3935, 0x3936, 0x3937, 0x3938, 0x3939, 0x3941, 0x3942, 0x3943, 0x3944, 0x3945, 0x3946,
0x4130, 0x4131, 0x4132, 0x4133, 0x4134, 0x4135, 0x4136, 0x4137, 0x4138, 0x4139, 0x4141, 0x4142, 0x4143, 0x4144, 0x4145, 0x4146,
0x4230, 0x4231, 0x4232, 0x4233, 0x4234, 0x4235, 0x4236, 0x4237, 0x4238, 0x4239, 0x4241, 0x4242, 0x4243, 0x4244, 0x4245, 0x4246,
0x4330, 0x4331, 0x4332, 0x4333, 0x4334, 0x4335, 0x4336, 0x4337, 0x4338, 0x4339, 0x4341, 0x4342, 0x4343, 0x4344, 0x4345, 0x4346,
0x4430, 0x4431, 0x4432, 0x4433, 0x4434, 0x4435, 0x4436, 0x4437, 0x4438, 0x4439, 0x4441, 0x4442, 0x4443, 0x4444, 0x4445, 0x4446,
0x4530, 0x4531, 0x4532, 0x4533, 0x4534, 0x4535, 0x4536, 0x4537, 0x4538, 0x4539, 0x4541, 0x4542, 0x4543, 0x4544, 0x4545, 0x4546,
0x4630, 0x4631, 0x4632, 0x4633, 0x4634, 0x4635, 0x4636, 0x4637, 0x4638, 0x4639, 0x4641, 0x4642, 0x4643, 0x4644, 0x4645, 0x4646
};

char QPDecodeTable[256] = {
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x0A, 0x01, 0x01, 0x0D, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

int decode_qp_ext2(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize , size_t* aOutLen,char cEnd)
{
	size_t inPos=0,outPos=0,nextLineStart,endLine;
	unsigned c1,c2,c3,cQP,nSoftBreak=0;
	unsigned nError=0;

	while (inPos<aInLen)
	{
		/*计算下一个行首位置*/
		nextLineStart=inPos;
		nSoftBreak=0;
		while (nextLineStart<aInLen && aIn[nextLineStart]!='\n') nextLineStart++;
		if (nextLineStart<aInLen) nextLineStart++;
		endLine=nextLineStart;
		if (endLine>0 && aIn[endLine-1]=='\n') endLine--;
		if (endLine>0 && aIn[endLine-1]=='\r') endLine--;
		if (endLine>0 && aIn[endLine-1]=='=') 
		{
			endLine--;/*soft break*/
			nSoftBreak=1;
		}
		else
		{
			while (endLine>0 && (aIn[endLine-1]==' '  || aIn[endLine-1]=='\t' ))  endLine--;
		}
		while (inPos<endLine)
		{
			c1=(unsigned char)aIn[inPos];
			if (c1=='\r' || c1=='\n')
			{
				inPos++;continue;
			}
			if (c1>0x7F) nError=1;
			if (c1!='=')
			{
				aOut[outPos]=aIn[inPos];
				outPos++;inPos++;
			}
			else
			{
				if ((inPos+1)<endLine)
				{
					
					c2=(unsigned char)aIn[inPos+1];
					if (IsHexChar(c2) )
					{
						if ((inPos+2)<endLine)
						{
							c3=(unsigned char)aIn[inPos+2];
							if (IsHexChar(c3) )
							{
								cQP=0;
								if (c2>='0' && c2<='9') cQP=c2-'0';
								else
								{
									if (c2>='A' && c2<='Z') cQP=c2-'A'+10;
									else cQP=c2-'a'+10;
								}
								cQP=16*cQP;
								if (c3>='0' && c3<='9') cQP+=(c3-'0');
								else
								{
									if (c3>='A' && c3<='Z') cQP+=(c3-'A')+10;
									else cQP+=(c3-'a')+10;
								}
								
								aOut[outPos]=cQP;
								outPos++;inPos+=3;
								
							}
							else
							{

								
								aOut[outPos]=aIn[inPos];
								#ifndef NO_DEBUG_MSG_CONTENT_FILTER
								putchar(aOut[outPos]);
								#endif
								outPos++;inPos++;
								
								aOut[outPos]=aIn[inPos];
								#ifndef NO_DEBUG_MSG_CONTENT_FILTER
								putchar(aOut[outPos]);
								#endif
								outPos++;inPos++;
								
								aOut[outPos]=aIn[inPos];
								#ifndef NO_DEBUG_MSG_CONTENT_FILTER
								putchar(aOut[outPos]);
								#endif
								outPos++;inPos++;
								
								nError=1;
								
								/*
								#ifndef NO_DEBUG_MSG_CONTENT_FILTER
				                        	for (i=outPos-50;i<outPos;i++) if (i>0) putchar(aOut[i]);
				                        	printf("\n##1\n");
				                        	#endif
				                        	*/
							}
						}
						else
						{
							#ifndef NO_DEBUG_MSG_CONTENT_FILTER
							printf("match end of line %d %d\n",inPos+2,endLine);
							#endif
						
							aOut[outPos]=aIn[inPos];
							outPos++;inPos++;
							
							aOut[outPos]=aIn[inPos];
							outPos++;inPos++;
							nError=1;
							/*
							#ifndef NO_DEBUG_MSG_CONTENT_FILTER
			                        	for (i=outPos-50;i<outPos;i++) if (i>0) putchar(aOut[i]);
			                        	printf("\n##2\n");
			                        	#endif*/
						}
					}
					else
					{
						
						
						aOut[outPos]=aIn[inPos];
						outPos++;inPos++;
						
						aOut[outPos]=aIn[inPos];
						outPos++;inPos++;
						nError=1;
						/*
						#ifndef NO_DEBUG_MSG_CONTENT_FILTER
		                        	for (i=outPos-50;i<outPos;i++) if (i>0) putchar(aOut[i]);
		                        	printf("\n##3\n");
		                        	#endif*/
					}
					
				}
				else
				{
					aOut[outPos]=aIn[inPos];
					outPos++;inPos++;
					nError=1;
					/*
					#ifndef NO_DEBUG_MSG_CONTENT_FILTER
	                        	for (i=outPos-50;i<outPos;i++) if (i>0) putchar(aOut[i]);
	                        	printf("\n##4\n");
	                        	#endif*/
				}
			}
		}//while endLine
		if (nSoftBreak==0) {aOut[outPos]='\n';outPos++;}
		inPos=nextLineStart;
		
	}
	 aOut[outPos]=cEnd;
	 *aOutLen = outPos;
	 if (nError) return -1;
	 return 0;
}



int decode_qp_ext(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize , size_t* aOutLen,char cEnd)
{
    size_t i, inPos, outPos, lineLen, nextLineStart, numChars, charsEnd;
    int isEolFound, softLineBrk, isError;
    int ch, c1, c2;
    unsigned char isPossibleError=0;

    if (!aIn || !aOut || !aOutLen)
        return -1;
    isError = 0;
    inPos = 0;
    outPos = 0;
    for (i=0; i < aInLen; ++i) {
        if (aIn[i] == 0) {
            aInLen = i;
            break;
        }
    }
    if (aInLen == 0) {
        aOut[0] = 0;
        *aOutLen = 0;
        return 0;
    }
    while (inPos < aInLen) {
        /* Get line */
        lineLen = 0;
        isEolFound = 0;
        while (!isEolFound && lineLen < aInLen - inPos) {
            ch = aIn[inPos+lineLen];
            ++lineLen;
            if (ch == '\n') {
                isEolFound = 1;
            }
        }
        nextLineStart = inPos + lineLen;
        numChars = lineLen;
        /* Remove white space from end of line */
        while (numChars > 0) {
            ch = aIn[inPos+numChars-1] & 0x7F;
            if (ch!='=' && ch != '\n' && ch != '\r' && ch != ' ' && ch != '\t') {
                break;
            }
            --numChars;
        }
        charsEnd = inPos + numChars;
        /* Decode line */
        softLineBrk = 0;
        while (inPos < charsEnd) {
            ch = aIn[inPos++] & 0x7F;
            if (ch != '=') {
                /* Normal printable char */
                if (outPos>aOutSize-1) return -1;
               /* aOut[outPos++] = (char) ch;*/
               aOut[outPos++] =aIn[inPos-1];
            }
            else /* if (ch == '=') */ {
                /* Soft line break */
                if (inPos >= charsEnd) {
                    softLineBrk = 1;
                    break;
                }
                /* Non-printable char */
                else if (inPos < charsEnd-1) {
                    //c1 = aIn[inPos++] & 0x7F;
                    c1 = aIn[inPos++] ;
                    isPossibleError=0;
                    if ('0' <= c1 && c1 <= '9')
                        c1 -= '0';
                    else if ('A' <= c1 && c1 <= 'F')
                        c1 = c1 - 'A' + 10;
                    else if ('a' <= c1 && c1 <= 'f')
                        c1 = c1 - 'a' + 10;
                    else
                        {
                        	aOut[outPos++] = '=';
                        	aOut[outPos++]=c1;
                        	isError=1;
                        	isPossibleError = 1;
                        	#ifndef NO_DEBUG_MSG_CONTENT_FILTER
                        	for (i=outPos-50;i<outPos;i++) if (i>0) putchar(aOut[i]);
                        	printf("\n##\n");
                        	#endif
                        	//printf("1)QP Error with %c%c%c%c--%c%c%c%c 0x%02X\n",aOut[outPos-3],aOut[outPos-2],aOut[outPos-1],aOut[outPos],aIn[inPos],aIn[inPos+1],aIn[inPos+2],aIn[inPos+3],c1);
                        }
                    if (isPossibleError==0)
                    {
	                    //c2 = aIn[inPos++] & 0x7F;
	                    c2 = aIn[inPos++];
	                    if ('0' <= c2 && c2 <= '9')
	                        c2 -= '0';
	                    else if ('A' <= c2 && c2 <= 'F')
	                        c2 = c2 - 'A' + 10;
	                    else if ('a' <= c2 && c2 <= 'f')
	                        c2 = c2 - 'a' + 10;
	                    else
	                        
	                        {
	                        	isPossibleError = 1;
	                        	aOut[outPos++] = '=';
                        		aOut[outPos++]=c1;
                        		aOut[outPos++]=c2;
	                        	isPossibleError = 1;
	                        	isError=1;
	                        	#ifndef NO_DEBUG_MSG_CONTENT_FILTER
	                        	for (i=outPos-50;i<outPos;i++) if (i>0) putchar(aOut[i]);
	                        	printf("\n##\n");
	                        	#endif
	                        	//printf("2)QP Error with %c%c%c%c--%c%c%c%c 0x%02X\n",aOut[outPos-3],aOut[outPos-2],aOut[outPos-1],aOut[outPos],aIn[inPos],aIn[inPos+1],aIn[inPos+2],aIn[inPos+3],c2);
	                        }
	                    if (isPossibleError==0)
	                    {
		                    if (outPos>aOutSize-1) return -1;
		                    aOut[outPos++] = (char) ((c1 << 4) + c2);
		             }
                    }
                }
                else /* if (inPos == charsEnd-1) */ {
                    isError = 1;
                }
            }
        }
        if (isEolFound && !softLineBrk) {
            const char* cp = DW_EOL;
            if (outPos>aOutSize-1) return -1;
            aOut[outPos++] = *cp++;
            if (*cp) {
            	if (outPos>aOutSize-1) return -1;
                aOut[outPos++] = *cp;
            }
        }
        inPos = nextLineStart;
    }
    aOut[outPos] = cEnd;
    *aOutLen = outPos;
    return (isError) ? -1 : 0;
}

int decode_qp(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize , size_t* aOutLen)
{
    size_t i, inPos, outPos, lineLen, nextLineStart, numChars, charsEnd;
    int isEolFound, softLineBrk, isError;
    int ch, c1, c2;

    if (!aIn || !aOut || !aOutLen)
        return -1;
    isError = 0;
    inPos = 0;
    outPos = 0;
    for (i=0; i < aInLen; ++i) {
        if (aIn[i] == 0) {
            aInLen = i;
            break;
        }
    }
    if (aInLen == 0) {
        aOut[0] = 0;
        *aOutLen = 0;
        return 0;
    }
    while (inPos < aInLen) {
        /* Get line */
        lineLen = 0;
        isEolFound = 0;
        while (!isEolFound && lineLen < aInLen - inPos) {
            ch = aIn[inPos+lineLen];
            ++lineLen;
            if (ch == '\n') {
                isEolFound = 1;
            }
        }
        nextLineStart = inPos + lineLen;
        numChars = lineLen;
        /* Remove white space from end of line */
        while (numChars > 0) {
            ch = aIn[inPos+numChars-1] & 0x7F;
            if (ch != '\n' && ch != '\r' && ch != ' ' && ch != '\t') {
                break;
            }
            --numChars;
        }
        charsEnd = inPos + numChars;
        /* Decode line */
        softLineBrk = 0;
        while (inPos < charsEnd) {
            ch = aIn[inPos++] & 0x7F;
            if (ch != '=') {
                /* Normal printable char */
                if (outPos>aOutSize-1) return -1;
               /* aOut[outPos++] = (char) ch;*/
               aOut[outPos++] =aIn[inPos-1];
            }
            else /* if (ch == '=') */ {
                /* Soft line break */
                if (inPos >= charsEnd) {
                    softLineBrk = 1;
                    break;
                }
                /* Non-printable char */
                else if (inPos < charsEnd-1) {
                    c1 = aIn[inPos++] & 0x7F;
                    if ('0' <= c1 && c1 <= '9')
                        c1 -= '0';
                    else if ('A' <= c1 && c1 <= 'F')
                        c1 = c1 - 'A' + 10;
                    else if ('a' <= c1 && c1 <= 'f')
                        c1 = c1 - 'a' + 10;
                    else
                        isError = 1;
                    c2 = aIn[inPos++] & 0x7F;
                    if ('0' <= c2 && c2 <= '9')
                        c2 -= '0';
                    else if ('A' <= c2 && c2 <= 'F')
                        c2 = c2 - 'A' + 10;
                    else if ('a' <= c2 && c2 <= 'f')
                        c2 = c2 - 'a' + 10;
                    else
                        isError = 1;
                    if (outPos>aOutSize-1) return -1;
                    aOut[outPos++] = (char) ((c1 << 4) + c2);
                }
                else /* if (inPos == charsEnd-1) */ {
                    isError = 1;
                }
            }
        }
        if (isEolFound && !softLineBrk) {
            const char* cp = DW_EOL;
            if (outPos>aOutSize-1) return -1;
            aOut[outPos++] = *cp++;
            if (*cp) {
            	if (outPos>aOutSize-1) return -1;
                aOut[outPos++] = *cp;
            }
        }
        inPos = nextLineStart;
    }
    aOut[outPos] = 0;
    *aOutLen = outPos;
    return (isError) ? -1 : 0;
}

int decode_base64(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen)
{

    size_t inLen = aInLen;
    char* out = aOut;
    size_t outSize = (inLen+3)/4*3;
    int isErr = 0, isEndSeen = 0;
    int b1, b2, b3, a[5],k;
    size_t inPos = 0, outPos = 0;
    register char c64Tag;
    
    if (!aIn || !aOut || !aOutLen)   return -1;
    
    if (aOutSize < outSize)        return -1;
    /* Get four input chars at a time and decode them. Ignore white space
     * chars (CR, LF, SP, HT). If '=' is encountered, terminate input. If
     * a char other than white space, base64 char, or '=' is encountered,
     * flag an input error, but otherwise ignore the char.
     */
    

    while (inPos < inLen) {
        a[1] = a[2] = a[3] = a[4] = 0;
				c64Tag = '\0';
				for ( k=1; k<=4; k++ ) {
	        while (inPos < inLen) {
						register char cMaskBit=0x01 ;
            a[k] = aIn[inPos++] & 0xFF;
            if (isbase64(a[k])) {
	    					cMaskBit = cMaskBit<<k ;
								c64Tag = c64Tag | cMaskBit ;
                break;
            }
            else if (a[k] == '=') {
                isEndSeen = 1;
                break;
            }
            else if (a[k] != '\r' && a[k] != '\n' && a[k] != ' ' && a[k] != '\t') {
                isErr = 1;
            }
  	      }
				} // for ( int k ...
        if ( c64Tag == 0x1E ) {  // a[1], a[2], a[3], a[4] base64 chars
            a[1] = base64idx[a[1]] & 0xFF;
            a[2] = base64idx[a[2]] & 0xFF;
            a[3] = base64idx[a[3]] & 0xFF;
            a[4] = base64idx[a[4]] & 0xFF;
            b1 = ((a[1] << 2) & 0xFC) | ((a[2] >> 4) & 0x03);
            b2 = ((a[2] << 4) & 0xF0) | ((a[3] >> 2) & 0x0F);
            b3 = ((a[3] << 6) & 0xC0) | ( a[4]       & 0x3F);
            out[outPos++] = (char)b1;
            out[outPos++] = (char)b2;
            out[outPos++] = (char)b3;
        }
        else if ( c64Tag == 0x0E && a[4]=='=' ) {	// a[1], a[2], a[3] base64 chars and a[4] is '='
            a[1] = base64idx[a[1]] & 0xFF;
            a[2] = base64idx[a[2]] & 0xFF;
            a[3] = base64idx[a[3]] & 0xFF;
            b1 = ((a[1] << 2) & 0xFC) | ((a[2] >> 4) & 0x03);
            b2 = ((a[2] << 4) & 0xF0) | ((a[3] >> 2) & 0x0F);
            out[outPos++] = (char)b1;
            out[outPos++] = (char)b2;
            break;
        }
        else if ( c64Tag == 0x06 && a[3]=='=' && a[4]=='=' ) {
            a[1] = base64idx[a[1]] & 0xFF;
            a[2] = base64idx[a[2]] & 0xFF;
            b1 = ((a[1] << 2) & 0xFC) | ((a[2] >> 4) & 0x03);
            out[outPos++] = (char)b1;
            break;
        }
        else {
            break;
        }
        if (isEndSeen) {
            break;
        }
    } /* end while loop */
    *aOutLen = outPos;
    aOut[outPos] = 0;
    return (isErr) ? -1 : 0;
}

int encode_base64(const char* aIn, size_t aInLen, char* aOut, size_t aOutSize, size_t* aOutLen)
{

    size_t inLen = aInLen;
    char* out = aOut;
    size_t outSize = (inLen+2)/3*4;     /* 3:4 conversion ratio */
    char* cp; 	    
    size_t inPos  = 0;
    size_t outPos = 0;
    size_t i;
    int c1, c2, c3;
    int lineLen = 0;
    
    /* Get three characters at a time and encode them. */
    outSize += strlen(DW_EOL)*outSize/MAXLINE_BASE64_PRIVATE + 2;  /* Space for newlines and NUL */
    if (!aIn || !aOut || !aOutLen)
        return -1;
    if (aOutSize < outSize)
        return -1;
        
    for (i=0; i < inLen/3; ++i) {
        c1 = aIn[inPos++] & 0xFF;
        c2 = aIn[inPos++] & 0xFF;
        c3 = aIn[inPos++] & 0xFF;
        out[outPos++] = base64tab[(c1 & 0xFC) >> 2];
        out[outPos++] = base64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
        out[outPos++] = base64tab[((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6)];
        out[outPos++] = base64tab[c3 & 0x3F];
		lineLen += 4;
        if (lineLen >= MAXLINE_BASE64_PRIVATE-3) {
			char* cp = DW_EOL;
            out[outPos++] = *cp++;
			if (*cp) {
				out[outPos++] = *cp;
			}
			lineLen = 0;
        }
    }
    /* Encode the remaining one or two characters. */

    switch (inLen % 3) {
    case 0:
		cp = DW_EOL;
        out[outPos++] = *cp++;
		if (*cp) {
			out[outPos++] = *cp;
		}
        break;
    case 1:
        c1 = aIn[inPos] & 0xFF;
        out[outPos++] = base64tab[(c1 & 0xFC) >> 2];
        out[outPos++] = base64tab[((c1 & 0x03) << 4)];
        out[outPos++] = '=';
        out[outPos++] = '=';
		cp = DW_EOL;
        out[outPos++] = *cp++;
		if (*cp) {
			out[outPos++] = *cp;
		}
        break;
    case 2:
        c1 = aIn[inPos++] & 0xFF;
        c2 = aIn[inPos] & 0xFF;
        out[outPos++] = base64tab[(c1 & 0xFC) >> 2];
        out[outPos++] = base64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
        out[outPos++] = base64tab[((c2 & 0x0F) << 2)];
        out[outPos++] = '=';
		cp = DW_EOL;
        out[outPos++] = *cp++;
		if (*cp) {
			out[outPos++] = *cp;
		}
        break;
    }
    out[outPos] = 0;
    *aOutLen = outPos;
    return 0;
}



int encode_base64_asm(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen)
{
	if(aIn==NULL || aOut==NULL || aIn==aOut || aInLen<=0 || aOutSize<=0) return -1;
	if(aOutSize < (aInLen+2)/3*4) return -2;
	
#ifndef __LP64__
	__asm__
	(
	"movl %0, %%esi;"	//源串地址
	"movl %1, %%edi;"	//目标地址
	"movl %2, %%eax;"	//源串长度
	"movl %4, %%ecx;"
	"movl %5, %%edx;"

	"pushl %%edi;"	//%1
	"pushl %%ecx;"	//%4
	"pushl %%edx;"	//%5
	"pushl %%eax;"	//%2
	"pushl %%edx;"	//%5
	
	"cdq;"
	"movl $0xC, %%ecx;"	//每12字节为一大组（每3字节为一小组）
	"idivl %%ecx;"
	"movl %%eax, %%ecx;"	//ecx保存需编码的大组数
	"testl %%ecx, %%ecx;"
	"jz left;"		//没有大组需编码则跳到小组编码环节

"start_enc:"			//大组编码（12字节变16字节）
	"pushl %%ecx;"		//保存起大组数
	"movl (%%esi), %%eax;"	//头4字节放eax
	"movl 0x4(%%esi), %%ebx;"	//中4字节放ebx
	"movl 0x8(%%esi), %%edx;"	//末4字节放edx
	"addl $0xC, %%esi;"	//源指针增加12
	"bswap %%eax;"		//转为大尾数序，与内存中字节顺序一样
	"bswap %%ebx;"
	"bswap %%edx;"

	"movw %%bx,%%cx;"	//原来eax放0/1/2/3字节，ebx放
	"shll $0x10, %%ecx;"	//4/5/6/7字节，edx放8/9/A/B字
	"roll $0x8, %%edx;"	//节，转变为eax放0/1/2字节，
	"movb %%dl, %%ch;"	//ebx放3/4/5字节，ecx放6/7/8字
	"movb %%al, %%bl;"	//节，edx放9/A/B字节，其中al、
	"rorl $0x8, %%ebx;"	//bl、cl、dl没有使用

	"pushl %%edx;"
	"pushl %%ecx;"
	"pushl %%ebx;"
	"pushl %%eax;"
	"movl 0x14(%%esp), %%ebx;"	//编码表地址
	"call encode_and_save;"	//对0/1/2字节编码并保存
	"call encode_and_save;"	//对3/4/5字节编码并保存
	"call encode_and_save;"	//对6/7/8字节编码并保存
	"call encode_and_save;"	//对9/A/B字节编码并保存

	"popl %%ecx;"		//取出大组数
	"decl %%ecx;"		//大组数减1
	"jz left;"		//大组数为0则小组编码
	"jmp start_enc;"	//循环

"encode_and_save:"
	"movl 0x4(%%esp), %%eax;"	//取出待编码数据
	
	"xorb %%al, %%al;"	//这些操作将
	"roll $0x6, %%eax;"	//保存在eax
	"shlb $0x2, %%al;"	//高位的24bits
	"rorl $0xA, %%eax;"	//分为4个6bits
	"shrw $0x2, %%ax;"	//并保存在eax
	"shrb $0x2, %%al;"	//中
	
	"movl %%eax, %%edx;"	//设edx的4字节表示为3/2/1/0
	"xorl %%eax, %%eax;"	//eax清零
	"movb %%dl, %%al;"
	"movb (%%ebx,%%eax), %%dl;"	//0位编码
	"xchgb %%dh, %%dl;"	//此时edx为3/2/0/1
	"movb %%dl, %%al;"
	"movb (%%ebx,%%eax), %%dl;"	//1位编码
	"rorl $0x10, %%edx;"	//此时edx为0/1/3/2
	"movb %%dl, %%al;"
	"movb (%%ebx,%%eax), %%dl;"	//2位编码
	"xchgb %%dh, %%dl;"	//此时eax为0/1/2/3
	"movb %%dl, %%al;"
	"movb (%%ebx,%%eax), %%dl;"	//3位编码
	"movl %%edx, (%%edi);"	//存入内存，内存中的顺序为3/2/1/0，正是编码前的顺序
	"addl $0x4, %%edi;"
	"ret $0x4;"

"left:"
	"popl %%eax;"
	"popl %%eax;"	//取出源串长度
	"cdq;"
	"movl $0xC, %%ecx;"
	"idivl %%ecx;"		
	"movl %%edx, %%ecx;"	//ecx保存需编码的小组数
	"popl %%ebx;"	//ebx放编码表地址

"left_enc:"
	"cmpl $0x0, %%ecx;"
	"jle finish_enc;"	//没有字节需编码则跳到结束
	"cmpl $0x3, %%ecx;"
	"jge enc_3b;"		//剩余3字节或以上则对完整的小组编码
	"cmpl $0x1, %%ecx;"
	"jne enc_2b;"		//剩余不为1字节则跳去2字节编码
"enc_1b:"			//剩余1字节的编码
	"movl $0x3D3D0000, %%edx;"	//edx高两位（则内存末两位）补'='
	"movb (%%esi), %%dl;"	//读入1字节
	"shlw $0x6, %%dx;"	//dh保存高6bits
	"shrb $0x2, %%dl;"	//dl为低2bits后跟4bits0
	"xorl %%eax, %%eax;"
	"movb %%dl, %%al;"
	"movb (%%ebx,%%eax), %%dl;"	//低位编码
	"xchgb %%dh, %%dl;"	//交换位置
	"movb %%dl, %%al;"
	"movb (%%ebx,%%eax), %%dl;"	//高位编码
	"movl %%edx, (%%edi);"	//保存，内存中顺序为高位/低位/'='/'='
	"addl $0x4, %%edi;"
	"jmp finish_enc;"	//结束
"enc_2b:"			//剩余2字节的编码
	"xorl %%edx, %%edx;"	//edx清零
	"movb (%%esi), %%dh;"	//dh放高字节
	"movb 0x1(%%esi), %%dl;"//dl放低字节
	"shll $0x6, %%edx;"	//edx次高位放头6bits
	"shrw $0x2, %%dx;"	//dh放中间6bits
	"shrb $0x2, %%dl;"	//dl放末6bits
	"xorl %%eax, %%eax;"
	"movb %%dl, %%al;"
	"movb (%%ebx,%%eax), %%dl;"	//末6bits编码
	"rorl $0x8, %%edx;"	//dl放中6bits
	"movb %%dl, %%al;"
	"movb (%%ebx,%%eax), %%dl;"	//中6bits编码
	"rorl $0x8, %%edx;"	//dl放头6bits
	"movb %%dl, %%al;"
	"movb (%%ebx,%%eax), %%dl;"	//头6bits编码
	"movb $0x3D, %%dh;"	//此时edx为中/末/'='/头
	"bswap %%edx;"		//顺序变为头/'='/末/中
	"roll $0x8, %%edx;"	//顺序变为'='/末/中/头
	"movl %%edx, (%%edi);"	//保存，内存顺序为头/中/末/'='
	"addl $0x4, %%edi;"
	"jmp finish_enc;"	//结束
"enc_3b:"			//完整小组（3个字节）的编码
	"movl (%%esi), %%eax;"	//读入数据
	"bswap %%eax;"
	"addl $0x3, %%esi;"	//源串指针加3
	"pushl %%eax;"
	"call encode_and_save;"	//编码并保存
	"subl $0x3, %%ecx;"	//需要编码的字节数减3
	"jmp left_enc;"		//循环

"finish_enc:"
	"movl %%edi, %%eax;"
	"popl %%ebx;"
	"popl %%ecx;"
	"subl %%ecx, %%eax;"
	"movl %%eax, (%%ebx);"
	:
	:"m"(aIn), "m"(aOut), "m"(aInLen), "m"(aOutSize), "m"(aOutLen), "i"(EncodeTable)
	:"%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi"
	);
#else
	__asm__
	(
	"movq %0, %%rsi;"	//源串地址
	"movq %1, %%rdi;"	//目标地址
	"movq %2, %%rax;"	//源串长度
	"movq %%rax, %%r11;"
	"movq %%rdi, %%r9;"
	"movq %4, %%r8;"
	"movq %5, %%r10;"
	
	"cqo;"
	"movq $0xC, %%rcx;"	//每12字节为一大组（每3字节为一小组）
	"idivq %%rcx;"
	"movq %%rax, %%rcx;"	//ecx保存需编码的大组数
	"testq %%rcx, %%rcx;"
	"jz left;"		//没有大组需编码则跳到小组编码环节

"start_enc:"			//大组编码（12字节变16字节）
	"push %%rcx;"		//保存起大组数
	"movl (%%rsi), %%eax;"	//头4字节放eax
	"movl 0x4(%%rsi), %%ebx;"	//中4字节放ebx
	"movl 0x8(%%rsi), %%edx;"	//末4字节放edx
	"addq $0xC, %%rsi;"	//源指针增加12
	"bswap %%eax;"		//转为大尾数序，与内存中字节顺序一样
	"bswap %%ebx;"
	"bswap %%edx;"

	"movw %%bx,%%cx;"	//原来eax放0/1/2/3字节，ebx放
	"shll $0x10, %%ecx;"	//4/5/6/7字节，edx放8/9/A/B字
	"roll $0x8, %%edx;"	//节，转变为eax放0/1/2字节，
	"movb %%dl, %%ch;"	//ebx放3/4/5字节，ecx放6/7/8字
	"movb %%al, %%bl;"	//节，edx放9/A/B字节，其中al、
	"rorl $0x8, %%ebx;"	//bl、cl、dl没有使用

	"push %%rdx;"
	"push %%rcx;"
	"push %%rbx;"
	"push %%rax;"
	"movq %%r10, %%rbx;"	//编码表地址
	"call encode_and_save;"	//对0/1/2字节编码并保存
	"call encode_and_save;"	//对3/4/5字节编码并保存
	"call encode_and_save;"	//对6/7/8字节编码并保存
	"call encode_and_save;"	//对9/A/B字节编码并保存

	"pop %%rcx;"		//取出大组数
	"decq %%rcx;"		//大组数减1
	"jz left;"		//大组数为0则小组编码
	"jmp start_enc;"	//循环

"encode_and_save:"
	"movq 0x8(%%rsp), %%rax;"	//取出待编码数据
	
	"xorb %%al, %%al;"	//这些操作将
	"roll $0x6, %%eax;"	//保存在eax
	"shlb $0x2, %%al;"	//高位的24bits
	"rorl $0xA, %%eax;"	//分为4个6bits
	"shrw $0x2, %%ax;"	//并保存在eax
	"shrb $0x2, %%al;"	//中
	
	"movl %%eax, %%edx;"	//设edx的4字节表示为3/2/1/0
	"xorq %%rax, %%rax;"	//eax清零
	"movb %%dl, %%al;"
	"movb (%%rbx,%%rax), %%dl;"	//0位编码
	"xchgb %%dh, %%dl;"	//此时edx为3/2/0/1
	"movb %%dl, %%al;"
	"movb (%%rbx,%%rax), %%dl;"	//1位编码
	"rorl $0x10, %%edx;"	//此时edx为0/1/3/2
	"movb %%dl, %%al;"
	"movb (%%rbx,%%rax), %%dl;"	//2位编码
	"xchgb %%dh, %%dl;"	//此时eax为0/1/2/3
	"movb %%dl, %%al;"
	"movb (%%rbx,%%rax), %%dl;"	//3位编码
	"movl %%edx, (%%rdi);"	//存入内存，内存中的顺序为3/2/1/0，正是编码前的顺序
	"addq $0x4, %%rdi;"
	"ret $0x8;"

"left:"
	"movq %%r11, %%rax;"	//取出源串长度
	"cqo;"
	"movq $0xC, %%rcx;"
	"idivq %%rcx;"
	"movq %%rdx, %%rcx;"	//ecx保存需编码的小组数
	"movq %%r10, %%rbx;"	//ebx放编码表地址

"left_enc:"
	"cmpq $0x0, %%rcx;"
	"jle finish_enc;"	//没有字节需编码则跳到结束
	"cmpq $0x3, %%rcx;"
	"jge enc_3b;"		//剩余3字节或以上则对完整的小组编码
	"cmpq $0x1, %%rcx;"
	"jne enc_2b;"		//剩余不为1字节则跳去2字节编码
"enc_1b:"			//剩余1字节的编码
	"movl $0x3D3D0000, %%edx;"	//edx高两位（则内存末两位）补'='
	"movb (%%rsi), %%dl;"	//读入1字节
	"shlw $0x6, %%dx;"	//dh保存高6bits
	"shrb $0x2, %%dl;"	//dl为低2bits后跟4bits0
	"xorq %%rax, %%rax;"
	"movb %%dl, %%al;"
	"movb (%%rbx,%%rax), %%dl;"	//低位编码
	"xchgb %%dh, %%dl;"	//交换位置
	"movb %%dl, %%al;"
	"movb (%%rbx,%%rax), %%dl;"	//高位编码
	"movl %%edx, (%%rdi);"	//保存，内存中顺序为高位/低位/'='/'='
	"addq $0x4, %%rdi;"
	"jmp finish_enc;"	//结束
"enc_2b:"			//剩余2字节的编码
	"xorl %%edx, %%edx;"	//edx清零
	"movb (%%rsi), %%dh;"	//dh放高字节
	"movb 0x1(%%rsi), %%dl;"//dl放低字节
	"shll $0x6, %%edx;"	//edx次高位放头6bits
	"shrw $0x2, %%dx;"	//dh放中间6bits
	"shrb $0x2, %%dl;"	//dl放末6bits
	"xorq %%rax, %%rax;"
	"movb %%dl, %%al;"
	"movb (%%rbx,%%rax), %%dl;"	//末6bits编码
	"rorl $0x8, %%edx;"	//dl放中6bits
	"movb %%dl, %%al;"
	"movb (%%rbx,%%rax), %%dl;"	//中6bits编码
	"rorl $0x8, %%edx;"	//dl放头6bits
	"movb %%dl, %%al;"
	"movb (%%rbx,%%rax), %%dl;"	//头6bits编码
	"movb $0x3D, %%dh;"	//此时edx为中/末/'='/头
	"bswap %%edx;"		//顺序变为头/'='/末/中
	"roll $0x8, %%edx;"	//顺序变为'='/末/中/头
	"movl %%edx, (%%rdi);"	//保存，内存顺序为头/中/末/'='
	"addq $0x4, %%rdi;"
	"jmp finish_enc;"	//结束
"enc_3b:"			//完整小组（3个字节）的编码
	"movl (%%rsi), %%eax;"	//读入数据
	"bswap %%eax;"
	"addq $0x3, %%rsi;"	//源串指针加3
	"push %%rax;"
	"call encode_and_save;"	//编码并保存
	"subq $0x3, %%rcx;"	//需要编码的字节数减3
	"jmp left_enc;"		//循环

"finish_enc:"
	"movq %%rdi, %%rax;"
	"subq %%r9, %%rax;"
	"movq %%rax, (%%r8);"
	:
	:"m"(aIn), "m"(aOut), "m"(aInLen), "m"(aOutSize), "m"(aOutLen), "i"(EncodeTable)
	:"%rax", "%rbx", "%rcx", "%rdx", "%rsi", "%rdi", "%r8", "%r9", "%r10", "%r11"
	);
#endif
	return 0;
}

int decode_base64_asm(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen)
{
	int iRet;

	if(aIn==NULL || aOut==NULL || aInLen<=0 || aOutSize<=0) return -1;
	iRet = aInLen/4*3 + ((aInLen&0x2)?((aInLen&0x3)-1):0);
	
	if((DecodeTable[(unsigned char)aIn[aInLen-1]]&0x3)!=0)
	{
		iRet--;
		if((DecodeTable[(unsigned char)aIn[aInLen-2]]&0x3)!=0)
		{
			iRet--;
			if((DecodeTable[(unsigned char)aIn[aInLen-3]]&0x3)!=0) 
			{
				iRet--;
			}
		}
	}
	
	if (aOutSize < iRet) return -2;

#ifndef __LP64__
	__asm__
	(
	"movl %1, %%esi;"	//需解码的串地址
	"movl %2, %%edi;"	//目标地址
	"movl %3, %%ecx;"	//串长度
	"movl %6, %%ebx;"	//解码表地址
	
"start_dec:"
	"xorl %%eax, %%eax;"
	"movl %%eax, %%edx;"
	"movl %%eax, %0;"
	
"byte1:"
	"cmpl $0x2, %%ecx;"
	"jl finish_dec;"	//若需解码的串不足两字节（最多就6bits），则不能解码，跳到结束
	"movb (%%esi), %%al;"	//读入
	"decl %%ecx;"		//需解码的字节数减1
	"movb (%%ebx,%%eax), %%dl;"	//解码
	"incl %%esi;"		//源指针增1
	"cmpb $0x0, %%dl;"
	"jge byte1_ok;"		//若为有效码则继续
	"cmpb $0xFF, %%dl;"
	"je err;"		//若非base64字符则错误
	"jmp byte1;"		//若为无效码则尝试再读入第1字节
	
"byte1_ok:"
	"shll $0x8, %%edx;"	//左移8位，空出dl
"byte2:"
	"testl %%ecx, %%ecx;"
	"jz finish_dec;"	//若没有第二个字节则结束
	"movb (%%esi), %%al;"	//读入
	"decl %%ecx;"		//需解码的字节数减1
	"movb (%%ebx,%%eax), %%dl;"	//解码
	"incl %%esi;"		//源指针增1
	"cmpb $0x0, %%dl;"
	"jge byte2_ok;"		//若为有效码则继续
	"cmpb $0xFF, %%dl;"
	"je err;"		//若非base64字符则错误
	"jmp byte2;"		//若为无效码则尝试再读入第2字节
	
"byte2_ok:"
	"shll $0x8, %%edx;"	//左移8位，空出dl
"byte3:"
	"testl %%ecx, %%ecx;"
	"jz dec_2_bytes;"	//若只有2个字节则跳去对2字节的合并
	"movb (%%esi), %%al;"	//读入
	"decl %%ecx;"		//需解码的字节数减1
	"movb (%%ebx,%%eax), %%dl;"	//解码
	"incl %%esi;"		//源指针增1
	"cmpb $0x0, %%dl;"
	"jge byte3_ok;"		//若为有效码则继续
	"cmpb $0xFF, %%dl;"
	"je err;"		//若非base64字符则错误
	"jmp byte3;"		//若为无效码则尝试再读入第3字节
	
"byte3_ok:"
	"shll $0x8, %%edx;"	//左移8位，空出al
"byte4:"
	"testl %%ecx, %%ecx;"
	"jz dec_3_bytes;"	//若只有3个字节则跳去对3字节的合并
	"movb (%%esi), %%al;"	//读入
	"decl %%ecx;"		//需解码的字节数减1
	"movb (%%ebx,%%eax), %%dl;"	//解码
	"incl %%esi;"		//源指针增1
	"cmpb $0x0, %%dl;"
	"jge dec_4_bytes;"		//若为有效码则继续
	"cmpb $0xFF, %%dl;"
	"je err;"		//若非base64字符则错误
	"jmp byte4;"		//若为无效码则尝试再读入第4字节

"dec_4_bytes:"			//对4字节的合并（24位变24位）
	"shlb $0x2, %%dl;"	//低6bits与次低6bits连接
	"shlw $0x2, %%dx;"	//低12bits与次高6bits连接
	"roll $0xA, %%edx;"	//高6bits放dl
	"shrb $0x2, %%dl;"	//去掉高6bits与后18bits之间的2bits0
	"rorl $0x6, %%edx;"	//edx为高/中/低/空
	"bswap %%edx;"
	"movb %%dl, (%%edi);"	//保存
	"movb %%dh, 0x1(%%edi);"
	"shrl $0x10, %%edx;"
	"movb %%dl, 0x2(%%edi);"
	"addl $0x3, %%edi;"	//目标指针增3
	"jmp start_dec;"	//继续解码
"dec_3_bytes:"			//对3字节的合并（18位变16位）
	"shrl $0x8, %%edx;"	//edx为空/高6bits/中6bits/低6bits
	"shlb $0x2, %%dl;"	//低6bits与中6bits连接
	"shlw $0x2, %%dx;"	//低12bits与高6bits连接
	"shrl $0x6, %%edx;"	//低8bits放dl，高8bits放dh
	"xchgb %%dl, %%dh;"	//dx为低/高
	"movb %%dl, (%%edi);"	//保存
	"movb %%dh, 0x1(%%edi);"
	"addl $0x2, %%edi;"
	"jmp finish_dec;"	//结束
"dec_2_bytes:"			//对2字节的合并（12位变8位）
	"shlb $0x2, %%dh;"	//低2bits与高6bits连接
	"shrl $0xE, %%edx;"	//有效的8bits放dl
	"movb %%dl, (%%edi);"	//保存
	"incl %%edi;"
	"jmp finish_dec;"	//结束

"err:"
	"movl $0xFFFFFFFF, %0;"

"finish_dec:"
	"movl %%edi, %%eax;"
	"movl %5, %%ebx;"
	"subl %2, %%eax;"
	"movl %%eax, (%%ebx);"

	:"=m"(iRet)
	:"m"(aIn), "m"(aOut), "m"(aInLen), "m"(aOutSize), "m"(aOutLen), "i"(DecodeTable)
	:"%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi"
	);
#else
	__asm__
	(	
	"movq %1, %%rsi;"	//需解码的串地址
	"movq %2, %%rdi;"	//目的地址
	"movq %3, %%r9;"	//串长度
	"movq %6, %%r8;"	//解码表地址
	
	"xorq %%rax, %%rax;"
	"movq %%rax, %%rdx;"
	"movl %%eax, %0;"

"get_byte_1:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_1_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_1_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_1;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_1_ok:"	//6
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_2:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_2_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_2_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_2;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_2_ok:"	//12
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_3:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_3_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_3_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_3;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_3_ok:"	//18
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_4:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_4_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_4_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_4;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_4_ok:"	//24
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_5:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_5_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_5_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_5;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_5_ok:"	//30
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_6:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_6_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_6_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_6;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_6_ok:"	//36
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_7:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_7_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_7_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_7;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_7_ok:"	//42
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_8:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_8_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_8_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_8;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_8_ok:"	//48
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_9:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_9_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_9_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_9;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_9_ok:"	//54
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_A:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_A_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_A_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_A;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_A_ok:"	//60, 2/60/2
	"movq %%rdx, %%rcx;"		//rcx保存rax已有的解码结果
	"shlq $0x6, %%rdx;"	//56/8
	"shlq $0x2, %%rcx;"	//60/4
	

"get_byte_B:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_B_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_B_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_B;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_B_ok:"	//62/2
	"shlq $0x4, %%rdx;"	//58/6
	"movb %%dh, %%cl;"
	"bswapq %%rcx;"
	"shlq $0x2, %%rdx;"	//2
	"movq %%rcx, (%%rdi);"
	"addq $0x8, %%rdi;"

"get_byte_C:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_C_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_C_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_C;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_C_ok:"	//8
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_D:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_D_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_D_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_D;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_D_ok:"	//14
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_E:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_E_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_E_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_E;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_E_ok:"	//20
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_F:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_F_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_F_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_F;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_F_ok:"	//26
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_10:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_10_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_10_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_10;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_10_ok:"	//32
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_11:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_11_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_11_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_11;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_11_ok:"	//38
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_12:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_12_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_12_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_12;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_12_ok:"	//44
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_13:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_13_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_13_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_13;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_13_ok:"	//50
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_14:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_14_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_14_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_14;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_14_ok:"	//56
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_15:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_15_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_15_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_15;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_15_ok:"	//62
	"movq %%rdx, %%rcx;"		//rcx保存rax已有的解码结果
	"shlq $0x6, %%rdx;"	//56/8

"get_byte_16:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_16_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_16_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_16;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_16_ok:"
	"shlq $0x2, %%rdx;"
	"movb %%dh, %%cl;"
	"bswapq %%rcx;"
	"shlq $0x4, %%rdx;"	//4
	"movq %%rcx, (%%rdi);"
	"addq $0x8, %%rdi;"

"get_byte_17:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_17_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_17_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_17;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_17_ok:"	//10
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_18:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_18_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_18_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_18;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_18_ok:"	//16
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_19:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_19_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_19_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_19;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_19_ok:"	//22
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_1A:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_1A_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_1A_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_1A;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_1A_ok:"	//28
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_1B:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_1B_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_1B_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_1B;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_1B_ok:"	//34
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_1C:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_1C_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_1C_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_1C;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_1C_ok:"	//40
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_1D:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_1D_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_1D_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_1D;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_1D_ok:"	//46
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_1E:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_1E_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_1E_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_1E;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_1E_ok:"	//52
	"shlq $0x6, %%rdx;"	//左移6位，空出dl

"get_byte_1F:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_1F_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_1F_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_1F;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_1F_ok:"	//58
	"movq %%rdx, %%rcx;"
	"shlq $0x6, %%rdx;"	//56/8
	"shlq $0x4, %%rcx;"

"get_byte_20:"
	"decq %%r9;"
	"movb (%%rsi), %%al;"	//读入
	"jl no_20_byte;"	//没有需解码的字节，跳到结束
	"incq %%rsi;"
	"movb (%%r8,%%rax), %%dl;"	//解码
	"testb $0x03, %%dl;"
	"je byte_20_ok;"	//无效码
	"cmpb $0x02, %%dl;"
	"je get_byte_20;"	//若为无效码则尝试再读入1字节
	"jmp err;"
"byte_20_ok:"	//64
	"shrq $0x2, %%rdx;"
	"movb %%dl, %%cl;"
	"bswapq %%rcx;"
	"movq %%rcx, (%%rdi);"
	"addq $0x8, %%rdi;"
	"jmp get_byte_1;"
	
"err:"
	"movl $0xFFFFFFFF, %0;"
	"jmp quit;"

"no_20_byte:"	//31
	"movq %%rcx, %%rax;"
	"movb $0x7, %%cl;"
	"jmp save_byte;"
"no_1F_byte:"	//30
	"movq %%rdx, %%rax;"
	"movb $0x6, %%cl;"		//6=(n*6)%8
	"shlq $0x4, %%rax;"		//2=64-8-n*6,
	"jmp save_byte;"
"no_1E_byte:"	//29
	"shrq $0x6, %%rdx;"
"no_1D_byte:"	//28
	"movq %%rdx, %%rax;"
	"movb $0x5, %%cl;"
	"shlq $0x10, %%rax;"
	"jmp save_byte;"
"no_1C_byte:"	//27
	"shrq $0xA, %%rdx;"
	"bswapl %%edx;"
	"movl %%edx, (%%rdi);"
	"addq $0x4, %%rdi;"
	"jmp quit;"
"no_1B_byte:"	//26
	"shrq $0xC, %%rdx;"
	"movb %%dl, 0x2(%%rdi);"
	"movb %%dh, 0x1(%%rdi);"
	"shrl $0x8, %%edx;"
	"movb %%dh, (%%rdi);"
	"addq $0x3, %%rdi;"
	"jmp quit;"
"no_1A_byte:"	//25
	"shrq $0x6, %%rdx;"
"no_19_byte:"	//24
	"shrq $0x8, %%rdx;"
	"movb %%dh, (%%rdi);"
	"movb %%dl, 0x1(%%rdi);"
	"addq $0x2, %%rdi;"
	"jmp quit;"
"no_18_byte:"	//23
	"shrq $0x2, %%rdx;"
	"movb %%dh, (%%rdi);"
	"incq %%rdi;"
	"jmp quit;"
	
"no_16_byte:"	//21
	"movq %%rcx, %%rdx;"
"no_15_byte:"	//20
	"movq %%rdx, %%rax;"
	"movb $0x7, %%cl;"
	"jmp save_byte;"
"no_14_byte:"	//19
	"movq %%rdx, %%rax;"
	"movb $0x6, %%cl;"		//6=(n*6)%8
	"shlq $0x6, %%rax;"		//2=64-8-n*6,
	"jmp save_byte;"
"no_13_byte:"	//18
	"movq %%rdx, %%rax;"
	"movb $0x5, %%cl;"
	"shlq $0xC, %%rax;"
	"jmp save_byte;"
"no_12_byte:"	//17
	"shrq $0x6, %%rdx;"
"no_11_byte:"	//16
	"shrq $0x8, %%rdx;"
	"bswapl %%edx;"
	"movl %%edx, (%%rdi);"
	"addq $0x4, %%rdi;"
	"jmp quit;"
"no_10_byte:"	//15
	"shrq $0xA, %%rdx;"
	"movb %%dl, 0x2(%%rdi);"
	"movb %%dh, 0x1(%%rdi);"
	"shrl $0x8, %%edx;"
	"movb %%dh, (%%rdi);"
	"addq $0x3, %%rdi;"
	"jmp quit;"
"no_F_byte:"	//14
	"shrq $0xC, %%rdx;"
	"movb %%dh, (%%rdi);"
	"movb %%dl, 0x1(%%rdi);"
	"addq $0x2, %%rdi;"
	"jmp quit;"
"no_E_byte:"	//13
	"shrq $0x6, %%rdx;"
"no_D_byte:"	//12
	"movb %%dh, (%%rdi);"
	"incq %%rdi;"
	"jmp quit;"
	
"no_B_byte:"	//10
	"movq %%rcx, %%rax;"
	"movb $0x7, %%cl;"
	"jmp save_byte;"
"no_A_byte:"	//9
	"movq %%rdx, %%rax;"
	"movb $0x6, %%cl;"		//6=(n*6)%8
	"shlq $0x2, %%rax;"		//2=64-8-n*6,
	"jmp save_byte;"
"no_9_byte:"	//8
	"movq %%rdx, %%rax;"
	"movb $0x6, %%cl;"
	"shlq $0x8, %%rax;"
	"jmp save_byte;"
"no_8_byte:"	//7
	"movq %%rdx, %%rax;"
	"movb $0x5, %%cl;"
	"shlq $0x0E, %%rax;"
	"jmp save_byte;"
"no_7_byte:"	//6
	"shrq $0xC, %%rdx;"
	"bswapl %%edx;"
	"movl %%edx, (%%rdi);"
	"addq $0x4, %%rdi;"
	"jmp quit;"
"no_6_byte:"	//5
	"shrq $0x6, %%rdx;"
"no_5_byte:"	//4
	"movb %%dh, 0x2(%%rdi);"
	"shrl $0x10, %%edx;"
	"movb %%dh, (%%rdi);"
	"movb %%dl, 0x1(%%rdi);"
	"addq $0x3, %%rdi;"
	"jmp quit;"
"no_4_byte:"	//3
	"shrq $0xA, %%rdx;"
	"movb %%dh, (%%rdi);"
	"movb %%dl, 0x1(%%rdi);"
	"addq $0x2, %%rdi;"
	"jmp quit;"
"no_3_byte:"	//2
	"shrq $0x4, %%rdx;"
	"movb %%dh, (%%rdi);"
	"incq %%rdi;"
	"jmp quit;"

"save_byte:"
	"bswapq %%rax;"
"mov_1_by_1:"
	"movb %%al, (%%rdi);"
	"incq %%rdi;"
	"shrq $0x8, %%rax;"
	"decb %%cl;"
	"jne mov_1_by_1;"
	
"no_17_byte:"	//22
"no_C_byte:"	//11
"no_2_byte:"	//1
"no_1_byte:"	//0
"quit:"
	"movq %%rdi, %%rax;"
	"movq %5, %%r8;"
	"subq %2, %%rax;"
	"movq %%rax, (%%r8);"
	:"=m"(iRet)
	:"m"(aIn), "m"(aOut), "m"(aInLen), "m"(aOutSize), "m"(aOutLen), "i"(DecodeTable)
	:"%rax", "%rcx", "%rdx", "%rsi", "rdi", "%r8", "%r9"
	);
#endif

	return iRet;
}

int encode_qp(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen,int *nPreLineLeft)
{
	int i=0,j=0;
	unsigned char c;
	int nLineCnt=0;
	
	nLineCnt=*nPreLineLeft;
	while (i<aInLen && j<aOutSize)
	{
		c=(unsigned char)aIn[i];
		if ((c>=33 && c<=60) || (c>=62 && c<=126) || c==' ' || c=='\t')
		{
			aOut[j]=aIn[i];
			i++;j++;nLineCnt++;
		}
		else
		{
			
							  
			i++;
			snprintf(aOut+j,aOutSize-j,"=%02X",(int)c);
			j+=3;nLineCnt+=3;
			if (j>=aOutSize) j=aOutSize;													
					
			
		}
		if (nLineCnt>=76 && j<aOutSize)
		{
			
			snprintf(aOut+j,aOutSize-j,"=\r\n");
			nLineCnt=0;
			j+=3;
			if (j>=aOutSize) j=aOutSize;
		}
	}
	*aOutLen=j;*nPreLineLeft=nLineCnt;
	return 0;
}

int encode_qp_asm(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen)
{
	int iRet;
	const char* _aIn;
	size_t _aInLen;
	char* _aOut;
	size_t _aOutSize;
	size_t* _aOutLen;
	
	if(aIn==NULL || aOut==NULL || aIn==aOut || aInLen<=0 || aOutSize<=0) return -1;
	if(aOutSize<aInLen) return -2;
	
	//将变量强制放在内存中
	_aIn=aIn;
	_aInLen=aInLen;
	_aOut=aOut;
	_aOutSize=aOutSize;
	_aOutLen=aOutLen;
	
#ifdef __LP64__
	__asm__
	(
	"push %%rax;"
	"push %%rcx;"
	"push %%rbx;"
	"push %%rdx;"
	"push %%rsi;"
	"push %%rdi;"
	
	"movq %1, %%rsi;"	//源串地址
	"movq %2, %%rdi;"	//目标地址
	"movq %3, %%rcx;"	//源串长度
	"movq %4, %%rdx;"	//目标空间长度
	"movq %6, %%rbx;"	//编码表地址
	
	"xorq %%rax, %%rax;"
	"movl %%eax, %0;"
	"push %%rax;"

"start_enc_qp:"
	"decq %%rcx;"
	"jl finish_enc_qp;"
	"movb (%%rsi), %%al;"
	"incq %%rsi;"
	"cmpb $0x9, %%al;"
	"je normal_char;"
	"cmpb $0x20, %%al;"
	"je normal_char;"
	"jl encode;"
	"cmpb $0x3D, %%al;"
	"je encode;"
	"cmpb $0x7F, %%al;"
	"je encode;"

"normal_char:"
	"cmpq $0x4B, (%%rsp);"
	"jl store_1;"
	"testq %%rcx, %%rcx;"
	"jz store_1;"

	"subq $0x3, %%rdx;"
	"jl no_encode_space;"
	"movb $0x3D, (%%rdi);"
	"movw $0x0A0D, 0x1(%%rdi);"
	"addq $0x3, %%rdi;"
	"movq $0x0, (%%rsp);"
"store_1:"
	"decq %%rdx;"
	"jl no_encode_space;"
	"movb %%al, (%%rdi);"
	"incq %%rdi;"
	"incq (%%rsp);"
	"jmp start_enc_qp;"
	
"encode:"
	"cmpq $0x49, (%%rsp);"
	"jl qp_enc;"
	"jg CRLF;"
	"testq %%rcx, %%rcx;"
	"jz qp_enc;"
"CRLF:"
	"subq $0x3, %%rdx;"
	"jl no_encode_space;"
	"movb $0x3D, (%%rdi);"
	"movw $0x0A0D, 0x1(%%rdi);"
	"addq $0x3, %%rdi;"
	"movq $0x0, (%%rsp);"
"qp_enc:"
	"subq $0x3, %%rdx;"
	"jl no_encode_space;"
	"movb $0x3D, (%%rdi);"
	
	"cmpb $0x3D, %%al;"
	"jne not_equal_sign;"
	"movw $0x3344, %%ax;"
	"jmp store_3;"
"not_equal_sign:"
	"cmpb $0x7F, %%al;"
	"jne not_7F;"
	"movw $0x3746, %%ax;"
	"jmp store_3;"
"not_7F:"
	"xorb %%ah, %%ah;"
	"cmpb $0x0, %%al;"
	"jge small_ascii;"
	"shlb $0x1, %%al;"
	"movw (%%rbx,%%rax), %%ax;"
	"jmp store_3;"
"small_ascii:"
	"shlb $0x1, %%al;"
	"movw (%%rbx,%%rax), %%ax;"
	"subb $0x8, %%ah;"
"store_3:"
	"movb %%ah, 0x1(%%rdi);"
	"movb %%al, 0x2(%%rdi);"
	"addq $0x3, %%rdi;"
	"addq $0x3, (%%rsp);"
	"jmp start_enc_qp;"

"no_encode_space:"
	"subl $0x2, %0;"
"finish_enc_qp:"
	"pop %%rax;"
	"movq %%rdi, %%rax;"
	"movq %5, %%rbx;"
	"subq %2, %%rax;"
	"movq %%rax, (%%rbx);"
	
	"pop %%rdi;"
	"pop %%rsi;"
	"pop %%rdx;"
	"pop %%rbx;"
	"pop %%rcx;"
	"pop %%rax;"
	:"=m"(iRet)
	:"m"(_aIn), "m"(_aOut), "m"(_aInLen), "m"(_aOutSize), "m"(_aOutLen), "i"(QPEncodeTable)
	);
#else
	__asm__
	(
	"pusha;"
	
	"movl %1, %%esi;"	//源串地址
	"movl %2, %%edi;"	//目标地址
	"movl %3, %%ecx;"	//源串长度
	"movl %4, %%edx;"	//目标空间长度
	"movl %6, %%ebx;"	//编码表地址
	
	"xorl %%eax, %%eax;"
	"movl %%eax, %0;"
	"pushl %%eax;"

"start_enc_qp:"
	"decl %%ecx;"
	"jl finish_enc_qp;"
	"movb (%%esi), %%al;"
	"incl %%esi;"
	"cmpb $0x9, %%al;"
	"je normal_char;"
	"cmpb $0x20, %%al;"
	"je normal_char;"
	"jl encode;"
	"cmpb $0x3D, %%al;"
	"je encode;"
	"cmpb $0x7F, %%al;"
	"je encode;"

"normal_char:"
	"cmpl $0x4B, (%%esp);"
	"jl store_1;"
	"testl %%ecx, %%ecx;"
	"jz store_1;"

	"subl $0x3, %%edx;"
	"jl no_encode_space;"
	"movb $0x3D, (%%edi);"
	"movw $0x0A0D, 0x1(%%edi);"
	"addl $0x3, %%edi;"
	"movl $0x0, (%%esp);"
"store_1:"
	"decl %%edx;"
	"jl no_encode_space;"
	"movb %%al, (%%edi);"
	"incl %%edi;"
	"incl (%%esp);"
	"jmp start_enc_qp;"
	
"encode:"
	"cmpl $0x49, (%%esp);"
	"jl qp_enc;"
	"jg CRLF;"
	"testl %%ecx, %%ecx;"
	"jz qp_enc;"
"CRLF:"
	"subl $0x3, %%edx;"
	"jl no_encode_space;"
	"movb $0x3D, (%%edi);"
	"movw $0x0A0D, 0x1(%%edi);"
	"addl $0x3, %%edi;"
	"movl $0x0, (%%esp);"
"qp_enc:"
	"subl $0x3, %%edx;"
	"jl no_encode_space;"
	"movb $0x3D, (%%edi);"
	
	"cmpb $0x3D, %%al;"
	"jne not_equal_sign;"
	"movw $0x3344, %%ax;"
	"jmp store_3;"
"not_equal_sign:"
	"cmpb $0x7F, %%al;"
	"jne not_7F;"
	"movw $0x3746, %%ax;"
	"jmp store_3;"
"not_7F:"
	"xorb %%ah, %%ah;"
	"cmpb $0x0, %%al;"
	"jge small_ascii;"
	"shlb $0x1, %%al;"
	"movw (%%ebx,%%eax), %%ax;"
	"jmp store_3;"
"small_ascii:"
	"shlb $0x1, %%al;"
	"movw (%%ebx,%%eax), %%ax;"
	"subb $0x8, %%ah;"
"store_3:"
	"movb %%ah, 0x1(%%edi);"
	"movb %%al, 0x2(%%edi);"
	"addl $0x3, %%edi;"
	"addl $0x3, (%%esp);"
	"jmp start_enc_qp;"

"no_encode_space:"
	"subl $0x2, %0;"
"finish_enc_qp:"
	"popl %%eax;"
	"movl %%edi, %%eax;"
	"movl %5, %%ebx;"
	"subl %2, %%eax;"
	"movl %%eax, (%%ebx);"
	"popa;"
	:"=m"(iRet)
	:"m"(_aIn), "m"(_aOut), "m"(_aInLen), "m"(_aOutSize), "m"(_aOutLen), "i"(QPEncodeTable)
	);
#endif

	return iRet;
}

/*decode_qp_asm的返回值若非0时， 由下面情况经过or运算构成
* 0x01: 含有大于7F的字符
* 0x02: ＝ch这样的错误
* 0x04: 空间不足
* 0x08: 行末只有一个＝
* 0x10: 行末只有一个＝和一个字符
*/

int decode_qp_asm(const char* aIn, size_t aInLen, char* aOut,size_t aOutSize, size_t* aOutLen, char cEnd)
{
	int iRet;

	if(aIn==NULL || aOut==NULL || aInLen<=0 || aOutSize<=0) return -1;

#ifdef __LP64__
	__asm__
	(
	"movq %4, %%rdx;"	//目标空间长度
	"movq %7, %%rbx;"	//编码表地址
	
	"movq %1, %%r8;"
	"movq %2, %%r9;"
	"xorq %%rax, %%rax;"
	"movq %3, %%r10;"
	"movl %%eax, %0;"	//%0保存错误标志，0为没有错误

"pre_deal:"
	"movq %%r10, %%rcx;"	//r10为剩余待解码长度
	"movq %%r8, %%rsi;"	//源串地址，rsi保存本行起始地址
	"cmpq $0x0, %%rcx;"
	"movb $0x0A, %%al;"
	"jle finish_dec_qp;"
	"cld;"
	"movq %%rsi, %%rdi;"
	"repne scasb;"			//在剩余内容里查找\n
	"movq %%rcx, %%r10;"		//r10保存\n后剩余长度，若没有\n则剩余为0
	"movq %%rdi, %%r8;"		//r8保存下行起始地址，若没有\n则指向源串后1字节，不能再使用
	"jne not_0A;"			//没找到\n
	"decq %%rdi;"
"not_0A:"
	"decq %%rdi;"
	"cmpb $0x0D, (%%rdi);"
	"jne not_0D;"			//没找到\r
	"decq %%rdi;"
"not_0D:"
	"movq %%rdi, %%rcx;"		//以下操作使rcx保存本行\r、\n前的有效长度
	"subq %%rsi, %%rcx;"
	"incq %%rcx;"

	"std;"
"no_end_space:"
	"movb $0x20, %%al;"
	"repe scasb;"
	"je pre_deal;"			//退出时仍是20,说明整行结束
	"incq %%rdi;"
	"incq %%rcx;"
	"movb $0x09, %%al;"
	"repe scasb;"
	"je pre_deal;"			//退出时仍是09,说明整行结束
	"incq %%rdi;"
	"incq %%rcx;"
	"cmpb $0x20, (%%rdi);"
	"je no_end_space;"			//仍然是20,继续搜索
	
	"xorq %%r11, %%r11;"
	"cmpb $0x3D, (%%rdi);"
	"jne decode;"			//结尾不为"="，则开始解码
	"decq %%rcx;"
	"incq %%r11;"			//r11置1为软换行
	
"decode:"
	"movq %%r9, %%rdi;"	//取出目的地址
	//"cld;"

"get_char:"
	"decq %%rcx;"
	"jl line_end;"			//本行结束
	//"lodsb;"
	"movb (%%rsi), %%al;"
	"incq %%rsi;"
	"cmpb $0x0D, %%al;"
	"je get_char;"			//为0D则忽略
	
	"cmpb $0x3D, %%al;"
	"je equal_sign;"		//为=则跳去解码，否则复制这字节
	
	"decq %%rdx;"
	"jl no_decode_space;"
	//"stosb;"
	"movb %%al, (%%rdi);"
	"incq %%rdi;"
	"cmpb $0x7F, %%al;"
	"jbe get_char;"			//<=7F则不用置错误标志
	"orl $0x1, %0;"			//源中含有最高位为1(>7F)的字符
	"jmp get_char;"
	
"equal_sign:"
	"decq %%rcx;"
	"jge c1;"			//=不为行末则继续，否则填入=并对行末情况进行处理
	"decq %%rdx;"
	"jl no_decode_space;"
	//"stosb;"
	"movb %%al, (%%rdi);"
	"incq %%rdi;"
	"orl $0x8, %0;"			//=为行末
	"jmp line_end;"
"c1:"
	//"lodsb;"
	"movb (%%rsi), %%al;"
	"incq %%rsi;"
	"decq %%rcx;"
	"jge c2;"			//=后有两字节则继续，否则填入=及其后1字节，并对行末情况进行处理
	"subq $0x2, %%rdx;"
	"jl no_decode_space;"
	"movb $0x3D, (%%rdi);"
	"movb %%al, 0x1(%%rdi);"
	"addq $0x2, %%rdi;"
	"orl $0x10, %0;"		//行末为=后跟1个字节
	"jmp line_end;"
"c2:"
	///"xlatb;"
	"movb (%%rbx,%%rax), %%dl;"
	///"movb %%al, %%ah;"
	//"lodsb;"
	"movb (%%rsi), %%al;"
	"movb %%dl, %%dh;"
	"incq %%rsi;"
	///"xlatb;"
	"movb (%%rbx,%%rax), %%dl;"

	///"rorw $0x4, %%ax;"
	///"rorb $0x4, %%ah;"
	///"cmpb %%ah, %%al;"
	"rorw $0x4, %%dx;"
	"rorb $0x4, %%dh;"
	"cmpb %%dh, %%dl;"
	"je dec_ok;"			//相等表示为正确的编码方式，解码正确，否则复制=及其后2字节，并继续本行内容处理
	"subq $0x3, %%rdx;"
	"jl no_decode_space;"
	"movb $0x3D, (%%rdi);"
	"movw -0x2(%%rsi), %%dx;"
	//"stosw;"
	"movw %%dx, 0x1(%%rdi);"
	"addq $0x3, %%rdi;"
	"orl $0x2, %0;"			//=后两个字符中存在不在0-F之间的字符
	"jmp get_char;"
"dec_ok:"
	"decq %%rdx;"		//正常解码则把解码后的复制解码后的内容
	"jl no_decode_space;"
	//"stosb;"
	"movb %%dl, (%%rdi);"
	"incq %%rdi;"
	"jmp get_char;"
"line_end:"
	"testq %%r11, %%r11;"
	"jne soft_break;"	//软回车标志为1则为软回车
	"decq %%rdx;"
	"jl no_decode_space;"
	"movb $0x0A, (%%rdi);"
	"incq %%rdi;"
"soft_break:"
	"movq %%rdi, %%r9;"	//保存目的地址
	"jmp pre_deal;"
	
"finish_dec_qp:"
	"decq %%rdx;"		//填充最后1字节
	"movb %6, %%al;"
	"jl no_decode_space;"
	"movb %%al, (%%rdi);"
	"jmp quit_qp;"
	
"no_decode_space:"
	"orl $0x4, %0;"	//空间不足
"quit_qp:"
	//"movq %%rdi, %%rax;"
	//"movq %5, %%rbx;"
	//"subq %2, %%rax;"
	//"movq %%rax, (%%rbx);"
	
	"movq %5, %%rbx;"
	"subq %2, %%r9;"
	"movq %%r9, (%%rbx);"
	
	:"=m"(iRet)
	:"m"(aIn), "m"(aOut), "m"(aInLen), "m"(aOutSize), "m"(aOutLen), "m"(cEnd), "i"(QPDecodeTable)
	:"%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi", "%r8", "%r9", "%r10", "%r11"
	);
#else
	__asm__
	(
	"movl %4, %%edx;"	//目标空间长度
	"movl %7, %%ebx;"	//编码表地址
	
	"movl %2, %%eax;"
	"movl %%eax, %4;"	//%4保存目标原起始地址
	"xorl %%eax, %%eax;"
	"movl %%eax, %0;"	//%0保存错误标志，0为没有错误

"pre_deal:"
	"movl %3, %%ecx;"	//%3为剩余待解码长度
	"cmpl $0x0, %%ecx;"
	"jle finish_dec_qp;"
	"movl %1, %%esi;"	//源串地址，esi保存本行起始地址
	"movl %%esi, %%edi;"
	"cld;"
	"movb $0x0A, %%al;"
	"repne scasb;"			//在剩余内容里查找\n
	"movl %%ecx, %3;"		//%3保存\n后剩余长度，若没有\n则剩余为0
	"movl %%edi, %1;"		//%1保存下行起始地址，若没有\n则指向源串后1字节，不能再使用
	"jne not_0A;"			//没找到\n
	"decl %%edi;"
"not_0A:"
	"decl %%edi;"
	"cmpb $0x0D, (%%edi);"
	"jne not_0D;"			//没找到\r
	"decl %%edi;"
"not_0D:"
	"movl %%edi, %%ecx;"		//以下操作使rcx保存本行\r、\n前的有效长度
	"subl %%esi, %%ecx;"
	"incl %%ecx;"

	"std;"
"no_end_space:"
	"movb $0x20, %%al;"
	"repe scasb;"
	"je pre_deal;"			//退出时仍是20,说明整行结束
	"incl %%edi;"
	"incl %%ecx;"
	"movb $0x09, %%al;"
	"repe scasb;"
	"je pre_deal;"			//退出时仍是09,说明整行结束
	"incl %%edi;"
	"incl %%ecx;"
	"cmpb $0x20, (%%edi);"
	"je no_end_space;"			//仍然是20,继续搜索
	
	"xorl %%eax, %%eax;"
	"cmpb $0x3D, (%%edi);"
	"jne decode;"			//结尾不为"="，则开始解码
	"decl %%ecx;"
	"incl %%eax;"
	"rorl $0x1, %%eax;"		//eax第31bit置1为软换行
	
"decode:"
	"movl %2, %%edi;"	//取出目的地址
	"cld;"

"get_char:"
	"decl %%ecx;"
	"jl line_end;"			//本行结束
	"lodsb;"
	"cmpb $0x0D, %%al;"
	"je get_char;"			//为0D则忽略
	
	"cmpb $0x3D, %%al;"
	"je equal_sign;"		//为=则跳去解码，否则复制这字节
	
	"decl %%edx;"
	"jl no_decode_space;"
	"stosb;"
	"cmpb $0x7F, %%al;"
	"jbe get_char;"			//<=7F则不用置错误标志
	"orl $0x1, %0;"		//源中含有最高位为1(>7F)的字符
	"jmp get_char;"
	
"equal_sign:"
	"decl %%ecx;"
	"jge c1;"			//=不为行末则继续，否则填入=并对行末情况进行处理
	"decl %%edx;"
	"jl no_decode_space;"
	"stosb;"
	"orl $0x8, %0;"		//=为行末
	"jmp line_end;"
"c1:"
	"lodsb;"
	"decl %%ecx;"
	"jge c2;"			//=后有两字节则继续，否则填入=及其后1字节，并对行末情况进行处理
	"subl $0x2, %%edx;"
	"jl no_decode_space;"
	"movb %%al, %%ah;"
	"movb $0x3D, %%al;"
	"stosw;"
	"orl $0x10, %0;"		//行末为=后跟1个字节
	"jmp line_end;"
"c2:"
	"xlatb;"
	"movb %%al, %%ah;"
	"lodsb;"
	"xlatb;"

	"rorw $0x4, %%ax;"
	"rorb $0x4, %%ah;"
	"cmpb %%ah, %%al;"
	"je dec_ok;"			//相等表示为正确的编码方式，解码正确，否则复制=及其后2字节，并继续本行内容处理
	"subl $0x3, %%edx;"
	"jl no_decode_space;"
	"movb $0x3D, (%%edi);"
	"incl %%edi;"
	"movw -0x2(%%esi), %%ax;"
	"stosw;"
	"orl $0x2, %0;"		//=后两个字符中存在不在0-F之间的字符
	"jmp get_char;"
"dec_ok:"
	"decl %%edx;"		//正常解码则把解码后的复制解码后的内容
	"jl no_decode_space;"
	"stosb;"
	"jmp get_char;"
"line_end:"
	"testl $0x80000000, %%eax;"
	"jne soft_break;"	//软回车标志为1则为软回车
	"decl %%edx;"
	"jl no_decode_space;"
	"movb $0x0A, %%al;"
	"stosb;"
"soft_break:"
	"movl %%edi, %2;"	//保存目的地址
	"jmp pre_deal;"
	
"finish_dec_qp:"
	"decl %%edx;"		//填充最后1字节
	"movb %6, %%al;"
	"jl no_decode_space;"
	"movb %%al, (%%edi);"
	"jmp quit_qp;"
	
"no_decode_space:"
	"orl $0x4, %0;"		//空间不足
"quit_qp:"
	"movl %2, %%eax;"
	"movl %5, %%ebx;"
	"subl %4, %%eax;"
	"movl %%eax, (%%ebx);"
	
	:"=m"(iRet)
	:"m"(aIn), "m"(aOut), "m"(aInLen), "m"(aOutSize), "m"(aOutLen), "m"(cEnd), "i"(QPDecodeTable)
	:"%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi"
	);
#endif

	return iRet;
}

void InsertBase64LineBreak(char *sLine, int szLine,char *sOut, int szOut,int *pnPreLineLeft,int *pnLen)
{
	int i=0,j=0,k=0;
	
	*pnLen=szLine;
	while (i<szLine && j<szOut)
	{
		sOut[j]=sLine[i];
		i++;j++;k++;
		if (k==76)
		{
			k=0;*pnPreLineLeft=0;
			if (j<szOut) {sOut[j]='\r';j++;*pnLen=*pnLen+1;}
			if (j<szOut) {sOut[j]='\n';j++;*pnLen=*pnLen+1;}
		}
		else *pnPreLineLeft=*pnPreLineLeft+1;
	}
}
