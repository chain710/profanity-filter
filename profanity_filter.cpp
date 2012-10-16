#include "profanity_filter.h"
#include "utf8.h"
#include <fstream>
#include <iostream>
#include <string>
#include <set>

using namespace std;

profanity_filter_t::filter_prefix_t extract_from_u32string( const profanity_filter_t::u32string_t& in, int offset, int size )
{
    profanity_filter_t::filter_prefix_t ret = 0;
    profanity_filter_t::u32string_t::const_iterator it = in.begin();
    it += offset;
    for (int i = 0; i < size && it != in.end(); ++i, ++it)
    {
        ret |= (profanity_filter_t::filter_prefix_t)(*it) << (32*i);
    }

    return ret;
}

bool compare_u32(profanity_filter_t::u32string_t src, profanity_filter_t::u32string_t dst, int offset)
{
    if (src.size() < dst.size() + offset)
    {
        return false;
    }

    profanity_filter_t::u32string_t::iterator src_it = src.begin();
    src_it += offset;
    profanity_filter_t::u32string_t::iterator dst_it = dst.begin();
    while (src_it != src.end() && dst_it != dst.end())
    {
        if (*src_it != *dst_it)
        {
            return false;
        }

        ++src_it;
        ++dst_it;
    }

    return true;
}

const profanity_filter_t::u32string_t* find_longest_match(const profanity_filter_t::u32string_t& in, const profanity_filter_t::u32string_vector_t& pat, int offset)
{
    int longest = 0;
    int match_len = 0;
    const profanity_filter_t::u32string_t* found = NULL;
    for (int i = 0; i < (int)pat.size(); ++i)
    {
        if (compare_u32(in, pat.at(i), offset))
        {
            // 匹配到了，是否最长匹配?
            match_len = pat.at(i).size();
            if (match_len > longest)
            {
                longest = match_len;
                found = &pat.at(i);
            }
        }
    }

    return found;
}

profanity_filter_t::profanity_filter_t()
{

}

int profanity_filter_t::load( const std::string& conf_path )
{
    fstream u8fs;
    u8fs.open(conf_path.c_str(), ios_base::in);
    if (!u8fs.is_open())
    {
        // can not open
        return -1;
    }

    string line;
    u32string_t u32str;
    set<u32string_t> u32str_set;
    filter_prefix_t prefix;
    while (getline(u8fs, line))
    {
        if (!utf8::is_valid(line.begin(), line.end()))
        {
            continue;
        }

        u32str.clear();
        utf8::utf8to32(line.begin(), line.end(), back_inserter(u32str));
        if (u32str.empty())
        {
            continue;
        }

        if (u32str_set.find(u32str) != u32str_set.end())
        {
            continue;
        }

        u32str_set.insert(u32str);
        prefix = extract_from_u32string(u32str, 0, 2);
        conf_[prefix].push_back(u32str);
    }

    return 0;
}


int profanity_filter_t::filter( std::string& utf8str ) const
{
    u32string_t u32str;
    const u32string_t* u32str_match = NULL;
    int replace = 0;
    try
    {
        utf8::utf8to32(utf8str.begin(), utf8str.end(), back_inserter(u32str));
        
        for (int i = 0; i < (int)u32str.size(); ++i)
        {
            filter_prefix_t prefix = extract_from_u32string(u32str, i, 2);
            profanity_map_t::const_iterator it = conf_.find(prefix);
            if (it != conf_.end())
            {
                u32str_match = find_longest_match(u32str, it->second, i);
                if (u32str_match)
                {
                    for (int j = 0; j < (int)u32str_match->size(); ++j)
                    {
                        u32str.at(j+i) = '*';
                    }

                    ++replace;
                    continue;
                }
            }
            
            prefix = extract_from_u32string(u32str, i, 1);
            it = conf_.find(prefix);
            if (it != conf_.end())
            {
                u32str.at(i) = '*';
                ++replace;
            }
        }

        utf8str.clear();
        utf8::utf32to8(u32str.begin(), u32str.end(), back_inserter(utf8str));
        return replace;
    }
    catch (const utf8::exception& e)
    {
        return -1;
    }
}
