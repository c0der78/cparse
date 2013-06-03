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
        CParseJSON *attributes;
        char *className;
        time_t updatedAt;
        time_t createdAt;
        char *objectId;
        CParseACL *acl;

        char *username;
        char *email;
        char *password;
        char *sessionToken;
        bool isNew;
    };

    CParseUser *cparse_current_user();

    void cparse_user_enable_automatic_user();

    CParseUser *cparse_user_login(const char *username, const char *password, CParseError **error);

    void cparse_user_login_in_background(const char *username, const char *password, CParseUserCallback callback);

    void cparse_user_logout();

    CParseQuery *cparse_user_query();


#ifdef __cplusplus
}
#endif

#endif
