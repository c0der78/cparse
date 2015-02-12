#ifndef CPARSE_CLIENT_H_
#define CPARSE_CLIENT_H_

#include <stdlib.h>
#include <cparse/defines.h>

typedef enum
{
    HTTPRequestMethodGet,
    HTTPRequestMethodPost,
    HTTPRequestMethodPut,
    HTTPRequestMethodDelete
} HTTPRequestMethod;

struct cparse_client_response
{
    char *text;
    size_t size;
    int code;
};

struct cparse_client_request
{
    char *path;
    char *payload;
    HTTPRequestMethod method;
};

typedef struct cparse_client_response CPARSE_CLIENT_RESP;

typedef struct cparse_client_request CPARSE_CLIENT_REQ;

CPARSE_CLIENT_RESP *cparse_client_request_get_response(CPARSE_CLIENT_REQ *request);

CPARSE_CLIENT_REQ *cparse_client_request_new();

void cparse_client_request_free(CPARSE_CLIENT_REQ *request);

void cparse_client_response_free(CPARSE_CLIENT_RESP *response);

CPARSE_JSON *cparse_client_request_get_json(CPARSE_CLIENT_REQ *request, CPARSE_ERROR **error);

void cparse_client_request_perform(CPARSE_CLIENT_REQ *request, CPARSE_ERROR **error);

#endif
