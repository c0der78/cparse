#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cparse/object.h>
#include <cparse/error.h>
#include <cparse/json.h>
#include <stdio.h>
#include "client.h"
#include "protocol.h"
#include <cparse/util.h>
#include <cparse/parse.h>
#include "private.h"
#include "json_private.h"

/* internals */

/* this is a background thread. The argument controlls functionality*/
static void *cparse_object_background_action(void *argument)
{
    cParseError *error = NULL;
    cParseObjectThread *arg = (cParseObjectThread *) argument;

    /* cparse_object_save or cparse_object_refresh */
    bool rval = (*arg->action)(arg->obj, &error);

    if (arg->callback)
    {
        (*arg->callback)(arg->obj, rval, error);
    }

    if (error)
    {
        cparse_error_free(error);
    }

    if (arg->cleanup)
    {
        (*arg->cleanup)(arg->obj);
    }

    free(arg);

    return NULL;
}

pthread_t cparse_object_run_in_background(cParseObject *obj, cParseObjectAction action, cParseObjectCallback callback, void (*cleanup)(cParseObject *obj))
{
    cParseObjectThread *arg;
    int rc;

    assert(obj != NULL);

    arg = malloc(sizeof(cParseObjectThread));
    arg->action = action;
    arg->obj = obj;
    arg->cleanup = cleanup;
    arg->callback = callback;

    rc = pthread_create(&arg->thread, NULL, cparse_object_background_action, arg);
    assert(rc == 0);

    return arg->thread;
}

void cparse_object_set_request_includes(cParseObject *obj, cParseRequest *request)
{
    char types[BUFSIZ + 1] = {0};

    /* parse some pointers to include */
    cparse_json_object_foreach_start(obj->attributes, key, val)
    {
        const char *typeVal;

        if (cparse_json_type(val) != cParseJsonObject) continue;

        typeVal = cparse_json_get_string(val, KEY_TYPE);

        if (typeVal && !strcmp(typeVal, TYPE_POINTER))
        {
            strncat(types, ",", BUFSIZ);
            strncat(types, key, BUFSIZ);
        }
    }
    cparse_json_object_foreach_end;

    if (types[0] != 0)
    {
        cparse_client_request_add_data(request, "include", &types[1]);
    }
}

static cParseRequest *cparse_object_create_request(cParseObject *obj, HttpRequestMethod method, cParseError **error)
{
    char buf[BUFSIZ + 1] = {0};

    if (!obj) return NULL;


    if (cparse_object_is_user(obj))
    {
        if (error)
            *error = cparse_error_with_message("Cannot create an object request for a user object");

        return NULL;
    }

    if (method != HttpRequestMethodPost && obj->objectId && *obj->objectId)
    {
        snprintf(buf, BUFSIZ, "%s/%s", obj->className, obj->objectId);
    }
    else
    {
        snprintf(buf, BUFSIZ, "%s", obj->className);
    }

    return cparse_client_request_with_method_and_path(method, buf);
}

/* initializers */
static cParseObject *cparse_object_new()
{
    cParseObject *obj = malloc(sizeof(cParseObject));

    obj->className = NULL;
    obj->objectId = NULL;
    obj->acl = NULL;
    obj->createdAt = 0;
    obj->updatedAt = 0;
    obj->attributes = cparse_json_new();

    return obj;
}

void cparse_object_copy(cParseObject *obj, cParseObject *other)
{
    obj->className = strdup(other->className);
    obj->objectId = strdup(other->objectId);
    obj->acl = cparse_acl_copy(other->acl);
    obj->createdAt = other->createdAt;
    obj->updatedAt = other->updatedAt;

    cparse_object_merge_json(obj, other->attributes);
}

cParseObject *cparse_object_with_class_name(const char *className)
{
    char buf[BUFSIZ + 1] = {0};

    cParseObject *obj = cparse_object_new();

    snprintf(buf, BUFSIZ, "%s%s", OBJECTS_PATH, className);

    obj->className = strdup(buf);

    return obj;
}

cParseObject *cparse_object_with_class_data(const char *className, cParseJson *attributes)
{
    cParseObject *obj = cparse_object_with_class_name(className);

    cparse_object_merge_json(obj, attributes);

    return obj;
}

/* cleanup */
void cparse_object_free(cParseObject *obj)
{

    cparse_json_free(obj->attributes);

    if (obj->className)
        free(obj->className);
    if (obj->objectId)
        free(obj->objectId);
    free(obj);
}

