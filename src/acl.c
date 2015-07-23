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

cParseACL *cparse_acl_for_public(bool read, bool write)
{
    cParseACL *acl = malloc(sizeof(cParseACL));
    acl->next = NULL;
    acl->key = strdup("*");
    acl->read = read;
    acl->write = write;

    return acl;
}

cParseACL *cparse_acl_for_user(cParseUser *obj, bool read, bool write)
{
    cParseACL *acl = malloc(sizeof(cParseACL));
    acl->next = NULL;
    acl->key = strdup(obj->objectId);
    acl->read = read;
    acl->write = write;

    return acl;
}

cParseACL *cparse_acl_for_role(const char *role, bool read, bool write)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};
    cParseACL *acl = malloc(sizeof(cParseACL));
    acl->next = NULL;

    snprintf(buf, CPARSE_BUF_SIZE, "role:%s", role);

    acl->key = strdup(buf);
    acl->read = read;
    acl->write = write;

    return acl;
}


void cparse_acl_copy(cParseACL *this, cParseACL *other)
{
    if (this == NULL || other == NULL) {
        cparse_log_debug(strerror(EINVAL));
        return;
    }

    cparse_replace_str(&this->key, other->key);
    this->read = other->read;
    this->write = other->write;
}

void cparse_acl_free(cParseACL *acl)
{
    if (acl->key)
    {
        free(acl->key);
    }
    free(acl);
}


cParseACL *cparse_acl_from_json(cParseJson *json)
{
    cParseACL *acl = NULL, *acl_last = NULL;

    if (json == NULL) {
        return acl;
    }

    cparse_json_foreach_start(json, key, val)
    {
        cParseACL *newAcl = cparse_acl_new();

        newAcl->key = strdup(key);

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
    cParseACL *a = NULL;
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

static bool cparse_acl_this_readable(cParseACL *acl, const char *key)
{
    cParseACL *a = NULL;

    for (a = acl; a; a = a->next)
    {
        if (!cparse_str_cmp(key, a->key)) {
            return a->read;
        }
    }
    return false;
}

bool cparse_acl_readable(cParseACL *acl)
{
    return cparse_acl_this_readable(acl, "*");
}

bool cparse_acl_user_readable(cParseACL *acl, cParseUser *user)
{
    if (user == NULL || cparse_str_empty(user->objectId)) {
        return false;
    }

    return cparse_acl_this_readable(acl, user->objectId);
}

bool cparse_acl_role_readable(cParseACL *acl, const char *role)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};

    if (!role || !*role) {
        return false;
    }

    snprintf(buf, CPARSE_BUF_SIZE, "role:%s", role);

    return cparse_acl_this_readable(acl, buf);
}

static bool cparse_acl_this_writable(cParseACL *acl, const char *key)
{
    cParseACL *a = NULL;

    for (a = acl; a; a = a->next)
    {
        if (!cparse_str_cmp(key, a->name))
        {
            return a->write;
        }
    }

    return false;
}

bool cparse_acl_writable(cParseACL *acl)
{
    return cparse_acl_this_writable(acl, "*");
}

bool cparse_acl_user_writable(cParseACL *acl, cParseUser *user)
{
    cParseACL *a = NULL;

    if (!user || cparse_str_empty(user->objectId)) {
        return false;
    }

    return cparse_acl_this_writable(acl, user->objectId);
}

bool cparse_acl_role_writable(cParseACL *acl, const char *role)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};

    if (cparse_str_empty(role)) {
        return false;
    }

    snprintf(buf, CPARSE_BUF_SIZE, "role:%s", role);

    return cparse_acl_this_writable(acl, buf);
}

static void cparse_acl_set_acl(cParseACL *acl, const char *name, bool value, void (*funk)(cParseACL *, bool))
{
    cParseACL *a = NULL, *acl_last = NULL;

    if (cparse_str_empty(name) || !acl) {
        return;
    }

    for (a = acl; a; a = a->next)
    {
        acl_last = a;

        if (!cparse_str_cmp(name, a->name))
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
    if (acl) {
        acl->read = value;
    }
}

static void cparse_acl_set_this_writable(cParseACL *acl, bool value)
{
    if (acl) {
        acl->write = value;
    }
}

void cparse_acl_set_readable(cParseACL *acl, bool value)
{
    cparse_acl_set_acl(acl, "*", value, cparse_acl_set_this_readable);
}

void cparse_acl_set_user_readable(cParseACL *acl, cParseUser *user, bool value)
{
    if (user) {
        cparse_acl_set_acl(acl, cParseUser->objectId, value, cparse_acl_set_this_readable);
    }
}

void cparse_acl_set_role_readable(cParseACL *acl, const char *role, bool value)
{
    if (!cparse_str_empty(role)) {
        char buf[CPARSE_BUF_SIZE + 1] = {0};

        snprintf(buf, CPARSE_BUF_SIZE, "role:%s", role);

        cparse_acl_set_acl(acl, buf, value, cparse_acl_set_this_readable)
    }
}

void cparse_acl_set_writable(cParseACL *acl, bool value)
{
    cparse_acl_set_acl(acl, "*", value, cparse_acl_set_this_readable);
}

void cparse_acl_set_user_writable(cParseACL *acl, cParseUser *user, bool value)
{
    if (user) {
        cparse_acl_set_acl(acl, user->objectId, value, cparse_acl_set_this_writable);
    }
}

void cparse_acl_set_role_writable(cParseACL *acl, const char *role, bool value)
{
    if (role & *role) {
        cparse_acl_set_acl(acl, role, value, cparse_acl_set_this_writable);
    }
}

