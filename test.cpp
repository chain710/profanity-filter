#include "profanity_filter.h"
#include <string>
#include <cstdio>

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf("usage: %s dict_file sentence\n", argv[0]);
        return -1;
    }

    profanity_filter_t filter;
    filter.load(argv[1]);
    string in(argv[2]);
    filter.filter(in);

    printf("output:%s\n", in.c_str());
    return 0;
}