/* getters/setters */

size_t cparse_object_sizeof()
{
    return sizeof(cParseObject);
}

const char *cparse_object_id(cParseObject *obj)
{
    return !obj ? NULL : obj->objectId;
}

const char *cparse_object_class_name(cParseObject *obj)
{
    return !obj ? NULL : obj->className;
}

time_t cparse_object_created_at(cParseObject *obj)
{
    return !obj ? 0 : obj->createdAt;
}
time_t cparse_object_updated_at(cParseObject *obj)
{
    return !obj ? 0 : obj->updatedAt;
}

cParseACL *cparse_object_acl(cParseObject *obj)
{
    return obj->acl;
}

bool cparse_object_exists(cParseObject *obj)
{
    return obj && obj->objectId && *obj->objectId;
}

/* client related functions */

bool cparse_object_delete(cParseObject *obj, cParseError **error)
{
    cParseRequest *request;
    bool rval;

    if (!obj) return false;

    if (!cparse_object_exists(obj))
    {
        if (error)
            *error = cparse_error_with_message("Object has no id");
        return false;
    }

    request = cparse_object_create_request(obj, HttpRequestMethodDelete, error);

    if (!request)
    {
        return false;
    }

    rval = cparse_client_request_perform(request, error);

    cparse_client_request_free(request);

    return rval;
}

bool cparse_object_fetch(cParseObject *obj, cParseError **error)
{
    cParseRequest *request;
    cParseJson *json;

    if (!obj) return false;

    if (!cparse_object_exists(obj))
    {
        if (error)
            *error = cparse_error_with_message("Object has no id");
        return false;
    }

    request = cparse_object_create_request(obj, HttpRequestMethodGet, error);

    if (!request)
    {
        return false;
    }

    cparse_object_set_request_includes(obj, request);

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json)
    {
        cparse_object_merge_json(obj, json);

        cparse_json_free(json);

        return true;
    }

    return false;
}

pthread_t cparse_object_fetch_in_background(cParseObject *obj, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(obj, cparse_object_fetch, callback, NULL);
}

bool cparse_object_refresh(cParseObject *obj, cParseError **error)
{
    cParseRequest *request;
    cParseJson *json;

    if (!obj) return false;

    if (!obj->objectId || !*obj->objectId)
    {
        if (error)
            *error = cparse_error_with_message("Object has no id");
        return false;
    }

    request = cparse_object_create_request(obj, HttpRequestMethodGet, error);

    if (!request)
    {
        return false;
    }

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json)
    {
        cparse_object_merge_json(obj, json);

        cparse_json_free(json);

        return true;

    }
    return false;
}

pthread_t cparse_object_refresh_in_background(cParseObject *obj, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(obj, cparse_object_refresh, callback, NULL);
}

bool cparse_class_name_is_object(const char *className)
{
    if (!className || !*className) return false;

    return !str_prefix(OBJECTS_PATH, className);
}

bool cparse_object_is_object(cParseObject *obj)
{
    return cparse_class_name_is_object(obj->className);
}

bool cparse_object_save(cParseObject *obj, cParseError **error)
{
    cParseRequest *request;
    char buf[BUFSIZ + 1] = {0};
    cParseJson *json;

    if (!obj) return false;

    if (cparse_object_is_user(obj))
    {
        if (error)
            *error = cparse_error_with_message("cannot request an object with a user object");

        return false;
    }

    /* build the request based on the id */
    if (!obj->objectId || !*obj->objectId)
    {
        request = cparse_client_request_with_method_and_path(HttpRequestMethodPost, obj->className);

    }
    else
    {
        snprintf(buf, BUFSIZ, "%s/%s", obj->className, obj->objectId);
        request = cparse_client_request_with_method_and_path(HttpRequestMethodPut, buf);
    }

    cparse_client_request_set_payload(request, cparse_json_to_json_string(obj->attributes));

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json != NULL)
    {
        cparse_object_merge_json(obj, json);

        cparse_json_free(json);

        return true;
    }

    return false;
}

pthread_t cparse_object_save_in_background(cParseObject *obj, cParseObjectCallback callback)
{
    return cparse_object_run_in_background(obj, cparse_object_save, callback, NULL);
}

/* setters */

void cparse_object_set_number(cParseObject *obj, const char *key, cParseNumber value)
{
    assert(obj != NULL);

    cparse_json_set_number(obj->attributes, key, value);
}

