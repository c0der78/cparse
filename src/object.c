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

/* internals */

/* for background threads */
typedef struct
{
    CPARSE_OBJ *obj;
    CPARSE_OBJ_CALLBACK callback; /* the callback passed by user */
    bool (*action)(CPARSE_OBJ *obj, CPARSE_ERROR **error); /* the method to call in thread */
    pthread_t thread;

} CPARSE_OBJ_CALLBACK_ARG;

/* this is a background thread. The argument controlls functionality*/
void *cparse_object_background_action(void *argument)
{
    CPARSE_ERROR *error = NULL;
    CPARSE_OBJ_CALLBACK_ARG *arg = (CPARSE_OBJ_CALLBACK_ARG *) argument;

    /* cparse_object_save or cparse_object_refresh */
    bool rval = (*arg->action)(arg->obj, &error);

    if (arg->callback)
    {
        (*arg->callback)(arg->obj, error);
    }
    else if (error)
    {
        cparse_error_free(error);
    }

    free(arg);

    return NULL;
}

void cparse_object_set_request_includes(CPARSE_OBJ *obj, CPARSE_REQUEST *request)
{
    char params[BUFSIZ + 1] = {0};
    char buf[BUFSIZ + 1] = {0};

    /* parse some pointers to include */
    json_object_object_foreach(obj->attributes, key, val)
    {
        const char *keyVal = cparse_json_get_string(val, KEY_TYPE);

        if (keyVal && !strcmp(keyVal, TYPE_POINTER))
        {
            strncat(params, ",", BUFSIZ);
            strncat(params, key, BUFSIZ);
        }
    }

    if (params[0] != 0)
    {
        params[0] = '=';
        snprintf(buf, BUFSIZ, "include%s", params);
    }

    if (buf[0])
    {
        request->payload = strdup(buf);
    }
}

CPARSE_REQUEST *cparse_object_create_request(CPARSE_OBJ *obj, HTTPRequestMethod method)
{
    char buf[BUFSIZ + 1] = {0};
    CPARSE_REQUEST *request;

    if (!obj) return NULL;

    request = cparse_client_request_new();

    if (method != HTTPRequestMethodPost && obj->objectId && *obj->objectId)
    {
        snprintf(buf, BUFSIZ, "classes/%s/%s", obj->className, obj->objectId);
    }
    else
    {
        snprintf(buf, BUFSIZ, "classes/%s", obj->className);
    }

    request->path = strdup(buf);
    request->method = method;

    return request;
}

/* initializers */
static CPARSE_OBJ *cparse_object_new()
{
    CPARSE_OBJ *obj = malloc(sizeof(CPARSE_OBJ));

    obj->className = NULL;
    obj->objectId = NULL;
    obj->acl = NULL;
    obj->createdAt = 0;
    obj->updatedAt = 0;
    obj->attributes = cparse_json_new();

    return obj;
}

CPARSE_OBJ *cparse_object_copy(CPARSE_OBJ *other)
{
    CPARSE_OBJ *obj = cparse_object_new();

    obj->className = strdup(other->className);
    obj->objectId = strdup(other->objectId);
    obj->acl = cparse_acl_copy(other->acl);
    obj->createdAt = other->createdAt;
    obj->updatedAt = other->updatedAt;

    cparse_object_merge_json(obj, other->attributes);

    return obj;
}

CPARSE_OBJ *cparse_object_with_class_name(const char *className)
{
    CPARSE_OBJ *obj = cparse_object_new();

    obj->className = strdup(className);

    return obj;
}

CPARSE_OBJ *cparse_object_with_class_data(const char *className, CPARSE_JSON *attributes)
{
    CPARSE_OBJ *obj = cparse_object_with_class_name(className);

    cparse_object_merge_json(obj, attributes);

    return obj;
}

/* cleanup */
void cparse_object_free(CPARSE_OBJ *obj)
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
    return sizeof(CPARSE_OBJ);
}

const char *cparse_object_id(CPARSE_OBJ *obj)
{
    return cparse_base_id((CPARSE_BASE_OBJ *) obj);
}

const char *cparse_object_class_name(CPARSE_OBJ *obj)
{
    return cparse_base_class_name((CPARSE_BASE_OBJ *) obj);
}

time_t cparse_object_created_at(CPARSE_OBJ *obj)
{
    return cparse_base_created_at((CPARSE_BASE_OBJ *) obj);
}
time_t cparse_object_updated_at(CPARSE_OBJ *obj)
{
    return cparse_base_updated_at((CPARSE_BASE_OBJ *) obj);
}

