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

struct cparse_client_request
{
    char *path;
    CPARSE_REQUEST_DATA *data;
    char *payload;
    size_t payloadSize; 
    HTTPRequestMethod method;
    CPARSE_REQUEST_HEADER *headers;
};

struct cparse_client_response
{
    char *text;
    size_t size;
    int code;
};

struct cparse_kv_list
{
    struct cparse_kv_list *next;
    char *key;
    char *value;
};

struct cparse_kv_list *cparse_kv_list_new()
{
    struct cparse_kv_list *list = malloc(sizeof(struct cparse_kv_list));

    list->next = NULL;
    list->key = NULL;
    list->value = NULL;
    return list;
}

void cparse_kv_list_free(struct cparse_kv_list *list)
{
    if(!list) return;

    if(list->key)
        free(list->key);
    if(list->value)
        free(list->value);
    free(list);
}

CPARSE_RESPONSE *cparse_client_request_get_response(CPARSE_REQUEST *request);

CPARSE_REQUEST *cparse_client_request_with_method_and_path(HTTPRequestMethod method, const char *path)
{
    CPARSE_REQUEST *request = malloc(sizeof(CPARSE_REQUEST));

    request->path = strdup(path);
    request->payload = NULL;
    request->payloadSize = 0;
    request->data = NULL;
    request->method = method;
    request->headers = NULL;

    return request;
}

void cparse_client_request_free(CPARSE_REQUEST *request)
{
    CPARSE_REQUEST_HEADER *header, *next_header;
    CPARSE_REQUEST_DATA *data, *next_data;

    if (request->path)
        free(request->path);
    if (request->payload)
        free(request->payload);

    for (header = request->headers; header; header = next_header)
    {
        next_header = header->next;

        cparse_kv_list_free(header);
    }

    for (data = request->data; data; data = next_data)
    {
        next_data = data->next;

        cparse_kv_list_free(data);
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
    CPARSE_REQUEST_HEADER *header = cparse_kv_list_new();

    header->next = request->headers;
    request->headers = header;

    header->key = strdup(key);
    header->value = strdup(value);
}

void cparse_client_request_set_payload(CPARSE_REQUEST *request, const char *payload)
{
    CPARSE_REQUEST_DATA *data, *next_data;

    for (data = request->data; data; data = next_data)
    {
        next_data = data->next;

        cparse_kv_list_free(data);
    }

    request->data = NULL;

    data = cparse_kv_list_new();

    data->next = request->data;
    request->data = data;

    data->value = strdup(payload);
}

void cparse_client_request_add_data(CPARSE_REQUEST *request, const char *key, const char *value)
{
    CPARSE_REQUEST_DATA *data = cparse_kv_list_new();

    data->next = request->data;
    request->data = data;

    data->key = strdup(key);
    data->value = strdup(value);
}

static size_t cparse_client_get_response(void *ptr, size_t size, size_t nmemb, void *data)
{
    CPARSE_RESPONSE *s;
    size_t new_len;

    assert(data != NULL);

    s = (CPARSE_RESPONSE*) data;
    
    new_len = s->size + size * nmemb;

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

static void cparse_client_set_headers(CURL *curl, CPARSE_REQUEST_HEADER *requestHeaders)
{
    char buf[BUFSIZ + 1];

    CPARSE_REQUEST_HEADER *header;

    struct curl_slist *headers = NULL;

    assert(cparse_app_id != NULL);

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

static void cparse_client_set_payload_from_data(CURL *curl, CPARSE_REQUEST * request, bool encode)
{
    char buf[BUFSIZ + 1] = {0};
    CPARSE_REQUEST_DATA *data;

    for(data = request->data; data; data = data->next)
    {   
        size_t bufLen;

        if(data->key)
            bufLen = snprintf(buf, BUFSIZ, "%s%s=%s", request->payload ? "&" : "", data->key, 
                                encode ? curl_easy_escape(curl, data->value, 0) : data->value);
        else
            bufLen = snprintf(buf, BUFSIZ, "%s", encode ? curl_easy_escape(curl, data->value, 0) : data->value);

        if(request->payload == NULL)
        {
            request->payload = strdup(buf);
        }
        else
        {
            request->payload = realloc(request->payload, request->payloadSize + bufLen + 1);
            strncat(request->payload, buf, bufLen);
        }
        request->payloadSize += bufLen;
    }
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

#ifdef HAVE_JSON_TOKENER_GET_ERROR
    enum json_tokener_error parseError = json_tokener_get_error(tok);

    if (parseError != json_tokener_success)
    {
        errorMessage = json_tokener_error_desc(parseError);
    }
#else
    if(obj == NULL)
    {
        errorMessage = "Unable to parse json";
    }
#endif    
    else
    {
        errorMessage = cparse_json_get_string(obj, "error");
    }

    json_tokener_free(tok);

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
        break;
    case HTTPRequestMethodPut:
        curl_easy_setopt(curl, CURLOPT_PUT, 1L);
        break;
    case HTTPRequestMethodDelete:
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    case HTTPRequestMethodGet:
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    default:
        break;
    }

    if(request->data)
    {
        if (request->method == HTTPRequestMethodGet)
        {
            char buf[BUFSIZ + 1] = {0};

            cparse_client_set_payload_from_data(curl, request, true);

            snprintf(buf, BUFSIZ,  "%s?%s", request->path, request->payload);

            cparse_client_set_request_url(curl, buf);
        }
        else
        {
            cparse_client_set_payload_from_data(curl, request, false);

            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->payload);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, request->payloadSize);

            cparse_client_set_request_url(curl, request->path);
        }
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
        fprintf(stderr, "problem requesting %s with curl %s\n", request->path, curl_easy_strerror(res));

    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, (long*) &response->code);

    /* always cleanup */
    curl_easy_cleanup(curl);

    return response;
}

bool cparse_client_request(HTTPRequestMethod method, const char *path, CPARSE_ERROR **error)
{
    CPARSE_JSON *json;

    CPARSE_REQUEST *request = cparse_client_request_with_method_and_path(method, path);

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

