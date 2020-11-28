#include "smtp_handler.h"
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "log_msg.h"
#include "socketopt.h"
#include "config.h"
#include "dwstring.h"

int smtp_getline(int sock, char* buffer, int len, int timeout)
{
    if (sock<1 || !buffer || --len<0)
        return (-1);

    buffer[0] = 0;
    buffer[len] = 0;
    int b_offset = 0;
    timeval ttl = {timeout, 0};

    while (b_offset < len )
    {
        int bytes = ::recv(sock, 
            (char*)buffer + b_offset, 
            len - b_offset,
            0);

        switch (bytes){
        case -1:
            if (errno == EINTR)
                break;
            else if (errno == EAGAIN){
                fd_set rds;
                FD_ZERO(&rds);
                FD_SET(sock,&rds);

                if (select(sock+1,&rds,0,0,&ttl) < 1
                    || !FD_ISSET(sock, &rds)) 
                    return -1;
                break;
            }
        case 0:
            return (-1);
        default:{
            int p_offset = 0;
            b_offset += bytes;

            do{
                char* pCRLF = (char*)memchr(buffer + p_offset, 
                    '\n', b_offset - p_offset);

                if ( !pCRLF )
                    break;

                char* p = buffer + p_offset;
                int L = pCRLF-(buffer+p_offset) + 1;

                if (L < 4 || !isdigit(p[0]) 
                    || !isdigit(p[1]) 
                    || !isdigit(p[2]) )
                    return -1;

                switch (p[3]){
                case '-': p_offset += L;
                    break;
                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    pCRLF[1] = 0;
                    memmove(buffer, buffer+p_offset, L+1);
                    return L;
                default: return -1;
                }

            } while (p_offset < b_offset);

            if ( b_offset > p_offset){
                memmove(buffer, buffer+b_offset, b_offset-p_offset);
                b_offset -= p_offset;
            }
            else
                b_offset = 0;

            break;}
        }
    }

    return (-1);
}

