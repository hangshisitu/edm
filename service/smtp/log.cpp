#include "log.h"
#include <stdarg.h>
#include <iostream>
#include <new>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#   include <direct.h>
#   include <WinBase.h>

#   define log_thread_id    ::GetCurrentThreadId()
#   define snprintf_t    _snprintf
#else
#   include <pthread.h>
#   define snprintf_t    snprintf
#   define log_thread_id    ::pthread_self()
#endif

#define LOG_PATH  "logs/"

Log* Log::m_plog = NULL;
SyncLock Log::m_loglock;

Log::Log()
:m_logstream(NULL),m_year(0),m_month(0),m_day(0),m_max_bytes(0)
{
	memset(m_logfile, 0x00, sizeof(m_logfile));
}

Log::~Log()
{
    if (m_logstream)
    {
        fclose(m_logstream);
    }
}

Log* Log::instance()
{
    if ( !Log::m_plog )
    {
        Log::m_plog = new (std::nothrow) Log();
    }

    return Log::m_plog;
}

//简单输出,不检测日切
void Log::log_text( const char* format,... )
{
    va_list vptr;
    va_start(vptr, format);
    Log::m_loglock.Lock();
    vfprintf(m_logstream,format,vptr);
    Log::m_loglock.UnLock();
    va_end(vptr);
    fflush(m_logstream);
}

void Log::log_printf( const char* format, ... )
{
    AutoGuard<SyncLock> tmp_lock(Log::m_loglock);

    time_t logtime_t;
    time( &logtime_t );
    tm* logtm = localtime( &logtime_t );
    static char text[256];

    int truncate = 0;

    if( access(m_logfile,W_OK)==0 && m_max_bytes > 0)
    {
        struct stat statbuff;
        if (stat(m_logfile, &statbuff) == 0)
            if (statbuff.st_size > m_max_bytes){
                truncate |= TRUNC_SIZE;
            }
    }

    if (logtm->tm_year != m_year || logtm->tm_mon != m_month || logtm->tm_mday != m_day)
    {
        m_year  = logtm->tm_year;
        m_month = logtm->tm_mon;
        m_day   = logtm->tm_mday;
        truncate |= TRUNC_DATE;
    }

    if (truncate)
    {
        if (m_logstream)
        {
            fclose(m_logstream);
            m_logstream = NULL;

            if (truncate & TRUNC_SIZE)
            {
                char newpath[512] = {0};
                snprintf_t(newpath, sizeof(newpath), "%s.%02d%02d%02d",Log::m_logfile,logtm->tm_hour,logtm->tm_min,logtm->tm_sec);
                rename(Log::m_logfile,newpath);
            }
        }
        
        memset(m_logfile, 0x00, sizeof(m_logfile));
        snprintf_t(m_logfile, sizeof(text), "%slog.%04d-%02d-%02d",
            Log::m_logpath.c_str(), logtm->tm_year + 1900,logtm->tm_mon + 1,logtm->tm_mday);

        m_logstream = fopen(m_logfile,"a+t");
        if ( !m_logstream){
            return;
        }
    }

    memset(text, 0x00, sizeof(text));
    snprintf_t(text, sizeof(text),"[%04d-%02d-%02d %02d:%02d:%02d][%u] ",
        logtm->tm_year + 1900,logtm->tm_mon + 1,logtm->tm_mday,logtm->tm_hour, logtm->tm_min,logtm->tm_sec,log_thread_id);

    va_list vptr;
    va_start(vptr, format);

    if (m_logstream)
    {
        fprintf(m_logstream,text);
        vfprintf(m_logstream,format,vptr);
    }

    va_end(vptr);
    fflush(m_logstream);

    return;
}

# if !defined (MAX_LOGMSG_LEN)
#   define MAX_LOGMSG_LEN (4 * 1024)
# endif 

int Log::log_hexdump (const char *buffer,size_t size, const char *text)
{
    size_t const buf_sz = MAX_LOGMSG_LEN - 128 - 58;

    char* buf = new(std::nothrow) char[buf_sz];
    if (!buf)
        return -1;

    const size_t text_sz = text ? strlen(text) : 0;
    char* msg_buf = new(std::nothrow) char[text_sz+58];
    if (!msg_buf)
        return -1;

    buf[0] = 0; // in case size = 0

    size_t const len = format_hexdump(buffer, size, buf, buf_sz / sizeof (char) - text_sz);
    int sz = 0;
    if (text)
        sz = sprintf (msg_buf,"%s - ",text);

    sz += sprintf (msg_buf + sz,"HEXDUMP %lu bytes",size);

    if (len < size)
        sprintf (msg_buf + sz," (showing first %lu bytes)",len);

    Log::m_loglock.Lock();
    if (m_logstream)
        fprintf(m_logstream,"%s\n%s",msg_buf,buf);
    Log::m_loglock.UnLock();

    delete [] msg_buf;
    delete [] buf;
    return 0;
}

size_t Log::format_hexdump (const char *buffer,size_t size,char *obuf,size_t obuf_sz)
{
    u_char c;
    char textver[16 + 1];
    size_t maxlen = (obuf_sz / 68) * 16;

    if (size > maxlen)
        size = maxlen;

    size_t i;
    size_t const lines = size / 16;
    for (i = 0; i < lines; i++)
    {
        size_t j;

        for (j = 0 ; j < 16; j++)
        {
            c = (u_char) buffer[(i << 4) + j];    // or, buffer[i*16+j]
            sprintf (obuf,"%02x ",c);
            obuf += 3;
            if (j == 7)
            {
                sprintf (obuf," ");
                ++obuf;
            }
            textver[j] = isprint (c) ? c : '.';
        }

        textver[j] = 0;
        sprintf (obuf,"  %s\n",textver);
        while (*obuf != '\0')
            ++obuf;
    }

    if (size % 16)
    {
        for (i = 0 ; i < size % 16; i++)
        {
            c = (u_char) buffer[size - size % 16 + i];
            sprintf (obuf,"%02x ",c);
            obuf += 3;
            if (i == 7)
            {
                sprintf (obuf," ");
                ++obuf;
            }
            textver[i] = isprint (c) ? c : '.';
        }

        for (i = size % 16; i < 16; i++)
        {
            sprintf (obuf,"   ");
            obuf += 3;
            if (i == 7)
            {
                sprintf (obuf," ");
                ++obuf;
            }
            textver[i] = ' ';
        }

        textver[i] = 0;
        sprintf (obuf,"  %s\n",textver);
    }
    return size;
}

int Log::set_path( const char* path )
{
    if ( !path){
        return -1;
    }

    Log::m_logpath = path;

    if (*Log::m_logpath.rbegin() != '/' /*&& m_logpath.rbegin() != '\\'*/)
    {
        Log::m_logpath.append("/");
    }

    return 0;
}

void Log::set_maxfilesize( unsigned long bytes )
{
    m_max_bytes = bytes;
}

std::string Log::m_logpath = LOG_PATH;
