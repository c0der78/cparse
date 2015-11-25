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
#include "client.h"
#include "protocol.h"
#include "private.h"
#include "log.h"

/*! the base domain for Parse requests */
const char *const cparse_domain = "https://api.parse.com";

extern const char *const cparse_lib_version;

extern const char *cparse_api_key;

extern const char *cparse_app_id;

int cparse_client_request_timeout = 0;

char cparse_client_session_token[CPARSE_BUF_SIZE + 1] = {0};

const char *const cParseHttpRequestMethodNames[] = {"GET", "POST", "PUT", "DELETE"};

cParseClient *cparse_client_instance = NULL;

cParseClient *cparse_client_with_version(const char *apiVersion)
{
    cParseClient *client = NULL;

    if (cparse_str_empty(apiVersion)) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    if (cparse_str_empty(cparse_app_id) || cparse_str_empty(cparse_api_key)) {
        cparse_log_error("cparse not configured");
        return NULL;
    }

    client = malloc(sizeof(cParseClient));

    if (client == NULL) {
        cparse_log_errno(ENOMEM);
        return NULL;
    }

    return client;
}

cParseClient *cparse_client_new()
{
    return cparse_client_with_version(CPARSE_API_VERSION);
}

void cparse_client_free(cParseClient *client)
{
    if (client == NULL) {
        return;
    }

    if (client->cURL) {
        curl_easy_cleanup(client->cURL);
    }

    if (client->apiVersion) {
        free(client->apiVersion);
    }

    if (client->headers) {
        curl_slist_free_all(client->headers);
    }

    free(client);
}

static bool cparse_curl_slist_append(struct curl_slist **list, const char *format, ...)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};
    va_list args;
    int rval = 0;

    va_start(args, format);
    rval = vsnprintf(buf, CPARSE_BUF_SIZE, format, args);
    va_end(args);

    if (rval < 0) {
        cparse_log_errno(errno);
        return false;
    }

    *list = curl_slist_append(*list, buf);

    if (list == NULL) {
        // TODO: log some error? curl doesn't say what happened
        return false;
    }

    return true;
}

struct curl_slist *cparse_client_default_headers()
{
    struct curl_slist *headers = NULL;

    if (cparse_str_empty(cparse_app_id) || cparse_str_empty(cparse_api_key)) {
        cparse_log_error("cparse not configured");
        return NULL;
    }

    if (!cparse_curl_slist_append(&headers, "Content-Type: application/json")) {
        return NULL;
    }

    if (!cparse_curl_slist_append(&headers, "User-Agent: libcparse-%s", cparse_lib_version)) {
        return NULL;
    }

    if (!cparse_curl_slist_append(&headers, "%s: %s", CPARSE_HEADER_APP_ID, cparse_app_id)) {
        return NULL;
    }

    if (!cparse_curl_slist_append(&headers, "%s: %s", CPARSE_HEADER_API_KEY, cparse_api_key)) {
        return NULL;
    }

    if (!cparse_str_empty(cparse_client_session_token)) {
        if (!cparse_curl_slist_append(&headers, "%s: %s", CPARSE_HEADER_SESSION_TOKEN, cparse_client_session_token)) {
            return NULL;
        }
    }

    return headers;
}
bool cparse_client_init(cParseClient *client, const char *apiVersion)
{
    if (client == NULL) {
        cparse_log_errno(EINVAL);
        return false;
    }

    client->cURL = curl_easy_init();

    if (client->cURL == NULL) {
        cparse_log_errno(ENOMEM);
        return false;
    }

    client->apiVersion = strdup(apiVersion);

    client->headers = cparse_client_default_headers();

    if (client->headers == NULL) {
        cparse_log_error("could not create default headers for client, most likely out of memory");
        return false;
    }

    return true;
}


/*! allocates a new key value list */
struct cparse_kv_list *cparse_kv_list_new()
{
    struct cparse_kv_list *list = malloc(sizeof(struct cparse_kv_list));

    if (list == NULL) {
        cparse_log_errno(ENOMEM);
        return NULL;
    }

