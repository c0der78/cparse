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
#include <errno.h>
#include "private.h"
#include "json_private.h"
#include "log.h"

/* internals */

#define CPARSE_OBJECT_UPDATE_ATTRIBUTES "__update_attributes"

extern cParseObject *cparse_current_user_;

/* this is a background thread. The argument controlls functionality*/
static void *cparse_object_background_action(void *argument)
{
    cParseError *error = NULL;
    cParseObjectThread *arg = NULL;

    if (argument == NULL) {
        cparse_log_debug(strerror(EINVAL));
        return NULL;
    }

    arg = (cParseObjectThread *) argument;

    if (arg->action == NULL) {
        cparse_log_error("object background action has no method");
        return NULL;
    }

    /* cparse_object_save or cparse_object_refresh */
    bool rval = (*arg->action)(arg->obj, &error);

    if (arg->callback)
    {
        (*arg->callback)(arg->obj, rval, error);
    }

    if (error)
    {
        cparse_log_warn(cparse_error_message(error));

        cparse_error_free(error);
    }

    if (arg->cleanup)
    {
        (*arg->cleanup)(arg->obj);
    }

    pthread_detach(arg->thread);

    free(arg);

    return NULL;
}

static void cparse_object_error(cParseError **error, const char *message)
{
    if (error) {
        *error = cparse_error_with_message(message);
    }

    cparse_log_error(message);
}

pthread_t cparse_object_run_in_background(cParseObject *obj, cParseObjectAction action, cParseObjectCallback callback, void (*cleanup)(cParseObject *obj))
{
    cParseObjectThread *arg = NULL;
    int rc = 0;

    if (obj == NULL || action == NULL) {
        cparse_log_debug(strerror(EINVAL));
        return 0;
    }

    arg = malloc(sizeof(cParseObjectThread));
    arg->action = action;
    arg->obj = obj;
    arg->cleanup = cleanup;
    arg->callback = callback;
    arg->thread = 0;

    rc = pthread_create(&arg->thread, NULL, cparse_object_background_action, arg);

    if (rc) {
        cparse_log_error("unable to create background thread");
    }

    return arg->thread;
}

void cparse_object_set_request_includes(cParseObject *obj, cParseRequest *request)
{
    char types[CPARSE_BUF_SIZE + 1] = {0};

    if (obj == NULL || request == NULL) {
        cparse_log_debug(strerror(EINVAL));
        return;
    }

    /* parse some pointers to include */
    cparse_json_foreach_start(obj->attributes, key, val)
    {
        const char *typeVal = NULL;

        if (cparse_json_type(val) != cParseJsonObject) { continue; }

        typeVal = cparse_json_get_string(val, CPARSE_KEY_TYPE);

        if (typeVal && !strcmp(typeVal, CPARSE_TYPE_POINTER))
        {
            strncat(types, ",", CPARSE_BUF_SIZE);
            strncat(types, key, CPARSE_BUF_SIZE);
        }
    }
    cparse_json_foreach_end;

    if (types[0] != 0)
    {
        cparse_client_request_add_data(request, "include", &types[1]);
    }
}

static cParseRequest *cparse_object_create_request(cParseObject *obj, cParseHttpRequestMethod method, cParseError **error)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};

    if (!obj) {
        cparse_object_error(error, strerror(EINVAL));
        return NULL;
    }

    if (!cparse_object_is_object(obj))
    {
        cparse_object_error(error, "Cannot create an object request for a non-object");
        return NULL;
    }

    if (method != cParseHttpRequestMethodPost && obj->objectId && *obj->objectId)
    {
        snprintf(buf, CPARSE_BUF_SIZE, "%s/%s", obj->className, obj->objectId);
    }
    else
    {
        snprintf(buf, CPARSE_BUF_SIZE, "%s", obj->className);
    }

    return cparse_client_request_with_method_and_path(method, buf);
}

