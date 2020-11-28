/* author: weide@richinfo.cn 2014/02 */
#ifndef DATA_STRUCT_H_201402
#define DATA_STRUCT_H_201402

#include <string>
#include <map>
#include "berkeleydb.h"

#define BDB_RESULT "1"
#define BDB_REGION "2"
#define BDB_DOMAIN "3"
#define BDB_URL    "4"
#define BDB_HOUR   "5"

using std::string;
using std::map;

/* KEY: corp_id,task_id,template_id */
typedef struct {
    int email_count;
    int sent_count; 
    int reach_count;
    int read_count; 
    int read_user_count; 
    int click_count;
    int click_user_count;
    int unsubscribe_count;
    int forward_count;
    int soft_bounce_count;
    int hard_bounce_count; 
    int none_exist_count; 
    int none_exist_count1;  
    int dns_fail_count;
    int spam_garbage_count;
    int begin_send_time;
    int end_send_time;
} result_t;

/* KEY: corp_id,task_id,template_id,region_id */
typedef struct {
    int read_count;
    int read_user_count;
    int click_count;
    int click_user_count;
    int unsubscribe_count;
    int forward_count;
} region_result_t;

/* KEY: corp_id,task_id,template_id,email_domain */
typedef struct {
    int email_count;
    int sent_count;
    int reach_count;
    int read_count;
    int read_user_count;
    int click_count;
    int click_user_count;
    int unsubscribe_count;
    int forward_count;
    int soft_bounce_count;
    int hard_bounce_count;
    int none_exist_count;
    int none_exist_count1;
    int dns_fail_count;
    int spam_garbage_count;
} domain_result_t;

/* KEY: corp_id,task_id,template_id,url */
typedef struct {
    int click_count;
} url_result_t;

/* KEY: corp_id,task_id,template_id,hour */
typedef struct {
    int read_count;
    int read_user_count;
    int click_count;
    int click_user_count;
} hour_result_t;

/* +++++ */
typedef struct {
    int  corp_id;
    int  task_id;
    int  template_id;
    char email[128];
    int  havior;
    int  osid;
    int  browserid;
    int  slangid;
    int  open;
    int  click;
    time_t updatetime;
    char * url;   //for 2.9.3
} stat_havior_t;

/*****************************************************/

extern FileLog          fileLog;
extern SafeDBConnection m_dbConn;
extern SafeDBStatement  m_dbStmt;
extern BerkeleyDB       bdbCache;
extern FILE*            fdHaviorDaily;
extern BerkeleyDB       bdbHaviorHistory;
extern table_name_t     g_tablename;

inline int execute_structured_query_language(const char* sqlstm)
{
    if ( !sqlstm )
        return -1;

    try {
        m_dbStmt = m_dbConn->getStatement(sqlstm);  
        m_dbStmt->execSQL();
    }
    catch (DBException & e){
        fileLog.writeLog(( FormatString("%\n").arg(e.what()).str() ));
        return -1;
    }

    return 0;
}

// Result
int update_cache_result(const char* key, result_t * rst )
{
    if ( !key || !rst){
        return -1;
    }

    result_t zxvf;
    memset( &zxvf, 0x00, sizeof(zxvf) );
    bdbCache.get(key, &zxvf, sizeof(zxvf));

    zxvf.email_count           += rst->email_count;
    zxvf.sent_count            += rst->sent_count;
    zxvf.reach_count           += rst->reach_count;
    zxvf.read_count            += rst->read_count; 
    zxvf.read_user_count       += rst->read_user_count; 
    zxvf.click_count           += rst->click_count;
    zxvf.click_user_count      += rst->click_user_count;
    zxvf.unsubscribe_count     += rst->unsubscribe_count;
    zxvf.forward_count         += rst->forward_count;
    zxvf.soft_bounce_count     += rst->soft_bounce_count;
    zxvf.hard_bounce_count     += rst->hard_bounce_count; 
    zxvf.none_exist_count      += rst->none_exist_count; 
    zxvf.none_exist_count1     += rst->none_exist_count1;  
    zxvf.dns_fail_count        += rst->dns_fail_count;
    zxvf.spam_garbage_count    += rst->spam_garbage_count;
    zxvf.begin_send_time       += rst->begin_send_time;
    zxvf.end_send_time         += rst->end_send_time;

    bdbCache.put(key, strlen(key), &zxvf, sizeof(zxvf));
    return 0;
}

