#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <string>
#include <vector>
#include <list>

using namespace std;

typedef struct {
    int   sockfd;
    char* buf;
    int   size;
    int   offset;
}sockbuf_t;

extern void sockbuf_destroy(sockbuf_t* psb);

typedef struct{
    vector<string> name;
    vector<string> value;
} mail_args_t;

typedef struct {
    int ad;
    int isradom;
    int rand;
    int samefrom;
    string id;
    string helo;
    string pipe;
    string subject;
    string sender;
    string nickname;
    string smstype;
    string istest;
    string replier;
    string robot;
    string appendixname;
    string appendixtext;
    string modula;
    string modula_text;
    string modula_qvga;
    mail_args_t maillist;
}taskinf_t;

#endif
