/*! \file object.h */
#ifndef CPARSE_ROLE_H_
#define CPARSE_ROLE_H_

#include <cparse/defines.h>

BEGIN_DECL

cParseRole *cparse_role_with_name(const char *name);

const char* cparse_role_name(cParseRole *role);

cParseJson *cparse_role_users(cParseRole *role);

cParseJson *cparse_role_roles(cParseRole *role);

bool cparse_role_save(cParseRole *role, cParseError **error);

bool cparse_role_update(cParseRole *role, cParseJson *attributes, cParseError **error);

END_DECL

#endif
