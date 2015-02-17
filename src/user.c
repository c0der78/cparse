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

extern CPARSE_QUERY *cparse_query_new();

extern void cparse_object_set_request_includes(CPARSE_OBJ *obj, CPARSE_REQUEST *request);

/* Private */

CPARSE_REQUEST *cparse_user_create_request(CPARSE_OBJ *obj, HTTPRequestMethod method)
{
    char buf[BUFSIZ + 1] = {0};
    CPARSE_REQUEST *request;

    if (!obj) return NULL;

    request = cparse_client_request_new();

    if (method != HTTPRequestMethodPost && obj->objectId && *obj->objectId)
    {
        snprintf(buf, BUFSIZ, "%s/%s", CPARSE_USER_CLASS_NAME, obj->objectId);
    }
    else
    {
        snprintf(buf, BUFSIZ, "%s", CPARSE_USER_CLASS_NAME);
    }

    request->path = strdup(buf);
    request->method = method;

    return request;
}

/* initializers */

CPARSE_OBJ *cparse_user_new()
{
    return cparse_object_with_class_name(CPARSE_USER_CLASS_NAME);
}

CPARSE_OBJ *cparse_user_with_name(const char *username)
{
    CPARSE_OBJ *obj = cparse_object_with_class_name(CPARSE_USER_CLASS_NAME);

    cparse_object_set_string(obj, KEY_USER_NAME, username);

    return obj;
}

CPARSE_OBJ *cparse_current_user(CPARSE_ERROR **error)
{
    CPARSE_OBJ *user;

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

bool cparse_object_is_user(CPARSE_OBJ *obj)
{
    const char *className = cparse_object_class_name(obj);

    if (!className || !*className)
        return false;

    return !strcmp(className, CPARSE_USER_CLASS_NAME);
}

const char *cparse_user_name(CPARSE_OBJ *user)
{
    if (!user) return NULL;

    return cparse_object_get_string(user, KEY_USER_NAME);
}

void cparse_user_set_name(CPARSE_OBJ *user, char *name)
{
    if (user)
    {
        cparse_object_set_string(user, KEY_USER_NAME, name);
    }
}

const char *cparse_user_email(CPARSE_OBJ *user)
{
    return cparse_object_get_string(user, KEY_USER_EMAIL);
}

const char *cparse_user_session_token(CPARSE_OBJ *user)
{
    return cparse_object_get_string(user, KEY_USER_SESSION_TOKEN);
}
/* functions */

CPARSE_OBJ *cparse_user_login(const char *username, const char *password, CPARSE_ERROR **error)
{
    char buf[BUFSIZ + 1];
    CPARSE_OBJ *user;
    CPARSE_JSON *data;
    CPARSE_REQUEST *request;

    if (!username || !*username)
    {
        if (error)
            *error = cparse_error_with_message("No username, set with cparse_user_set_name()");

        return false;
    }

    if (!password || !*password)
    {
        if (error)
            *error = cparse_error_with_message("No password provided");

        return false;

    }
    user = cparse_object_with_class_name(CPARSE_USER_CLASS_NAME);

    request = cparse_client_request_new();

    request->path = strdup("login");

    request->method = HTTPRequestMethodGet;

    snprintf(buf, BUFSIZ, "username=%s&password=%s", username, password);
    request->payload = strdup(buf);

    /* do the deed */
    data = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (data == NULL)
    {
        cparse_object_free(user);
        return NULL;
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
    return user;
}

void cparse_user_login_in_background(const char *username, const char *password, CPARSE_OBJ_CALLBACK callback)
{

}

void cparse_user_logout()
{

}

bool cparse_user_delete(CPARSE_OBJ *obj, CPARSE_ERROR **error)
{
    CPARSE_REQUEST *request;
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

    request = cparse_user_create_request(obj, HTTPRequestMethodDelete);

    cparse_client_request_add_header(request, HEADER_SESSION_TOKEN, sessionToken);

    rval = cparse_client_request_perform(request, error);

    cparse_client_request_free(request);

    return rval;
}


CPARSE_QUERY *cparse_user_query()
{
    CPARSE_QUERY *query = cparse_query_new();

    return query;
}

bool cparse_user_sign_up(CPARSE_OBJ *user, const char *password, CPARSE_ERROR **error)
{
    const char *username;
    CPARSE_REQUEST *request;
    CPARSE_JSON *json;

    if (!user) return false;

    username = cparse_object_get_string(user, KEY_USER_NAME);

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

    request = cparse_user_create_request(user, HTTPRequestMethodPost);

    /* temporarily set the password on the user for the request json */
    cparse_object_set_string(user, "password", password);

    request->payload = strdup(cparse_object_to_json_string(user));

    /* remove the passwrod from the user attributes for security */
    cparse_object_remove(user, "password");

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json != NULL)
    {
        cparse_object_merge_json(user, json);

        cparse_json_free(json);

        if (cparse_object_contains(user, KEY_USER_SESSION_TOKEN))
        {
            const char *sessionToken = cparse_object_get_string(user, KEY_USER_SESSION_TOKEN);

            strncpy(current_user_token, sessionToken, BUFSIZ);
        }
        return true;
    }

    return false;
}

bool cparse_user_fetch(CPARSE_OBJ *obj, CPARSE_ERROR **error)
{
    CPARSE_REQUEST *request;
    CPARSE_JSON *json;

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

    request = cparse_user_create_request(obj, HTTPRequestMethodGet);

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

bool cparse_user_refresh(CPARSE_OBJ *obj, CPARSE_ERROR **error)
{
    CPARSE_REQUEST *request;
    CPARSE_JSON *json;

    if (!cparse_object_exists(obj))
    {
        if (error)
            *error = cparse_error_with_message("User has no id");

        return false;
    }

    request = cparse_user_create_request(obj, HTTPRequestMethodGet);

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

bool cparse_user_validate(CPARSE_OBJ *user, const char *sessionToken, CPARSE_ERROR **error)
{
    CPARSE_REQUEST *request;

    CPARSE_JSON *json;

    request = cparse_client_request_new();

    request->method = HTTPRequestMethodGet;

    request->path = strdup("me");

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

bool cparse_user_validate_email(CPARSE_OBJ *user, CPARSE_ERROR **error)
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

bool cparse_user_reset_password(CPARSE_OBJ *user, CPARSE_ERROR **error)
{
    CPARSE_REQUEST *request;
    CPARSE_JSON *json;

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

    request = cparse_client_request_new();

    request->method = HTTPRequestMethodPost;

    request->path = strdup("requestPasswordReset");

    json = cparse_json_new();

    cparse_json_set_string(json, KEY_USER_EMAIL, cparse_object_get_string(user, KEY_USER_EMAIL));

    request->payload = strdup(cparse_json_to_json_string(json));

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

