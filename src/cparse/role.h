/*! \file object.h */
#ifndef CPARSE_ROLE_H_
#define CPARSE_ROLE_H_

#include <cparse/defines.h>

BEGIN_DECL

cParseRole *cparse_role_with_name(const char *name);

const char* cparse_role_name(cParseRole *role);

cParseQuery *cparse_role_query_users(cParseRole *role, cParseError **error);

cParseQuery *cparse_role_query_roles(cParseRole *role, cParseError **error);

bool cparse_role_update(cParseRole *role, cParseJson *attributes, cParseError **error);

void cparse_role_add_user(cParseRole *role, cParseUser *user);

void cparse_role_add_role(cParseRole *role, cParseRole *other);

extern void (*cparse_role_set_public_acl)(cParseRole *role, bool read, bool write);

extern void (*cparse_role_set_user_acl)(cParseRole *role, cParseUser *user, bool read, bool write);

extern void (*cparse_role_set_role_acl)(cParseRole *role, cParseRole *other, bool read, bool write);

extern void (*cparse_role_free)(cParseRole *role);

extern bool (*cparse_role_save)(cParseRole *role, cParseError **error);

extern bool (*cparse_role_delete)(cParseRole *role, cParseError **error);

END_DECL

#endif