/* initializers */
cParseObject *cparse_object_new()
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
    if (obj == NULL || other == NULL) {
        cparse_log_debug(strerror(EINVAL));
        return;
    }

    cparse_replace_str(&obj->className, other->className);
    cparse_replace_str(&obj->objectId, other->objectId);
    obj->acl = cparse_acl_new();
    cparse_acl_copy(obj->acl, other->acl);
    obj->createdAt = other->createdAt;
    obj->updatedAt = other->updatedAt;

    cparse_object_merge_json(obj, other->attributes);
}

cParseObject *cparse_object_with_class_name(const char *className)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};

    if (!className || !*className) {
        cparse_log_debug(strerror(EINVAL));
        return NULL;
    }

    cParseObject *obj = cparse_object_new();

    snprintf(buf, CPARSE_BUF_SIZE, "%s%s", CPARSE_OBJECTS_PATH, className);

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
    if (cparse_current_user_ == obj) {
        cparse_current_user_ = NULL;
    }

    cparse_json_free(obj->attributes);

    if (obj->className) {
        free(obj->className);
    }
    if (obj->objectId) {
        free(obj->objectId);
    }
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
    return !obj ? NULL : obj->acl;
}

bool cparse_object_exists(cParseObject *obj)
{
    return obj && !cparse_str_empty(obj->objectId);
}

/* client related functions */

bool cparse_object_delete(cParseObject *obj, cParseError **error)
{
    cParseRequest *request = NULL;
    bool rval = false;

    if (!obj) {
        cparse_object_error(error, strerror(EINVAL));
        return false;
    }

    if (!cparse_object_exists(obj))
    {
        cparse_object_error(error, "Object has no id");
        return false;
    }

    request = cparse_object_create_request(obj, cParseHttpRequestMethodDelete, error);

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
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!obj) {
        cparse_object_error(error, strerror(EINVAL));
        return false;
    }

    if (!cparse_object_exists(obj))
    {
        cparse_object_error(error, "Object has no id");
        return false;
    }

    request = cparse_object_create_request(obj, cParseHttpRequestMethodGet, error);

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
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!obj) {
        cparse_object_error(error, strerror(EINVAL));
        return false;
    }

    if (cparse_str_empty(obj->objectId))
    {
        cparse_object_error(error, "Object has no id");
        return false;
    }

    request = cparse_object_create_request(obj, cParseHttpRequestMethodGet, error);

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
    if (cparse_str_empty(className)) { return false; }

    return !cparse_str_prefix(CPARSE_OBJECTS_PATH, className);
}

bool cparse_object_is_object(cParseObject *obj)
{
    return cparse_class_name_is_object(obj->className);
}

bool cparse_object_save(cParseObject *obj, cParseError **error)
{
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!obj) {
        cparse_object_error(error, strerror(EINVAL));
        return false;
    }

    /* build the request based on the id */
    if (cparse_str_empty(obj->objectId))
    {
        request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodPost, obj->className);

    }
    else
    {
        char buf[CPARSE_BUF_SIZE + 1] = {0};
        snprintf(buf, CPARSE_BUF_SIZE, "%s/%s", obj->className, obj->objectId);
        request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodPut, buf);
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

bool cparse_object_update(cParseObject *obj, cParseJson *json, cParseError **error)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};
    cParseRequest *request = NULL;
    cParseJson *response = NULL;

    if (!obj || !json) {
        cparse_object_error(error, strerror(EINVAL));
        return false;
    }

    /* build the request based on the id */
    if (cparse_str_empty(obj->objectId))
    {
        cparse_object_error(error, "object has no id");
        return false;
    }

    snprintf(buf, CPARSE_BUF_SIZE, "%s/%s", obj->className, obj->objectId);

    request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodPut, buf);

    cparse_client_request_set_payload(request, cparse_json_to_json_string(json));

    response = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (response != NULL)
    {
        cparse_object_merge_json(obj, json);

        cparse_object_merge_json(obj, response);

        cparse_json_free(json);

        return true;
    }

    return false;
}

