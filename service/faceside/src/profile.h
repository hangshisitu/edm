#ifndef __PROFILE_H__
#define __PROFILE_H__

#include <list>
#include <map>
#include <string>
#include <stdarg.h>
#include <limits>
#include <sstream>

using namespace std;

typedef std::string  node_t;
typedef std::string  keys_t;
typedef std::string  value_t;

typedef std::list<value_t> value_list_t;
typedef std::map<keys_t, value_list_t> key_map_t;
typedef std::map<node_t, key_map_t>  node_map_t;

template<class T>
struct cast_precision
{
    static inline void set(std::stringstream &ss) {ss.rdstate();};
};

template<>
struct cast_precision<double> 
{
    static inline void set(std::stringstream &ss)
    {
        ss.precision(std::numeric_limits<double>::digits10);
    };
};

template<>
struct cast_precision<float> 
{
    static inline void set(std::stringstream &ss) 
    {
        ss.precision(std::numeric_limits<float>::digits10);
    };
};

class CProfile
{
public:
    CProfile(void){}
    CProfile(const char *file);

    int open(const char *file);
    int get(const char* node, const char* key, value_t& target);
    int gets(const char* node, const char* key, value_list_t& target);

    template <typename T>
    int get(const char* node, const char* key, T &target)
    {
        value_t target_i;
        if (0 != this->get(node, key, target_i)){
            return -1;
        }

        stringstream sstr;
        cast_precision<T>::set(sstr);
        sstr << target_i;sstr >> target;
        return 0;
    }

    void print(void);

private:
    node_map_t nodes_;
};

#endif

