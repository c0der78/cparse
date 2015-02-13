#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cparse/user.h>
#include <cparse/parse.h>
#include <cparse/json.h>
#include <cparse/object.h>

#include "protocol.h"
#include "client.h"

#define USER_CLASS_NAME "users"

#define USER_USERNAME "username"

#define USER_EMAIL "email"

#define USER_IS_NEW "isNew"

#define USER_SESSION_TOKEN "sessionToken"


CPARSE_OBJ *current_user;

CPARSE_OBJ *automatic_user;

extern CPARSE_QUERY *cparse_query_new();

extern bool cparse_object_request_json(CPARSE_OBJ *obj, HTTPRequestMethod method, const char *path, CPARSE_ERROR **error);

CPARSE_OBJ *cparse_user_new()
{
    return cparse_object_with_class_name(USER_CLASS_NAME);
}

CPARSE_OBJ *cparse_user_with_name(const char *username)
{
    CPARSE_OBJ *obj = cparse_object_with_class_name(USER_CLASS_NAME);

    cparse_object_set_string(obj, USER_USERNAME, username);

    return obj;
}

CPARSE_OBJ *cparse_current_user()
{
    if (current_user == NULL)
    {
        return automatic_user;
    }
    return current_user;
}

/* getters/setters */

const char *cparse_user_name(CPARSE_OBJ *user)
{
    if (!user) return NULL;

    return cparse_object_get_string(user, USER_USERNAME);
}

void cparse_user_set_name(CPARSE_OBJ *user, char *name)
{
    if (user)
    {
        cparse_object_set_string(user, USER_USERNAME, name);
    }
}

const char *cparse_user_email(CPARSE_OBJ *user)
{
    return cparse_object_get_string(user, USER_EMAIL);
}

const char *cparse_user_session_token(CPARSE_OBJ *user)
{
    return cparse_object_get_string(user, USER_SESSION_TOKEN);
}

bool cparse_user_is_new(CPARSE_OBJ *user)
{
    return cparse_object_get_bool(user, USER_IS_NEW);
}

/* functions */
void cparse_user_enable_automatic_user()
{
    if (automatic_user == NULL)
    {
        automatic_user = cparse_object_with_class_name(USER_CLASS_NAME);
    }
}

CPARSE_OBJ *cparse_user_login(const char *username, const char *password, CPARSE_ERROR **error)
{
    CPARSE_CLIENT_REQ *request;
    char buf[BUFSIZ + 1];
    CPARSE_OBJ *user;
    CPARSE_JSON *data;
    bool rval;

    if (!username || !*username)
    {
        *error = cparse_error_new();

        cparse_error_set_message(*error, "No username, set with cparse_user_set_name()");

        return false;
    }

    if (!password || !*password)
    {
        *error = cparse_error_new();

        cparse_error_set_message(*error, "No password provided");

        return false;
    }
    user = cparse_object_with_class_name(USER_CLASS_NAME);

    request = cparse_client_request_new();

    request->method = HTTPRequestMethodGet;

    request->path = strdup("login");

    snprintf(buf, BUFSIZ, "username=%s&password=%s", username, password);

    request->payload = strdup(buf);

    data = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (error != NULL && *error != NULL)
    {
        cparse_json_free(data);

        cparse_object_free(user);

        return NULL;
    }

    /* merge the response with the object */
    cparse_object_merge_json(user, data);

    cparse_json_free(data);

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
    CPARSE_CLIENT_REQ *request;
    char buf[BUFSIZ + 1];
    const char *userId;
    const char *sessionToken;

    if (!obj || !(userId = cparse_object_id(obj)) || !*userId)
    {
        return false;
    }

    if (!(sessionToken = cparse_user_session_token(obj)) || !*sessionToken)
    {
        return false;
    }

    request = cparse_client_request_new();

    snprintf(buf, BUFSIZ, "%s/%s", USER_CLASS_NAME, userId);

    request->path = strdup(buf);

    request->method = HTTPRequestMethodDelete;

    cparse_client_request_add_header(request, HEADER_SESSION_TOKEN, sessionToken);

    cparse_client_request_perform(request, error);

    cparse_client_request_free(request);

    return error == NULL || *error == NULL;
}


CPARSE_QUERY *cparse_user_query()
{
    CPARSE_QUERY *query = cparse_query_new();

    return query;
}

bool cparse_user_sign_up(CPARSE_OBJ *user, const char *password, CPARSE_ERROR **error)
{
    const char *username;
    bool rval;

    if (!user) return false;

    username = cparse_object_get_string(user, USER_USERNAME);

    if (!username || !*username)
    {
        *error = cparse_error_new();

        cparse_error_set_message(*error, "No username, set with cparse_user_set_name()");

        return false;
    }

    if (!password || !*password)
    {
        *error = cparse_error_new();

        cparse_error_set_message(*error, "No password provided");

        return false;
    }

    cparse_object_set_string(user, "password", password);

    rval = cparse_object_request_json(user, HTTPRequestMethodPost, USER_CLASS_NAME, error);

    cparse_object_remove(user, "password");

    return rval;
}
