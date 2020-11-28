#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "yxlist.h"
#include "writelog.h"

static YX_list_t* yx_head = NULL;
static pthread_mutex_t yx_mutex = PTHREAD_MUTEX_INITIALIZER;

YX_list_t* yx_create()
{
    YX_list_t* p = (YX_list_t*)malloc(sizeof(YX_list_t));
    if ( p )
        memset(p, 0x00,sizeof(YX_list_t));
    return p;
}

YX_list_t* yx_insert(YX_list_t* h, int corp_id, int task_id, int tmpl_id)
{
    if ( !h )
        return h;

    YX_list_t* p = (YX_list_t*)malloc(sizeof(YX_list_t));
    if ( !p )
        return p;

    p->corp_id = corp_id;
    p->task_id = task_id;
    p->tmpl_id = tmpl_id;
    p->reseverd = 0;
    p->next = h->next;
    h->next = p;
    return p;
}

YX_list_t* yx_insert_str(YX_list_t* h, const char* str)
{
    int corp_id = -1, task_id = -1, tmpl_id = -1;
    if (3 != sscanf(str, "%d.%d.%d", &corp_id, &task_id, &tmpl_id))
        return (YX_list_t*)0;

    return yx_insert(h,corp_id,task_id,tmpl_id);
}

YX_list_t* yx_find(YX_list_t* h, int corp_id, int task_id, int tmpl_id)
{
    while (h)
    {
        if (h->corp_id == corp_id 
            && h->task_id == task_id 
            && h->tmpl_id == tmpl_id){
            break;
        }

        h = h->next;
    }

    return h;
}

void yx_release(YX_list_t* h)
{
    YX_list_t *p;

    while ( p=h ){
        h = p->next;
        free(p);
    }
}

int yx_reload_list(const char* file, void *p1, void *p2)
{
    if ( !file)
        return -1;

    if (access(file,R_OK) != 0)
        return -1;

    static time_t yxtime = 0;
    struct stat yxstat;
    stat(file, &yxstat);

    if(yxstat.st_mtime == yxtime)
        return -1;

    yxtime = yxstat.st_mtime;

    FILE* fd = fopen(file, "r");
    if ( !fd )
        return -1;

    pthread_mutex_lock(&yx_mutex);

    yx_release(yx_head);
    yx_head = yx_create();

    YX_list_t* p = yx_head;
    char buf[64];

    while (fgets(buf, sizeof(buf), fd))
        p = yx_insert_str(p, buf);

    if (fd)
        fclose(fd);

    p = yx_head;
    char msg[256];
    while (p){        
        int b = snprintf(msg, sizeof(msg), 
            "youxian task -> %d.%d.%d\t%d\n",
            p->corp_id, p->task_id, p->tmpl_id, p->reseverd);
        //WriteLog((CONFIGURE *)p1, (DABUDAI *)p2, msg, b);

        p = p->next;
    }

    pthread_mutex_unlock(&yx_mutex);
    return 0;
}

// -1: non-youxian
// 
int yx_test(int corp_id, int task_id, int tmpl_id, int level)
{
    int ret = -1;
    pthread_mutex_lock(&yx_mutex);
    YX_list_t* p = yx_find(yx_head, corp_id,task_id,tmpl_id);

    do {
        if ( !p )
            break;

        if (level > p->reseverd)
            p->reseverd++;
        else
            p->reseverd = 0;
        ret = p->reseverd;
    }
    while (0);

    pthread_mutex_unlock(&yx_mutex);
    return ret;
}
