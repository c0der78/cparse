#include "config.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

time_t cparse_date_time(const char *str)
{
    int y, M, d, h, m;
    float s;
    int tzh, tzm;
    struct tm time;

    if (8 > sscanf(str, "%d-%d-%dT%d:%d:%f%d:%dZ", &y, &M, &d, &h, &m, &s, &tzh, &tzm))
    {
        sscanf(str, "%d-%d-%dT%d:%d:%fZ", &y, &M, &d, &h, &m, &s);
    }

    time.tm_year = y - 1900; // Year since 1900
    time.tm_mon = M - 1;     // 0-11
    time.tm_mday = d;        // 1-31
    time.tm_hour = h;        // 0-23
    time.tm_min = m;         // 0-59
    time.tm_sec = (int) s;    // 0-61 (0-60 in C++11)

    return mktime(&time);
}

void replace_str(char **a, const char *b)
{
    if (!a) return;

    if (*a)
        free(*a);

    (*a) = strdup(b);
}