    list->next = NULL;
    list->key = NULL;
    list->value = NULL;
    return list;
}

/*! deallocates a key value list */
void cparse_kv_list_free(struct cparse_kv_list *list)
{
    if (!list) {
        return;
    }

    if (list->key) {
        free(list->key);
    }
    if (list->value) {
        free(list->value);
    }
    free(list);
}

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

        cparse_kv_list_free(header);
    }

    for (data = request->data; data; data = next_data) {
        next_data = data->next;

        cparse_kv_list_free(data);
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

    header = cparse_kv_list_new();

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
    cParseRequestData *data = NULL, *next_data = NULL;

    if (request == NULL || cparse_str_empty(body)) {
        cparse_log_errno(EINVAL);
        return;
    }

    /* free the request data, as the two are synonymous */
    for (data = request->data; data; data = next_data) {
        next_data = data->next;

        /* data has a key */
        if (data->key) {
            cparse_kv_list_free(data);
        }
    }

    request->data = NULL;

    data = cparse_kv_list_new();

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
    cParseRequestData *data = NULL, *next_data = NULL;

    if (!request || cparse_str_empty(key) || cparse_str_empty(value)) {
        cparse_log_errno(EINVAL);
        return;
    }

    /* free the request body, as the two are synonymous */
    for (data = request->data; data; data = next_data) {
        next_data = data->next;

        /* body has no key */
        if (data->key == NULL) {
            cparse_kv_list_free(data);
        }
    }

    request->data = NULL;

    data = cparse_kv_list_new();

    if (data == NULL) {
        return;
    }

    data->next = request->data;
    request->data = data;

    data->key = strdup(key);
    data->value = strdup(value);
}

static size_t cparse_client_get_response(void *ptr, size_t size, size_t nmemb, void *data)
{
    cParseResponse *s = NULL;
    size_t new_len = 0;

    if (data == NULL || ptr == NULL) {
        cparse_log_errno(EINVAL);
        return 0;
    }

    s = (cParseResponse *)data;

    new_len = s->size + size * nmemb;

    s->text = realloc(s->text, new_len + 1);
    if (s->text == NULL) {
        cparse_log_errno(ENOMEM);
        return 0;
    }
    memcpy(s->text + s->size, ptr, size * nmemb);
    s->text[new_len] = '\0';
    s->size = new_len;

    return size * nmemb;
}

static bool cparse_request_append_body(cParseRequest *request, const char *value)
{
    size_t size = 0;

    if (!request || !value) {
        cparse_log_errno(EINVAL);
        return false;
    }

    size = strlen(value);

    if (!cparse_str_append(&request->body, value, size)) {
        return false;
    }

    request->bodySize += size;
    return true;
}


static bool cparse_request_append_data(cParseRequest *request, const char *key, const char *value)
{
    size_t ssize = 0;

    if (!request || cparse_str_empty(value)) {
        cparse_log_errno(ENOMEM);
        return false;
    }


    if (key == NULL) {
        ssize = strlen(value);

        if (!cparse_request_append_body(request, value)) {
            return false;
        }

    } else {
        if (!cparse_str_empty(request->body)) {
            if (!cparse_request_append_body(request, "&")) {
                return false;
            }
        }

        if (!cparse_request_append_body(request, key)) {
            return false;
        }

        if (!cparse_request_append_body(request, "=")) {
            return false;
        }

        if (!cparse_request_append_body(request, value)) {
            return false;
        }
    }

    return true;
}

