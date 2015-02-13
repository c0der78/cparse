#include <stdlib.h>
#include <stdio.h>
#include <cparse/error.h>
#include <string.h>

struct cparse_error
{
    int code;
    char *message;
};

CPARSE_ERROR *cparse_error_new()
{
    CPARSE_ERROR *e = malloc(sizeof(CPARSE_ERROR));

    if (e == NULL)
    {
        fputs("Unable to allocate memory", stderr);
        exit(EXIT_FAILURE);
    }

    e->code = 0;
    e->message = NULL;

    return e;
}

void cparse_error_free(CPARSE_ERROR *e)
{
    if (e->message)
        free(e->message);

    free(e);
}

const char *cparse_error_message(CPARSE_ERROR *error)
{
    return !error ? NULL : error->message;
}

void cparse_error_set_message(CPARSE_ERROR *error, const char *message)
{
    if (error)
    {
        error->message = strdup(message);
    }
}

int cparse_error_code(CPARSE_ERROR *error)
{
    return !error ? -1 : error->code;
}

void cparse_error_set_code(CPARSE_ERROR *error, int code)
{
    if (error)
    {
        error->code = code;
    }
}