int smtp_post(const char* file, int refcnt)
{
    if ( !file || access(file,R_OK) != 0){
        LOG_MSG((LM_ERROR, "x,Can not read '%s'\n",file));
        return SMTP_FAIL;
    }

    int    err = 0;
    int    sockfd = 0;
    char   buf[256];
    string line;
    string command[3];
    string message;

    ifstream fs(file, ios_base::in);

    while (!fs.eof()) {

        buf[0] = 0;
        getline(fs,line,'\n');

        if ( !strncasecmp(line.c_str(), "MESSAGE:", 8) )
            message = line.substr(8);

        if ( !strncasecmp(line.c_str(), "HELO", 4) )
            command[0] = line;

        else if ( !strncasecmp(line.c_str(), "MAIL", 4) )
            command[1] = line;

        else if ( !strncasecmp(line.c_str(), "RCPT", 4) )
            command[2] = line;

        else if ( !strncasecmp(line.c_str(), "DATA", 4)){

            sockfd = sock_nbconnect(
                DW::smtp_address.c_str(), 
                DW::smtp_port,
                DW::smtp_timeout);

            if (sockfd < 1){
                err = SMTP_RETRY;
                break;
            }

            buf[0] = 0;
            err = smtp_getline(sockfd, buf, sizeof(buf), DW::smtp_timeout);
            if (err < 1){
                err = SMTP_FAIL;
                break;
            }

            if (buf[0] != '2'){
                err = (buf[0]=='4' || buf[0]=='6')
                    ? SMTP_RETRY : SMTP_FAIL;
                break;
            }

            // HELO MAIL RCPT 
            for (int i=0; i<(int)sizeof(command)/(int)sizeof(command[0]); i++)
            {
                err = send_until_edge( sockfd, 
                    (command[i] + "\n").c_str(), 
                    command[i].size()+1, 
                    DW::smtp_timeout );

                if((int)command[i].size()+1 != err){
                    err = SMTP_RETRY;
                    goto LAST_STEP;
                }

                buf[0] = 0;
                err = smtp_getline(sockfd, buf, sizeof(buf), DW::smtp_timeout);
                if (err < 1){
                    err = SMTP_RETRY;
                    goto LAST_STEP;
                }

                if (buf[0] != '2'){
                    err = (buf[0]=='4' || buf[0]=='6')
                        ? SMTP_RETRY : SMTP_FAIL;
                    goto LAST_STEP;
                }
            }

            // DATA
            line += "\n";
            err = send_until_edge( sockfd, line.c_str(), line.size(), DW::smtp_timeout);
            if((int)line.size() != err) {
                err = SMTP_RETRY;
                break;
            }

            buf[0] = 0;
            err = smtp_getline(sockfd, buf, sizeof(buf), DW::smtp_timeout);
            if (err < 1){
                err = SMTP_RETRY;
                break;
            }

            if (buf[0] != '3'){
                err = (buf[0]=='4' || buf[0]=='6')
                    ? SMTP_RETRY : SMTP_FAIL;
                break;
            }
            
            buf[0] = 0;
            while (!fs.eof()) {
                getline(fs,line,'\n');
                line += "\n";

                err = send_until_edge( sockfd, line.c_str(), line.size(),DW::smtp_timeout );
                if((int)line.size() != err){
                    err = SMTP_RETRY;
                    break;
                }
            }
            
            err = send_until_edge( sockfd, "\r\n.\r\n", 5, DW::smtp_timeout );
            if(5 != err){
                err = SMTP_RETRY;
                break;
            }

            buf[0] = 0;
            err = smtp_getline(sockfd, buf, sizeof(buf), DW::smtp_timeout);
            if (err < 1){
                err = SMTP_RETRY;
                break;
            }

            if (buf[0] != '2'){
                err = (buf[0]=='4' || buf[0]=='6')
                    ? SMTP_RETRY : SMTP_FAIL;
                break;
            }

            send_until_edge( sockfd, "QUIT\r\n", 6, DW::smtp_timeout);
            //smtp_getline(sockfd, buf, sizeof(buf), DW::smtp_timeout);
            err = SMTP_OK;
            break;
        }
    }

LAST_STEP:

    fs.close();
    if (sockfd){
        close(sockfd);
    }

    TRIMSTR(command[1], FILTER_STR);
    TRIMSTR(command[2], FILTER_STR);

    if (err == SMTP_FAIL){

        if (strcasestr(buf,"domain not exist"))
            err = SMTP_DNS_FAIL;

        else if (strcasestr(buf,"spam"))
            err = SMTP_SPAM;

        else if (strcasestr(buf,"user doesn")
            || strcasestr(buf,"not exist")
            || strcasestr(buf," unknown")
            || strcasestr(buf,"not found")
            || strcasestr(buf,"such user")
            || strcasestr(buf," unknown")
            || strcasestr(buf,"unavailable")
            || strcasestr(buf,"is disable")
            || strcasestr(buf,"ddress rejected")
            || strcasestr(buf,"such recipient")
            || strcasestr(buf,"no mailbox")
            || strcasestr(buf,"ccess denied"))
            err = SMTP_NONE_EXIST;
    }

    //V2.9.1
    if (err ==  SMTP_OK && message.length() > 0){
        char name[256];
        timeval tv;
        timerclear(&tv);
        gettimeofday(&tv,NULL);
        snprintf(name, sizeof(name),"%s_%ld%ld_%lu",message.substr(0,message.find("|")).c_str(),tv.tv_sec,tv.tv_usec,pthread_self());

        string oldpath = DW::sms_path+name;
        FILE* f = fopen(oldpath.c_str(),"w");
        if (f){
            fprintf(f, message.c_str());
            fclose(f);
            rename(oldpath.c_str(), (oldpath+".o").c_str());
        }        
    }//V2.9.1

    LOG_MSG((LM_INFO, "SMTP=%s|%d|%s|%s|%s|%s\n",
        SMTP_ERR_MSG(err),
        refcnt,
        file, 
        command[1].c_str(),
        command[2].c_str(),
        buf[0]?buf:"unknow"));

    return err;
}
