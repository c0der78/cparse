#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <curl/curl.h>
#include "json_private.h"
#include <cparse/json.h>
#include <cparse/object.h>
#include <cparse/error.h>
#include "client.h"
#include "protocol.h"
#include "private.h"

const char *const cparse_domain = "https://api.parse.com";
const char *const cparse_api_version = "1";

extern const char *const cparse_lib_version;

extern const char *cparse_api_key;

extern const char *cparse_app_id;

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

CPARSE_RESPONSE *cparse_client_request_get_response(CPARSE_REQUEST *request);

CPARSE_REQUEST *cparse_client_request_new()
{
    CPARSE_REQUEST *request = malloc(sizeof(CPARSE_REQUEST));

    request->path = NULL;
    request->payload = NULL;
    request->method = HTTPRequestMethodGet;
    request->headers = NULL;

    return request;
};

void cparse_client_request_free(CPARSE_REQUEST *request)
{
    REQUEST_HEADER *header, *next_header;

    if (request->path)
        free(request->path);
    if (request->payload)
        free(request->payload);

    for (header = request->headers; header; header = next_header)
    {
        next_header = header->next;

        if (header->key)
            free(header->key);
        if (header->value)
            free(header->value);

        free(header);
    }
    free(request);
}

void cparse_client_response_free(CPARSE_RESPONSE *response)
{
    if (response->size > 0 && response->text)
        free(response->text);

    free(response);
}

void cparse_client_request_add_header(CPARSE_REQUEST *request, const char *key, const char *value)
{
    REQUEST_HEADER *header = malloc(sizeof(REQUEST_HEADER));

    header->next = request->headers;
    request->headers = header;

    header->key = strdup(key);
    header->value = strdup(value);
}

static size_t cparse_client_get_response(void *ptr, size_t size, size_t nmemb, CPARSE_RESPONSE *s)
{
    assert(s != NULL);

    size_t new_len = s->size + size * nmemb;
    s->text = realloc(s->text, new_len + 1);
    if (s->text == NULL)
    {
        fputs("realloc() failed", stderr);
        exit(EXIT_FAILURE);
    }
    memcpy(s->text + s->size, ptr, size * nmemb);
    s->text[new_len] = '\0';
    s->size = new_len;

    return size * nmemb;
}

static void cparse_client_set_request_url(CURL *curl, const char *path)
{
    char buf[BUFSIZ + 1];

    snprintf(buf, BUFSIZ, "%s/%s/%s", cparse_domain, cparse_api_version, path);

    curl_easy_setopt(curl, CURLOPT_URL, buf);
}

