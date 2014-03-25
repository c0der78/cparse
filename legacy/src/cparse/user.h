#ifndef CPARSE_USER_H_
#define CPARSE_USER_H_

#include <cparse/defines.h>
#include <cparse/query.h>
#include <cparse/acl.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cparse_user
{
    /*
     * the following fields MUST match the object structure
     * in order to do casting and make use of the object functions
     */
    CPARSE_JSON *attributes;
    char *className;
    time_t updatedAt;
    time_t createdAt;
    char *objectId;
    CPARSE_ACL *acl;

    char *username;
    char *email;
    char *password;
    char *sessionToken;
    bool isNew;
};

CPARSE_USER *cparse_current_user();

void cparse_user_enable_automatic_user();

CPARSE_USER *cparse_user_login(const char *username, const char *password, CPARSE_ERROR **error);

void cparse_user_login_in_background(const char *username, const char *password, CPARSE_USER_CALLBACK callback);

void cparse_user_logout();

CPARSE_QUERY *cparse_user_query();


#ifdef __cplusplus
}
#endif

#endif
