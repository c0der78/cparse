#ifndef CPARSE_CLIENT_H_
#define CPARSE_CLIENT_H_

#include <stdlib.h>
#include <cparse/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cparse_client_response CPARSE_RESPONSE;

typedef struct cparse_client_request CPARSE_REQUEST;

typedef struct cparse_kv_list CPARSE_REQUEST_HEADER;

typedef struct cparse_kv_list CPARSE_REQUEST_DATA;

typedef enum
{
    HTTPRequestMethodGet,
    HTTPRequestMethodPost,
    HTTPRequestMethodPut,
    HTTPRequestMethodDelete
} HTTPRequestMethod;

CPARSE_REQUEST *cparse_client_request_with_method_and_path(HTTPRequestMethod method, const char *path);

CPARSE_JSON *cparse_client_request_get_json(CPARSE_REQUEST *request, CPARSE_ERROR **error);

void cparse_client_request_add_header(CPARSE_REQUEST *request, const char *key, const char *value);

void cparse_client_request_set_payload(CPARSE_REQUEST *request, const char *payload);

void cparse_client_request_add_data(CPARSE_REQUEST *request, const char *key, const char *value);

void cparse_client_request_free(CPARSE_REQUEST *request);

bool cparse_client_request_perform(CPARSE_REQUEST *request, CPARSE_ERROR **error);

#ifdef __cplusplus
}
#endif

#endif