static void cparse_client_set_headers(CURL *curl, REQUEST_HEADER *requestHeaders)
{
    assert(cparse_app_id != NULL);

    char buf[BUFSIZ + 1];

    REQUEST_HEADER *header;

    struct curl_slist *headers = NULL;

    snprintf(buf, BUFSIZ, "%s: %s", HEADER_APP_ID, cparse_app_id);

    headers = curl_slist_append(headers, buf);

    snprintf(buf, BUFSIZ, "%s: %s", HEADER_API_KEY, cparse_api_key);

    headers = curl_slist_append(headers, buf);

    headers = curl_slist_append(headers, "Content-Type: application/json");

    snprintf(buf, BUFSIZ, "User-Agent: libcparse-%s", cparse_lib_version);

    headers = curl_slist_append(headers, buf);

    for (header = requestHeaders; header; header = header->next)
    {
        snprintf(buf, BUFSIZ, "%s: %s", header->key, header->value);

        headers = curl_slist_append(headers, buf);
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

bool cparse_client_request_perform(CPARSE_REQUEST *request, CPARSE_ERROR **error)
{
    CPARSE_RESPONSE *response = cparse_client_request_get_response(request);

    if (response != NULL)
    {
        cparse_client_response_free(response);

        return true;
    }

    return false;
}

CPARSE_JSON *cparse_client_response_parse_json(CPARSE_RESPONSE *response, CPARSE_ERROR **error)
{
    json_tokener *tok = json_tokener_new();

    CPARSE_JSON *obj = json_tokener_parse_ex(tok, response->text, response->size);

    const char *errorMessage = NULL;

    enum json_tokener_error parseError = json_tokener_get_error(tok);

    json_tokener_free(tok);

    if (parseError != json_tokener_success)
    {
#ifdef HAVE_JSON_TOKENER_ERROR_DESC
        errorMessage = json_tokener_error_desc(parseError);
#else
        errorMessage = json_tokener_errors[parseError];
#endif
    }
    else
    {
        errorMessage = cparse_json_get_string(obj, "error");
    }

    if (errorMessage != NULL)
    {
        if (error)
        {
            *error = cparse_error_with_message(errorMessage);

            cparse_error_set_code(*error, cparse_json_get_number(obj, "code", 0));
        }

        return NULL;
    }

    return obj;
}

CPARSE_JSON *cparse_client_request_get_json(CPARSE_REQUEST *request, CPARSE_ERROR **error)
{
    CPARSE_RESPONSE *response = cparse_client_request_get_response(request);

    if (response != NULL)
    {
        CPARSE_JSON *json = cparse_client_response_parse_json(response, error);

        cparse_client_response_free(response);

        return json;
    }

    return NULL;
}

CPARSE_RESPONSE *cparse_client_request_get_response(CPARSE_REQUEST *request)
{
    CURL *curl;
    CURLcode res;
    CPARSE_RESPONSE *response;

    curl = curl_easy_init();
    if (curl == NULL)
    {
        fputs("unable to init curl", stderr);
        return NULL;
    }

    response = malloc(sizeof(CPARSE_RESPONSE));
    response->text = NULL;
    response->code = 0;
    response->size = 0;

    switch (request->method)
    {
    case HTTPRequestMethodPost:
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->payload);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request->payload));
        break;
    case HTTPRequestMethodPut:
        curl_easy_setopt(curl, CURLOPT_PUT, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->payload);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request->payload));
        break;
    case HTTPRequestMethodDelete:
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    case HTTPRequestMethodGet:
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    default:
        break;
    }

    if (request->method == HTTPRequestMethodGet && request->payload)
    {
        char buf[BUFSIZ + 1] = {0};
        snprintf(buf, BUFSIZ, "%s?%s", request->path, request->payload);
        cparse_client_set_request_url(curl, buf);
    }
    else
    {
        cparse_client_set_request_url(curl, request->path);
    }

    cparse_client_set_headers(curl, request->headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cparse_client_get_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
        fputs("problem requesting with curl", stderr);

    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &response->code);

    /* always cleanup */
    curl_easy_cleanup(curl);

    return response;
}

bool cparse_client_object_request(CPARSE_OBJ *obj, HTTPRequestMethod method, const char *payload, CPARSE_ERROR **error)
{
    char buf[BUFSIZ + 1] = {0};
    CPARSE_JSON *json;
    CPARSE_RESPONSE *response;

    CPARSE_REQUEST *request;

    if (!obj)
    {
        return false;
    }

    if (cparse_object_is_user(obj))
    {
        switch (method)
        {
        default:

            if (!obj->objectId || !*obj->objectId)
            {
                if (error)
                    *error = cparse_error_with_message("Object has no id");
                return false;
            }

            snprintf(buf, BUFSIZ, "%s/%s", obj->className, obj->objectId);
            break;
        case HTTPRequestMethodPost:
            snprintf(buf, BUFSIZ, "%s", obj->className);
            break;
        }
    }
    else
    {
        switch (method)
        {
        default:

            if (!obj->objectId || !*obj->objectId)
            {
                if (error)
                    *error = cparse_error_with_message("Object has no id");
                return false;
            }

            snprintf(buf, BUFSIZ, "classes/%s/%s", obj->className, obj->objectId);
            break;
        case HTTPRequestMethodPost:
            snprintf(buf, BUFSIZ, "classes/%s", obj->className);
            break;
        }
    }

    request = cparse_client_request_new();

    request->path = strdup(buf);

    request->method = method;

    if (cparse_object_contains(obj, KEY_USER_SESSION_TOKEN))
    {
        const char *token = cparse_object_get_string(obj, KEY_USER_SESSION_TOKEN);

        if (token)
        {
            cparse_client_request_add_header(request, HEADER_SESSION_TOKEN, token);
        }
    }

    if (payload)
    {
        request->payload = strdup(payload);
    }

    /* do the deed */
    response = cparse_client_request_get_response(request);

    cparse_client_request_free(request);

    /* TODO: check HTTP code */

    json = cparse_client_response_parse_json(response, error);

    if (json == NULL)
    {
        return false;
    }

    /* merge the result with the object */
    cparse_object_merge_json(obj, json);

    cparse_json_free(json);

    return true;
}


bool cparse_client_request(HTTPRequestMethod method, const char *path, CPARSE_ERROR **error)
{
    CPARSE_JSON *json;

    CPARSE_REQUEST *request = cparse_client_request_new();

    request->path = strdup(path);

    request->method = method;

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json == NULL)
    {
        return false;
    }
    else
    {
        cparse_json_free(json);

        return true;
    }
}