static bool cparse_request_build_body(cParseClient *client, cParseRequest *request, bool encode)
{
    cParseRequestData *data = NULL;

    if (!client || !request) {
        cparse_log_errno(EINVAL);
        return false;
    }

    for (data = request->data; data; data = data->next) {
        char *encoded = encode ? curl_easy_escape(client->cURL, data->value, 0) : NULL;

        if (!cparse_request_append_data(request, data->key, encoded ? encoded : data->value)) {
            return false;
        }

        if (encoded) {
            free(encoded);
        }
    }

    return true;
}
static bool cparse_client_set_request_url(cParseClient *client, cParseRequest *request)
{
    char *buf = NULL;
    bool success = true;

    if (client == NULL || !request) {
        cparse_log_errno(EINVAL);
        return false;
    }

    /*
     * Maximum length of a URI doesn't not seem to be standard.
     * Going with a dynamic string.
     */
    buf = strdup(cparse_domain);

    if (buf == NULL) {
        cparse_log_errno(ENOMEM);
        return false;
    }

    success = success && cparse_str_append(&buf, "/", 1);

    success = success && cparse_str_append(&buf, client->apiVersion, strlen(client->apiVersion));

    success = success && cparse_str_append(&buf, "/", 1);

    success = success && cparse_str_append(&buf, request->path, strlen(request->path));

    if (success && request->data) {
        if (request->method == cParseHttpRequestMethodGet) {
            success = success && cparse_request_build_body(client->cURL, request, true);

            success = success && cparse_str_append(&buf, "?", 1);

            success = success && cparse_str_append(&buf, request->body, request->bodySize);

        } else {
            success = success && cparse_request_build_body(client->cURL, request, false);

            curl_easy_setopt(client->cURL, CURLOPT_POSTFIELDS, request->body);
            curl_easy_setopt(client->cURL, CURLOPT_POSTFIELDSIZE, request->bodySize);
        }
    }

    success = success && curl_easy_setopt(client->cURL, CURLOPT_URL, buf) == CURLE_OK;

    if (success) {
        cparse_log_debug("URL: %s", buf);
    }

    free(buf);

    return success;
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
    enum json_tokener_error;
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

static struct curl_slist *cparse_request_build_headers(cParseRequest *request)
{
    struct curl_slist *headers = NULL;


    if (request->headers != NULL) {
        cParseRequestHeader *header = NULL;

        for (header = request->headers; header != NULL; header = header->next) {
            if (!cparse_curl_slist_append(&headers, "%s: %s", header->key, header->value)) {
                cparse_log_error("Could not build HTTP headers for request, likely out of memory.");
                return NULL;
            }
        }
    }

    if (headers == NULL) {
        headers = cparse_client_instance->headers;
    } else {
        struct curl_slist *header = NULL;

        for (header = headers; header != NULL; header = header->next) {
            if (header->next == NULL) {
                header->next = cparse_client_instance->headers;
                break;
            }
        }
    }

    return headers;
}

cParseResponse *cparse_client_execute(cParseRequest *request)
{
    CURL *curl = NULL;
    CURLcode res = 0;
    cParseResponse *response = NULL;
    struct curl_slist *headers = NULL;

    if (request == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    if (cparse_client_instance == NULL) {
        cparse_client_instance = cparse_client_new();

        if (cparse_client_instance == NULL || !cparse_client_init(cparse_client_instance, CPARSE_API_VERSION)) {
            cparse_log_error("Could not create client instance, most likely out of memory!");
            return NULL;
        }
    }

    /* until i get around to simplifing this function */
    curl = cparse_client_instance->cURL;

    /* reset from last request */
    curl_easy_reset(curl);

    switch (request->method) {
        case cParseHttpRequestMethodPost:
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            break;
        case cParseHttpRequestMethodPut:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, cParseHttpRequestMethodNames[cParseHttpRequestMethodPut]);
            break;
        case cParseHttpRequestMethodDelete:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, cParseHttpRequestMethodNames[cParseHttpRequestMethodDelete]);
            break;
        case cParseHttpRequestMethodGet:
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        default:
            break;
    }

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, cparse_client_request_timeout);

    cparse_client_set_request_url(cparse_client_instance, request);

    cparse_log_trace("Method: %s", cParseHttpRequestMethodNames[request->method]);

    if (request->body) {
        cparse_log_trace("Body: %s", request->body);
    }

    headers = cparse_request_build_headers(request);

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    response = cparse_response_new();

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cparse_client_get_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        cparse_log_error("problem with cparse request (%s)", curl_easy_strerror(res));
        cparse_response_free(response);
        return NULL;
    }

    cparse_log_trace("Response: %s", response->text);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, (long *)&response->code);

    return response;
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
