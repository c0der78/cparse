#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cparse/user.h>
#include <cparse/parse.h>
#include <cparse/json.h>
#include <cparse/object.h>

#include "protocol.h"
#include "client.h"
#include "private.h"

char current_user_token[BUFSIZ + 1] = {0};

extern void cparse_object_set_request_includes(cParseObject *obj, cParseRequest *request);

/* Private */

static cParseRequest *cparse_user_create_request(cParseObject *obj, HttpRequestMethod method)
{
    char buf[BUFSIZ + 1] = {0};

    if (!obj) return NULL;

    if (method != HttpRequestMethodPost && obj->objectId && *obj->objectId)
    {
        snprintf(buf, BUFSIZ, "%s/%s", CPARSE_USER_CLASS_NAME, obj->objectId);
    }
    else
    {
        snprintf(buf, BUFSIZ, "%s", CPARSE_USER_CLASS_NAME);
    }

    return cparse_client_request_with_method_and_path(method, buf);
}

/* initializers */

cParseObject *cparse_user_new()
{
    return cparse_object_with_class_name(CPARSE_USER_CLASS_NAME);
}

cParseObject *cparse_user_with_name(const char *username)
{
    cParseObject *obj = cparse_object_with_class_name(CPARSE_USER_CLASS_NAME);

    cparse_object_set_string(obj, KEY_USER_NAME, username);

    return obj;
}

cParseObject *cparse_current_user(cParseError **error)
{
    cParseObject *user;

    if (!*current_user_token)
        return NULL;

    user = cparse_object_with_class_name(CPARSE_USER_CLASS_NAME);

    if (!cparse_user_validate(user, current_user_token, error))
    {
        cparse_object_free(user);

        return NULL;
    }

    return user;
}

/* getters/setters */

bool cparse_object_is_user(cParseObject *obj)
{
    if (!obj) return false;

    return cparse_class_name_is_user(obj->className);
}

bool cparse_class_name_is_user(const char *className)
{
    if (!className || !*className)
        return false;

    return !strcmp(className, CPARSE_USER_CLASS_NAME);
}

const char *cparse_user_name(cParseObject *user)
{
    if (!user) return NULL;

    return cparse_object_get_string(user, KEY_USER_NAME);
}

void cparse_user_set_name(cParseObject *user, char *name)
{
    if (user)
    {
        cparse_object_set_string(user, KEY_USER_NAME, name);
    }
}

const char *cparse_user_email(cParseObject *user)
{
    return cparse_object_get_string(user, KEY_USER_EMAIL);
}

const char *cparse_user_session_token(cParseObject *user)
{
    return cparse_object_get_string(user, KEY_USER_SESSION_TOKEN);
}
/* functions */

bool cparse_user_login_user(cParseObject *user, cParseError **error)
{
    cParseJson *data;
    cParseRequest *request;
    const char *username, *password;

    if (!user) return false;

    username = cparse_object_get_string(user, KEY_USER_NAME);
    password = cparse_object_get_string(user, KEY_USER_PASSWORD);

    if (!username || !*username)
    {
        if (error)
            *error = cparse_error_with_message("No username provided");

        return false;
    }

    if (!password || !*password)
    {
        if (error)
            *error = cparse_error_with_message("No password provided");

        return false;

    }

    request = cparse_client_request_with_method_and_path(HttpRequestMethodGet, "login");

    cparse_client_request_add_data(request, "username", username);
    cparse_client_request_add_data(request, "password", password);

    /* do the deed */
    data = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (data == NULL)
    {
        return false;
    }

    cparse_object_merge_json(user, data);

    cparse_json_free(data);

    if (cparse_object_contains(user, KEY_USER_SESSION_TOKEN))
    {
        const char *sessionToken = cparse_object_get_string(user, KEY_USER_SESSION_TOKEN);

        if (sessionToken)
        {
            strncpy(current_user_token, sessionToken, BUFSIZ);
        }
    }
    return true;
}

cParseObject *cparse_user_login(const char *username, const char *password, cParseError **error)
{
    cParseObject *user;

    user = cparse_user_with_name(username);

    cparse_object_set_string(user, KEY_USER_PASSWORD, password);

    if (!cparse_user_login_user(user, error))
    {
        cparse_object_free(user);
        return NULL;
    }

    return user;
}

pthread_t cparse_user_login_in_background(const char *username, const char *password, cParseObjectCallback callback)
{
    cParseObject *obj = cparse_user_with_name(username);

    cparse_object_set_string(obj, KEY_USER_PASSWORD, password);

    return cparse_object_run_in_background(obj, cparse_user_login_user, callback, cparse_object_free);
}

void cparse_user_logout()
{
    memset(current_user_token, 0, BUFSIZ);
}


