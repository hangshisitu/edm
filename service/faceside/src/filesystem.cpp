#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include "filesystem.h"

int getfiles(const char* path, list<string>& files)
{
    DIR *d;
    struct dirent *f;
    if(!(d = opendir(path)))
        return -1;

    while((f = readdir(d)) != NULL){

        if(!strcmp(f->d_name, ".")
            || !strcmp(f->d_name, ".."))
            continue;

        if (f->d_type & DT_REG)
            files.push_back(f->d_name);
    }

    closedir(d);
    return 0;
}