CPARSE_ACL *cparse_object_acl(CPARSE_OBJ *obj)
{
    return obj->acl;
}

bool cparse_object_exists(CPARSE_OBJ *obj)
{
    return obj && obj->objectId && *obj->objectId;
}

/* client related functions */

bool cparse_object_delete(CPARSE_OBJ *obj, CPARSE_ERROR **error)
{
    char buf[BUFSIZ + 1] = {0};
    CPARSE_REQUEST *request;
    bool rval;

    if (!obj) return false;

    if (!cparse_object_exists(obj))
    {
        if (error)
            *error = cparse_error_with_message("Object has no id");
        return false;
    }

    request = cparse_object_create_request(obj, HTTPRequestMethodDelete);

    rval = cparse_client_request_perform(request, error);

    cparse_client_request_free(request);

    return rval;
}

bool cparse_object_fetch(CPARSE_OBJ *obj, CPARSE_ERROR **error)
{
    char buf[BUFSIZ + 1] = {0};
    CPARSE_REQUEST *request;
    CPARSE_JSON *json;

    if (!obj) return false;

    if (!cparse_object_exists(obj))
    {
        if (error)
            *error = cparse_error_with_message("Object has no id");
        return false;
    }

    request = cparse_object_create_request(obj, HTTPRequestMethodGet);

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

pthread_t cparse_object_fetch_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback)
{
    assert(obj != NULL);
    CPARSE_OBJ_CALLBACK_ARG *arg = malloc(sizeof(CPARSE_OBJ_CALLBACK_ARG));

    arg->action = cparse_object_fetch;
    arg->obj = obj;
    arg->callback = callback;

    int rc = pthread_create(&arg->thread, NULL, cparse_object_background_action, arg);
    assert(rc == 0);

    return arg->thread;
}

bool cparse_object_refresh(CPARSE_OBJ *obj, CPARSE_ERROR **error)
{
    CPARSE_REQUEST *request;
    CPARSE_JSON *json;
    char buf[BUFSIZ + 1] = {0};

    if (!obj) return false;

    if (!obj->objectId || !*obj->objectId)
    {
        if (error)
            *error = cparse_error_with_message("Object has no id");
        return false;
    }

    request = cparse_client_request_new();

    request->method = HTTPRequestMethodGet;

    snprintf(buf, BUFSIZ, "classes/%s/%s", obj->className, obj->objectId);
    request->path = strdup(buf);

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

pthread_t cparse_object_refresh_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback)
{
    assert(obj != NULL);
    CPARSE_OBJ_CALLBACK_ARG *arg = malloc(sizeof(CPARSE_OBJ_CALLBACK_ARG));

    arg->action = cparse_object_refresh;
    arg->obj = obj;
    arg->callback = callback;

    int rc = pthread_create(&arg->thread, NULL, cparse_object_background_action, arg);
    assert(rc == 0);

    return arg->thread;
}


bool cparse_object_save(CPARSE_OBJ *obj, CPARSE_ERROR **error)
{
    CPARSE_REQUEST *request;
    char buf[BUFSIZ + 1] = {0};

    if (!obj) return false;

    request = cparse_client_request_new();

    /* build the request based on the id */
    if (!obj->objectId || !*obj->objectId)
    {
        request->method = HTTPRequestMethodPost;
        snprintf(buf, BUFSIZ, "classes/%s", obj->className);
    }
    else
    {
        request->method = HTTPRequestMethodPut;
        snprintf(buf, BUFSIZ, "classes/%s/%s", obj->className, obj->objectId);
    }

    request->path = strdup(buf);

    request->payload = strdup(cparse_json_to_json_string(obj->attributes));

    CPARSE_JSON *json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json != NULL)
    {
        cparse_object_merge_json(obj, json);

        cparse_json_free(json);

        return true;
    }

    return false;
}

pthread_t cparse_object_save_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback)
{
    assert(obj != NULL);
    CPARSE_OBJ_CALLBACK_ARG *arg = malloc(sizeof(CPARSE_OBJ_CALLBACK_ARG));

    arg->action = cparse_object_save;
    arg->obj = obj;
    arg->callback = callback;

    int rc = pthread_create(&arg->thread, NULL, cparse_object_background_action, arg);
    assert(rc == 0);

    return arg->thread;
}

/* setters */

