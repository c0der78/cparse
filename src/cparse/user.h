#ifndef CPARSE_USER_H_
#define CPARSE_USER_H_

#include <cparse/defines.h>
#include <cparse/query.h>
#include <cparse/acl.h>

#ifdef __cplusplus
extern "C" {
#endif

CPARSE_OBJ *cparse_current_user();

CPARSE_OBJ *cparse_user_new();

CPARSE_OBJ *cparse_user_with_name(const char *username);

CPARSE_OBJ *cparse_user_login(const char *username, const char *password, CPARSE_ERROR **error);

void cparse_user_login_in_background(const char *username, const char *password, CPARSE_OBJ_CALLBACK callback);

void cparse_user_logout();

bool cparse_user_delete(CPARSE_OBJ *user, CPARSE_ERROR **error);

CPARSE_QUERY *cparse_user_query();

bool cparse_user_sign_up(CPARSE_OBJ *user, const char *password, CPARSE_ERROR **error);

/* getters/setters */

const char *cparse_user_name(CPARSE_OBJ *user);

void cparse_user_set_name(CPARSE_OBJ *user, char *value);

const char *cparse_user_email(CPARSE_OBJ *user);

const char *cparse_user_session_token(CPARSE_OBJ *user);

bool cparse_user_validate(CPARSE_OBJ *user, const char *sessionToken, CPARSE_ERROR **error);

bool cparse_user_validate_email(CPARSE_OBJ *user, CPARSE_ERROR **error);

bool cparse_user_refresh(CPARSE_OBJ *obj, CPARSE_ERROR **error);

bool cparse_user_fetch(CPARSE_OBJ *obj, CPARSE_ERROR **error);

bool cparse_user_reset_password(CPARSE_OBJ *user, CPARSE_ERROR **error);

bool cparse_user_delete(CPARSE_OBJ *obj, CPARSE_ERROR **error);

#ifdef __cplusplus
}
#endif

#endif
