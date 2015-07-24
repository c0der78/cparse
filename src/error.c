#include <stdlib.h>
#include <stdio.h>
#include <cparse/error.h>
#include <string.h>
#include "log.h"
#include <errno.h>

struct cparse_error
{
    int code;
    char *message;
};

cParseError *cparse_error_new()
{
    cParseError *e = malloc(sizeof(cParseError));

    if (e == NULL)
    {
        cparse_log_errno(ENOMEM);
        return NULL;
    }

    e->code = 0;
    e->message = NULL;

    return e;
}

cParseError *cparse_error_with_message(const char *message)
{
    cParseError *e = cparse_error_new();

    if (e == NULL) { return NULL; }

    e->message = strdup(message);

    return e;
}

cParseError *cparse_error_with_code_and_message(int code, const char *message)
{
    cParseError *e = cparse_error_new();

    if (e == NULL) {
        return NULL;
    }

    e->code = code;
    e->message = strdup(message);

    return e;
}

void cparse_error_free(cParseError *e)
{
    if (!e) {
        return;
    }

    if (e->message) {
        free(e->message);
    }

    free(e);
}

const char *cparse_error_message(cParseError *error)
{
    return !error ? NULL : error->message;
}

void cparse_error_set_message(cParseError *error, const char *message)
{
    if (error)
    {
        error->message = strdup(message);
    }
}

int cparse_error_code(cParseError *error)
{
    return !error ? -1 : error->code;
}

void cparse_error_set_code(cParseError *error, int code)
{
    if (error)
    {
        error->code = code;
    }
}