void cparse_object_set_real(cParseObject *obj, const char *key, double value)
{
    assert(obj != NULL);

    cparse_json_set_real(obj->attributes, key, value);
}
void cparse_object_set_bool(cParseObject *obj, const char *key, bool value)
{
    assert(obj != NULL);

    cparse_json_set_bool(obj->attributes, key, value);
}

void cparse_object_set_string(cParseObject *obj, const char *key, const char *value)
{
    assert(obj != NULL);

    cparse_json_set_string(obj->attributes, key, value);
}

void cparse_object_set(cParseObject *obj, const char *key, cParseJson *value)
{
    assert(obj != NULL);

    cparse_json_set(obj->attributes, key, cparse_json_new_reference(value));
}


void cparse_object_foreach_attribute(cParseObject *obj, void (*foreach) (cParseJson *data))
{
    cparse_json_object_foreach_start(obj->attributes, key, val)
    {
        foreach(val);
    }
    cparse_json_object_foreach_end;
}

cParseJson *cparse_object_remove(cParseObject *obj, const char *key)
{
    return cparse_json_remove(obj->attributes, key);
}

/* getters */

cParseJson *cparse_object_get(cParseObject *obj, const char *key)
{
    return cparse_json_get(obj->attributes, key);
}

cParseNumber cparse_object_get_number(cParseObject *obj, const char *key, cParseNumber def)
{
    return cparse_json_get_number(obj->attributes, key, def);
}

double cparse_object_get_real(cParseObject *obj, const char *key, double def)
{
    return cparse_json_get_real(obj->attributes, key, def);
}

bool cparse_object_get_bool(cParseObject *obj, const char *key)
{
    return cparse_json_get_bool(obj->attributes, key);
}

const char *cparse_object_get_string(cParseObject *obj, const char *key)
{
    return cparse_json_get_string(obj->attributes, key);
}

size_t cparse_object_attribute_size(cParseObject *obj)
{
    return cparse_json_num_keys(obj->attributes);
}

bool cparse_object_contains(cParseObject *obj, const char *key)
{
    return cparse_json_contains(obj->attributes, key);
}

void cparse_object_set_reference(cParseObject *obj, const char *key, cParseObject *ref)
{
    /* create a data object representing a pointer */
    cParseJson *data = cparse_json_new();

    /* set type to pointer */
    cparse_json_set_string(data, KEY_TYPE, TYPE_POINTER);

    /* add class name */
    if (ref->className && *ref->className)
    {
        if (!str_prefix(OBJECTS_PATH, ref->className))
            cparse_json_set_string(data, KEY_CLASS_NAME, ref->className + strlen(OBJECTS_PATH));
        else
            cparse_json_set_string(data, KEY_CLASS_NAME, ref->className);
    }

    /* add object id */
    if (ref->objectId && *ref->objectId)
        cparse_json_set_string(data, KEY_OBJECT_ID, ref->objectId);

    /* set key for the object */
    cparse_json_set(obj->attributes, key, data);

    cparse_json_free(data);
}

void cparse_object_merge_json(cParseObject *a, cParseJson *b)
{

    /* objectId, createdAt, and updatedAt are special attributes
     * we're remove them from the b if they exist and add them to a
     */
    cParseJson *id = cparse_json_remove(b, KEY_OBJECT_ID);

    if (id != NULL)
    {
        replace_str(&a->objectId, cparse_json_to_string(id));

        cparse_json_free(id);
    }

    id = cparse_json_remove(b, KEY_CREATED_AT);

    if (id != NULL)
    {
        a->createdAt = cparse_date_time(cparse_json_to_string(id));

        cparse_json_free(id);
    }

    id = cparse_json_remove(b, KEY_UPDATED_AT);

    if (id != NULL)
    {
        a->updatedAt = cparse_date_time(cparse_json_to_string(id));

        cparse_json_free(id);
    }

    id = cparse_json_remove(b, KEY_CLASS_NAME);

    if (id != NULL)
    {
        cparse_json_free(id);
    }

    cparse_json_copy(a->attributes, b, true);
}

cParseObject *cparse_object_from_json(cParseJson *jobj)
{
    cParseObject *obj = cparse_object_new();

    cparse_json_copy(obj->attributes, jobj, false);

    return obj;
}

const char *cparse_object_to_json_string(cParseObject *obj)
{
    return cparse_json_to_json_string(obj->attributes);
}
