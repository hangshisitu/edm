#include <stdarg.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include "logger.h"

const char* priority_names[] = {
    "LM_DEBUG",
    "LM_INFO",
    "LM_WARNING",
    "LM_ERROR"
};

#define LM2STRING(x) ((x) == LM_DEBUG ? "LM_DEBUG" : \
(x) == LM_INFO ? "LM_INFO":\
(x) == LM_WARNING ? "LM_WARNING":\
(x) == LM_ERROR ? "LM_ERROR":"LM_UNKNOWN")

#define test_date_equal(x,y) \
    ((x).tm_mday==(y).tm_mday \
    && (x).tm_mon==(y).tm_mon \
    && (x).tm_year==(y).tm_year) 

#define FILE_CLOSE(x) do{ if(x){fclose(x);x=NULL;} }while (0)

#define SET_24HMS(buf, x) \
    snprintf(buf, sizeof(buf), \
    ".%02d%02d%02d",\
    x.tm_hour,x.tm_min,x.tm_sec)

#define SET_YMD(buf, x) \
    snprintf(buf, sizeof(buf), \
        ".%04d%02d%02d",\
        x.tm_year+1900,\
        x.tm_mon+1,\
        x.tm_mday)

#define SET_PREFIX(buf,x,s,l) \
    snprintf(buf, sizeof(buf),\
        "[%02d:%02d:%02d] [%u]",\
        x.tm_hour,\
        x.tm_min,\
        x.tm_sec,\
        (unsigned int)pthread_self())

Logger* Logger::_logger = NULL;

Logger* Logger::instance()
{
    if ( !Logger::_logger )
        Logger::_logger = new (std::nothrow) Logger();

    return Logger::_logger;
}

Logger::Logger() 
{
    this->_mask = 0;
    this->_stream = NULL;
    this->_size = 0;
    this->_bytes = 0;
    memset(&(_logtm), 0x00, sizeof(_logtm) );
}

Logger::~Logger()
{
    this->close();
}

int Logger::open(const char* file, uint32_t mask, size_t size)
{
    if ( !file )
        return -1;

    FILE_CLOSE(_stream);

    this->_bytes = 0;
    this->_mask = mask;
    this->_size = size;
    this->_basefile.assign(file);
    this->_fullfile = this->_basefile;
    return 0;
}

void Logger::close()
{
    FILE_CLOSE(_stream);

    this->_mask = 0;
    this->_stream = NULL;
    this->_size = 0;
    this->_bytes = 0;
    this->_basefile.clear();
    this->_fullfile.clear();
    memset(&(_logtm), 0x00, sizeof(_logtm) );
    return;
}

int Logger::write(Log_Priority level, const char* format, ...)
{
    if ( !format || !(_mask&level) ) 
        return 0;

    tm xtm;
    timeval tv = {0,0};
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &xtm);

    int mode = 0;
    if ( !_stream )
        mode |= 1;
    else if (!test_date_equal(_logtm, xtm))
        mode |= 4;
    else if (_size && _bytes > _size)
        mode |= 2;

    static char prefix[256];

    if (mode)
    {
        this->_logtm = xtm;
        FILE_CLOSE(_stream);

        if (mode & 2){
            SET_24HMS(prefix, xtm);
            rename(_fullfile.c_str(), (_fullfile + prefix).c_str() );
        }

        SET_YMD(prefix, xtm);
        this->_fullfile = _basefile + prefix;
        this->_stream = fopen((_fullfile+".0000").c_str(), "a");

        if (this->_stream) {
            struct stat stb;
            fstat(fileno(_stream), &stb);
            this->_bytes = stb.st_size;
            fchmod(fileno(_stream), S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);
        }
    }

    if (this->_stream){
        va_list vptr;
        va_start(vptr, format);
        this->_bytes += SET_PREFIX(prefix, xtm, tv.tv_usec, level);
        fputs(prefix, _stream);
        this->_bytes += vfprintf(_stream,format,vptr);
        fflush(_stream);
        va_end(vptr);
    }

    return 0;
}

