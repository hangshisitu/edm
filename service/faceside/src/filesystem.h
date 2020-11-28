#ifndef _FILESYSTME_H_
#define _FILESYSTEM_H_

#include <list>
#include <string>

using std::list;
using std::string;

extern int getfiles(const char* path, list<string>& files);

#endif
