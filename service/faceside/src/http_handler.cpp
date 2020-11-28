#include "http_handler.h"
#include <string.h>

int parse_http_request_line(const char* line, http_request_t* req)
{
    const char* url = NULL;

    if (strncasecmp(line, "GET", 3) == 0){
        req->method = HTTP_GET;
        url = line + 3;
    }
    else if (strncasecmp(line, "POST", 4) == 0){
        req->method = HTTP_POST;
        url = line + 4;
    }
    else
        return -1;

    if ( !isblank(*url))
        return -1;

    while ( isblank(*url) )
        ++url;

    char* version = (char*)strchr(url, ' ');

    if ( !version )
        version = (char*)strchr(url, '\t');

    if ( !version )
        return -1;

    req->url.assign(url,  version-url);

    while ( isblank(*version) ) ++version;

    if (strncasecmp(version, "HTTP/",5) != 0)
        return -1;

    if ( !isdigit(version[5])
        || version[6] != '.'
        || !isdigit(version[7]))
        return -1;

    req->version.assign(version,8);

    char* next = version + 8;

    while ( *next != '\0' && *next!='\r' && *next!='\n'){
        if (!isblank(*next++))
            return -1;
    }

    return 0;
}

void get_html_text(string& html, string& text)
{
    int offset = 0;
    char* begin = (char*)strcasestr(html.c_str(), "<body>");

    if (begin)
        offset = begin - (char*)html.c_str();

    while (offset < (int)html.size()) {
        string::size_type pos = html.find('>', offset);
        if (pos == string::npos)
            break;

        begin = (char*)html.data() + pos + 1;
        while (isblank(*begin))
            begin++;

        char* end = strchr(begin, '<');
        if ( !end )
            break;

        if (end>begin)
            text.append(begin, end-begin);

        if ( !strncasecmp(end,"<br", 3) )
            text.append("\r\n");

        offset = end - (char*)html.c_str() + 1;
    }
    
    return;
}

