#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <string.h>
#include <errno.h>
#include "task_handler.h"
#include "dwstring.h"
#include "http_handler.h"
#include "config.h"
#include "log_msg.h"

#define BOUNDARY_B "$"
#define BOUNDARY_E "$"
#define TR_CLASS "__many__"

typedef vector<string> column_t;
typedef map<string, column_t> tables_t; 

// var
// 111|222|333|444|555
int table_first_step(string& vars, string& values, tables_t & tables)
{
    vector<string> data;
    if (vars.length()<1 || values.find("|")==string::npos)
        return -1;

    dw_split(values.c_str(), '|', data);
    //string v = BOUNDARY_B + vars + BOUNDARY_E;
    //string v = BOUNDARY_B + vars + BOUNDARY_E;
    //tables[v].insert(tables[v].end(), data.begin(), data.end());
    tables[vars] = data;
    return 0;
}

// <tr class="__many__"><td>$birthday$</td><td>$age$</td><td>$email$</td></tr> 
void table_second_step(string& html, tables_t & tables)
{
    if (html.length()<1 || tables.size()<1)
        return;

    unsigned int offset = 0;
    while (offset < html.length()){
        const char* haystack = html.c_str();
        char* begin = (char*)strcasestr(haystack + offset, "<tr ");
        if (!begin)
            break;

        char *b = begin + 4;
        while ( isblank(*b) )
            ++b;

        if (0 != strncasecmp(b, "class=", 6)){
            offset = b - haystack;
            continue;
        }

        b += 6;
        if (*b!='\'' && *b!='\"'){
            offset = b - haystack;
            continue;
        }
 
        string name;
        ++b;
        while ( *b!='\0' && *b!='\'' && *b!='\"'){
            name.append(1, *b);
            ++b;
        }

        if (name.length()<1 || name!=TR_CLASS){
            offset = b - haystack;
            continue;
        }

        if (!(b=strchr(b, '>')))
            break;

        ++b;
        while ( isblank(*b) )
            ++b;

        char* endtr = strcasestr(b, "</tr>");
        if (!endtr)
            break;
        
        endtr += 5;
        string example(begin, endtr - begin);
        string trs;
        for (unsigned int pos=0; ; pos++) {
            int count = 0;
            string tr = "\r\n" + example;
            tables_t::iterator itr = tables.begin();
            for ( ; itr!=tables.end(); ++itr){
                if (itr->second.size() > pos)
                    count += dw_replace(tr, itr->first.c_str(), itr->second[pos].c_str());
            }

            if (count < 1)
                break;
            trs += tr;
        }

        if (trs.length() > 0){
            html.erase(begin-haystack, example.length());
            html.insert(begin-haystack, trs.c_str());
            endtr = begin + trs.length();
        }

        offset = endtr - haystack;
    }
}

///=================================================================

