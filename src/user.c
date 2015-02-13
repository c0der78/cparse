#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cparse/user.h>
#include <cparse/parse.h>
#include <cparse/json.h>
#include <cparse/object.h>

#include "client.h"

#define USER_CLASS_NAME "users"

#define USER_USERNAME "username"

#define USER_EMAIL "email"

#define USER_IS_NEW "isNew"

#define USER_SESSION_TOKEN "sessionToken"


CPARSE_OBJ *current_user;

CPARSE_OBJ *automatic_user;

extern CPARSE_QUERY *cparse_query_new();

extern bool cparse_object_request(CPARSE_OBJ *obj, HTTPRequestMethod method, const char *path, CPARSE_ERROR **error);

CPARSE_OBJ *cparse_user_new()
{
    return cparse_object_with_class_name(USER_CLASS_NAME);
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
    CPARSE_OBJ *user = cparse_object_with_class_name(USER_CLASS_NAME);

    /* TODO: do network logic and set error */


    return user;
}

void cparse_user_login_in_background(const char *username, const char *password, CPARSE_OBJ_CALLBACK callback)
{

}

void cparse_user_logout()
{

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

    rval = cparse_object_request(user, HTTPRequestMethodPost, USER_CLASS_NAME, error);

    cparse_object_remove(user, "password");

    return rval;
}
