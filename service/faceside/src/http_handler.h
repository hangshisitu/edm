#ifndef _HTTP_HANDLER_H_
#define _HTTP_HANDLER_H_

#include <string>

using std::string;

enum {
    HTTP_NULL = 0,
    HTTP_GET  = 1,
    HTTP_POST = 2
};

typedef struct {
    int    method;
    string url;
    string version;
}http_request_t;

extern int parse_http_request_line(const char* line, http_request_t* req);
extern void get_html_text(string& html, string& text);

#endif