//Region
int update_cache_region_result(const char* key, region_result_t * rst)
{
    if ( !key || !rst)
        return -1;

    region_result_t zxvf;
    memset( &zxvf, 0x00, sizeof(zxvf) );
    bdbCache.get(key, &zxvf, sizeof(zxvf));

    zxvf.read_count            += rst->read_count; 
    zxvf.read_user_count       += rst->read_user_count; 
    zxvf.click_count           += rst->click_count;
    zxvf.click_user_count      += rst->click_user_count;
    zxvf.unsubscribe_count     += rst->unsubscribe_count;
    zxvf.forward_count         += rst->forward_count;

    bdbCache.put(key, strlen(key), &zxvf, sizeof(zxvf));
    return 0;
}

//Domain
int update_cache_domain_result(const char* key, domain_result_t * rst)
{
    if ( !key || !rst)
        return -1;

    domain_result_t zxvf;
    memset( &zxvf, 0x00, sizeof(zxvf) );
    bdbCache.get(key, &zxvf, sizeof(zxvf));

    zxvf.email_count           += rst->email_count;
    zxvf.sent_count            += rst->sent_count;
    zxvf.reach_count           += rst->reach_count;
    zxvf.read_count            += rst->read_count; 
    zxvf.read_user_count       += rst->read_user_count; 
    zxvf.click_count           += rst->click_count;
    zxvf.click_user_count      += rst->click_user_count;
    zxvf.unsubscribe_count     += rst->unsubscribe_count;
    zxvf.forward_count         += rst->forward_count;
    zxvf.soft_bounce_count     += rst->soft_bounce_count;
    zxvf.hard_bounce_count     += rst->hard_bounce_count; 
    zxvf.none_exist_count      += rst->none_exist_count; 
    zxvf.none_exist_count1     += rst->none_exist_count1;  
    zxvf.dns_fail_count        += rst->dns_fail_count;
    zxvf.spam_garbage_count    += rst->spam_garbage_count;

    bdbCache.put(key, strlen(key), &zxvf, sizeof(zxvf));
    return 0;
}


//Url
int update_cache_url_result(const char* key, url_result_t * rst)
{
    if ( !key || !rst)
        return -1;

    url_result_t zxvf;
    memset( &zxvf, 0x00, sizeof(zxvf) );
    bdbCache.get(key, &zxvf, sizeof(zxvf));
    zxvf.click_count += rst->click_count;
    bdbCache.put(key, strlen(key), &zxvf, sizeof(zxvf));
    return 0;
}

//Hour
int update_cache_hour_result(const char* key, hour_result_t * rst)
{
    if ( !key || !rst)
        return -1;

    hour_result_t zxvf;
    memset( &zxvf, 0x00, sizeof(zxvf) );
    bdbCache.get(key, &zxvf, sizeof(zxvf));

    zxvf.read_count       += rst->read_count;
    zxvf.read_user_count  += rst->read_user_count;
    zxvf.click_count      += rst->click_count;
    zxvf.click_user_count += rst->click_user_count;

    bdbCache.put(key, strlen(key), &zxvf, sizeof(zxvf));
    return 0;
}

// Stat_havior
int update_cache_stat_havior(const char* table, stat_havior_t * rst)
{
    if ( !table || !rst)
        return -1;

    char buf[1024] = {0};
    snprintf(buf, sizeof(buf), 
        "insert into %s ("
        " corp_id"
        ",task_id"
        ",template_id"
        ",email"
        ",havior"
        ",osid"
        ",browserid"
        ",slangid"
        ",open"
        ",click"
        ",updatetime"
        ",url) "   // add by WD at 2015-10-26 for V2.9.3
        " values (%d,%d,%d,'%s',%d,%d,%d,%d,%d,%d,from_unixtime(%d),'%s')\n",  // for 2.9.3
        table,
        rst->corp_id,
        rst->task_id,
        rst->template_id,
        rst->email,
        rst->havior,
        rst->osid,
        rst->browserid, 
        rst->slangid,
        rst->open,
        rst->click,
        rst->updatetime,
        rst->url?rst->url:"");//for 2.9.3

    if ( fdHaviorDaily ) {
        fprintf(fdHaviorDaily,buf);
        fflush(fdHaviorDaily);
    } 
    else {
        //写入失败,手工扫描日志补数据
        fileLog.writeLog( FormatString("write havior: %").arg(buf).str() );
    }

    return 0;
}


/****************************************************************/

