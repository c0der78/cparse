/*! \file acl.h */
#ifndef CPARSE_ACL_H_
#define CPARSE_ACL_H_

#include <cparse/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

cParseACL *cparse_acl_new();

cParseACL *cparse_acl_new_with_user(cParseUser *user);

cParseACL *cparse_acl_with_name(const char *name);

void cparse_acl_copy(cParseACL *orig, cParseACL *other);

void cparse_set_default_acl(cParseACL *acl, bool currentUserAccess);

void cparse_acl_free(cParseACL *acl);

cParseACL *cparse_acl_from_json(cParseJson *json);

cParseJson *cparse_acl_to_json(cParseACL *acl);

bool cparse_acl_is_readable(cParseACL *acl, const char *name);

bool cparse_acl_is_writable(cParseACL *acl, const char *name);

void cparse_acl_set_readable(cParseACL *acl, const char *name, bool value);

void cparse_acl_set_writable(cParseACL *acl, const char *name, bool value);

#ifdef __cplusplus
}
#endif

#endif
