#include "berkeleydb.h"
#include "log.h"

//#define bdb_printf(x) 
//#define bdb_printf(x) printf x
#define bdb_printf LOG_DEBUG

RwLock BerkeleyDB::lock_;
DB_ENV* BerkeleyDB::dbenv = NULL;

void BerkeleyDB::bdb_env_init(const char* envpath)
{
	int ret = db_env_create(&BerkeleyDB::dbenv, 0);
	if (ret != 0) {
	    bdb_printf(("Error creating env handle: %s\n", db_strerror(ret)));
	    return;
	}
	
	BerkeleyDB::dbenv->set_data_dir(BerkeleyDB::dbenv,"data");
    BerkeleyDB::dbenv->set_tmp_dir(BerkeleyDB::dbenv,"temp");
    BerkeleyDB::dbenv->set_lg_dir(BerkeleyDB::dbenv,"log");
    	
	ret = BerkeleyDB::dbenv->open(
	        BerkeleyDB::dbenv,
	        envpath,
	        DB_CREATE | DB_INIT_LOCK | DB_INIT_MPOOL,
	        0);

	if (ret != 0) {
	    bdb_printf(("Environment open failed: %s\n", db_strerror(ret)));
	    BerkeleyDB::bdb_env_close();
	}
}

void BerkeleyDB::bdb_env_close()
{
	if (BerkeleyDB::dbenv){
	    BerkeleyDB::dbenv->close(BerkeleyDB::dbenv, 0);
	    BerkeleyDB::dbenv = NULL;
	}
}

void BerkeleyDB::bdb_sync()
{
    if (BerkeleyDB::dbenv){
        //BerkeleyDB::dbenv->memp_sync();
    }
}

	
BerkeleyDB::BerkeleyDB()
{
    this->dbp_ = NULL;
}

BerkeleyDB::~BerkeleyDB()
{
    this->close();
}

int BerkeleyDB::open(const char* dbfile, unsigned int pagesize, unsigned int cachesize)
{
    if (!dbfile || dbp_){
        return -1;
    }

    int retval = db_create(&dbp_, BerkeleyDB::dbenv, 0);

    if (0 != retval){
        bdb_printf(("{%s:%d}%s %s\n",__FILE__, __LINE__, __FUNCTION__, db_strerror(retval)));
        return -1;
    }

	//dbp_->set_pagesize(dbp_, xxx);
	//dbp_->set_cachesize(dbp_, 0, ?*1024*1024, 1);

    retval = dbp_->open(dbp_, NULL, dbfile, NULL, DB_BTREE, DB_CREATE|DB_THREAD, 0);

    if (0 != retval){
        bdb_printf(("{%s:%d}%s %s\n",__FILE__, __LINE__, __FUNCTION__, db_strerror(retval)));
        return -1;
    }

    return 0;
}

void BerkeleyDB::close()
{
    if(dbp_ != NULL){
        dbp_->sync(dbp_,0);
        dbp_->close(dbp_, 0);
    }
}

void BerkeleyDB::sync()
{
    if (this->dbp_){
        dbp_->sync(dbp_,0);
    }
}

int BerkeleyDB::put( const void* key, unsigned int klen, const void* data, unsigned int dlen )
{
    if (!this->dbp_)
        return -1;

    RwGuard<RwLock> guard(BerkeleyDB::lock_, WLOCK);
    DBT k, d;
    memset((void*)&k, 0, sizeof(k));
    memset((void*)&d, 0, sizeof(d));

    k.data = (void*)key;
    k.size = klen;

    d.data = (void*)data;
    d.size = dlen;

    int retval = dbp_->put(dbp_, NULL, &k, &d, 0/*DB_NOOVERWRITE*/); //DB_AUTO_COMMIT

    if (0 != retval){
        bdb_printf(("{%s:%d}%s %s\n",__FILE__, __LINE__, __FUNCTION__, db_strerror(retval)));
        return -1;
    }

    //dbp_->sync(dbp_,0);
    return 0;
}

int BerkeleyDB::put( const void* key, const void* data )
{
    return this->put(key,strlen((char*)key),data,strlen((char*)data));
}

int BerkeleyDB::get( const void* key, unsigned int klen, const void* data, unsigned int dlen )
{
    if (!this->dbp_)
        return -1;

    RwGuard<RwLock> guard(BerkeleyDB::lock_, RLOCK);
    DBT k, d;
    memset((void*)&k, 0, sizeof(k));
    memset((void*)&d, 0, sizeof(d));

    k.data = (void*)key;
    k.size = klen;

    d.data = (void*)data;
    d.ulen = dlen; 
    d.flags = DB_DBT_USERMEM;
    
    int retval = dbp_->get(dbp_, NULL, &k, &d, 0);

    if (0 != retval){
        bdb_printf(("{%s:%d}%s %s\n",__FILE__, __LINE__, __FUNCTION__, db_strerror(retval)));
        return -1;
    }

    return 0;
}

int BerkeleyDB::get( const void* key, const void* data, unsigned int dlen )
{
    return this->get(key,strlen((char*)key),data,dlen);
}

int BerkeleyDB::del( const void* key, unsigned int klen )
{
    if (!this->dbp_)
        return -1;

    RwGuard<RwLock> guard(BerkeleyDB::lock_, WLOCK);
    DBT k;
    memset((void*)&k, 0, sizeof(k));

    k.data = (void*)key;
    k.size = klen;

    int retval = dbp_->del(dbp_, NULL, &k, 0);

    if (0 != retval){
        bdb_printf(("{%s:%d}%s %s\n",__FILE__, __LINE__, __FUNCTION__, db_strerror(retval)));
        return -1;
    }

    return 0;
}

int BerkeleyDB::del( const void* key )
{
    return this->del(key,strlen((char*)key));
}

void BerkeleyDB::cursor_traversal(traversal_func_pt func, void* param, int flag)//DB_NEXT,DB_PREV
{
    if (!func)
        return;

    RwGuard<RwLock> guard(BerkeleyDB::lock_, WLOCK);

    /* 定义一个游标变量 */
    DBC * cur;
    dbp_->cursor(dbp_, NULL, &cur, 0);

    DBT k, d;
    memset((void*)&k, 0, sizeof(k));
    memset((void*)&d, 0, sizeof(d));

    /* 因为KEY和DATA为空，则游标遍历整个数据库记录 */
    while(cur->c_get(cur, &k, &d, flag) == 0){
        func(k.data, k.size, d.data,d.size, param);
    }

    /*
    char *p = "xxxx";
    k.data = (void*)p;
    k.size = strlen((char*)k.data);
    while(cur->c_get(cur, &k, &d, DB_SET) == 0){
        //do something with key and data
    }
    */
    
    cur->c_close(cur);
    return;
}


