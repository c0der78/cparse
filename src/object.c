#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cparse/object.h>
#include <cparse/error.h>
#include <cparse/json.h>
#include <cparse/role.h>
#include <stdio.h>
#include "client.h"
#include "protocol.h"
#include <cparse/util.h>
#include <cparse/parse.h>
#include <cparse/types.h>
#include <errno.h>
#include "private.h"
#include <json.h>
#include "log.h"

/* internals */

#define CPARSE_OBJECT_UPDATE_ATTRIBUTES "__update_attributes"

extern cParseUser *__cparse_current_user;

/* this is a background thread. The argument controlls functionality*/
static void *cparse_object_background_action(void *argument)
{
    cParseError *error = NULL;
    cParseObjectThread *arg = NULL;

    if (argument == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    arg = (cParseObjectThread *)argument;

    if (arg->action == NULL) {
        cparse_log_error("object background action has no method");
        return NULL;
    }

    /* cparse_object_save or cparse_object_refresh for example */
    (*arg->action)(arg->obj, &error);

    if (arg->callback) {
        (*arg->callback)(arg->obj, error, arg->param);
    }

    /* should never free an error in a callback as we do it here */
    if (error) {
        cparse_log_warn(cparse_error_message(error));

        cparse_error_free(error);
    }

    if (arg->cleanup) {
        (*arg->cleanup)(arg->obj);
    }

    if (pthread_detach(arg->thread)) {
        cparse_log_errno(errno);
    }

    free(arg);

    return NULL;
}

cparse_thread cparse_object_run_in_background(cParseObject *obj, cParseObjectAction action,
                                              cParseObjectCallback callback, void *param,
                                              void (*cleanup)(cParseObject *obj))
{
    cParseObjectThread *arg = NULL;
    int rc = 0;

    if (obj == NULL || action == NULL) {
        cparse_log_errno(EINVAL);
        return 0;
    }

    arg = malloc(sizeof(cParseObjectThread));

    if (arg == NULL) {
        cparse_log_errno(ENOMEM);
        return 0;
    }

    arg->action = action;
    arg->obj = obj;
    arg->param = param;
    arg->cleanup = cleanup;
    arg->callback = callback;
    arg->thread = 0;

    rc = pthread_create(&arg->thread, NULL, cparse_object_background_action, arg);

    if (rc) {
        cparse_log_error("unable to create background thread");
        return 0;
    }

    return arg->thread;
}

void cparse_object_set_request_includes(cParseObject *obj, cParseRequest *request)
{
    char types[CPARSE_BUF_SIZE + 1] = {0};

    if (obj == NULL || request == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    /* parse some pointers to include */
    cparse_json_foreach_start(obj->attributes, key, val)
    {
        const char *typeVal = NULL;

        if (cparse_json_type(val) != cParseJsonObject) {
            continue;
        }

        typeVal = cparse_json_get_string(val, CPARSE_KEY_TYPE);

        if (typeVal && !strcmp(typeVal, CPARSE_TYPE_POINTER)) {
            strncat(types, ",", CPARSE_BUF_SIZE);
            strncat(types, key, CPARSE_BUF_SIZE);
        }
    }
    cparse_json_foreach_end;

    if (types[0] != 0) {
        cparse_client_request_add_data(request, "include", &types[1]);
    }
}

cParseRequest *cparse_object_create_request(cParseObject *obj, cParseHttpRequestMethod method, cParseError **error)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};

    if (!obj) {
        cparse_log_set_errno(error, EINVAL);
        return NULL;
    }

    if (method != cParseHttpRequestMethodPost && !cparse_str_empty(obj->objectId)) {
        snprintf(buf, CPARSE_BUF_SIZE, "%s/%s", obj->urlPath, obj->objectId);
    } else {
        snprintf(buf, CPARSE_BUF_SIZE, "%s", obj->urlPath);
    }

    return cparse_client_request_with_method_and_path(method, buf);
}

/* initializers */
cParseObject *cparse_object_new()
{
    cParseObject *obj = malloc(sizeof(cParseObject));

    if (obj == NULL) {
        cparse_log_errno(ENOMEM);
        return NULL;
    }

    obj->className = NULL;
    obj->urlPath = NULL;
    obj->objectId = NULL;
    obj->createdAt = 0;
    obj->updatedAt = 0;
    obj->attributes = cparse_json_new();

    return obj;
}