void cparse_object_set_number(CPARSE_OBJ *obj, const char *key, long long value)
{
    assert(obj != NULL);

    cparse_object_set(obj, key, cparse_json_new_number(value));
}

void cparse_object_set_real(CPARSE_OBJ *obj, const char *key, double value)
{
    assert(obj != NULL);

    cparse_object_set(obj, key, cparse_json_new_real(value));
}
void cparse_object_set_bool(CPARSE_OBJ *obj, const char *key, bool value)
{
    assert(obj != NULL);

    cparse_object_set(obj, key, cparse_json_new_bool(value));
}

void cparse_object_set_string(CPARSE_OBJ *obj, const char *key, const char *value)
{
    assert(obj != NULL);

    cparse_object_set(obj, key, cparse_json_new_string(value));
}

void cparse_object_set(CPARSE_OBJ *obj, const char *key, CPARSE_JSON *value)
{
    assert(obj != NULL);

    cparse_json_set(obj->attributes, key, cparse_json_new_reference(value));
}


void cparse_object_foreach_attribute(CPARSE_OBJ *obj, void (*foreach) (CPARSE_JSON *data))
{
    json_object_object_foreach(obj->attributes, key, val)
    {
        foreach(val);
    }
}

CPARSE_JSON *cparse_object_remove(CPARSE_OBJ *obj, const char *key)
{
    return cparse_json_remove(obj->attributes, key);
}

/* getters */

CPARSE_JSON *cparse_object_get(CPARSE_OBJ *obj, const char *key)
{
    return cparse_json_get(obj->attributes, key);
}

long long cparse_object_get_number(CPARSE_OBJ *obj, const char *key, long long def)
{
    return cparse_json_get_number(obj->attributes, key, def);
}

double cparse_object_get_real(CPARSE_OBJ *obj, const char *key, double def)
{
    return cparse_json_get_real(obj->attributes, key, def);
}

bool cparse_object_get_bool(CPARSE_OBJ *obj, const char *key)
{
    return cparse_json_get_bool(obj->attributes, key);
}

const char *cparse_object_get_string(CPARSE_OBJ *obj, const char *key)
{
    return cparse_json_get_string(obj->attributes, key);
}

size_t cparse_object_attribute_size(CPARSE_OBJ *obj)
{
    return cparse_json_num_keys(obj->attributes);
}

bool cparse_object_contains(CPARSE_OBJ *obj, const char *key)
{
    return cparse_json_contains(obj->attributes, key);
}

void cparse_object_set_reference(CPARSE_OBJ *obj, const char *key, CPARSE_OBJ *ref)
{
    /* create a data object representing a pointer */
    CPARSE_JSON *data = cparse_json_new();

    /* set type to pointer */
    cparse_json_set_string(data, KEY_TYPE, TYPE_POINTER);

    /* add class name */
    if (ref->className && *ref->className)
        cparse_json_set_string(data, KEY_CLASS_NAME, ref->className);

    /* add object id */
    if (ref->objectId && *ref->objectId)
        cparse_json_set_string(data, KEY_OBJECT_ID, ref->objectId);

    /* set key for the object */
    cparse_json_set(obj->attributes, key, data);
}

void cparse_object_merge_json(CPARSE_OBJ *a, CPARSE_JSON *b)
{

    /* objectId, createdAt, and updatedAt are special attributes
     * we're remove them from the b if they exist and add them to a
     */
    CPARSE_JSON *id = cparse_json_remove(b, KEY_OBJECT_ID);

    if (id != NULL)
    {
        replace_str(&a->objectId, cparse_json_to_string(id));
    }

    id = cparse_json_remove(b, KEY_CREATED_AT);

    if (id != NULL)
        a->createdAt = cparse_date_time(cparse_json_to_string(id));

    id = cparse_json_remove(b, KEY_UPDATED_AT);

    if (id != NULL)
        a->updatedAt = cparse_date_time(cparse_json_to_string(id));

    cparse_json_remove(b, KEY_CLASS_NAME);

    cparse_json_copy(a->attributes, b, true);
}

CPARSE_OBJ *cparse_object_from_json(CPARSE_JSON *jobj)
{
    CPARSE_OBJ *obj = cparse_object_new();

    cparse_json_copy(obj->attributes, jobj, false);

    return obj;
}

const char *cparse_object_to_json_string(CPARSE_OBJ *obj)
{
    return cparse_json_to_json_string(obj->attributes);
}