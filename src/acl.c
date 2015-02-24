#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <cparse/acl.h>
#include <cparse/user.h>
#include "private.h"

cParseACL *first_acl;

cParseACL *default_acl = 0;


cParseACL *cparse_acl_new()
{
    cParseACL *acl = malloc(sizeof(cParseACL));
    acl->next = NULL;
    acl->name = NULL;
    acl->read = false;
    acl->write = false;
    return acl;
}

cParseACL *cparse_acl_new_with_user(cParseObject *user)
{
    cParseACL *acl = cparse_acl_new();

    acl->name = strdup(cparse_user_name(user));

    return acl;
}

cParseACL *cparse_acl_copy(cParseACL *other)
{
    cParseACL *acl = cparse_acl_new();

    acl->name = strdup(other->name);
    acl->read = other->read;
    acl->write = other->write;

    return acl;
}

void cparse_set_default_acl(cParseACL *acl, bool currentUserAccess)
{
    default_acl = acl;

    if (currentUserAccess)
    {

    }
}

void cparse_acl_free(cParseACL *acl)
{
    if (acl->name)
    {
        free(acl->name);
    }
    free(acl);
}