int file_get_taskinf(const char* file, taskinf_t* task)
{
    if (!task)
        return -1;

    task->ad = 0;
    task->isradom = 0;
    task->rand = 0;
    task->samefrom = 0;
    task->smstype = "0";
    task->modula.reserve(MODULA_BUFSIZE);
    task->modula_qvga.reserve(MODULA_BUFSIZE);

    string line;
    ifstream fs(file, ios_base::in);
   
    while (!fs.eof()) {

        getline(fs,line,'\n');
        TRIMSTR(line, FILTER_STR);

        if(!strcasecmp(line.c_str(), "<$ID>")){
            getline(fs,task->id,'\n');
            getline(fs, line, '\n');
            TRIMSTR(task->id, FILTER_STR);
        }
        else if(!strcasecmp(line.c_str(), "<$AD>")){
            getline(fs, line, '\n');
            TRIMSTR(line, FILTER_STR);
            if (!strcasecmp(line.c_str(), "true"))
                task->ad = 1;
            getline(fs, line, '\n');
        }
        else if(!strcasecmp(line.c_str(), "<$HELO>")){
            getline(fs, task->helo, '\n');
            getline(fs, line, '\n');
            TRIMSTR(task->helo, FILTER_STR);
        }
        else if(!strcasecmp(line.c_str(), "<$ISRADOM>true")){ /// ****
            task->isradom = 1;
            getline(fs, line, '\n');
        }
        else if(!strcasecmp(line.c_str(), "<$ISRADOM>")){ // ****
            getline(fs, line, '\n');
            TRIMSTR(line, FILTER_STR);
            if (!strcasecmp(line.c_str(), "true"))
                task->isradom = 1;
            getline(fs, line, '\n');
        }
        else if(strcasecmp(line.c_str(), "<$RAND>") == 0){
            getline(fs, line, '\n');
            TRIMSTR(line, FILTER_STR);
            if (!strcasecmp(line.c_str(), "true"))
                task->rand = 1;
            getline(fs, line, '\n');
        }
        else if(strcasecmp(line.c_str(), "<$SAMEFROM>") == 0){
            getline(fs, line, '\n');
            TRIMSTR(line, FILTER_STR);
            if (!strcasecmp(line.c_str(), "true"))
                task->samefrom = 1;
            getline(fs, line, '\n');
        }
        else if(!strcasecmp(line.c_str(), "<$PIPE>")){
            getline(fs, task->pipe, '\n');
            getline(fs, line, '\n');
            TRIMSTR(task->pipe, FILTER_STR);
        }
        else if(!strcasecmp(line.c_str(), "<$SUBJECT>")){
            getline(fs, task->subject, '\n');
            getline(fs, line, '\n');
            TRIMSTR(task->subject, FILTER_STR);
        }
        else if(!strcasecmp(line.c_str(), "<$SENDER>")){
            getline(fs, task->sender, '\n');
            getline(fs, line, '\n');
            TRIMSTR(task->sender, FILTER_STR);
        }
        else if(!strcasecmp(line.c_str(), "<$NICKNAME>")){
            getline(fs, task->nickname, '\n');
            getline(fs, line, '\n');
            TRIMSTR(task->nickname, FILTER_STR);
        }
        else if(!strcasecmp(line.c_str(), "<$SMS>")){
            getline(fs, task->smstype, '\n');
            getline(fs, line, '\n');
            TRIMSTR(task->smstype, FILTER_STR);
        }
        else if(!strcasecmp(line.c_str(), "<$TEST>")){
            getline(fs, task->istest, '\n');
            getline(fs, line, '\n');
            TRIMSTR(task->istest, FILTER_STR);
        }
        else if(!strcasecmp(line.c_str(), "<$REPLIER>")){
            getline(fs, task->replier, '\n');
            getline(fs, line, '\n');
            TRIMSTR(task->replier, FILTER_STR);
        }
        else if(!strcasecmp(line.c_str(), "<$ROBOT>")){
            getline(fs, task->robot, '\n');
            getline(fs, line, '\n');
            TRIMSTR(task->robot, FILTER_STR);
        }
        else if(!strcasecmp(line.c_str(), "<$APPENDIXNAME>")){
            getline(fs, task->appendixname, '\n');
            getline(fs, line, '\n');
            TRIMSTR(task->appendixname, FILTER_STR);
        }
        else if(!strcasecmp(line.c_str(), "<$APPENDIX>")){

            while (!fs.eof()) {
                getline(fs, line, '\n');
                if (!strncasecmp(line.c_str(), "</$APPENDIX>", 12))
                    break;
                line += "\n";
                task->appendixtext += line;
            }
        }
        else if(!strcasecmp(line.c_str(), "<$MODULA_TEXT>")){

            while (!fs.eof()) {
                getline(fs, line, '\n');
                //line.erase(0,line.find_first_not_of(FILTER_STR) );
                if (!strncasecmp(line.c_str(), "</$MODULA_TEXT>", 15))
                    break;
                line += "\n";
                task->modula_text += line;
            }
        }
        else if(!strcasecmp(line.c_str(), "<$MODULA_QVGA>")){

            while (!fs.eof()) {
                getline(fs, line, '\n');
                //line.erase(0,line.find_first_not_of(FILTER_STR) );
                if (!strncasecmp(line.c_str(), "</$MODULA_QVGA>", 15))
                    break;
                line += "\n";
                task->modula_qvga += line;
            }
        }
        else if(!strcasecmp(line.c_str(), "<$MODULA>")){

            while (!fs.eof()) {
                getline(fs, line, '\n');
                //line.erase(0,line.find_first_not_of(FILTER_STR) );
                if (!strncasecmp(line.c_str(), "</$MODULA>", 10))
                    break;
                line += "\n";
                task->modula += line;
            }
        }
        else if(!strcasecmp(line.c_str(), "<$MAILLIST>")){
            getline(fs, line, '\n');
            TRIMSTR(line, FILTER_STR);
            dw_split(line.c_str(), ',', task->maillist.name);

            for (int i=0; i<(int)task->maillist.name.size(); i++)
                task->maillist.name[i] = VAR_FLAG + task->maillist.name[i] + VAR_FLAG;

            getline(fs, line, '\n');
            TRIMSTR(line, FILTER_STR);
            dw_split(line.c_str(), ',', task->maillist.value);

            while(!fs.eof()) {
                getline(fs, line, '\n');
                line.erase(0,line.find_first_not_of(FILTER_STR) );
                if (!strncasecmp(line.c_str(), "</$MAILLIST>", 12))
                    break;
            }
        }
        else;

    } //end of while (!fs.eof())

    fs.close();
    return 0;
}

