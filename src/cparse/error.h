#ifndef CPARSE_ERROR_H_
#define CPARSE_ERROR_H_

#include <cparse/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

CPARSE_ERROR *cparse_error_new();

void cparse_error_free(CPARSE_ERROR *error);

const char *cparse_error_message(CPARSE_ERROR *error);
void cparse_error_set_message(CPARSE_ERROR *error, const char *message);

int cparse_error_code(CPARSE_ERROR *error);
void cparse_error_set_code(CPARSE_ERROR *error, int code);

#ifdef __cplusplus
}
#endif

#endif
