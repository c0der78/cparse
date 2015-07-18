#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cparse/user.h>
#include <cparse/parse.h>
#include <cparse/json.h>
#include <cparse/object.h>
#include <cparse/util.h>
#include "protocol.h"
#include "client.h"
#include "private.h"

const char *const CPARSE_USER_CLASS_NAME = "users";

char cparse_current_user_token[CPARSE_BUF_SIZE + 1] = {0};

cParseUser *cparse_current_user_ = NULL;

extern cParseUser *cparse_object_new();

extern void cparse_object_set_request_includes(cParseUser *obj, cParseRequest *request);

/* Private */

static cParseRequest *cparse_user_create_request(cParseUser *obj, cParseHttpRequestMethod method)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};

    if (!obj) { return NULL; }

    if (method != cParseHttpRequestMethodPost && obj->objectId && *obj->objectId)
    {
        snprintf(buf, CPARSE_BUF_SIZE, "%s/%s", CPARSE_USER_CLASS_NAME, obj->objectId);
    }
    else
    {
        snprintf(buf, CPARSE_BUF_SIZE, "%s", CPARSE_USER_CLASS_NAME);
    }

    return cparse_client_request_with_method_and_path(method, buf);
}

/* initializers */

cParseUser *cparse_user_new()
{
    cParseUser *obj = cparse_object_new();

    obj->className = strdup(CPARSE_USER_CLASS_NAME);

    return obj;
}

cParseUser *cparse_user_with_name(const char *username)
{
    cParseUser *obj = cparse_object_with_class_name(CPARSE_USER_CLASS_NAME);

    cparse_object_set_string(obj, CPARSE_KEY_USER_NAME, username);

    return obj;
}

cParseUser *cparse_current_user(cParseError **error)
{
    if (cparse_current_user_ != NULL) {
        return cparse_current_user_;
    }

    if (!cparse_current_user_token[0]) {
        return NULL;
    }

    cparse_current_user_ = cparse_object_with_class_name(CPARSE_USER_CLASS_NAME);

    if (!cparse_user_validate(cparse_current_user_, cparse_current_user_token, error))
    {
        cparse_object_free(cparse_current_user_);

        cparse_current_user_ = NULL;

        return NULL;
    }

    return cparse_current_user_;
}

/* getters/setters */

bool cparse_object_is_user(cParseObject *obj)
{
    if (!obj) { return false; }

    return cparse_class_name_is_user(obj->className);
}

bool cparse_class_name_is_user(const char *className)
{
    if (cparse_str_empty(className)) {
        return false;
    }

    return !strcmp(className, CPARSE_USER_CLASS_NAME);
}

const char *cparse_user_name(cParseUser *user)
{
    if (!user) { return NULL; }

    return cparse_object_get_string(user, CPARSE_KEY_USER_NAME);
}

void cparse_user_set_name(cParseUser *user, char *name)
{
    if (user)
    {
        cparse_object_set_string(user, CPARSE_KEY_USER_NAME, name);
    }
}

const char *cparse_user_email(cParseUser *user)
{
    return cparse_object_get_string(user, CPARSE_KEY_USER_EMAIL);
}

const char *cparse_user_session_token(cParseUser *user)
{
    return cparse_object_get_string(user, CPARSE_KEY_USER_SESSION_TOKEN);
}
/* functions */

static bool cparse_user_login_user(cParseUser *user, cParseError **error)
{
    cParseJson *data = NULL;
    cParseRequest *request = NULL;
    const char *username = NULL, *password = NULL;

    if (!user) { return false; }

    username = cparse_object_get_string(user, CPARSE_KEY_USER_NAME);

    password = cparse_object_get_string(user, CPARSE_KEY_USER_PASSWORD);

    if (cparse_str_empty(username))
    {
        if (error) {
            *error = cparse_error_with_message("No username provided");
        }

        return false;
    }

    if (cparse_str_empty(password))
    {
        if (error) {
            *error = cparse_error_with_message("No password provided");
        }

        return false;

    }

    request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodGet, "login");

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
            strncpy(cparse_current_user_token, sessionToken, CPARSE_BUF_SIZE);
        }

        cparse_current_user_ = user;
    }
    return true;
}

cParseUser *cparse_user_login(const char *username, const char *password, cParseError **error)
{
    cParseUser *user = cparse_user_with_name(username);

    cparse_object_set_string(user, CPARSE_KEY_USER_PASSWORD, password);

    if (!cparse_user_login_user(user, error))
    {
        cparse_object_free(user);
        return NULL;
    }

    return user;
}

pthread_t cparse_user_login_in_background(const char *username, const char *password, cParseObjectCallback callback)
{
    cParseUser *obj = cparse_user_with_name(username);

    cparse_object_set_string(obj, CPARSE_KEY_USER_PASSWORD, password);

    return cparse_object_run_in_background(obj, cparse_user_login_user, callback, cparse_object_free);
}

void cparse_user_logout()
{
    memset(cparse_current_user_token, 0, CPARSE_BUF_SIZE);
    cparse_current_user_ = NULL;
}


