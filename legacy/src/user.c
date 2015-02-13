#include <stdlib.h>
#include <string.h>
#include <cparse/user.h>
#include <cparse/parse.h>

struct cparse_user
{
    /*
     * the following fields MUST match the base object structure
     * in order to do casting and make use of the object functions
     */
    char *className;
    char *objectId;
    time_t updatedAt;
    time_t createdAt;

    /* object fields */
    CPARSE_JSON *attributes;
    CPARSE_ACL *acl;

    /* user fields */
    char *username;
    char *email;
    char *password;
    char *sessionToken;
    bool isNew;
};


CPARSE_USER *current_user;

CPARSE_USER *automatic_user;

extern CPARSE_QUERY *cparse_query_new();

CPARSE_USER *cparse_current_user()
{
    if (current_user == NULL)
    {
        return automatic_user;
    }
    return current_user;
}

static CPARSE_USER *cparse_user_new()
{
    CPARSE_USER *user = malloc(sizeof(CPARSE_USER));

    user->username = NULL;
    user->email = NULL;
    user->password = NULL;
    user->sessionToken = NULL;
    user->isNew = true;
    return user;
}

void cparse_user_delete(CPARSE_USER *user)
{
    if (user->username)
        free(user->username);
    if (user->email)
        free(user->email);
    if (user->password)
        free(user->password);
    if (user->sessionToken)
        free(user->sessionToken);

    free(user);
}

/* getters/setters */

const char *cparse_user_class_name(CPARSE_USER *user)
{
    return cparse_base_class_name((CPARSE_BASE_OBJ *) user);
}

time_t cparse_user_updated_at(CPARSE_USER *user)
{
    return cparse_base_updated_at((CPARSE_BASE_OBJ *) user);
}

time_t cparse_user_created_at(CPARSE_USER *user)
{
    return cparse_base_created_at((CPARSE_BASE_OBJ *) user);
}

const char *cparse_user_id(CPARSE_USER *user)
{
    return cparse_base_id((CPARSE_BASE_OBJ *) user);
}

CPARSE_ACL *cparse_user_acl(CPARSE_USER *user)
{
    return !user ? NULL : user->acl;
}

const char *cparse_user_name(CPARSE_USER *user)
{
    return !user ? NULL : user->username;
}

const char *cparse_user_email(CPARSE_USER *user)
{
    return !user ? NULL : user->email;
}

void cparse_user_set_password(CPARSE_USER *user, char *value)
{
    if (user)
    {
        user->password = value;
    }
}

const char *cparse_user_session_token(CPARSE_USER *user)
{
    return !user ? NULL : user->sessionToken;
}

bool cparse_user_is_new(CPARSE_USER *user)
{
    return !user ? false : user->isNew;
}

/* functions */
void cparse_user_enable_automatic_user()
{
    if (automatic_user == NULL)
    {
        automatic_user = cparse_user_new();
    }
}

CPARSE_USER *cparse_user_login(const char *username, const char *password, CPARSE_ERROR **error)
{
    CPARSE_USER *user = cparse_user_new();

    user->username = strdup(username);
    user->password = strdup(password);

    /* TODO: do network logic and set error */


    return user;
}

void cparse_user_login_in_background(const char *username, const char *password, CPARSE_USER_CALLBACK callback)
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
