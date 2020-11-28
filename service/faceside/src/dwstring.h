#ifndef _DWSTRING_H_
#define _DWSTRING_H_

#include <string>
#include <vector>
#include "ngx_base64.h"

using namespace std;

#define TRIMSTR(s, ch) {\
    s.erase(0,s.find_first_not_of(ch) );\
    s.erase(s.find_last_not_of(ch) + 1);}

extern int dw_split(const char* line, char ch, vector<string> &result);
extern int dw_check_netaddr(const char* cp);
extern int dw_check_mailbox(const char* mail);
extern void dw_encode_base64(string& dst);
extern int dw_decode_base64(string& dst);
extern int dw_replace(string& , const char* , const char* );
extern void dw_line_wrap(string& text, int pos, char* str);
extern void dw_linefeed(string& text, int pos);
extern void dw_substr_base64_encode(string &text, const char* bgptr, const char* edptr);

#endif
