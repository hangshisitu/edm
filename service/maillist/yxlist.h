#ifndef __YXLIST_H__
#define __YXLIST_H__

/* Copyright (c) All people */

#ifndef __cplusplus
typedef enum {false = 0, true = 1} bool;
#endif

typedef struct YX_list{
    int corp_id;
    int task_id;
    int tmpl_id;
    int reseverd;
    struct YX_list* next;
}YX_list_t;

//extern YX_list_t* yx_head;
static YX_list_t* yx_create();
static YX_list_t* yx_insert(YX_list_t* h, int corp_id, int task_id, int tmpl_id);
static YX_list_t* yx_insert_str(YX_list_t* h, const char* str);
static YX_list_t* yx_find(YX_list_t* h,int corp_id,int task_id,int tmpl_id);
static void yx_release(YX_list_t* h);

extern int yx_reload_list(const char* file, void* p1, void* p2);
extern int yx_test(int corp_id, int task_id, int tmpl_id, int level);

#endif