bool cparse_user_delete(cParseUser *obj, cParseError **error)
{
    cParseRequest *request = NULL;
    const char *sessionToken = NULL;
    bool rval = false;

    if (!obj) { return false; }

    if (!cparse_object_exists(obj))
    {
        if (error) {
            *error = cparse_error_with_message("User has no id");
        }
        return false;
    }

    if (!(sessionToken = cparse_user_session_token(obj)) || !*sessionToken)
    {
        if (error) {
            *error = cparse_error_with_message("User has no session token");
        }
        return false;
    }

    request = cparse_user_create_request(obj, cParseHttpRequestMethodDelete);

    cparse_client_request_add_header(request, CPARSE_HEADER_SESSION_TOKEN, sessionToken);

    rval = cparse_client_request_perform(request, error);

    cparse_client_request_free(request);

    return rval;
}

pthread_t cparse_user_delete_in_background(cParseUser *obj, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(obj, cparse_user_delete, callback, NULL);
}


cParseQuery *cparse_user_query_new()
{
    cParseQuery *query = cparse_query_with_class_name(CPARSE_USER_CLASS_NAME);

    return query;
}

static bool cparse_user_sign_up_user(cParseUser *user, cParseError **error)
{
    const char *username = NULL;
    const char *password = NULL;
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!user) { return false; }

    username = cparse_object_get_string(user, CPARSE_KEY_USER_NAME);
    password = cparse_object_get_string(user, CPARSE_KEY_USER_PASSWORD);

    if (cparse_str_empty(username))
    {
        if (error) {
            *error = cparse_error_with_message("User has no username");
        }

        return false;
    }

    if (cparse_str_empty(password))
    {
        if (error) {
            *error = cparse_error_with_message("No password provided");
        }

        return false;
    }

    request = cparse_user_create_request(user, cParseHttpRequestMethodPost);

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
                strncpy(cparse_current_user_token, sessionToken, CPARSE_BUF_SIZE);
            }

            cparse_current_user_ = user;
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

pthread_t cparse_user_sign_up_in_background(cParseUser *user, const char *password, cParseObjectCallback callback)
{
    if (!cparse_str_empty(password)) {
        cparse_object_set_string(user, CPARSE_KEY_USER_PASSWORD, password);
    }

    return cparse_object_run_in_background(user, cparse_user_sign_up_user, callback, NULL);
}

bool cparse_user_fetch(cParseUser *obj, cParseError **error)
{
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!obj)
    {
        return false;
    }

    if (!cparse_object_exists(obj))
    {
        if (error) {
            *error = cparse_error_with_message("User has no id");
        }

        return false;
    }

    request = cparse_user_create_request(obj, cParseHttpRequestMethodGet);

    cparse_object_set_request_includes(obj, request);

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json)
    {
        cparse_object_merge_json(obj, json);

        cparse_json_free(json);

        return true;
    }
    return false;
}

pthread_t cparse_user_fetch_in_background(cParseUser *obj, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(obj, cparse_user_fetch, callback, NULL);
}

bool cparse_user_refresh(cParseUser *obj, cParseError **error)
{
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!cparse_object_exists(obj))
    {
        if (error) {
            *error = cparse_error_with_message("User has no id");
        }

        return false;
    }

    request = cparse_user_create_request(obj, cParseHttpRequestMethodGet);

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json)
    {
        cparse_object_merge_json(obj, json);

        cparse_json_free(json);

        return true;
    }

    return false;
}

pthread_t cparse_user_refresh_in_background(cParseUser *user, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(user, cparse_user_refresh, callback, NULL);
}

bool cparse_user_validate(cParseUser *user, const char *sessionToken, cParseError **error)
{
    cParseRequest *request = NULL;
    cParseJson *json = NULL;
    char buf[CPARSE_BUF_SIZE + 1] = {0};

    if (cparse_str_empty(sessionToken))
    {
        if (error) {
            *error = cparse_error_with_message("missing session token");
        }
        return false;
    }

    snprintf(buf, CPARSE_BUF_SIZE, "%s/me", CPARSE_USER_CLASS_NAME);

    request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodGet, buf);

    cparse_client_request_add_header(request, CPARSE_HEADER_SESSION_TOKEN, sessionToken);

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json == NULL)
    {
        return false;
    }

    cparse_object_merge_json(user, json);

    cparse_json_free(json);

    return true;
}

bool cparse_user_validate_email(cParseUser *user, cParseError **error)
{
    if (!user) { return false; }

    if (!cparse_object_contains(user, "emailVerified"))
    {
        return false;
    }

    if (!cparse_object_get_bool(user, "emailVerified"))
    {
        if (!cparse_user_refresh(user, error))
        {
            return false;
        }

        return cparse_object_get_bool(user, "emailVerified");
    }

    return true;
}

bool cparse_user_reset_password(cParseUser *user, cParseError **error)
{
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!user) { return false; }

    if (!cparse_object_contains(user, CPARSE_KEY_USER_EMAIL))
    {
        if (error) {
            *error = cparse_error_with_message("User has no email");
        }
        return false;
    }

    if (!cparse_user_validate_email(user, error))
    {
        if (error) {
            *error = cparse_error_with_message("User has no valid email");
        }
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

pthread_t cparse_user_reset_password_in_background(cParseUser *user, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(user, cparse_user_reset_password, callback, NULL);
}

