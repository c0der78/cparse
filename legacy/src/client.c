#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <curl/curl.h>
#include <cparse/json.h>
#include <cparse/object.h>
#include <cparse/error.h>
#include "client.h"
#include "protocol.h"

const char *const cparse_domain = "https://api.parse.com";
const char *const cparse_api_version = "1";

extern const char *const cparse_lib_version;

extern const char *cparse_api_key;

extern const char *cparse_app_id;

CPARSE_CLIENT_REQ *cparse_client_request_new()
{
    CPARSE_CLIENT_REQ *request = malloc(sizeof(CPARSE_CLIENT_REQ));

    request->path = NULL;
    request->payload = NULL;
    request->method = kHTTPRequestGet;

    return request;
};

void cparse_client_request_free(CPARSE_CLIENT_REQ *request)
{
    if (request->path)
        free(request->path);
    if (request->payload)
        free(request->payload);

    free(request);
}

void cparse_client_response_free(CPARSE_CLIENT_RESP *response)
{
    if (response->size > 0 && response->text)
        free(response->text);

    free(response);
}

static size_t cparse_client_get_response(void *ptr, size_t size, size_t nmemb, CPARSE_CLIENT_RESP *s)
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

static void cparse_client_set_headers(CURL *curl)
{
    assert(cparse_app_id != NULL);

    char buf[BUFSIZ + 1];

    struct curl_slist *headers = NULL;

    snprintf(buf, BUFSIZ, "%s: %s", HEADER_APP_ID, cparse_app_id);

    headers = curl_slist_append(headers, buf);

    snprintf(buf, BUFSIZ, "%s: %s", HEADER_API_KEY, cparse_api_key);

    headers = curl_slist_append(headers, buf);

    headers = curl_slist_append(headers, "Content-Type: application/json");

    snprintf(buf, BUFSIZ, "User-Agent: libcparse-%s", cparse_lib_version);

    headers = curl_slist_append(headers, buf);

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

void cparse_client_request_perform(CPARSE_CLIENT_REQ *request, CPARSE_ERROR **error)
{
    CPARSE_CLIENT_RESP *response = cparse_client_request_get_response(request);

    CPARSE_JSON *obj = json_tokener_parse(response->text);

    const char *errorMessage = cparse_json_get_string(obj, "error");

    cparse_client_response_free(response);

    if (errorMessage != NULL)
    {
        *error = cparse_error_new();

        (*error)->message = strdup(errorMessage);

        (*error)->code = cparse_json_get_number(obj, "code");
    }

    cparse_json_free(obj);
}

CPARSE_JSON *cparse_client_request_get_json(CPARSE_CLIENT_REQ *request, CPARSE_ERROR **error)
{
    CPARSE_CLIENT_RESP *response = cparse_client_request_get_response(request);

    json_tokener *tok = json_tokener_new();

    CPARSE_JSON *obj = json_tokener_parse_ex(tok, response->text, response->size);

    cparse_client_response_free(response);

    const char *errorMessage = NULL;

    enum json_tokener_error parseError = json_tokener_get_error(tok);

    json_tokener_free(tok);

    if (parseError != json_tokener_success)
    {
        errorMessage = json_tokener_error_desc(parseError);
    }

    else
    {
        errorMessage = cparse_json_get_string(obj, "error");
    }

    if (errorMessage != NULL)
    {
        *error = cparse_error_new();

        (*error)->message = strdup(errorMessage);

        (*error)->code = cparse_json_get_number(obj, "code");

        cparse_json_free(obj);

        return NULL;
    }

    return obj;
}

CPARSE_CLIENT_RESP *cparse_client_request_get_response(CPARSE_CLIENT_REQ *request)
{
    CURL *curl;
    CURLcode res;
    CPARSE_CLIENT_RESP *response;

    curl = curl_easy_init();
    if (curl == NULL)
    {
        fputs("unable to init curl", stderr);
        return NULL;
    }

    response = malloc(sizeof(CPARSE_CLIENT_RESP));
    response->text = NULL;
    response->code = 0;
    response->size = 0;

    switch (request->method)
    {
    case kHTTPRequestPost:
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->payload);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request->payload));
        break;
    case kHTTPRequestPut:
        curl_easy_setopt(curl, CURLOPT_PUT, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request->payload);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request->payload));
        break;
    case kHTTPRequestDelete:
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    case kHTTPRequestGet:
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

    default:
        break;
    }


    cparse_client_set_request_url(curl, request->path);
    cparse_client_set_headers(curl);
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