void cparse_object_copy(cParseObject *obj, cParseObject *other)
{
    if (obj == NULL || other == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    cparse_replace_str(&obj->className, other->className);
    cparse_replace_str(&obj->urlPath, other->urlPath);
    cparse_replace_str(&obj->objectId, other->objectId);
    obj->createdAt = other->createdAt;
    obj->updatedAt = other->updatedAt;

    cparse_object_merge_json(obj, other->attributes);
}

cParseObject *cparse_object_with_class_name(const char *className)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};
    cParseObject *obj = NULL;

    if (cparse_str_empty(className)) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    obj = cparse_object_new();

    obj->className = strdup(className);

    snprintf(buf, CPARSE_BUF_SIZE, "%s%s", CPARSE_OBJECTS_PATH, className);

    obj->urlPath = strdup(buf);

    return obj;
}

cParseObject *cparse_object_from_query(cParseQuery *query, cParseJson *json)
{
    cParseObject *obj = NULL;

    if (query == NULL || json == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    obj = cparse_object_new();

    if (obj == NULL) {
        return NULL;
    }

    obj->className = strdup(query->className);
    obj->urlPath = strdup(query->urlPath);

    cparse_object_merge_json(obj, json);

    return obj;
}

cParseObject *cparse_object_with_class_data(const char *className, cParseJson *attributes)
{
    cParseObject *obj = NULL;

    if (cparse_str_empty(className)) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    obj = cparse_object_with_class_name(className);

    if (obj == NULL) {
        return NULL;
    }

    cparse_object_merge_json(obj, attributes);

    return obj;
}

/* cleanup */
void cparse_object_free(cParseObject *obj)
{
    if (obj == NULL) {
        return;
    }

    if (__cparse_current_user == obj) {
        __cparse_current_user = NULL;
    }

    cparse_json_free(obj->attributes);

    if (obj->className) {
        free(obj->className);
    }
    if (obj->urlPath) {
        free(obj->urlPath);
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

cParseJson *cparse_object_acl(cParseObject *obj)
{
    if (obj == NULL || obj->attributes == NULL) {
        return NULL;
    }

    return cparse_json_get(obj->attributes, CPARSE_KEY_ACL);
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
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    if (!cparse_object_exists(obj)) {
        cparse_log_set_error(error, "Object has no id");
        return false;
    }

    request = cparse_object_create_request(obj, cParseHttpRequestMethodDelete, error);

    if (!request) {
        return false;
    }

    rval = cparse_client_request_perform(request, error);

    cparse_client_request_free(request);

    return rval;
}

cparse_thread cparse_object_delete_in_background(cParseObject *obj, cParseObjectCallback callback, void *param)
{
    if (!obj) {
        cparse_log_errno(EINVAL);
        return 0;
    }

    return cparse_object_run_in_background(obj, cparse_object_delete, callback, param, NULL);
}

bool cparse_object_fetch(cParseObject *obj, cParseError **error)
{
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!obj) {
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    if (!cparse_object_exists(obj)) {
        cparse_log_set_error(error, "Object has no id");
        return false;
    }

    request = cparse_object_create_request(obj, cParseHttpRequestMethodGet, error);

    if (!request) {
        return false;
    }

    cparse_object_set_request_includes(obj, request);

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json) {
        cparse_object_merge_json(obj, json);

        cparse_json_free(json);

        return true;
    }

    return false;
}

cparse_thread cparse_object_fetch_in_background(cParseObject *obj, cParseObjectCallback callback, void *param)
{
    if (!obj) {
        cparse_log_errno(EINVAL);
        return 0;
    }

    return cparse_object_run_in_background(obj, cparse_object_fetch, callback, param, NULL);
}

bool cparse_object_refresh(cParseObject *obj, cParseError **error)
{
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!obj) {
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    if (cparse_str_empty(obj->objectId)) {
        cparse_log_set_error(error, "Object has no id");
        return false;
    }

    request = cparse_object_create_request(obj, cParseHttpRequestMethodGet, error);

    if (!request) {
        return false;
    }

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json) {
        cparse_object_merge_json(obj, json);

        cparse_json_free(json);

        return true;
    }
    return false;
}

cparse_thread cparse_object_refresh_in_background(cParseObject *obj, cParseObjectCallback callback, void *param)
{
    if (!obj) {
        cparse_log_errno(EINVAL);
        return 0;
    }

    return cparse_object_run_in_background(obj, cparse_object_refresh, callback, param, NULL);
}

bool cparse_object_is_object(cParseObject *obj)
{
    if (obj == NULL) {
        return false;
    }
    return !cparse_str_prefix(CPARSE_OBJECTS_PATH, obj->urlPath);
}

bool cparse_object_save(cParseObject *obj, cParseError **error)
{
    cParseRequest *request = NULL;
    cParseJson *json = NULL;

    if (!obj) {
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    /* build the request based on the id */
    if (cparse_str_empty(obj->objectId)) {
        request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodPost, obj->urlPath);
    } else {
        char buf[CPARSE_BUF_SIZE + 1] = {0};
        snprintf(buf, CPARSE_BUF_SIZE, "%s/%s", obj->urlPath, obj->objectId);
        request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodPut, buf);
    }

    if (request == NULL) {
        cparse_log_set_error(error, "Unable to create request");
        return false;
    }

    cparse_client_request_set_payload(request, cparse_json_to_json_string(obj->attributes));

    json = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (json != NULL) {
        cparse_object_merge_json(obj, json);

        cparse_json_free(json);

        return true;
    }

    return false;
}

