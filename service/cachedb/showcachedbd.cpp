#include "berkeleydb.h"
#include <string>
#include <unistd.h>

using std::string;

typedef struct {
    time_t time;
    int reserve;
} havior_history_t;

// build: g++ showcachedbd.cpp berkeleydb.cpp lock.cpp -ldb -o showcachedbd

void* pf_select(const void* key, unsigned int kl, const void* data, unsigned int dl, void* param)
{
    if (!key || kl<1 || !data || dl<1 || !param){
        return NULL;
    }

    FILE* out = (FILE*)param;
    string skey((char*)key, kl);

    havior_history_t* pdata = (havior_history_t*)data;

    fprintf(out, "%s,%d,%d\r\n", skey.c_str(), pdata->reserve, pdata->time);
    return NULL;
}

int main(int argc, char* argv[])
{
    if ( argc != 3){
        printf("usage: %s [berkeleydb] [output]\n", basename(argv[0]) );
        return -1;
    }

    BerkeleyDB bdb;
    FILE *out = NULL;

    do{

        if ( access(argv[1], R_OK) != 0 ){
            printf("berkeleydb file not exists: %s\n", argv[1]);
            break;
        }

        out = fopen(argv[2], "w");
        if ( !out ) {
            printf("failed ot open output file:%s, %s\n", argv[2], strerror(errno));
            break;
        }

        if (bdb.open(argv[1],0,0) != 0){
            printf("failed to open berkeleydb file: %s\n", argv[1]);
            break;
        }

        fprintf(out, "corp_id,task_id,template_id,mail,havior,count,time\r\n");
        bdb.cursor_traversal(pf_select, out);

    } while (0);

    if ( out ) {
        fflush(out);
        fclose(out);
        out = NULL;
    }

    return 0;
}

