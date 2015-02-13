#ifndef CPARSE_CLIENT_H_
#define CPARSE_CLIENT_H_

#include <stdlib.h>
#include <cparse/defines.h>

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

void cparse_client_request_add_header(CPARSE_REQUEST *request, const char *key, const char *value);

void cparse_client_request_free(CPARSE_REQUEST *request);

void cparse_client_request_perform(CPARSE_REQUEST *request, CPARSE_ERROR **error);

CPARSE_JSON *cparse_client_request_perform_and_get_json(CPARSE_REQUEST *request, CPARSE_ERROR **error);

bool cparse_client_object_request(CPARSE_OBJ *obj, HTTPRequestMethod method, const char *path, const char *payload, CPARSE_ERROR **error);

bool cparse_client_request(HTTPRequestMethod method, const char *path, CPARSE_ERROR **error);

#endif
