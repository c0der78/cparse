#ifndef CPARSE_CLIENT_H_
#define CPARSE_CLIENT_H_

#include <stdlib.h>
#include <cparse/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cparse_client_response CPARSE_RESPONSE;

typedef struct cparse_client_request CPARSE_REQUEST;

typedef struct cparse_request_header REQUEST_HEADER;

typedef enum
{
    HTTPRequestMethodGet,
    HTTPRequestMethodPost,
    HTTPRequestMethodPut,
    HTTPRequestMethodDelete
} HTTPRequestMethod;

struct cparse_client_request
{
    char *path;
    char *payload;
    HTTPRequestMethod method;
    REQUEST_HEADER *headers;
};

CPARSE_REQUEST *cparse_client_request_new();

CPARSE_JSON *cparse_client_request_get_json(CPARSE_REQUEST *request, CPARSE_ERROR **error);

void cparse_client_request_add_header(CPARSE_REQUEST *request, const char *key, const char *value);

void cparse_client_request_free(CPARSE_REQUEST *request);

bool cparse_client_request_perform(CPARSE_REQUEST *request, CPARSE_ERROR **error);

bool cparse_client_request(HTTPRequestMethod method, const char *path, CPARSE_ERROR **error);

#ifdef __cplusplus
}
#endif

#endif
