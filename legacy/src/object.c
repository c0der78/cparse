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

/* internals */

/* for background threads */
typedef struct cparse_object_background_arg
{
    CPARSE_OBJ *obj;
    CPARSE_OBJ_CALLBACK callback; /* the callback passed by user */
    bool (*action)(CPARSE_OBJ *obj, CPARSE_ERROR **error); /* the method to call in thread */
    pthread_t thread;

} CPARSE_OBJ_BACKGROUND_ARG;

/* this is a background thread. The argument controlls functionality*/
void *cparse_object_background_action(void *argument)
{
    CPARSE_ERROR *error = NULL;
    CPARSE_OBJ_BACKGROUND_ARG *arg = (CPARSE_OBJ_BACKGROUND_ARG *) argument;

    /* cparse_object_save or cparse_object_refresh */
    (*arg->action)(arg->obj, &error);

    if (arg->callback)
    {
        (*arg->callback)(arg->obj, error);
    }

    free(arg);

    return NULL;
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

/* client related functions */

bool cparse_object_delete(CPARSE_OBJ *obj, CPARSE_ERROR **error)
{
    CPARSE_CLIENT_REQ *request;
    char buf[BUFSIZ + 1];

    if (!obj->objectId || !*obj->objectId)
    {
        printf("no object id to delete\n");
        return false;
    }

    request = cparse_client_request_new();

    snprintf(buf, BUFSIZ, "classes/%s/%s", obj->className, obj->objectId);

    request->path = strdup(buf);

    request->method = HTTPRequestMethodDelete;

    cparse_client_request_perform(request, error);

    cparse_client_request_free(request);

    if (error && *error)
    {
        printf("delete error %s\n", (*error)->message);
    }
    return error == NULL || *error == NULL;
}

bool cparse_object_fetch(CPARSE_OBJ *obj, CPARSE_ERROR **error)
{
    CPARSE_CLIENT_REQ *request;
    CPARSE_JSON *data;
    char buf[BUFSIZ + 1];
    char params[BUFSIZ + 1] = {0};

    if (!obj->objectId || !*obj->objectId)
    {
        return false;
    }

    request = cparse_client_request_new();

    /* build the request */
    snprintf(buf, BUFSIZ, "classes/%s/%s", obj->className, obj->objectId);

    request->path = strdup(buf);

    request->method = HTTPRequestMethodGet;

    buf[0] = 0;

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

    request->payload = strdup(buf);

    /* do the deed */
    data = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (error != NULL && *error != NULL)
    {
        cparse_json_free(data);

        return false;
    }

    /* merge the response with the object */
    cparse_object_merge_json(obj, data);

    cparse_json_free(data);

    return true;
}

pthread_t cparse_object_fetch_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback)
{
    assert(obj != NULL);
    CPARSE_OBJ_BACKGROUND_ARG *arg = malloc(sizeof(CPARSE_OBJ_BACKGROUND_ARG));

    arg->action = cparse_object_fetch;
    arg->obj = obj;
    arg->callback = callback;

    int rc = pthread_create(&arg->thread, NULL, cparse_object_background_action, arg);
    assert(rc == 0);

    return arg->thread;
}

bool cparse_object_refresh(CPARSE_OBJ *obj, CPARSE_ERROR **error)
{
    CPARSE_CLIENT_REQ *request;
    CPARSE_JSON *response;
    char buf[BUFSIZ + 1];

    if (!obj->objectId || !*obj->objectId)
    {
        return false;
    }

    request = cparse_client_request_new();

    /* build the request */
    snprintf(buf, BUFSIZ, "classes/%s/%s", obj->className, obj->objectId);

    request->path = strdup(buf);

    request->method = HTTPRequestMethodGet;

    /* do the deed */
    response = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (error != NULL && *error != NULL)
    {
        return false;
    }

    /* merge the response with the object */
    cparse_object_merge_json(obj, response);

    cparse_json_free(response);

    return true;
}

pthread_t cparse_object_refresh_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback)
{
    assert(obj != NULL);
    CPARSE_OBJ_BACKGROUND_ARG *arg = malloc(sizeof(CPARSE_OBJ_BACKGROUND_ARG));

    arg->action = cparse_object_refresh;
    arg->obj = obj;
    arg->callback = callback;

    int rc = pthread_create(&arg->thread, NULL, cparse_object_background_action, arg);
    assert(rc == 0);

    return arg->thread;
}

bool cparse_object_save(CPARSE_OBJ *obj, CPARSE_ERROR **error)
{
    CPARSE_JSON *response;
    CPARSE_CLIENT_REQ *request = cparse_client_request_new();
    char buf[BUFSIZ + 1];

    /* build the request based on the id */
    if (!obj->objectId || !*obj->objectId)
    {
        snprintf(buf, BUFSIZ, "classes/%s", obj->className);

        request->path = strdup(buf);

        request->method = HTTPRequestMethodPost;
    }
    else
    {
        snprintf(buf, BUFSIZ, "classes/%s/%s", obj->className, obj->objectId);

        request->path = strdup(buf);

        request->method = HTTPRequestMethodPut;
    }

    /* build the json payload */

    if (obj->attributes)
    {
        request->payload = strdup(cparse_json_to_json_string(obj->attributes));
    }


    /* do the deed */
    response = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (error != NULL && *error != NULL)
    {
        return false;
    }

    /* merge the result with the object */
    cparse_object_merge_json(obj, response);

    cparse_json_free(response);

    return true;
}

pthread_t cparse_object_save_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback)
{
    assert(obj != NULL);
    CPARSE_OBJ_BACKGROUND_ARG *arg = malloc(sizeof(CPARSE_OBJ_BACKGROUND_ARG));

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

CPARSE_JSON_ARRAY *cparse_object_get_array(CPARSE_OBJ *obj, const char *key)
{
    return cparse_json_get_array(obj->attributes, key);
}

size_t cparse_object_attributes(CPARSE_OBJ *obj)
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