static bool cparse_object_update_object(cParseObject *obj, cParseError **error)
{
    cParseJson *json = NULL;
    bool rval = false;

    if (!obj) {
        cparse_object_error(error, strerror(EINVAL));
        return false;
    }

    json = cparse_object_remove_and_get(obj, CPARSE_OBJECT_UPDATE_ATTRIBUTES);

    rval = cparse_object_update(obj, json, error);

    cparse_json_free(json);

    return rval;
}

pthread_t cparse_object_update_in_background(cParseObject *obj, cParseJson *json, cParseObjectCallback callback)
{
    if (obj == NULL || json == NULL) {
        return NULL;
    }

    // can't pass to our callback method, so place inside the object for retrieval
    cparse_object_set(obj, CPARSE_OBJECT_UPDATE_ATTRIBUTES, json);

    return cparse_object_run_in_background(obj, cparse_object_update_object, callback, NULL);
}

/* setters */

void cparse_object_set_number(cParseObject *obj, const char *key, cParseNumber value)
{
    if (obj != NULL && obj->attributes) {
        cparse_json_set_number(obj->attributes, key, value);
    } else {
        cparse_log_error(strerror(EINVAL));
    }
}

void cparse_object_set_real(cParseObject *obj, const char *key, double value)
{
    if (obj != NULL && obj->attributes) {
        cparse_json_set_real(obj->attributes, key, value);
    } else {
        cparse_log_error(strerror(EINVAL));
    }
}
void cparse_object_set_bool(cParseObject *obj, const char *key, bool value)
{
    if (obj != NULL && obj->attributes) {
        cparse_json_set_bool(obj->attributes, key, value);
    } else {
        cparse_log_error(strerror(EINVAL));
    }
}

void cparse_object_set_string(cParseObject *obj, const char *key, const char *value)
{
    if (obj != NULL && obj->attributes) {
        cparse_json_set_string(obj->attributes, key, value);
    } else {
        cparse_log_error(strerror(EINVAL));
    }
}

void cparse_object_set(cParseObject *obj, const char *key, cParseJson *value)
{
    if (obj != NULL && obj->attributes) {
        cparse_json_set(obj->attributes, key, value);
    } else {
        cparse_log_error(strerror(EINVAL));
    }
}


void cparse_object_foreach_attribute(cParseObject *obj, cParseObjectAttributeCallback callback, void *param)
{
    if (obj == NULL || obj->attributes == NULL || callback == NULL) {
        cparse_log_error(strerror(EINVAL));
        return;
    }

    cparse_json_foreach_start(obj->attributes, key, val)
    {
        callback(obj, key, val, param);
    }
    cparse_json_foreach_end;
}

cParseJson *cparse_object_remove_and_get(cParseObject *obj, const char *key)
{
    return cparse_json_remove_and_get(obj->attributes, key);
}

void cparse_object_remove(cParseObject *obj, const char *key)
{
    cparse_json_remove(obj->attributes, key);
}

/* getters */

cParseJson *cparse_object_get(cParseObject *obj, const char *key)
{
    return !obj ? NULL : cparse_json_get(obj->attributes, key);
}

cParseNumber cparse_object_get_number(cParseObject *obj, const char *key, cParseNumber def)
{
    return !obj ? def : cparse_json_get_number(obj->attributes, key, def);
}

double cparse_object_get_real(cParseObject *obj, const char *key, double def)
{
    return !obj ? def : cparse_json_get_real(obj->attributes, key, def);
}

bool cparse_object_get_bool(cParseObject *obj, const char *key)
{
    return !obj ? false : cparse_json_get_bool(obj->attributes, key);
}

const char *cparse_object_get_string(cParseObject *obj, const char *key)
{
    return !obj ? NULL : cparse_json_get_string(obj->attributes, key);
}

size_t cparse_object_attribute_size(cParseObject *obj)
{
    return !obj ? 0 : cparse_json_num_keys(obj->attributes);
}

