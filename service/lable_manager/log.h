#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <string>
#include "lock.h"

#define LOG_TXT(x)  Log::instance()->log_text x
#define LOG_MSG(x)  Log::instance()->log_printf x
#define LOG_HEX_DUMP(x)  Log::instance()->log_hexdump x

class Log
{
public:
    static Log* instance();
    int set_path(const char* path);
    void set_maxfilesize(unsigned long bytes);
    void log_text(const char* format,...);
    void log_printf(const char* format, ...);
    int  log_hexdump (const char *buffer,size_t size, const char *text);
    ~Log();

protected:
    size_t format_hexdump (const char *buffer,size_t size,char *obuf,size_t obuf_sz);
    enum {TRUNC_SIZE = 0x01,TRUNC_DATE=0x02};

private:
    static Log* m_plog;
    static SyncLock m_loglock;
    FILE*   m_logstream;
    int     m_year,m_month,m_day;
    char    m_logfile[512];
    unsigned long m_max_bytes;
    static std::string m_logpath;

    Log();
    Log(Log &){};

    class Recycler{
    public:
        ~Recycler()
        {
            if (m_plog)
            {
                delete m_plog;
                m_plog = NULL;
            }
        }
    };
};

#endif
