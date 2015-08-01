#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cparse/user.h>
#include <cparse/parse.h>
#include <cparse/json.h>
#include <cparse/object.h>
#include <cparse/util.h>
#include <errno.h>
#include "protocol.h"
#include "client.h"
#include "private.h"
#include "log.h"

cParseUser *__cparse_current_user = NULL;

extern cParseUser *cparse_object_new();

extern cParseQuery *cparse_query_new();

extern void cparse_object_set_request_includes(cParseObject *obj, cParseRequest *request);

extern cParseRequest *cparse_object_create_request(cParseObject *obj, cParseHttpRequestMethod method, cParseError **error);

void (*cparse_user_free)(cParseUser *user) = &cparse_object_free;

bool (*cparse_user_delete)(cParseUser *obj, cParseError **error) = cparse_object_delete;

cparse_thread (*cparse_user_delete_in_background)(cParseUser *obj, cParseObjectCallback callback, void *param) = cparse_object_delete_in_background;

bool (*cparse_user_fetch)(cParseUser *obj, cParseError **error) = cparse_object_fetch;

cparse_thread (*cparse_user_fetch_in_background)(cParseUser *obj, cParseObjectCallback callback, void *param) = cparse_object_fetch_in_background;

bool (*cparse_user_refresh)(cParseUser *obj, cParseError **error) = cparse_object_refresh;

cparse_thread (*cparse_user_refresh_in_background)(cParseUser *user, cParseObjectCallback callback, void *param) = cparse_object_refresh_in_background;

extern char cparse_client_session_token[];

extern bool cparse_revocable_sessions;

/* initializers */

cParseUser * cparse_user_new()
{
    cParseUser *obj = cparse_object_new();

    obj->className = strdup(CPARSE_CLASS_USER);

    obj->urlPath = strdup(CPARSE_USERS_PATH);

    return obj;
}

cParseUser *cparse_user_with_name(const char *username)
{
    cParseUser *obj = cparse_object_new();

    obj->className = strdup(CPARSE_CLASS_USER);

    obj->urlPath = strdup(CPARSE_USERS_PATH);

    cparse_object_set_string(obj, CPARSE_KEY_USER_NAME, username);

    return obj;
}

cParseUser *cparse_current_user(cParseError **error)
{
    if (__cparse_current_user != NULL) {
        return __cparse_current_user;
    }

    if (cparse_str_empty(cparse_client_session_token)) {
        cparse_log_set_error(error, "No valid session token");
        return NULL;
    }

    __cparse_current_user = cparse_user_validate(cparse_client_session_token, error);

    return __cparse_current_user;
}

/* getters/setters */

bool cparse_object_is_user(cParseObject *obj)
{
    if (!obj) {
        return false;
    }

    return !cparse_str_cmp(obj->className, CPARSE_CLASS_USER);
}

const char *cparse_user_name(cParseUser *user)
{
    if (!user) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    return cparse_object_get_string(user, CPARSE_KEY_USER_NAME);
}

void cparse_user_set_name(cParseUser *user, const char *name)
{
    if (!user || cparse_str_empty(name))
    {
        cparse_log_errno(EINVAL);
        return;
    }

    cparse_object_set_string(user, CPARSE_KEY_USER_NAME, name);
}

