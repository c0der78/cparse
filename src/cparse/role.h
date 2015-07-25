/*! \file object.h */
#ifndef CPARSE_ROLE_H_
#define CPARSE_ROLE_H_

#include <cparse/defines.h>

BEGIN_DECL

cParseRole *cparse_role_with_name(const char *name);

const char* cparse_role_name(cParseRole *role);

cParseJson *cparse_role_query_users(cParseRole *role);

cParseJson *cparse_role_query_roles(cParseRole *role);

bool cparse_role_update(cParseRole *role, cParseJson *attributes, cParseError **error);

extern void (*cparse_role_set_public_acl)(cParseRole *role, bool read, bool write);

extern void (*cparse_role_set_user_acl)(cParseRole *role, cParseUser *user, bool read, bool write);

extern void (*cparse_role_set_role_acl)(cParseRole *role, const char *name, bool read, bool write);

extern void (*cparse_role_free)(cParseRole *role);

extern bool (*cparse_role_save)(cParseRole *role, cParseError **error);

END_DECL

#endif
