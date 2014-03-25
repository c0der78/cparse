#include <stdlib.h>
#include <stdio.h>
#include <cparse/error.h>

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
