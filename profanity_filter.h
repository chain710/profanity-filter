#ifndef _PROFANITY_FILTER_
#define _PROFANITY_FILTER_

#ifndef _WIN32
#include <inttypes.h>
#endif
#include <map>
#include <vector>
#include <string>

class profanity_filter_t
{
public:
    profanity_filter_t();
    int load(const std::string& conf_path);
    int filter( std::string& utf8str );
#ifdef _WIN32
    typedef __int64 filter_prefix_t;
#else
    typedef int64_t filter_prefix_t;
#endif
    typedef std::vector<unsigned int> u32string_t;
    typedef std::vector<u32string_t> u32string_vector_t;
    typedef std::map<filter_prefix_t, u32string_vector_t> profanity_map_t;
private:
    profanity_map_t conf_;
};

#endif
