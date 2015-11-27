#ifndef CPARSE_CLIENT_H_
#define CPARSE_CLIENT_H_

#include <stdlib.h>
#include <curl/curl.h>
#include <cparse/defines.h>
#include "private.h"

/*! HTTP Request Method Types */
typedef enum {
    cParseHttpRequestMethodGet,
    cParseHttpRequestMethodPost,
    cParseHttpRequestMethodPut,
    cParseHttpRequestMethodDelete
} cParseHttpRequestMethod;

/*! a parse request */
struct cparse_request {
    char *path;
    cParseRequestData *data;
    char *body;
    size_t bodySize;
    cParseHttpRequestMethod method;
    cParseRequestHeader *headers;
    int flags;
};

#define CPARSE_REQUEST_NEW_CLIENT (1 << 0)

extern int cparse_request_timeout;

/*! a parse response */
struct cparse_client_response {
    char *text;
    size_t size;
    int code;
};

struct cparse_client {
    CURL *cURL;
    struct curl_slist *headers;
    /* may want a client for an older version simultaneously */
    char *apiVersion;
};

BEGIN_DECL

/*! allocates a client request
 * \param method the HTTP method to use
 * \param path the endpoint to use
 * \returns the allocated request
 */
cParseRequest *cparse_request_with_method_and_path(cParseHttpRequestMethod method, const char *path);

/*! issues a request and returns the response data as a json object
 * \param request the request instance
 * \param error a pointer to an error that will get allocated if not successfull.
 * \returns the json response
 */
cParseJson *cparse_request_get_json(cParseRequest *request, cParseError **error);

/*! adds a HTTP header to the request.
 * \param request the request instance
 * \param key the header key ex. 'Content-Type'
 * \param value the header value ex 'application/json'
 */
void cparse_request_add_header(cParseRequest *request, const char *key, const char *value);

/*! sets the request body. Anything provided with this method will be URI encoded.
 * NOTE: this will overwrite anything set with cparse_request_add_data
 * \see cparse_request_add_data
 * \param request the request instance
 * \param body the body text to set
 */
void cparse_request_add_body(cParseRequest *request, const char *body);

/*! adds data for the request body. Any values will be URI encoded.
 * NOTE: this will overwrite anything set with cparse_request_add_body
 * \see cparse_request_add_body
 * \param request the request instance
 * \param key the key for the value
 * \param value the value to send
 */
void cparse_request_add_data(cParseRequest *request, const char *key, const char *value);

/*! deallocates a request
 * \param request the request instance
 */
void cparse_request_free(cParseRequest *request);

/*! performs a request
 * \param request the request instance
 * \param error a pointer to an error that will get allocated if not successful
 * \return true if successfull otherwise false
 */
bool cparse_request_execute(cParseRequest *request, cParseError **error);

END_DECL

#endif