int taskinf_dump(taskinf_t* task, const char* file)
{
    char tmpbuf[256];

    if (task->id.empty()){
        LOG_MSG((LM_ERROR, "<$ID> is empty\n"));
        return -1;
    }

    if (task->maillist.value.empty()){
        LOG_MSG((LM_ERROR, "<$MAILLIST>->value is empty\n"));
        return -1;
    }

    const char* rcpt = task->maillist.value[0].c_str();
    if (0 != dw_check_mailbox(rcpt)){
        LOG_MSG((LM_ERROR, "<$MAILLIST>->mailbox: bad address\n"));
        //return -1;
    }

    if (0 != dw_check_mailbox(task->sender.c_str())){
        LOG_MSG((LM_ERROR, "<$SENDER>: bad address\n"));
        //return -1;
    }

    //char* pat = strchr(rcpt, '@');
    //string neteasystr = "|@163.com|@126.com|@vip.163.com|@vip.126.com|@yeah.net|@188.com|";

    if (task->ad /*&& pat && strcasestr(neteasystr.c_str(), pat)*/)
        task->subject = "(AD)" + task->subject;

    if ( task->helo.empty())
        task->helo = DW::smtp_default_helo;

    if (0 != dw_check_mailbox(task->replier.c_str()))
        task->replier = DW::smtp_default_reply;

    if ( task->samefrom || 0 != dw_check_mailbox(task->robot.c_str()))
        task->robot = task->sender;
    
    unsigned int seedp = time(0);
    int randnum = rand_r(&seedp);
    snprintf(tmpbuf, sizeof(tmpbuf), "%d", randnum);

    task->maillist.name.push_back(VAR_FLAG "iedm_ran" VAR_FLAG);
    task->maillist.value.push_back(tmpbuf);

    if ( task->isradom && task->sender.size())
        task->sender.insert(task->sender.find('@'), tmpbuf);

    if (task->rand && task->robot.size())
        task->robot.insert(task->robot.find('@'), tmpbuf);

    task->maillist.name.push_back(VAR_FLAG "mailbox" VAR_FLAG);
    task->maillist.value.push_back(rcpt);

    string b64_rcpt = rcpt;
    dw_encode_base64(b64_rcpt);
    task->maillist.name.push_back(VAR_FLAG "iedm2_to" VAR_FLAG);
    task->maillist.value.push_back(b64_rcpt);

    task->maillist.name.push_back(VAR_FLAG "iedm_to" VAR_FLAG);
    task->maillist.value.push_back(rcpt);

    if (task->subject.find(VAR_FLAG) != string::npos) {
        for (int i=0; i<(int)task->maillist.name.size(); i++){
            if (i < (int)task->maillist.value.size())
                dw_replace(task->subject, task->maillist.name[i].c_str(),task->maillist.value[i].c_str());
        }
    }

    string encode_subject;
    string decode_subject;
    if (!task->subject.empty()){
        decode_subject = task->subject;
        dw_encode_base64(task->subject);
        encode_subject = task->subject;
        task->subject = "=?utf-8?B?" + task->subject + "?=";
    }

    if (task->nickname.empty())
        task->nickname = task->sender.substr(0, task->sender.find('@'));

    dw_encode_base64(task->nickname);

    tm lct;
    time_t tt = time(0);
    localtime_r(&tt, &lct);

    FILE* out = fopen(file, "w");
    if ( !out ){
        LOG_MSG((LM_ERROR, "%s:%s\n", file, strerror(errno)));
        return -1;
    }

    strftime( tmpbuf, sizeof(tmpbuf), "%a, %d %b %Y %H:%M:%S %z (%Z)", &lct);

    //V2.9.1
    if (task->smstype=="99"){
        string myrcpt(rcpt);
        string cellphone = myrcpt.substr(0,myrcpt.find("@"));
        string domain = myrcpt.substr(myrcpt.find("@")); 
        if (strcasecmp("@139.com", domain.c_str()) == 0 
            && cellphone.length() == 11 
            && cellphone.at(0) == '1' 
            && cellphone.find_first_not_of("1234567890") == string::npos){
            fprintf(out, "MESSAGE:%s|%s\r\n",cellphone.c_str(),decode_subject.c_str());
        }
    }//V2.9.1

    fprintf(out, "HELO %s\r\n"
        "MAIL FROM: <%s>\r\n"
        "RCPT TO: <%s>\r\n"
        "DATA\r\n", 
        task->helo.c_str(),
        task->robot.c_str(),
        rcpt);

    fprintf(out, "Received: from %s[127.0.0.1] (account %s)\r\n"
        "\tby Side.MTarget with SMTP for %s;\r\n"
        "\t%s\r\n"
        "From: \"=?utf-8?B?%s?=\" <%s>\r\n"
        "To: %s\r\n"
        "Subject: %s\r\n"
        "Date: %s\r\n"
        "Reply-To: <%s>\r\n"
        "X-mailer: iedm version 1.2\r\n"
        "MIME-Version: 1.0\r\n"
        "Message-ID: <%d.%d.%s>\r\n",
        task->helo.c_str(), task->robot.c_str(),
        rcpt,
        tmpbuf,
        task->nickname.c_str(), task->sender.c_str(),
        rcpt,
        task->subject.c_str(),
        tmpbuf,
        task->replier.c_str(),
        (int)time(0), randnum, rcpt);

    if (task->istest.length())
        fprintf(out, "ISTESTMAIL:%s\r\n",task->istest.c_str());

    fprintf(out, "X-RICHINFO:NOTIFYTYPE:%s;ISMULTITEMPLATE:%d;TEMPLATELIST:4\r\n",
        task->smstype=="99"?"0":task->smstype.c_str(),
        task->modula_qvga.length() ? 1:0);

    string content_type = "multipart/alternative";
    if (task->appendixname.length() && task->appendixtext.length())
        content_type = "multipart/mixed";

    fprintf(out, "Content-Type: %s;\r\n"
        "\tboundary=\"" BOUNDARY "\"\r\n\r\n"
        "This is a multi-part message in MIME format.\r\n\r\n", 
         content_type.c_str());

    if (task->modula_text.length() < 1)
        get_html_text(task->modula, task->modula_text);

    tables_t tables;
    for (int i=0; i<(int)task->maillist.name.size(); i++) {
        if (i < (int)task->maillist.value.size()){
            if (0 != table_first_step(task->maillist.name[i],task->maillist.value[i], tables)){
                dw_replace(task->modula, task->maillist.name[i].c_str(),task->maillist.value[i].c_str());
                dw_replace(task->modula_qvga, task->maillist.name[i].c_str(),task->maillist.value[i].c_str());
                dw_replace(task->modula_text, task->maillist.name[i].c_str(),task->maillist.value[i].c_str());
            }
        }
    }

    table_second_step(task->modula, tables);
    table_second_step(task->modula_qvga, tables);
    dw_substr_base64_encode(task->modula, "<$BASE64>", "</$BASE64>");
    dw_substr_base64_encode(task->modula_qvga, "<$BASE64>", "</$BASE64>");
    dw_substr_base64_encode(task->modula_text, "<$BASE64>", "</$BASE64>");

    if (task->modula_text.length()) {
        fprintf(out, "\r\n\r\n--" BOUNDARY "\r\n"
            "Content-Type: text/plain;\r\n"
            "\tcharset=\"utf-8\"\r\n"
            "Content-Transfer-Encoding: base64\r\n\r\n");

        dw_encode_base64(task->modula_text);
        dw_line_wrap(task->modula_text, 76, "\r\n");
        fprintf(out, task->modula_text.c_str());
    }

    if (task->modula_qvga.length()) {
        dw_encode_base64(task->modula_qvga);
        dw_line_wrap(task->modula_qvga, 76, "\r\n");
        //task->modula.append("\r\n<!--\r\n<RICHINFO>\r\n<WEB TYPE=\"QVGA\">");
        task->modula.append("\r\n<!--\r\n<RICHINFO>\r\n<NOTI TYPE=\"SMS\">");
        task->modula.append(encode_subject);
        task->modula.append("</NOTI>\r\n<NOTI TYPE=\"LONGSMS\">");
        task->modula.append(task->modula_text);
        task->modula.append("</NOTI>\r\n<WEB TYPE=\"QVGA\">");
        task->modula.append(task->modula_qvga);
        task->modula.append("</WEB>\r\n</RICHINFO>\r\n-->");
    }

    if (task->modula.length()){
        fprintf(out, "\r\n\r\n--" BOUNDARY "\r\n"
            "Content-Type: text/html;\r\n"
            "\tcharset=\"utf-8\"\r\n"
            "Content-Transfer-Encoding: base64\r\n\r\n");

        dw_encode_base64(task->modula);
        dw_line_wrap(task->modula, 76, "\r\n");
        fprintf(out, task->modula.c_str());
    }

    if (task->appendixname.length() && task->appendixtext.length()){

        dw_encode_base64(task->appendixname);
        fprintf(out, "\r\n\r\n--" BOUNDARY "\r\n"
            "Content-Type: application/octet-stream;\r\n"
            "\tcharset=\"utf-8\"\r\n"
            "\tname=\"=?utf-8?B?%s?=\"\r\n"
            "Content-Disposition: attachment;\r\n"
            "\tfilename=\"=?utf-8?B?%s?=\"\r\n"
            "Content-Transfer-Encoding: base64\r\n\r\n",
             task->appendixname.c_str(),
             task->appendixname.c_str());

        dw_linefeed(task->appendixtext, 76);
        fprintf(out, task->appendixtext.c_str());
    }

    fprintf(out, "\r\n\r\n--" BOUNDARY "--\r\n");
    fclose(out);
    return 0;
}

