#ifndef BERKELEY_DB_H
#define BERKELEY_DB_H

#include <db.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lock.h"

typedef void* (*traversal_func_pt)(const void* key, unsigned int kl, const void* data, unsigned int dl, void* param);

class BerkeleyDB
{
public:
	static void bdb_env_init(const char* envpath);
	static void bdb_env_close();
	static void bdb_sync();

    BerkeleyDB();
    ~BerkeleyDB();

    int  open(const char* dbfile, unsigned int pagesize, unsigned int cachesize);
    void close();
    void sync();

    int put(const void* key, unsigned int klen, const void* data, unsigned int dlen);
    int put(const void* key, const void* data);
    
    int get(const void* key, unsigned int klen, const void* data, unsigned int dlen);
    int get(const void* key, const void* data, unsigned int dlen);

    int del(const void* key, unsigned int klen);
    int del(const void* key);

    void cursor_traversal(traversal_func_pt func, void* param = NULL, int flag = DB_NEXT);
protected:

private:
    static RwLock lock_;
	static DB_ENV *dbenv;
    DB* dbp_;
};

#endif
