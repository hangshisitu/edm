/* author: D.Wei 2014.03 Richinfo */
#ifndef __EDM_SMTP_H__
#define __EDM_SMTP_H__

#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <functional>
#include <sys/types.h>
#include <strings.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string>
#include <libgen.h>
#include <fstream>
#include <time.h>

using namespace std;

#define MAXHEADER       4096
#define EPOLL_SIZE      256
#define CRLF            "\r\n"

// define response message
#define ESMTP_220       "220 M.Target System" CRLF
#define ESMTP_221       "221 Bye" CRLF
#define ESMTP_235       "235 Authentication successful" CRLF
#define ESMTP_250       "250 OK" CRLF
#define ESMTP_U334      "334 VXNlcm5hbWU6" CRLF
#define ESMTP_P334      "334 UGFzc3dvcmQ6" CRLF
#define ESMTP_354       "354 End with <CRLF>.<CRLF>" CRLF
#define ESMTP_451       "451 Requested action aborted: local error in processing" CRLF
#define ESMTP_550       "550 Requested action not taken: hostname unavailable" CRLF
#define ESMTP_501       "501 Syntax error" CRLF
#define ESMTP_H501      "501 Syntax: HELO/EHLO hostname" CRLF
#define ESMTP_M501      "501 Syntax: MAIL FROM: <address>" CRLF
#define ESMTP_R501      "501 Syntax: RCPT TO: <address>" CRLF
#define ESMTP_502       "502 Error: Command not implemented" CRLF
#define ESMTP_503       "503 Error: Bad sequence of commands" CRLF
#define ESMTP_554       "554 Transaction failed: local error in processing" CRLF

#define LOG_MSG(x) log_write x

#ifdef DEBUG
# define LOG_DEBUG  LOG_MSG
#else
# define LOG_DEBUG(x)
#endif

#if !defined(DW_SYSIO)
# define dw_null     NULL
# define dw_file_t   FILE*
# define dw_fopen    fopen
# define dw_fclose   fclose
# define dw_fchmod(x,y,z) chmod(y,z)
# define dw_write(x,y,z)  fwrite(y, z, 1, x)
# define RW_FLAGS    "w"
#else
# define dw_null     -1
# define dw_file_t   int
# define dw_fopen    open
# define dw_fclose   close
# define dw_fchmod(x,y,z) fchmod(x,z)
# define dw_write    write
# define RW_FLAGS    (O_APPEND|O_CREAT|O_RDWR|O_TRUNC|O_NONBLOCK)
#endif

typedef struct {
    int offset;
    int length;
    char* buffer;
} msg_buf_t;

enum edmsmtp_forward {
    FW_NULL = 0x00,
    FW_HELO = 0x01,
    FW_EHLO = 0x02,
    FW_AUTH = 0x04,
    FW_PASS = 0x08,
    FW_MAIL = 0x10,
    FW_RCPT = 0x20,
    FW_DATA = 0x40,
    FW_RSET = 0x80,
    FW_NOOP = 0x100,
    FW_QUIT = 0x200,
    FW_EXIT = 0x400
};

enum edmsmtp_structure{
    EDMSMTP_HEADER = 0x01,
    EDMSMTP_BODY   = 0x02,
    EDMSMTP_END    = 0x04
};

#define DEFAULT_DOMAIN  "default"
#define DEFAULT_MAILBOX "default"
#define DEFAULT_ADDRESS  0

typedef struct {
    string backup;
    string filter;
    std::map<string, int> domain;
    std::map<string, int> mailbox;
    std::map<unsigned long int, int> address;
} storage_t;

#define RWBUF_SIZE 4096

typedef struct 
esmtp_commands {
    string helo;
    string user;
    string pass;
    string mail;
    string rcpt;
    string emlpath;
    u_long size;
} esmtp_commands_t;

typedef 
struct edmsmtp_ctx {
    int twid;
    int serial;
    int actime;
    int sockfd;
    int epolfd;
    int status;
    int forword;
    int follow;
    unsigned long int address;
    dw_file_t fdev;
    msg_buf_t rdbuf;
    msg_buf_t wtbuf;
    esmtp_commands_t cmds;
    edmsmtp_ctx* prev_ptr;
    edmsmtp_ctx* next_ptr;
} edmsmtp_ctx_t;

typedef  edmsmtp_ctx_t tw_node_t;

typedef 
struct {
    unsigned int size;
    tw_node_t* head_ptr;
    tw_node_t* tail_ptr;
} tw_part_t;

#endif

