#ifndef CPARSE_LOG_H_
#define CPARSE_LOG_H_


#ifndef __attribute__
#define __attribute__(x)
#endif

void log_error(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void log_warn(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void log_info(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void log_debug(const char *const format, ...) __attribute__((format(printf, 1, 2)));

void log_trace(const char *const format, ...) __attribute__((format(printf, 1, 2)));

#endif
