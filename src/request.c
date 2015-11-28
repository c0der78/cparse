#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <curl/curl.h>
#include <stdarg.h>
#include <json.h>
#include <cparse/json.h>
#include <cparse/object.h>
#include <cparse/error.h>
#include <cparse/util.h>
#include "request.h"
#include "protocol.h"
#include "private.h"
#include "data_list.h"
#include "log.h"

cParseResponse *cparse_client_execute(cParseRequest *request);

/*! allocates a new client request
 * \param method the http method to use
 * \param path the path/endpoint to request
 */
cParseRequest *cparse_request_with_method_and_path(cParseHttpRequestMethod method, const char *path)
{
    cParseRequest *request = malloc(sizeof(cParseRequest));

    if (request == NULL) {
        cparse_log_errno(ENOMEM);
        return NULL;
    }

    request->path = strdup(path);
    request->body = NULL;
    request->bodySize = 0;
    request->data = NULL;
    request->method = method;
    request->headers = NULL;

    return request;
}

/*! deallocates a client request */
void cparse_request_free(cParseRequest *request)
{
    cParseRequestHeader *header = NULL, *next_header = NULL;
    cParseRequestData *data = NULL, *next_data = NULL;

    if (!request) {
        return;
    }

    if (request->path) {
        free(request->path);
    }
    if (request->body) {
        free(request->body);
    }

    for (header = request->headers; header; header = next_header) {
        next_header = header->next;

        cparse_dlist_free(header);
    }

    for (data = request->data; data; data = next_data) {
        next_data = data->next;

        cparse_dlist_free(data);
    }
    free(request);
}

cParseResponse *cparse_response_new()
{
    cParseResponse *response = malloc(sizeof(cParseResponse));

    if (response == NULL) {
        cparse_log_errno(ENOMEM);
        return NULL;
    }

    response->text = NULL;
    response->code = 0;
    response->size = 0;

    return response;
}

/*! deallocates a response */
void cparse_response_free(cParseResponse *response)
{
    if (!response) {
        return;
    }

    if (response->size > 0 && response->text) {
        free(response->text);
    }

    free(response);
}

void cparse_request_add_header(cParseRequest *request, const char *key, const char *value)
{
    cParseRequestHeader *header = NULL;

    if (!request || cparse_str_empty(key) || cparse_str_empty(value)) {
        cparse_log_errno(EINVAL);
        return;
    }

    header = cparse_dlist_new();

    if (header == NULL) {
        return;
    }

    header->next = request->headers;
    request->headers = header;

    header->key = strdup(key);
    header->value = strdup(value);
}

void cparse_request_add_body(cParseRequest *request, const char *body)
{
    cParseRequestData *data = NULL;

    if (request == NULL || cparse_str_empty(body)) {
        cparse_log_errno(EINVAL);
        return;
    }

    cparse_dlist_remove(&request->data, cparse_dlist_is_data);

    data = cparse_dlist_new();

    if (data == NULL) {
        return;
    }

    data->next = request->data;
    request->data = data;

    /* body has no key */
    data->key = NULL;
    data->value = strdup(body);
}

void cparse_request_add_data(cParseRequest *request, const char *key, const char *value)
{
    cParseRequestData *data = NULL;

    if (!request || cparse_str_empty(key) || cparse_str_empty(value)) {
        cparse_log_errno(EINVAL);
        return;
    }

    /* free the request body, as the two are synonymous */
    cparse_dlist_remove(&request->data, cparse_dlist_is_value);

    data = cparse_dlist_new();

    if (data == NULL) {
        return;
    }

    data->key = strdup(key);
    data->value = strdup(value);

    data->next = request->data;
    request->data = data;
}

bool cparse_request_execute(cParseRequest *request, cParseError **error)
{
    cParseResponse *response = NULL;

    if (request == NULL) {
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    response = cparse_client_execute(request);

    if (response != NULL) {
        cparse_response_free(response);

        return true;
    }

    return false;
}

cParseJson *cparse_response_parse_json(cParseResponse *response, cParseError **error)
{
    json_tokener *tok = NULL;

    cParseJson *obj = NULL;

    const char *errorMessage = NULL;

#ifdef HAVE_JSON_TOKENER_GET_ERROR
    enum json_tokener_error parseError;
#endif

    if (response == NULL) {
        cparse_log_set_errno(error, EINVAL);
        return NULL;
    }

    tok = json_tokener_new();

    obj = json_tokener_parse_ex(tok, response->text, response->size);

#ifdef HAVE_JSON_TOKENER_GET_ERROR
    parseError = json_tokener_get_error(tok);

    if (parseError != json_tokener_success) {
        errorMessage = json_tokener_error_desc(parseError);
    }
#else
    if (obj == NULL) {
        errorMessage = "Unable to parse json";
    }
#endif

    if (cparse_json_contains(obj, "error")) {
        errorMessage = cparse_json_get_string(obj, "error");
    }

    json_tokener_free(tok);

    if (errorMessage != NULL) {
        if (error) {
            *error = cparse_error_with_message(errorMessage);

            cparse_error_set_code(*error, cparse_json_get_number(obj, "code", 0));
        }

        if (obj) {
            cparse_json_free(obj);
        }

        return NULL;
    }

    return obj;
}

cParseJson *cparse_request_get_json(cParseRequest *request, cParseError **error)
{
    cParseResponse *response = NULL;

    if (!request) {
        cparse_log_set_errno(error, EINVAL);
        return NULL;
    }

    response = cparse_client_execute(request);

    if (response != NULL) {
        cParseJson *json = cparse_response_parse_json(response, error);

        cparse_response_free(response);

        return json;
    }

    cparse_log_set_error(error, "Unable to get response to request");

    return NULL;
}

bool cparse_request_execute_method_for_path(cParseHttpRequestMethod method, const char *path, cParseError **error)
{
    cParseJson *json = NULL;

    cParseRequest *request = NULL;

    if (cparse_str_empty(path)) {
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    request = cparse_request_with_method_and_path(method, path);

    json = cparse_request_get_json(request, error);

    cparse_request_free(request);

    if (json == NULL) {
        return false;
    } else {
        cparse_json_free(json);

        return true;
    }
}
