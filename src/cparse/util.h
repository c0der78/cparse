/*! \file util.h */
#ifndef CPARSE_UTIL_H_
#define CPARSE_UTIL_H_

#include <time.h>
#include <cparse/defines.h>

BEGIN_DECL

int cparse_str_empty(const char *str);

time_t cparse_date_time(const char *str);

void cparse_replace_str(char **a, const char *b);

int cparse_str_prefix(const char *a, const char *b);

int cparse_str_cmp(const char *a, const char *b);

END_DECL

#endif
