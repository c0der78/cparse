#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

time_t cparse_date_time(const char *s)
{
    struct tm tp;

    strptime(s, "%FT%T%z", &tp);

    return mktime(&tp);
}

void replace_str(char **a, const char *b)
{
    if (!a) return;

    if (*a)
        free(*a);

    (*a) = strdup(b);
}