bool cparse_object_contains(cParseObject *obj, const char *key)
{
    return !obj ? false : cparse_json_contains(obj->attributes, key);
}

void cparse_object_set_reference(cParseObject *obj, const char *key, cParseObject *ref)
{
    cParseJson *data = NULL;

    if (!obj || cparse_str_empty(key) || !ref) {
        cparse_log_error(strerror(EINVAL));
        return;
    }

    /* create a data object representing a pointer */
    data = cparse_json_new();

    /* set type to pointer */
    cparse_json_set_string(data, CPARSE_KEY_TYPE, CPARSE_TYPE_POINTER);

    /* add class name */
    if (!cparse_str_empty(ref->className))
    {
        if (!cparse_str_prefix(CPARSE_OBJECTS_PATH, ref->className)) {
            cparse_json_set_string(data, CPARSE_KEY_CLASS_NAME, ref->className + strlen(CPARSE_OBJECTS_PATH));
        }
        else {
            cparse_json_set_string(data, CPARSE_KEY_CLASS_NAME, ref->className);
        }
    }

    /* add object id */
    if (!cparse_str_empty(ref->objectId)) {
        cparse_json_set_string(data, CPARSE_KEY_OBJECT_ID, ref->objectId);
    }

    /* set key for the object */
    cparse_json_set(obj->attributes, key, data);

    cparse_json_free(data);
}

void cparse_object_merge_json(cParseObject *a, cParseJson *b)
{

    /* objectId, createdAt, and updatedAt are special attributes
     * we're remove them from the b if they exist and add them to a
     */
    cParseJson *id = cparse_json_remove_and_get(b, CPARSE_KEY_OBJECT_ID);

    if (id != NULL)
    {
        cparse_replace_str(&a->objectId, cparse_json_to_string(id));

        cparse_json_free(id);
    }

    id = cparse_json_remove_and_get(b, CPARSE_KEY_CREATED_AT);

    if (id != NULL)
    {
        a->createdAt = cparse_date_time(cparse_json_to_string(id));

        cparse_json_free(id);
    }

    id = cparse_json_remove_and_get(b, CPARSE_KEY_UPDATED_AT);

    if (id != NULL)
    {
        a->updatedAt = cparse_date_time(cparse_json_to_string(id));

        cparse_json_free(id);
    }

    id = cparse_json_remove_and_get(b, CPARSE_KEY_CLASS_NAME);

    if (id != NULL)
    {
        cparse_json_free(id);
    }

    id = cparse_json_remove_and_get(b, CPARSE_KEY_ACL);

    if (id != NULL)
    {
        cParseACL *acl = cparse_acl_from_json(id);

        if (a->acl == NULL) {
            a->acl = acl;
        } else {
            cparse_acl_copy(a->acl, acl);
        }
        cparse_json_free(id);
    }

    cparse_json_copy(a->attributes, b, true);
}

cParseObject *cparse_object_from_json(cParseJson *jobj)
{
    cParseObject *obj = cparse_object_new();

    cparse_object_merge_json(obj, jobj);

    return obj;
}

const char *cparse_object_to_json_string(cParseObject *obj)
{
    cParseJson *json = cparse_json_new();

    cparse_json_copy(json, obj->attributes, true);

    if (obj->acl != NULL) {
        cparse_json_copy(json, cparse_acl_to_json(obj->acl), true);
    }

    return cparse_json_to_json_string(json);
}


void cparse_object_set_readable_by(cParseObject *obj, const char *name, bool value)
{
    if (name == NULL || !*name) {
        return;
    }

    if (obj->acl == NULL) {
        obj->acl = cparse_acl_with_name(name);
    }

    cparse_acl_set_readable(obj->acl, name, value);
}

void cparse_object_set_writable_by(cParseObject *obj, const char *name, bool value)
{
    if (name == NULL || !*name) {
        return;
    }

    if (obj->acl == NULL) {
        obj->acl = cparse_acl_with_name(name);
    }

    cparse_acl_set_writable(obj->acl, name, value);
}




