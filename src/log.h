#ifndef CPARSE_LOG_H_
#define CPARSE_LOG_H_


#include <string.h>

#ifndef __attribute__
#define __attribute__(x)
#endif

void cparse_log_error(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void cparse_log_warn(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void cparse_log_info(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void cparse_log_debug(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void cparse_log_trace(const char *const format, ...) __attribute__((format(printf, 1, 2)));


#define cparse_log_errno(errnum) cparse_log_error("%s:%d %s (%d)", __FILE__, __LINE__, strerror(errnum), errnum)

void cparse_log_set_error(cParseError **error, const char *const format, ...) __attribute__((format(printf, 2, 3)));

void cparse_log_set_errno(cParseError **error, int errnum);

#endif
