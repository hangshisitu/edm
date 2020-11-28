/*        Copyright (c) 2004 Richinfo Inc, All Rights Reserved      */
/*        Author: wengshanjin                    Date: 2010-08      */

#ifndef RFC_DEFINE_H_201008
#define RFC_DEFINE_H_201008

#include <algorithm>
#include <string.h>


#ifdef NOT_USE_NAME_SPACE
#define RFC_NAMESPACE_NAME
#define RFC_NAMESPACE_BEGIN
#define RFC_NAMESPACE_END
#define USING_RFC_NAMESPACE
#else
#define RFC_NAMESPACE_NAME	rfc
#define RFC_NAMESPACE_BEGIN	namespace RFC_NAMESPACE_NAME {
#define RFC_NAMESPACE_END	}
#define USING_RFC_NAMESPACE using namespace RFC_NAMESPACE_NAME;
#endif

RFC_NAMESPACE_BEGIN

#if	defined(_WIN32) || defined(__WIN32__) || defined(WINDOWS)
	#define RFC_WIN32_VER
	typedef int socklen_t;
	#define SUPPORT_POSIX_SPINLOCK 0
#else
	#define RFC_LINUX_VER
	#define socketerrno	errno
#endif

typedef	char					rfc_int_8;
typedef	short					rfc_int_16;
typedef	int						rfc_int_32;
typedef	long long				rfc_int_64;

typedef unsigned char			rfc_uint_8;
typedef unsigned short			rfc_uint_16;
typedef unsigned int			rfc_uint_32;
typedef unsigned long long		rfc_uint_64;

typedef unsigned short			rfc_wchar;

typedef	int						typeSocketHandle;

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET -1
#endif

#ifndef INVALID_HANDLE
	#define INVALID_HANDLE -1
#endif

#ifndef INADDR_NONE
	#define INADDR_NONE 0xffffffff
#endif

#if !defined(ERRINTR) && defined(EINTR)
	#define ERRINTR	EINTR
#endif

#define RFC_CHAR_NULL	'\0'

#define RFC_MAX_PATH_LEN		1024

#define RFC_CHAR_PATH_SLASH		'/'

#ifndef IPC_MSG_DATA_SIZE
	#define IPC_MSG_DATA_SIZE 1024
#endif

#ifndef MAX_IP_STRING_LEN
	#define MAX_IP_STRING_LEN 48
#endif

#ifndef SUPPORT_MYSQL_DB
	#define SUPPORT_MYSQL_DB 0
#endif

#ifndef SUPPORT_ORACLE_DB
	#define SUPPORT_ORACLE_DB 0
#endif

#ifndef MAX_FD_SETSIZE
	#define MAX_FD_SETSIZE	1024
#endif

#ifndef SUPPORT_POSIX_SPINLOCK
	#define SUPPORT_POSIX_SPINLOCK 1
#endif

#ifndef SUPPORT_ICONV_ENCODER
	#define SUPPORT_ICONV_ENCODER 0
#endif

RFC_NAMESPACE_END

#endif	//RFC_DEFINE_H_201008
