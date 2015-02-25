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
#include "log.h"

/*! the base domain for Parse requests */
const char *const cparse_domain = "https://api.parse.com";

/*! the Parse api version */
const char *const cparse_api_version = "1";

extern const char *const cparse_lib_version;

extern const char *cparse_api_key;

extern const char *cparse_app_id;

const char *const HttpRequestMethodNames[] = {
    "GET",
    "POST",
    "PUT",
    "DELETE"
};

/*! allocates a new key value list */
struct cparse_kv_list *cparse_kv_list_new()
{
    struct cparse_kv_list *list = malloc(sizeof(struct cparse_kv_list));

    list->next = NULL;
    list->key = NULL;
    list->value = NULL;
    return list;
}

/*! deallocates a key value list */
void cparse_kv_list_free(struct cparse_kv_list *list)
{
    if (!list) return;

    if (list->key)
        free(list->key);
    if (list->value)
        free(list->value);
    free(list);
}

cParseResponse *cparse_client_request_get_response(cParseRequest *request);

/*! allocates a new client request
 * \param method the http method to use
 * \param path the path/endpoint to request
 */
cParseRequest *cparse_client_request_with_method_and_path(HttpRequestMethod method, const char *path)
{
    cParseRequest *request = malloc(sizeof(cParseRequest));

    request->path = strdup(path);
    request->payload = NULL;
    request->payloadSize = 0;
    request->data = NULL;
    request->method = method;
    request->headers = NULL;

    return request;
}

/*! deallocates a client request */
void cparse_client_request_free(cParseRequest *request)
{
    cParseRequestHeader *header, *next_header;
    cParseRequestData *data, *next_data;

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

/*! deallocates a response */
void cparse_client_response_free(cParseResponse *response)
{
    if (response->size > 0 && response->text)
        free(response->text);

    free(response);
}

void cparse_client_request_add_header(cParseRequest *request, const char *key, const char *value)
{
    cParseRequestHeader *header = cparse_kv_list_new();

    header->next = request->headers;
    request->headers = header;

    header->key = strdup(key);
    header->value = strdup(value);
}

void cparse_client_request_set_payload(cParseRequest *request, const char *payload)
{
    cParseRequestData *data, *next_data;

    for (data = request->data; data; data = next_data)
    {
        next_data = data->next;

        if (data->key)
            cparse_kv_list_free(data);
    }

    request->data = NULL;

    data = cparse_kv_list_new();

    data->next = request->data;
    request->data = data;

    data->key = NULL;
    data->value = strdup(payload);
}

void cparse_client_request_add_data(cParseRequest *request, const char *key, const char *value)
{
    cParseRequestData *data = cparse_kv_list_new();

    data->next = request->data;
    request->data = data;

    data->key = strdup(key);
    data->value = strdup(value);
}

static size_t cparse_client_get_response(void *ptr, size_t size, size_t nmemb, void *data)
{
    cParseResponse *s;
    size_t new_len;

    assert(data != NULL);

    s = (cParseResponse *) data;

    new_len = s->size + size * nmemb;

    s->text = realloc(s->text, new_len + 1);
    if (s->text == NULL)
    {
        log_error("realloc() failed");
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

    log_debug("cparse request URL: %s", buf);

    curl_easy_setopt(curl, CURLOPT_URL, buf);
}

static struct curl_slist *cparse_client_set_headers(CURL *curl, cParseRequestHeader *requestHeaders)
{
    char buf[BUFSIZ + 1];

    cParseRequestHeader *header;

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

    return headers;
}

static void cparse_client_set_payload_from_data(CURL *curl, cParseRequest *request, bool encode)
{
    char buf[BUFSIZ + 1] = {0};
    cParseRequestData *data;

    for (data = request->data; data; data = data->next)
    {
        size_t bufLen;

        char *encoded = encode ? curl_easy_escape(curl, data->value, 0) : NULL;

        if (data->key)
            bufLen = snprintf(buf, BUFSIZ, "%s%s=%s", request->payload ? "&" : "", data->key,
                              encoded ? encoded : data->value);
        else
            bufLen = snprintf(buf, BUFSIZ, "%s", encoded ? encoded : data->value);

        if (encoded)
            free(encoded);

        if (request->payload == NULL)
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

bool cparse_client_request_perform(cParseRequest *request, cParseError **error)
{
    cParseResponse *response = cparse_client_request_get_response(request);

    if (response != NULL)
    {
        cparse_client_response_free(response);

        return true;
    }

    return false;
}

cParseJson *cparse_client_response_parse_json(cParseResponse *response, cParseError **error)
{
    json_tokener *tok = json_tokener_new();

    cParseJson *obj = json_tokener_parse_ex(tok, response->text, response->size);

    const char *errorMessage = NULL;

#ifdef HAVE_JSON_TOKENER_GET_ERROR
    enum json_tokener_error parseError = json_tokener_get_error(tok);

    if (parseError != json_tokener_success)
    {
        errorMessage = json_tokener_error_desc(parseError);
    }
#else
    if (obj == NULL)
    {
        errorMessage = "Unable to parse json";
    }
#endif

    if (cparse_json_contains(obj, "error"))
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

        if (obj)
            cparse_json_free(obj);

        return NULL;
    }

    return obj;
}

cParseJson *cparse_client_request_get_json(cParseRequest *request, cParseError **error)
{
    cParseResponse *response = cparse_client_request_get_response(request);

    if (response != NULL)
    {
        cParseJson *json = cparse_client_response_parse_json(response, error);

        cparse_client_response_free(response);

        return json;
    }

    return NULL;
}

cParseResponse *cparse_client_request_get_response(cParseRequest *request)
{
    CURL *curl;
    CURLcode res;
    cParseResponse *response;
    struct curl_slist *headers;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl == NULL)
    {
        log_error("unable to init curl");
        return NULL;
    }

    response = malloc(sizeof(cParseResponse));
    response->text = NULL;
    response->code = 0;
    response->size = 0;

    switch (request->method)
    {
    case HttpRequestMethodPost:
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        break;
    case HttpRequestMethodPut:
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, HttpRequestMethodNames[HttpRequestMethodPut]);
        break;
    case HttpRequestMethodDelete:
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, HttpRequestMethodNames[HttpRequestMethodDelete]);
        break;
    case HttpRequestMethodGet:
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    default:
        break;
    }

    if (request->data)
    {
        if (request->method == HttpRequestMethodGet)
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

        log_trace("cparse request payload: %s", request->payload);
    }
    else
    {
        cparse_client_set_request_url(curl, request->path);
    }

    log_trace("cparse request method: %s", HttpRequestMethodNames[request->method]);

    headers = cparse_client_set_headers(curl, request->headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cparse_client_get_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
        log_error("problem with cparse request %s", curl_easy_strerror(res));

    log_trace("cparse response: %s", response->text);

    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, (long *) &response->code);

    curl_slist_free_all(headers);

    /* always cleanup */
    curl_easy_cleanup(curl);

    curl_global_cleanup();

    return response;
}

bool cparse_client_request(HttpRequestMethod method, const char *path, cParseError **error)
{
    cParseJson *json;

    cParseRequest *request = cparse_client_request_with_method_and_path(method, path);

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