bool cparse_user_delete(cParseObject *obj, cParseError **error)
{
    cParseRequest *request;
    const char *sessionToken;
    bool rval;

    if (!obj) return false;

    if (!cparse_object_exists(obj))
    {
        if (error)
            *error = cparse_error_with_message("User has no id");
        return false;
    }

    if (!(sessionToken = cparse_user_session_token(obj)) || !*sessionToken)
    {
        if (error)
            *error = cparse_error_with_message("User has no session token");
        return false;
    }

    request = cparse_user_create_request(obj, HttpRequestMethodDelete);

    cparse_client_request_add_header(request, HEADER_SESSION_TOKEN, sessionToken);

    rval = cparse_client_request_perform(request, error);

    cparse_client_request_free(request);

    return rval;
}

pthread_t cparse_user_delete_in_background(cParseObject *obj, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(obj, cparse_user_delete, callback, NULL);
}


cParseQuery *cparse_user_query_new()
{
    cParseQuery *query = cparse_query_with_class_name(CPARSE_USER_CLASS_NAME);

    return query;
}

static bool cparse_user_sign_up_user(cParseObject *user, cParseError **error)
{
    const char *username;
    const char *password;
    cParseRequest *request;
    cParseJson *json;

    if (!user) return false;

    username = cparse_object_get_string(user, KEY_USER_NAME);
    password = cparse_object_get_string(user, KEY_USER_PASSWORD);

    if (!username || !*username)
    {
        if (error)
            *error = cparse_error_with_message("User has no username");

        return false;
    }

    if (!password || !*password)
    {
        if (error)
            *error = cparse_error_with_message("No password provided");

        return false;
    }

    request = cparse_user_create_request(user, HttpRequestMethodPost);

    cparse_client_request_set_payload(request, cparse_object_to_json_string(user));

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json != NULL)
    {
        cparse_object_merge_json(user, json);

        cparse_json_free(json);

        /* remove the passwrod from the user attributes for security */
        cparse_object_remove(user, "password");

        if (cparse_object_contains(user, KEY_USER_SESSION_TOKEN))
        {
            const char *sessionToken = cparse_object_get_string(user, KEY_USER_SESSION_TOKEN);

            strncpy(current_user_token, sessionToken, BUFSIZ);
        }
        return true;
    }

    /* remove the passwrod from the user attributes for security */
    cparse_object_remove(user, "password");

    return false;
}

bool cparse_user_sign_up(cParseObject *user, const char *password, cParseError **error)
{
    if (password && *password)
        cparse_object_set_string(user, KEY_USER_PASSWORD, password);

    return cparse_user_sign_up_user(user, error);
}

pthread_t cparse_user_sign_up_in_background(cParseObject *user, const char *password, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(user, cparse_user_sign_up_user, callback, NULL);
}

bool cparse_user_fetch(cParseObject *obj, cParseError **error)
{
    cParseRequest *request;
    cParseJson *json;

    if (!obj)
    {
        return false;
    }

    if (!cparse_object_exists(obj))
    {
        if (error)
            *error = cparse_error_with_message("User has no id");

        return false;
    }

    request = cparse_user_create_request(obj, HttpRequestMethodGet);

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

pthread_t cparse_user_fetch_in_background(cParseObject *obj, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(obj, cparse_user_fetch, callback, NULL);
}

bool cparse_user_refresh(cParseObject *obj, cParseError **error)
{
    cParseRequest *request;
    cParseJson *json;

    if (!cparse_object_exists(obj))
    {
        if (error)
            *error = cparse_error_with_message("User has no id");

        return false;
    }

    request = cparse_user_create_request(obj, HttpRequestMethodGet);

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

pthread_t cparse_user_refresh_in_background(cParseObject *user, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(user, cparse_user_refresh, callback, NULL);
}

bool cparse_user_validate(cParseObject *user, const char *sessionToken, cParseError **error)
{
    cParseRequest *request;

    cParseJson *json;

    request = cparse_client_request_with_method_and_path(HttpRequestMethodGet, "me");

    cparse_client_request_add_header(request, HEADER_SESSION_TOKEN, sessionToken);

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

bool cparse_user_validate_email(cParseObject *user, cParseError **error)
{
    if (!user) return false;

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

bool cparse_user_reset_password(cParseObject *user, cParseError **error)
{
    cParseRequest *request;
    cParseJson *json;

    if (!user) return false;

    if (!cparse_object_contains(user, KEY_USER_EMAIL))
    {
        if (error)
            *error = cparse_error_with_message("User has no email");
        return false;
    }

    if (!cparse_user_validate_email(user, error))
    {
        if (error)
            *error = cparse_error_with_message("User has no valid email");
        return false;
    }

    request = cparse_client_request_with_method_and_path(HttpRequestMethodPost, "requestPasswordReset");

    json = cparse_json_new();

    cparse_json_set_string(json, KEY_USER_EMAIL, cparse_object_get_string(user, KEY_USER_EMAIL));

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

pthread_t cparse_user_reset_password_in_background(cParseObject *user, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(user, cparse_user_reset_password, callback, NULL);
}

