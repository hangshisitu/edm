#include "dwstring.h"
#include <algorithm>
#include <string.h>

int dw_split(const char* line, char ch, vector<string> &result)
{
    char* start = (char*)line;

    while(1) {
        char* p = strchr(start, ch);

        if (!p) {
            result.push_back(start);
            return 0;
        }

        result.push_back(string(start, p-start));
        start = p + 1;
    }
    return 0;
}

int dw_check_netaddr(const char* cp)
{
    if (!cp)
        return -1;

    int i=0, dot=0;
    char buf[4] = {0,0,0,0};

    while (*cp != '\0'){
        if (isdigit(*cp)){
            if (i > 3)
                return -1;
            buf[i++] = *cp;
        }
        else if (*cp == '.'){
            if (++dot > 3)
                return -1;
            if ( i<1|| atoi(buf)>255)
                return -1;
            i = buf[1] = buf[2] = 0;
        }
        else
            return -1;
        cp++;
    }

    return  (dot!=3 || i<1 || atoi(buf)>255) ? -1:0;
}

int dw_check_mailbox(const char* mail)
{
    const char* flag = NULL;
    const char* p = mail;
    while (isalnum(*p) || *p=='.' || *p=='_' || *p=='-') ++p;
    //while (isprint(*p) && *p!=' ' && *p!='@') ++p;

    if (*p != '@' || p == mail)
        return -1;

    flag = p++; // '@'
    while (isalnum(*p) || *p=='-') ++p;

    if (*p != '.' || p == flag+1)
        return -1;

    flag = p++; // '.'
    while (isalnum(*p) || *p=='-') ++p;

    if (p == flag+1)
        return -1;

    while ( *p != '\0'){
        if (*p != '.')
            return -1;

        flag = p++; // '.'
        while (isalnum(*p) || *p=='-') ++p;

        if (p == flag+1)
            return -1;
    }

    return 0;
}

void dw_encode_base64(string& dst)
{
    if (dst.size() < 1)
        return;

    ngx_str_t d,s;
    s.len = dst.length();
    s.data = (u_char*)dst.c_str();
    d.len = (int)dst.length() * 2;
    d.data = new(std::nothrow) u_char[d.len];

    if (!d.data)
        return;

    ngx_encode_base64(&d, &s);
    dst.assign((char*)d.data, d.len);
    delete [] d.data;
}

int dw_decode_base64(string& dst)
{
    if (dst.size() < 1)
        return -1;

    ngx_str_t d,s;
    s.len = dst.length();
    s.data = (u_char*)dst.c_str();
    d.len = dst.length();
    d.data = new(std::nothrow) u_char[d.len];

    if (!d.data)
        return -1;

    int err = ngx_decode_base64(&d, &s);
    if (!err)
        dst.assign((char*)d.data, d.len);

    delete [] d.data;
    return err;
}

int dw_replace(string& text, const char* optr, const char* nptr)
{
    int num = 0;
    if (text.length() < 1 || !optr || !nptr)
        return num;

    string::size_type offset = 0;
    string::size_type idx = string::npos;
    int olen = strlen(optr);
    int nlen = strlen(nptr);
    while(offset < text.length()){
        idx = text.find(optr, offset);
        if (idx == string::npos)
            break;
        text.erase(idx, olen);
        text.insert(idx, nptr);
        offset = idx + nlen;
        num++;
    }
    return num;
}

void dw_line_wrap(string& text, int pos, char* str)
{
    if (!str || !(*str) || pos<1)
        return;
    int offset = 0;
    int L = strlen(str);
    while (offset+pos < (int)text.size()){
        text.insert(offset + pos, str);
        offset += (pos + L);
    }
}

void dw_linefeed(string& text, int pos)
{
    if (text.size() < 1 || pos < 1)
        return;

    int offset = 0;
    string::iterator it;
    static string crlf("\r\n");

    for (it=text.begin(); it!=text.end(); ){
        if (offset == pos){
            if (!std::equal (it, it+crlf.size(), crlf.begin()))
                text.insert(it, crlf.begin(),crlf.end());
            it += crlf.size();
            offset = 0;
        }
        else{
            switch (*it){
            case '\r':
            case '\n':
                it = text.erase(it);
                break;
            default:
                ++it;
                ++offset;
                break;
            }
        }
    }
}

void dw_substr_base64_encode(string &text, const char* bgptr, const char* edptr)
{
    string::size_type offset = 0;
    string::size_type p_start = 0;
    string::size_type p_end = string::npos;

    while(offset < text.length())
    {
        p_start = text.find(bgptr, offset);
        if (p_start == string::npos)
            return;

        p_end = text.find(edptr, p_start + strlen(bgptr));
        if (p_end == string::npos)
            return;

        string result = text.substr(p_start + strlen(bgptr), p_end - p_start - strlen(bgptr));
        dw_encode_base64(result);
	    text.erase(p_start, p_end - p_start + strlen(edptr));
	    text.insert(p_start, result);
        offset = (p_start + result.length());
    }

    return;
}

