#include <stdarg.h>
#include <cparse/parse.h>
#include <stdio.h>
#include <time.h>
#include <execinfo.h>
#define _GNU_SOURCE
#include <dlfcn.h>
#include "log.h"
#include <stdlib.h>
#include <string.h>

const char *cParseLogLevelNames[] =
{
    "UNKNOWN", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"
};

cParseLogLevel cparse_current_log_level;

static void cparse_log_vargs(cParseLogLevel level, const char *const format, va_list args)
{
    char buf[BUFSIZ + 1] = {0};

    void *callstack[3];

    time_t t = time(0);

    int frames = backtrace(callstack, 3);

    const char *last_func = "unk";

    Dl_info info;

    if (frames > 2) {

        if (dladdr(callstack[2], &info) && info.dli_sname) {
            last_func = info.dli_sname;
        }
    }

    strftime(buf, BUFSIZ, "%Y-%m-%d %H:%M:%S", localtime(&t));

    fprintf(stdout, "%s %s: [%s] ", buf, cParseLogLevelNames[level], last_func);
    vfprintf(stdout, format, args);
    fputs("\n", stdout);
    fflush(stdout);

}

void cparse_log_error(const char *const format, ...)
{
    va_list args;

    if (cParseLogError > cparse_current_log_level) { return; }

    va_start(args, format);
    cparse_log_vargs(cParseLogError, format, args);
    va_end(args);
}

void cparse_log_warn(const char *const format, ...)
{
    va_list args;

    if (cParseLogWarn > cparse_current_log_level) { return; }

    va_start(args, format);
    cparse_log_vargs(cParseLogWarn, format, args);
    va_end(args);
}

void cparse_log_info(const char *const format, ...)
{
    va_list args;

    if (cParseLogInfo > cparse_current_log_level) { return; }

    va_start(args, format);
    cparse_log_vargs(cParseLogInfo, format, args);
    va_end(args);
}

void cparse_log_debug(const char *const format, ...)
{
    va_list args;

    if (cParseLogDebug > cparse_current_log_level) { return; }

    va_start(args, format);
    cparse_log_vargs(cParseLogDebug, format, args);
    va_end(args);
}

void cparse_log_trace(const char *const format, ...)
{
    va_list args;

    if (cParseLogTrace > cparse_current_log_level) { return; }

    va_start(args, format);
    cparse_log_vargs(cParseLogTrace, format, args);
    va_end(args);
}

void cparse_log_errno(int errno) {
    cparse_log_error("%d: %s", errno, strerror(errno));
}

