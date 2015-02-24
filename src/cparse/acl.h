/*! \file acl.h */
#ifndef CPARSE_ACL_H_
#define CPARSE_ACL_H_

#include <cparse/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

cParseACL *cparse_acl_new();

cParseACL *cparse_acl_new_with_user(cParseObject *user);

cParseACL *cparse_acl_copy(cParseACL *other);

void cparse_set_default_acl(cParseACL *acl, bool currentUserAccess);

void cparse_acl_free(cParseACL *acl);


#ifdef __cplusplus
}
#endif

#endif
