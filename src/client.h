#ifndef CPARSE_CLIENT_H_
#define CPARSE_CLIENT_H_

#include <stdlib.h>
#include <cparse/defines.h>

typedef struct cparse_client_response CPARSE_CLIENT_RESP;

typedef struct cparse_client_request CPARSE_CLIENT_REQ;

typedef struct cparse_request_header REQUEST_HEADER;

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

struct cparse_request_header
{
    REQUEST_HEADER *next;
    char *key;
    char *value;
};

struct cparse_client_request
{
    char *path;
    char *payload;
    HTTPRequestMethod method;
    REQUEST_HEADER *headers;
};

CPARSE_CLIENT_RESP *cparse_client_request_get_response(CPARSE_CLIENT_REQ *request);

CPARSE_CLIENT_REQ *cparse_client_request_new();

void cparse_client_request_add_header(CPARSE_CLIENT_REQ *request, const char *key, const char *value);

void cparse_client_request_free(CPARSE_CLIENT_REQ *request);

void cparse_client_response_free(CPARSE_CLIENT_RESP *response);

CPARSE_JSON *cparse_client_request_get_json(CPARSE_CLIENT_REQ *request, CPARSE_ERROR **error);

void cparse_client_request_perform(CPARSE_CLIENT_REQ *request, CPARSE_ERROR **error);

#endif
