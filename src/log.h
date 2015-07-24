#ifndef CPARSE_LOG_H_
#define CPARSE_LOG_H_


#ifndef __attribute__
#define __attribute__(x)
#endif

void cparse_log_error(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void cparse_log_warn(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void cparse_log_info(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void cparse_log_debug(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void cparse_log_trace(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void cparse_log_errno(int err);

#endif
