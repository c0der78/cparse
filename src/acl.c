#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <cparse/acl.h>
#include <cparse/user.h>
#include <cparse/util.h>
#include <cparse/json.h>
#include "json_private.h"
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

cParseACL *cparse_acl_with_name(const char *name)
{
    cParseACL *acl = cparse_acl_new();

    acl->name = strdup(name);

    return acl;
}

cParseACL *cparse_acl_new_with_user(cParseUser *user)
{
    cParseACL *acl = cparse_acl_new();

    acl->name = strdup(cparse_object_id(user));

    return acl;
}

void cparse_acl_copy(cParseACL *orig, cParseACL *other)
{
    replace_str(&orig->name, other->name);
    orig->read = other->read;
    orig->write = other->write;
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


cParseACL *cparse_acl_from_json(cParseJson *json)
{
    cParseACL *acl = NULL, *acl_last = NULL;

    if (json == NULL)
        return acl;

    cparse_json_foreach_start(json, key, val)
    {
        cParseACL *newAcl = cparse_acl_new();

        newAcl->name = strdup(key);

        newAcl->read = cparse_json_get_bool(val, "read");

        newAcl->write = cparse_json_get_bool(val, "write");

        newAcl->next = NULL;

        if (acl_last)
        {
            acl_last->next = newAcl;
        }
        else
        {
            acl = newAcl;
        }

        acl_last = newAcl;
    }
    cparse_json_foreach_end;

    return acl;
}

cParseJson *cparse_acl_to_json(cParseACL *acl)
{
    cParseACL *a;
    cParseJson *json = cparse_json_new();

    for (a = acl; a; a = a->next)
    {
        cParseJson *perms = cparse_json_new();
        cparse_json_set_bool(perms, "read", a->read);
        cparse_json_set_bool(perms, "write", a->write);
        cparse_json_set(json, a->name, perms);
    }

    return json;
}

bool cparse_acl_is_readable(cParseACL *acl, const char *name)
{
    cParseACL *a;

    if (str_empty(name))
        return false;

    for (a = acl; a; a = a->next)
    {
        if (!str_cmp(name, a->name))
        {
            return a->read;
        }
    }

    return false;
}

bool cparse_acl_is_writable(cParseACL *acl, const char *name)
{
    cParseACL *a;

    if (str_empty(name))
        return false;

    for (a = acl; a; a = a->next)
    {
        if (!str_cmp(name, a->name))
        {
            return a->write;
        }
    }

    return false;
}

static void cparse_acl_set_acl(cParseACL *acl, const char *name, bool value, void (*funk)(cParseACL *, bool))
{
    cParseACL *a, *acl_last = NULL;

    if (str_empty(name) || !acl)
        return;

    for (a = acl; a; a = a->next)
    {
        acl_last = a;

        if (!str_cmp(name, a->name))
        {
            (*funk)(a, value);
            break;
        }
    }

    if (a == NULL)
    {
        a = cparse_acl_with_name(name);

        (*funk)(a, value);

        if (acl_last == NULL)
        {
            acl->next = a;
        }
        else
        {
            acl_last->next = a;
        }
    }
}

static void cparse_acl_set_this_readable(cParseACL *acl, bool value)
{
    if (acl)
        acl->read = value;
}

static void cparse_acl_set_this_writable(cParseACL *acl, bool value)
{
    if (acl)
        acl->write = value;
}

void cparse_acl_set_readable(cParseACL *acl, const char *name, bool value)
{
    cparse_acl_set_acl(acl, name, value, cparse_acl_set_this_readable);
}

void cparse_acl_set_writable(cParseACL *acl, const char *name, bool value)
{
    cparse_acl_set_acl(acl, name, value, cparse_acl_set_this_writable);
}