//Result DB
int commit_cache_result(const char* key, result_t* rst, const char* table)
{
    char buf[2048] = {0};
    
    if (rst->email_count > 0 
        || rst->sent_count > 0
        || rst->reach_count > 0
        || rst->read_count > 0 
        || rst->read_user_count > 0 
        || rst->click_count > 0
        || rst->click_user_count > 0
        || rst->unsubscribe_count > 0
        || rst->forward_count > 0
        || rst->soft_bounce_count > 0
        || rst->hard_bounce_count > 0
        || rst->none_exist_count > 0
        || rst->none_exist_count1 > 0
        || rst->dns_fail_count > 0
        || rst->spam_garbage_count > 0
        || rst->end_send_time > 0) {

        memset(buf, 0x00, sizeof(buf) );

        snprintf(buf, sizeof(buf)-1, 
            "insert into %s (corp_id,task_id,template_id"
            ",email_count"
            ",sent_count"
            ",reach_count"
            ",read_count"
            ",read_user_count"
            ",click_count"
            ",click_user_count"
            ",unsubscribe_count"
            ",forward_count"
            ",soft_bounce_count"
            ",hard_bounce_count"
            ",none_exist_count"
            ",none_exist_count1"
            ",dns_fail_count"
            ",spam_garbage_count"
            ",begin_send_time"
            ",end_send_time) "
            " values (%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,now(),%s) "
            " ON DUPLICATE KEY UPDATE "
            " email_count = email_count + %d"
            ",sent_count = sent_count + %d"
            ",reach_count = reach_count + %d"
            ",read_count = read_count + %d"
            ",read_user_count = read_user_count + %d"
            ",click_count = click_count + %d"
            ",click_user_count = click_user_count + %d"
            ",unsubscribe_count = unsubscribe_count + %d"
            ",forward_count = forward_count + %d"
            ",soft_bounce_count = soft_bounce_count + %d"
            ",hard_bounce_count = hard_bounce_count + %d"
            ",none_exist_count = none_exist_count + %d"
            ",none_exist_count1 = none_exist_count1 + %d"
            ",dns_fail_count = dns_fail_count + %d"
            ",spam_garbage_count = spam_garbage_count + %d"
            ",begin_send_time = %s"
            ",end_send_time = %s", 
            
            //insert line
            table,
            key,
            rst->email_count, 
            rst->sent_count,
            rst->reach_count,
            rst->read_count,
            rst->read_user_count, 
            rst->click_count,
            rst->click_user_count,
            rst->unsubscribe_count,
            rst->forward_count,
            rst->soft_bounce_count,
            rst->hard_bounce_count,
            rst->none_exist_count,
            rst->none_exist_count1,
            rst->dns_fail_count,
            rst->spam_garbage_count,
            //rst->begin_send_time>0 ? "now()":"NULL",
            rst->end_send_time>0 ? "now()":"NULL",

            // ON DUPLICATE KEY UPDATE line
            rst->email_count, 
            rst->sent_count,
            rst->reach_count,
            rst->read_count,
            rst->read_user_count, 
            rst->click_count,
            rst->click_user_count,
            rst->unsubscribe_count,
            rst->forward_count,
            rst->soft_bounce_count,
            rst->hard_bounce_count,
            rst->none_exist_count,
            rst->none_exist_count1,
            rst->dns_fail_count,
            rst->spam_garbage_count,
            rst->begin_send_time>0 ? "now()":"begin_send_time",
            rst->end_send_time>0 ? "now()":"end_send_time" );

        return execute_structured_query_language(buf);
    }

    return -1;
}

//Region DB
int commit_cache_region_result(const char* key, region_result_t* rst, const char* table)
{
    char buf[1024] = {0};

    if (rst->read_count > 0 
        || rst->read_user_count > 0 
        || rst->click_count > 0
        || rst->click_user_count > 0
        || rst->unsubscribe_count > 0
        || rst->forward_count > 0) {

        memset(buf, 0x00, sizeof(buf) );

        snprintf(buf, sizeof(buf)-1, 
            "insert into %s (corp_id,task_id,template_id,region_id"
            ",read_count"
            ",read_user_count"
            ",click_count"
            ",click_user_count"
            ",unsubscribe_count"
            ",forward_count) "
            " values (%s,%d,%d,%d,%d,%d,%d) "
            " ON DUPLICATE KEY UPDATE "
            " read_count = read_count + %d"
            ",read_user_count = read_user_count + %d"
            ",click_count = click_count + %d"
            ",click_user_count = click_user_count + %d"
            ",unsubscribe_count = unsubscribe_count + %d"
            ",forward_count = forward_count + %d",
            
            //insert
            table,
            key,
            rst->read_count,
            rst->read_user_count, 
            rst->click_count,
            rst->click_user_count,
            rst->unsubscribe_count,
            rst->forward_count,

            // ON DUPLICATE KEY UPDATE 
            rst->read_count,
            rst->read_user_count, 
            rst->click_count,
            rst->click_user_count,
            rst->unsubscribe_count,
            rst->forward_count);

        return execute_structured_query_language(buf);
    }

    return -1;
}

