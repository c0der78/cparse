/*! \file acl.h */
#ifndef CPARSE_ACL_H
#define CPARSE_ACL_H

#include <cparse/defines.h>

BEGIN_DECL

void cparse_acl_copy(cParseACL *orig, cParseACL *other);

void cparse_set_default_acl(cParseACL *acl, bool currentUserAccess);

void cparse_acl_free(cParseACL *acl);

cParseACL *cparse_acl_from_json(cParseJson *json);

cParseJson *cparse_acl_to_json(cParseACL *acl);

bool cparse_acl_is_readable(cParseACL *acl, const char *name);

bool cparse_acl_is_writable(cParseACL *acl, const char *name);

void cparse_acl_set_readable(cParseACL *acl, const char *name, bool value);

void cparse_acl_set_writable(cParseACL *acl, const char *name, bool value);

END_DECL

#endif
