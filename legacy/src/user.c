#include <stdlib.h>
#include <string.h>
#include <cparse/user.h>


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