cparse_thread cparse_object_save_in_background(cParseObject *obj, cParseObjectCallback callback, void *param)
{
    if (!obj) {
        cparse_log_errno(EINVAL);
        return 0;
    }

    return cparse_object_run_in_background(obj, cparse_object_save, callback, param, NULL);
}

bool cparse_object_update(cParseObject *obj, cParseJson *attributes, cParseError **error)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};
    cParseRequest *request = NULL;
    cParseJson *response = NULL;

    if (!obj || !attributes) {
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    /* build the request based on the id */
    if (cparse_str_empty(obj->objectId)) {
        cparse_log_set_error(error, "object has no id");
        return false;
    }

    snprintf(buf, CPARSE_BUF_SIZE, "%s/%s", obj->urlPath, obj->objectId);

    request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodPut, buf);

    if (request == NULL) {
        cparse_log_set_error(error, "unable to create request");
        return false;
    }

    cparse_client_request_set_payload(request, cparse_json_to_json_string(attributes));

    response = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (response != NULL) {
        cparse_object_merge_json(obj, attributes);

        cparse_object_merge_json(obj, response);

        cparse_json_free(response);

        return true;
    }

    return false;
}

static bool cparse_object_update_object(cParseObject *obj, cParseError **error)
{
    cParseJson *json = NULL;
    bool rval = false;

    if (!obj) {
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    json = cparse_object_remove_and_get(obj, CPARSE_OBJECT_UPDATE_ATTRIBUTES);

    rval = cparse_object_update(obj, json, error);

    cparse_json_free(json);

    return rval;
}

cparse_thread cparse_object_update_in_background(cParseObject *obj, cParseJson *json, cParseObjectCallback callback,
                                                 void *param)
{
    if (obj == NULL || json == NULL) {
        return 0;
    }

    /* can't pass to our callback method, so place inside the object for retrieval */
    cparse_object_set(obj, CPARSE_OBJECT_UPDATE_ATTRIBUTES, json);

    return cparse_object_run_in_background(obj, cparse_object_update_object, callback, param, NULL);
}

/* setters */

void cparse_object_set_number(cParseObject *obj, const char *key, cParseNumber value)
{
    if (obj != NULL && obj->attributes) {
        cparse_json_set_number(obj->attributes, key, value);
    } else {
        cparse_log_errno(EINVAL);
    }
}

void cparse_object_set_real(cParseObject *obj, const char *key, double value)
{
    if (obj != NULL && obj->attributes) {
        cparse_json_set_real(obj->attributes, key, value);
    } else {
        cparse_log_errno(EINVAL);
    }
}
void cparse_object_set_bool(cParseObject *obj, const char *key, bool value)
{
    if (obj != NULL && obj->attributes) {
        cparse_json_set_bool(obj->attributes, key, value);
    } else {
        cparse_log_errno(EINVAL);
    }
}

void cparse_object_set_string(cParseObject *obj, const char *key, const char *value)
{
    if (obj != NULL && obj->attributes) {
        cparse_json_set_string(obj->attributes, key, value);
    } else {
        cparse_log_errno(EINVAL);
    }
}

void cparse_object_set(cParseObject *obj, const char *key, cParseJson *value)
{
    if (obj != NULL && obj->attributes) {
        cparse_json_set(obj->attributes, key, value);
    } else {
        cparse_log_errno(EINVAL);
    }
}


void cparse_object_foreach_attribute(cParseObject *obj, cParseObjectAttributeCallback callback, void *param)
{
    if (obj == NULL || obj->attributes == NULL || callback == NULL) {
        cparse_log_errno(EINVAL);
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
    if (!obj || cparse_str_empty(key) || !obj->attributes) {
        cparse_log_errno(EINVAL);
        return;
    }
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
        cparse_log_errno(EINVAL);
        return;
    }

    /* create a data object representing a pointer */
    data = cparse_pointer_from_object(ref);

    /* set key for the object */
    cparse_json_set(obj->attributes, key, data);
}

void cparse_object_merge_json(cParseObject *a, cParseJson *b)
{
    /* objectId, createdAt, and updatedAt are special attributes
     * we're remove them from the b if they exist and add them to a
     */
    cParseJson *id = NULL;

    if (!a || !b) {
        cparse_log_errno(EINVAL);
        return;
    }

    id = cparse_json_remove_and_get(b, CPARSE_KEY_OBJECT_ID);

    if (id != NULL) {
        cparse_replace_str(&a->objectId, cparse_json_to_string(id));

        cparse_json_free(id);
    }

    id = cparse_json_remove_and_get(b, CPARSE_KEY_CREATED_AT);

    if (id != NULL) {
        a->createdAt = cparse_date_time(cparse_json_to_string(id));

        cparse_json_free(id);
    }

    id = cparse_json_remove_and_get(b, CPARSE_KEY_UPDATED_AT);

    if (id != NULL) {
        a->updatedAt = cparse_date_time(cparse_json_to_string(id));

        cparse_json_free(id);
    }

    id = cparse_json_remove_and_get(b, CPARSE_KEY_CLASS_NAME);

    if (id != NULL) {
        cparse_json_free(id);
    }

    cparse_json_copy(a->attributes, b, true);
}

const char *cparse_object_to_json_string(cParseObject *obj)
{
    return !obj ? NULL : cparse_json_to_json_string(obj->attributes);
}

static void cparse_object_add_acl(cParseObject *obj, const char *key, cParseAccess access, bool value)
{
    cParseJson *acl = NULL;
    cParseJson *item = NULL;

    if (!obj || !obj->attributes || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return;
    }

    acl = cparse_json_get(obj->attributes, CPARSE_KEY_ACL);

    if (acl == NULL) {
        acl = cparse_json_new();

        cparse_json_set(obj->attributes, CPARSE_KEY_ACL, acl);
    }

    item = cparse_json_get(acl, key);

    if (item == NULL) {
        item = cparse_json_new();

        cparse_json_set(acl, key, item);
    }

    switch (access) {
        case cParseAccessRead:
            cparse_json_set_bool(item, "read", value);
            break;
        case cParseAccessWrite:
            cparse_json_set_bool(item, "write", value);
            break;
    }
}

void cparse_object_set_public_acl(cParseObject *obj, cParseAccess access, bool value)
{
    if (!obj) {
        cparse_log_errno(EINVAL);
        return;
    }
    cparse_object_add_acl(obj, CPARSE_ACL_PUBLIC, access, value);
}

void cparse_object_set_user_acl(cParseObject *obj, cParseUser *user, cParseAccess access, bool value)
{
    if (!obj || !user || !cparse_object_is_user(user) || cparse_str_empty(user->objectId)) {
        cparse_log_errno(EINVAL);
        return;
    }

    cparse_object_add_acl(obj, user->objectId, access, value);
}

void cparse_object_set_role_acl(cParseObject *obj, cParseRole *role, cParseAccess access, bool value)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};

    if (!obj || !role) {
        cparse_log_errno(EINVAL);
        return;
    }

    snprintf(buf, CPARSE_BUF_SIZE, "role:%s", cparse_role_name(role));

    cparse_object_add_acl(obj, buf, access, value);
}

bool cparse_thread_wait(cparse_thread t)
{
    if (pthread_join(t, NULL)) {
        return false;
    }

    return true;
}
