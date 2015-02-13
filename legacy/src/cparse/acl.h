#ifndef CPARSE_ACL_H_
#define CPARSE_ACL_H_

#include <cparse/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

CPARSE_ACL *cparse_acl_new();

CPARSE_ACL *cparse_acl_new_with_user(CPARSE_OBJ *user);

CPARSE_ACL *cparse_acl_copy(CPARSE_ACL *other);

void cparse_set_default_acl(CPARSE_ACL *acl, bool currentUserAccess);

void cparse_acl_free(CPARSE_ACL *acl);


#ifdef __cplusplus
}
#endif

#endif