//Domain DB
int commit_cache_domain_result(const char* key, domain_result_t* rst, const char* table)
{
    char buf[2048] = {0};

    if (rst->email_count > 0 
        || rst->sent_count > 0
        || rst->reach_count > 0
        || rst->read_count > 0 
        || rst->read_user_count > 0 
        || rst->click_count > 0
        || rst->click_user_count > 0
        || rst->unsubscribe_count > 0
        || rst->forward_count > 0
        || rst->soft_bounce_count > 0
        || rst->hard_bounce_count > 0
        || rst->none_exist_count > 0
        || rst->none_exist_count1 > 0
        || rst->dns_fail_count > 0
        || rst->spam_garbage_count > 0 ) {

        memset(buf, 0x00, sizeof(buf) );

        snprintf(buf, sizeof(buf)-1, 
            "insert into %s (corp_id,task_id,template_id,email_domain"
            ",email_count"
            ",sent_count"
            ",reach_count"
            ",read_count"
            ",read_user_count"
            ",click_count"
            ",click_user_count"
            ",unsubscribe_count"
            ",forward_count"
            ",soft_bounce_count"
            ",hard_bounce_count"
            ",none_exist_count"
            ",none_exist_count1"
            ",dns_fail_count"
            ",spam_garbage_count) "
            " values (%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d) "
            " ON DUPLICATE KEY UPDATE "
            " email_count = email_count + %d"
            ",sent_count = sent_count + %d"
            ",reach_count = reach_count + %d"
            ",read_count = read_count + %d"
            ",read_user_count = read_user_count + %d"
            ",click_count = click_count + %d"
            ",click_user_count = click_user_count + %d"
            ",unsubscribe_count = unsubscribe_count + %d"
            ",forward_count = forward_count + %d"
            ",soft_bounce_count = soft_bounce_count + %d"
            ",hard_bounce_count = hard_bounce_count + %d"
            ",none_exist_count = none_exist_count + %d"
            ",none_exist_count1 = none_exist_count1 + %d"
            ",dns_fail_count = dns_fail_count + %d"
            ",spam_garbage_count = spam_garbage_count + %d", 
            
            //insert line
            table,
            key,
            rst->email_count, 
            rst->sent_count,
            rst->reach_count,
            rst->read_count,
            rst->read_user_count, 
            rst->click_count,
            rst->click_user_count,
            rst->unsubscribe_count,
            rst->forward_count,
            rst->soft_bounce_count,
            rst->hard_bounce_count,
            rst->none_exist_count,
            rst->none_exist_count1,
            rst->dns_fail_count,
            rst->spam_garbage_count,

            // ON DUPLICATE KEY UPDATE
            rst->email_count, 
            rst->sent_count,
            rst->reach_count,
            rst->read_count,
            rst->read_user_count, 
            rst->click_count,
            rst->click_user_count,
            rst->unsubscribe_count,
            rst->forward_count,
            rst->soft_bounce_count,
            rst->hard_bounce_count,
            rst->none_exist_count,
            rst->none_exist_count1,
            rst->dns_fail_count,
            rst->spam_garbage_count );

        return execute_structured_query_language(buf);
    }

    return -1;
}

//Url DB
int commit_cache_url_result(const char* key, url_result_t* rst, const char* table)
{
    char buf[512] = {0};

    if (rst->click_count > 0) {

        memset(buf, 0x00, sizeof(buf) );

        snprintf(buf, sizeof(buf)-1, 
            "insert into %s (corp_id,task_id,template_id,url,click_count) "
            " values (%s,%d) "
            " ON DUPLICATE KEY UPDATE "
            " click_count = click_count + %d",
            table,
            key,
            rst->click_count,
            rst->click_count);

        return execute_structured_query_language(buf);
    }

    return -1;
}

//Hour DB
int commit_cache_hour_result(const char* key,  hour_result_t *rst, const char* table)
{
    char buf[1024] = {0};

    if (rst->read_count > 0
        || rst->read_user_count > 0
        || rst->click_count > 0
        || rst->click_user_count > 0) {

        memset(buf, 0x00, sizeof(buf) );

        snprintf(buf, sizeof(buf)-1, 
            "insert into %s (corp_id,task_id,template_id,hour"
            ",read_count"
            ",read_user_count"
            ",click_count"
            ",click_user_count) "
            " values (%s,%d,%d,%d,%d) ON DUPLICATE KEY UPDATE "
            " read_count = read_count + %d"
            ",read_user_count = read_user_count + %d"
            ",click_count = click_count + %d"
            ",click_user_count = click_user_count + %d", 
            table,
            key,
            rst->read_count,
            rst->read_user_count, 
            rst->click_count,
            rst->click_user_count,
                
            rst->read_count,
            rst->read_user_count, 
            rst->click_count,
            rst->click_user_count);

        return execute_structured_query_language(buf);
    }

    return -1;
}

