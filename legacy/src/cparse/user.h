#ifndef CPARSE_USER_H_
#define CPARSE_USER_H_

#include <cparse/defines.h>
#include <cparse/query.h>
#include <cparse/acl.h>

#ifdef __cplusplus
extern "C" {
#endif

CPARSE_USER *cparse_current_user();

void cparse_user_enable_automatic_user();

CPARSE_USER *cparse_user_login(const char *username, const char *password, CPARSE_ERROR **error);

void cparse_user_login_in_background(const char *username, const char *password, CPARSE_USER_CALLBACK callback);

void cparse_user_logout();

CPARSE_QUERY *cparse_user_query();

/* getters/setters */

const char *cparse_user_class_name(CPARSE_USER *user);

time_t cparse_user_updated_at(CPARSE_USER *user);

time_t cparse_user_created_at(CPARSE_USER *user);

const char *cparse_user_id(CPARSE_USER *user);

CPARSE_ACL *cparse_user_acl(CPARSE_USER *user);

const char *cparse_user_name(CPARSE_USER *user);

const char *cparse_user_email(CPARSE_USER *user);

void cparse_user_set_password(CPARSE_USER *user, char *value);

const char *cparse_user_session_token(CPARSE_USER *user);

bool cparse_user_is_new(CPARSE_USER *user);


#ifdef __cplusplus
}
#endif

#endif