const char *cparse_user_email(cParseUser *user)
{
    if (user == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    return cparse_object_get_string(user, CPARSE_KEY_USER_EMAIL);
}

const char *cparse_user_session_token(cParseUser *user)
{
    if (user == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    return cparse_object_get_string(user, CPARSE_KEY_USER_SESSION_TOKEN);
}

/* functions */

static bool cparse_user_login_user(cParseUser *user, cParseError **error)
{
    cParseJson *data = NULL;
    cParseRequest *request = NULL;
    const char *username = NULL, *password = NULL;

    if (!user) {
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    username = cparse_object_get_string(user, CPARSE_KEY_USER_NAME);

    password = cparse_object_get_string(user, CPARSE_KEY_USER_PASSWORD);

    if (cparse_str_empty(username))
    {
        cparse_log_set_error(error, "No username provided");
        return false;
    }

    if (cparse_str_empty(password))
    {
        cparse_log_set_error(error, "No password provided");
        return false;

    }

    request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodGet, "login");

    if (cparse_revocable_sessions) {
        cparse_client_request_add_header(request, CPARSE_HEADER_REVOCABLE_SESSION, "1");
    }

    cparse_client_request_add_data(request, "username", username);
    cparse_client_request_add_data(request, "password", password);

    /* do the deed */
    data = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    cparse_object_remove(user, CPARSE_KEY_USER_PASSWORD);

    if (data == NULL)
    {
        return false;
    }

    cparse_object_merge_json(user, data);

    cparse_json_free(data);

    if (cparse_object_contains(user, CPARSE_KEY_USER_SESSION_TOKEN))
    {
        const char *sessionToken = cparse_object_get_string(user, CPARSE_KEY_USER_SESSION_TOKEN);

        if (sessionToken)
        {
            strncpy(cparse_client_session_token, sessionToken, CPARSE_BUF_SIZE);
        }

        __cparse_current_user = user;
    }
    return true;
}

cParseUser *cparse_user_login(const char *username, const char *password, cParseError **error)
{
    cParseUser *user = NULL;

    if (cparse_str_empty(username) || cparse_str_empty(password)) {
        cparse_log_set_errno(error, EINVAL);
        return NULL;
    }

    user = cparse_user_with_name(username);

    cparse_object_set_string(user, CPARSE_KEY_USER_PASSWORD, password);

    if (!cparse_user_login_user(user, error))
    {
        cparse_user_free(user);
        return NULL;
    }

    return user;
}

cparse_thread cparse_user_login_in_background(const char *username, const char *password, cParseObjectCallback callback, void *param)
{
    cParseUser *obj = NULL;

    if (cparse_str_empty(username) || cparse_str_empty(password)) {
        cparse_log_errno(EINVAL);
        return 0;
    }

    obj = cparse_user_with_name(username);

    cparse_object_set_string(obj, CPARSE_KEY_USER_PASSWORD, password);

    return cparse_object_run_in_background(obj, cparse_user_login_user, callback, param, cparse_object_free);
}

void cparse_user_logout()
{
    memset(cparse_client_session_token, 0, CPARSE_BUF_SIZE);
    __cparse_current_user = NULL;
}


cParseQuery *cparse_user_query_new()
{
    cParseQuery *query = cparse_query_new();

    query->className = strdup(CPARSE_CLASS_USER);

    query->urlPath = strdup(CPARSE_USERS_PATH);

    return query;
}

static bool cparse_user_sign_up_user(cParseUser *user, cParseError **error)
{
    const char *username = NULL;
    const char *password = NULL;
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!user) {
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    username = cparse_object_get_string(user, CPARSE_KEY_USER_NAME);
    password = cparse_object_get_string(user, CPARSE_KEY_USER_PASSWORD);

    if (cparse_str_empty(username))
    {
        cparse_log_set_error(error, "User has no username");
        return false;
    }

    if (cparse_str_empty(password))
    {
        cparse_log_set_error(error, "No password provided");
        return false;
    }

    request = cparse_object_create_request(user, cParseHttpRequestMethodPost, error);

    if (request == NULL) {
        return false;
    }

    cparse_client_request_set_payload(request, cparse_object_to_json_string(user));

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    cparse_object_remove(user, CPARSE_KEY_USER_PASSWORD);

    if (json != NULL)
    {
        cparse_object_merge_json(user, json);

        cparse_json_free(json);

        if (cparse_object_contains(user, CPARSE_KEY_USER_SESSION_TOKEN))
        {
            const char *sessionToken = cparse_object_get_string(user, CPARSE_KEY_USER_SESSION_TOKEN);

            if (sessionToken) {
                strncpy(cparse_client_session_token, sessionToken, CPARSE_BUF_SIZE);
            }

            __cparse_current_user = user;
        }
        return true;
    }

    return false;
}

bool cparse_user_sign_up(cParseUser *user, const char *password, cParseError **error)
{
    if (!cparse_str_empty(password)) {
        cparse_object_set_string(user, CPARSE_KEY_USER_PASSWORD, password);
    }

    return cparse_user_sign_up_user(user, error);
}

cparse_thread cparse_user_sign_up_in_background(cParseUser *user, const char *password, cParseObjectCallback callback, void *param)
{
    if (!cparse_str_empty(password)) {
        cparse_object_set_string(user, CPARSE_KEY_USER_PASSWORD, password);
    }

    return cparse_object_run_in_background(user, cparse_user_sign_up_user, callback, param, NULL);
}

cParseUser * cparse_user_validate(const char *sessionToken, cParseError **error)
{
    cParseRequest *request = NULL;
    cParseJson *json = NULL;
    cParseUser *user = NULL;
    char buf[CPARSE_BUF_SIZE + 1] = {0};

    if (cparse_str_empty(sessionToken))
    {
        cparse_log_set_error(error, "missing session token");
        return NULL;
    }

    snprintf(buf, CPARSE_BUF_SIZE, "%s/me", CPARSE_USERS_PATH);

    request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodGet, buf);

    if (request == NULL) {
        cparse_log_set_error(error, "could not create client request");
        return NULL;
    }

    cparse_client_request_add_header(request, CPARSE_HEADER_SESSION_TOKEN, sessionToken);

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json == NULL)
    {
        return NULL;
    }

    user = cparse_user_new();

    if (user == NULL) {
        return NULL;
    }

    cparse_object_merge_json(user, json);

    cparse_json_free(json);

    return user;
}

bool cparse_user_validate_email(cParseUser *user, cParseError **error)
{
    if (!user) {
        return false;
    }

    if (!cparse_object_contains(user, CPARSE_KEY_EMAIL_VERIFIED))
    {
        return false;
    }

    if (!cparse_object_get_bool(user, CPARSE_KEY_EMAIL_VERIFIED))
    {
        if (!cparse_user_refresh(user, error))
        {
            return false;
        }

        return cparse_object_get_bool(user, CPARSE_KEY_EMAIL_VERIFIED);
    }

    return true;
}

bool cparse_user_reset_password(cParseUser *user, cParseError **error)
{
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!user) {
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    if (!cparse_object_contains(user, CPARSE_KEY_USER_EMAIL))
    {
        cparse_log_set_error(error, "User has no email");
        return false;
    }

    if (!cparse_user_validate_email(user, error))
    {
        cparse_log_set_error(error, "User has no valid email");
        return false;
    }

    request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodPost, "requestPasswordReset");

    json = cparse_json_new();

    cparse_json_set_string(json, CPARSE_KEY_USER_EMAIL, cparse_object_get_string(user, CPARSE_KEY_USER_EMAIL));

    cparse_client_request_set_payload(request, cparse_json_to_json_string(json));

    cparse_json_free(json);

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json == NULL)
    {
        return false;
    }

    cparse_json_free(json);

    return true;
}

cparse_thread cparse_user_reset_password_in_background(cParseUser *user, cParseObjectCallback callback, void *param)
{
    return cparse_object_run_in_background(user, cparse_user_reset_password, callback, param, NULL);
}

