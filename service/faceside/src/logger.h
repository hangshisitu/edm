/* Simple-Logger  */
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <string>
#include <time.h>
#include <stdint.h>

using std::string;

enum Log_Priority {
    LM_DEBUG   = 0x01,      /// Debug messages.
    LM_INFO    = 0x02,      /// Informational messages.
    LM_WARNING = 0x04,      /// Warning messages.
    LM_ERROR   = 0x08       /// Error messages.
};

class Logger
{
public:
    static Logger* instance();
    ~Logger();

    int open(const char* file, uint32_t mask = 0xFFFFFFFF, size_t size = 0);
    int write(Log_Priority priority, const char* format, ...);
    void close();

protected:

private:
    static Logger* _logger;

    struct Recycler{
      ~Recycler(){
         if ( Logger::_logger )
           delete Logger::_logger;
      }
    };

    Logger();
    Logger(Logger &) { /* NULL */ };

    uint32_t  _mask;
    string    _basefile;
    string    _fullfile;
    FILE*     _stream;
    size_t    _size;
    size_t    _bytes;
    struct tm _logtm;
};

#endif
