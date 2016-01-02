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
#include "request.h"
#include "data_list.h"
#include "log.h"

/*! the base domain for Parse requests
 * TODO: make this configurable
 */
const char *const cparse_domain = "https://api.parse.com";

extern const char *const cparse_lib_version;

extern const char *cparse_api_key;

extern const char *cparse_app_id;

const char *const cParseHttpRequestMethodNames[] = {"GET", "POST", "PUT", "DELETE"};

/*! the global client instance
 */
cParseClient *cparse_this_client = NULL;

cParseClient *cparse_client_new()
{
    cParseClient *client = malloc(sizeof(cParseClient));

    if (client == NULL) {
        cparse_log_errno(ENOMEM);
        return NULL;
    }

    client->timeout = CPARSE_CLIENT_TIMEOUT;

    client->headers = NULL;

    client->cURL = NULL;

    client->sessionToken = NULL;

    pthread_mutex_init(&client->lock, NULL);

    return client;
}

cParseClient *cparse_client_with_version(const char *apiVersion)
{
    cParseClient *client = NULL;

    if (cparse_str_empty(apiVersion)) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    client = cparse_client_new();

    if (client == NULL) {
        return NULL;
    }

    client->apiVersion = strdup(apiVersion);

    return client;
}

void cparse_client_free(cParseClient *client)
{
    if (client == NULL) {
        return;
    }

    if (client->cURL) {
        pthread_mutex_lock(&client->lock);
        curl_easy_cleanup(client->cURL);
        pthread_mutex_unlock(&client->lock);
    }

    pthread_mutex_destroy(&client->lock);

    if (client->apiVersion) {
        free(client->apiVersion);
    }

    if (client->headers) {
        curl_slist_free_all(client->headers);
    }

    if (client->sessionToken) {
        free(client->sessionToken);
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
        /* TODO: log some error? curl doesn't say what happened */
        return false;
    }

    return true;
}

static struct curl_slist *cparse_client_default_headers()
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

    return headers;
}


cParseClient *cparse_get_client()
{
    cParseClient *client = cparse_this_client;

    if (client != NULL) {
        return client;
    }

    client = cparse_this_client = cparse_client_with_version(CPARSE_API_VERSION);

    if (client == NULL) {
        cparse_log_error("Could not create client instance, most likely out of memory!");
        return NULL;
    }

    if (client->cURL == NULL) {
        pthread_mutex_lock(&client->lock);
        client->cURL = curl_easy_init();
        pthread_mutex_unlock(&client->lock);

        if (client->cURL == NULL) {
            cparse_log_errno(ENOMEM);
            return false;
        }
    }

    if (client->headers == NULL) {
        client->headers = cparse_client_default_headers();

        if (client->headers == NULL) {
            cparse_log_error("could not create default headers for client, most likely out of memory");
            return false;
        }
    }
    return cparse_this_client;
}

void cparse_free_client()
{
    if (cparse_this_client != NULL) {
        cparse_client_free(cparse_this_client);
        cparse_this_client = NULL;
    }
}

void cparse_client_set_session_token(const char *token)
{
    if (cparse_this_client != NULL) {
        if (token != NULL) {
            cparse_replace_str(&cparse_this_client->sessionToken, token);
        } else if (cparse_this_client->sessionToken != NULL) {
            free(cparse_this_client->sessionToken);
            cparse_this_client->sessionToken = NULL;
        }
    }
}

const char *cparse_client_get_session_token()
{
    if (cparse_this_client != NULL) {
        return cparse_this_client->sessionToken;
    }

    return NULL;
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
    if (!request || cparse_str_empty(value)) {
        cparse_log_errno(ENOMEM);
        return false;
    }

    if (key == NULL) {
        if (!cparse_request_append_body(request, value)) {
            return false;
        }

    } else {
        /* append &key=value */
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

    if (client == NULL || request == NULL) {
        cparse_log_errno(EINVAL);
        return false;
    }
    /*
     * Maximum length of a URI doesn't not seem to be standard.
     * Going with a dynamic string.
     */
    if (!cparse_build_string(&buf, cparse_domain, "/", client->apiVersion, "/", request->path, NULL)) {
        return false;
    }

    if (request->data) {
        if (request->method == cParseHttpRequestMethodGet) {
            if (!cparse_request_build_body(client->cURL, request, true)) {
                free(buf);
                return false;
            }

            if (!cparse_build_string(&buf, "?", request->body, NULL)) {
                /* don't free buf here, cparse_build_string() will do that */
                return false;
            }

        } else {
            if (!cparse_request_build_body(client->cURL, request, false)) {
                free(buf);
                return false;
            }

            curl_easy_setopt(client->cURL, CURLOPT_POSTFIELDS, request->body);
            curl_easy_setopt(client->cURL, CURLOPT_POSTFIELDSIZE, request->bodySize);
        }
    }

    cparse_log_debug("URL: %s", buf);

    if (curl_easy_setopt(client->cURL, CURLOPT_URL, buf) != CURLE_OK) {
        free(buf);
        return false;
    }

    free(buf);

    return true;
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
        headers = cparse_this_client->headers;
    } else {
        struct curl_slist *header = NULL;

        for (header = headers; header != NULL; header = header->next) {
            if (header->next == NULL) {
                header->next = cparse_this_client->headers;
                break;
            }
        }
    }

    return headers;
}

cParseResponse *cparse_client_execute(cParseRequest *request)
{
    cParseClient *client = NULL;
    CURL *curl = NULL;
    CURLcode res = 0;
    cParseResponse *response = NULL;
    struct curl_slist *headers = NULL;

    if (request == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    client = cparse_get_client();

    if (client == NULL) {
        return NULL;
    }

    /* until i get around to simplifing this function */
    curl = client->cURL;

    pthread_mutex_lock(&client->lock);

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

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, client->timeout);

    cparse_client_set_request_url(client, request);

    cparse_log_trace("Method: %s", cParseHttpRequestMethodNames[request->method]);

    if (request->body) {
        cparse_log_trace("Body: %s", request->body);
    }

    headers = cparse_request_build_headers(request);

    if (!cparse_str_empty(client->sessionToken)) {
        if (!cparse_curl_slist_append(&headers, "%s: %s", CPARSE_HEADER_SESSION_TOKEN, client->sessionToken)) {
            pthread_mutex_unlock(&client->lock);
            return NULL;
        }
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    response = cparse_response_new();

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cparse_client_get_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        cparse_log_error("problem with cparse request (%s)", curl_easy_strerror(res));
        cparse_response_free(response);
        pthread_mutex_unlock(&client->lock);
        return NULL;
    }

    cparse_log_trace("Response: %s", response->text);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, (long *)&response->code);

    pthread_mutex_unlock(&client->lock);

    return response;
}
