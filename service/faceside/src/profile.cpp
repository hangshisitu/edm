#include "profile.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#if defined(WIN32) || defined(_WIN32)
#   include <io.h>
#else
#   include <unistd.h>
#endif

# if !defined (R_OK)
#   define R_OK    04      /* Test for Read permission. */
# endif /* R_OK */

CProfile::CProfile( const char *file )
{
    open(file);
}

int CProfile::open( const char *file )
{
    if ( !file || access(file,R_OK) != 0){
        printf("the configuration file not exists or permission that is denied\n");
        return -1;
    }

    int linenum = 0;
    node_t node;
    nodes_.clear();

    ifstream filestream(file,ios_base::in);

    while (!filestream.eof())
    {
    	linenum ++;
        string line,alternate;
        getline(filestream,line,'\n');
        alternate = line;

        string::size_type pos = line.find("//");
        if (pos != string::npos){
            line.erase(pos);
        }

        pos = line.find("#");
        if (pos != string::npos){
            line.erase(pos);
        }

        line.erase(0,line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n")+1);

        if (line.length() < 1){
            continue;
        }

        if (*(line.begin()) == '[' && *(line.rbegin()) == ']')
        {
            node = line.substr(1,line.length()-2);

            if (node.length() < 1)
            {
                filestream.close();
                nodes_.clear();
                printf("%s:%d: error: '%s' is invalid section\n",file,linenum,alternate.c_str());
                return -1;
            }
        }
        else
        {
            pos = line.find('=');
            if (string::npos == pos)
            {
                filestream.close();
                nodes_.clear();
                printf("%s:%d: error: expected '=', after '%s'\n",file,linenum,alternate.c_str());
                return -1;
            }

            keys_t  key  = line.substr(0,pos);
            value_t val = line.substr(pos + 1);

            key.erase(key.find_last_not_of(" \t")+1);
            val.erase(0,val.find_first_not_of(" \t"));

            if (node.length() > 0 && key.length() > 0 ){
                nodes_[node][key].push_back(val);
            }
        }
    }

    filestream.close();
    return 0;
}

void CProfile::print(void)
{
    node_map_t::iterator node = nodes_.begin();
    for (; node != nodes_.end(); ++node)
    {
        printf("[%s]\n",node->first.c_str());

        key_map_t::iterator key = node->second.begin();
        for (; key != node->second.end(); ++key)
        {
            value_list_t::iterator val = key->second.begin();
            for (; val != key->second.end(); ++ val)
            {
                printf("%s = %s\n",key->first.c_str(),val->c_str());
            }
        }
    }
}

int CProfile::get( const char* node, const char* key, value_t& target )
{
    if ( !node || !key ){
        return -1;
    }

    node_map_t::iterator it_node = nodes_.find(node);
    if (it_node == nodes_.end()){
        return -1;
    }

    key_map_t::iterator val = it_node->second.find(key);
    if (val == it_node->second.end()){
        return -1;
    }

    target = val->second.front();
    return 0;
}

int CProfile::gets( const char* node, const char* key, value_list_t& target )
{
    if ( !node || !key ){
        return -1;
    }

    node_map_t::iterator it_node = nodes_.find(node);
    if (it_node == nodes_.end()){
        return -1;
    }

    key_map_t::iterator itkey = it_node->second.find(key);
    if (itkey == it_node->second.end()){
        return -1;
    }

    target.assign(itkey->second.begin(),itkey->second.end());
    return 0;
}

