#ifndef CPARSE_UTIL_H_
#define CPARSE_UTIL_H_

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

time_t cparse_date_time(const char *str);

void replace_str(char **a, const char *b);

#ifdef __cplusplus
}
#endif

#endif
