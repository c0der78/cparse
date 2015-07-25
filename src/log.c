#include "config.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <cparse/parse.h>
#include <cparse/error.h>
#include <stdio.h>
#include <time.h>
#include <execinfo.h>
#include <dlfcn.h>
#include "log.h"
#include "protocol.h"

const char *cParseLogLevelNames[] =
{
    "UNKNOWN", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"
};

cParseLogLevel cparse_current_log_level;

static void cparse_log_vargs(cParseLogLevel level, const char *const format, va_list args)
{
    char buf[BUFSIZ + 1] = {0};

    time_t t = time(0);

#ifdef HAVE_DLADDR

    const char *last_func = "unk";

    void *callstack[4];

    int frames = backtrace(callstack, 4);

    Dl_info info;

    if (frames > 0) {

        if (dladdr(callstack[2], &info) && info.dli_sname) {
            last_func = info.dli_sname;
        }
    }
#endif

    strftime(buf, BUFSIZ, "%Y-%m-%d %H:%M:%S", localtime(&t));

#ifdef HAVE_DLADDR
    fprintf(stdout, "%s %s: [%s] ", buf, cParseLogLevelNames[level], last_func);
#else
    fprintf(stdout, "%s %s: ", buf, cParseLogLevelNames[level]);
#endif

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

void cparse_log_set_error(cParseError **error, const char *const format, ...)
{
    va_list args;

    va_start(args, format);

    if (error) {
        char buf[CPARSE_BUF_SIZE + 1] = {0};
        vsnprintf(buf, CPARSE_BUF_SIZE, format, args);
        *error = cparse_error_with_message(buf);
    }

    if (cParseLogError > cparse_current_log_level) {
        va_end(args);
        return;
    }

    cparse_log_vargs(cParseLogError, format, args);
    va_end(args);
}

void cparse_log_set_errno(cParseError **error, int errnum)
{
    if (error) {
        *error = cparse_error_with_code_and_message(errnum, strerror(errnum));
    }

    cparse_log_error("%d: %s", errnum, strerror(errnum));
}



