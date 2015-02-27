/*! \file util.h */
#ifndef CPARSE_UTIL_H_
#define CPARSE_UTIL_H_

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef str_empty
#define str_empty(x) (!(x) || !*(x))
#endif

time_t cparse_date_time(const char *str);

void replace_str(char **a, const char *b);

int str_prefix(const char *a, const char *b);

int str_cmp(const char *a, const char *b);

#ifdef __cplusplus
}
#endif

#endif