/* havior history */
typedef struct {
    time_t time;
    int reserve;
} havior_history_t;

int find_havior_history(const char* key, bool merge = true)
{
    havior_history_t zxvf;
    zxvf.reserve = 0;

    int retval = bdbHaviorHistory.get(key, &zxvf, sizeof(zxvf));

    if ( retval != 0 && merge) {
        zxvf.time = ::time(0);
        zxvf.reserve ++;
        bdbHaviorHistory.put(key, strlen(key), &zxvf, sizeof(zxvf));
    }

    return retval;
}

typedef struct {
    char haviortable[128];
    unsigned int result_fails;
    unsigned int result_successful;
    unsigned int region_fails;
    unsigned int region_successful;
    unsigned int domain_fails;
    unsigned int domain_successful;
    unsigned int url_fails;
    unsigned int url_successful;
    unsigned int hour_fails;
    unsigned int hour_successful;
} traversal_result_t;

void* pf_submit_select(const void* key, unsigned int kl, const void* data, unsigned int dl, void* param)
{
    if ( !key || kl<1 || !data || dl<1 || !param){
        return (void*)-1;
    }

    int retval = 0;
    traversal_result_t* prst = (traversal_result_t*)param;
    string skey((char*)key + 1, kl - 1);

    switch ( *(char*)key ){

    case '1':
        commit_cache_result(skey.c_str(), 
                            (result_t*)data, 
                            g_tablename.tb_result.c_str()) == 0 ? 
        prst->result_successful++:prst->result_fails++;
        break;

    case '2':
        commit_cache_region_result(skey.c_str(), 
                                   (region_result_t*)data, 
                                   g_tablename.tb_result_by_region.c_str()) == 0 ?
        prst->region_successful++:prst->region_fails++;
        break;

    case '3':
        commit_cache_domain_result(skey.c_str(), 
                                   (domain_result_t*)data, 
                                   g_tablename.tb_result_by_domain.c_str()) == 0 ?
        prst->domain_successful++:prst->domain_fails++;
        break;

    case '4':
        commit_cache_url_result(skey.c_str(), 
                                (url_result_t*)data, 
                                g_tablename.tb_result_by_url.c_str()) == 0 ?
        prst->url_successful++:prst->url_fails++;
        break;

    case '5':
        commit_cache_hour_result(skey.c_str(), 
                                 (hour_result_t*)data, 
                                 g_tablename.tb_result_by_hour.c_str()) == 0 ?
        prst->hour_successful++:prst->hour_fails++;
        break;

    default:
        return (void*)-1;
    }

    return (void*)0;
}

int commit_bdbcache_to_mysql()
{    
    traversal_result_t trst;
    memset(&trst, 0x00, sizeof(trst));

    timeval start;
    gettimeofday(&start, 0); 

    if (bdbCache.cursor_traversal_delete(pf_submit_select, &trst, DB_NEXT) > 0)
    {
        timeval end;
        gettimeofday(&end, 0);
        fileLog.writeLog(( FormatString(
            "[BDB] flush cache finished, time:% \n"
            "\tupdate % , successful % , fails %\n"
            "\tupdate % , successful % , fails %\n"
            "\tupdate % , successful % , fails %\n"
            "\tupdate % , successful % , fails %\n"
            "\tupdate % , successful % , fails %\n" )
            .arg((end.tv_sec - start.tv_sec + (double)(end.tv_usec - start.tv_usec) / CLOCKS_PER_SEC))
            .arg(g_tablename.tb_result).arg(trst.result_successful).arg(trst.result_fails)
            .arg(g_tablename.tb_result_by_region).arg(trst.region_successful).arg(trst.region_fails)
            .arg(g_tablename.tb_result_by_domain).arg(trst.domain_successful).arg(trst.domain_fails)
            .arg(g_tablename.tb_result_by_url).arg(trst.url_successful).arg(trst.url_fails)
            .arg(g_tablename.tb_result_by_hour).arg(trst.hour_successful).arg(trst.hour_fails)
            .str() ));
    }

    return 0;
}

#endif
