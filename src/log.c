#include <stdarg.h>
#include <cparse/parse.h>
#include <stdio.h>
#include <time.h>
#include "log.h"

const char *cParseLogLevelNames[] =
{
    "UNKNOWN", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"
};

cParseLogLevel cparse_current_log_level;

static void log_vargs(cParseLogLevel level, const char *const format, va_list args)
{
    char buf[BUFSIZ + 1] = {0};

    time_t t = time(0);
    strftime(buf, BUFSIZ, "%F %T", localtime(&t));
    fprintf(stdout, "%s %s: ", buf, cParseLogLevelNames[level]);
    vfprintf(stdout, format, args);
    fputs("\n", stdout);
    fflush(stdout);
}

void log_error(const char *const format, ...)
{
    va_list args;

    if (cParseLogError > cparse_current_log_level) return;

    va_start(args, format);
    log_vargs(cParseLogError, format, args);
    va_end(args);
}

void log_warn(const char *const format, ...)
{
    va_list args;

    if (cParseLogWarn > cparse_current_log_level) return;

    va_start(args, format);
    log_vargs(cParseLogWarn, format, args);
    va_end(args);
}

void log_info(const char *const format, ...)
{
    va_list args;

    if (cParseLogInfo > cparse_current_log_level) return;

    va_start(args, format);
    log_vargs(cParseLogInfo, format, args);
    va_end(args);
}

void log_debug(const char *const format, ...)
{
    va_list args;

    if (cParseLogDebug > cparse_current_log_level) return;

    va_start(args, format);
    log_vargs(cParseLogDebug, format, args);
    va_end(args);
}

void log_trace(const char *const format, ...)
{
    va_list args;

    if (cParseLogTrace > cparse_current_log_level) return;

    va_start(args, format);
    log_vargs(cParseLogTrace, format, args);
    va_end(args);
}

