#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <cparse/json.h>
#include <cparse/object.h>
#include <cparse/util.h>
#include <assert.h>
#include "json_private.h"
#include "log.h"

/*initializers */
cParseJson *cparse_json_new()
{
    cParseJson *json = json_object_new_object();

    if (json == NULL) {
        cparse_log_errno(ENOMEM);
    }

    return json;
}

cParseJson *cparse_json_new_reference(cParseJson *orig)
{
    if (orig == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    return json_object_get(orig);
}

cParseJson *cparse_json_new_number(cParseNumber value)
{
#ifdef HAVE_JSON_EXTENDED
    return json_object_new_int64(value);
#else
    return json_object_new_int(value);
#endif
}

cParseJson *cparse_json_new_bool(bool value)
{
    return json_object_new_boolean(value);
}

cParseJson *cparse_json_new_real(double value)
{
    return json_object_new_double(value);
}

cParseJson *cparse_json_new_string(const char *value)
{
    if (cparse_str_empty(value)) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    return json_object_new_string(value);
}

cParseJson *cparse_json_tokenize(const char *str)
{
    if (cparse_str_empty(str)) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    return json_tokener_parse(str);
}

void cparse_json_copy(cParseJson *orig, cParseJson *other, bool replaceOnConflict)
{
    if (orig == NULL || other == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    cparse_json_foreach_start(other, key, val)
    {
        if (replaceOnConflict &&
#ifdef HAVE_JSON_EXTENDED
            json_object_object_get_ex(orig, key, NULL)
#else
            json_object_object_get(orig, key) != NULL
#endif
                ) {
            json_object_object_del(orig, key);
        }

        json_object_object_add(orig, key, json_object_get(val));
    }
    cparse_json_foreach_end;
}

cParseJson *cparse_json_new_array()
{
    return json_object_new_array();
}

/* destructors */
void cparse_json_free(cParseJson *value)
{
    if (value == NULL) {
        return;
    }

    json_object_put(value);
}

/* setters */

void cparse_json_set_number(cParseJson *obj, const char *key, cParseNumber value)
{
    if (obj == NULL || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return;
    }

    json_object_object_add(obj, key,
#ifdef HAVE_JSON_EXTENDED
                           json_object_new_int64(value)
#else
                           json_object_new_int(value)
#endif
                               );
}

void cparse_json_set_real(cParseJson *obj, const char *key, double value)
{
    if (obj == NULL || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return;
    }

    json_object_object_add(obj, key, json_object_new_double(value));
}

void cparse_json_set_bool(cParseJson *obj, const char *key, bool value)
{
    if (obj == NULL || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return;
    }

    json_object_object_add(obj, key, json_object_new_boolean(value));
}

void cparse_json_set_string(cParseJson *obj, const char *key, const char *value)
{
    if (obj == NULL || cparse_str_empty(key) || cparse_str_empty(value)) {
        cparse_log_errno(EINVAL);
        return;
    }
    json_object_object_add(obj, key, json_object_new_string(value));
}

void cparse_json_set(cParseJson *obj, const char *key, cParseJson *value)
{
    if (obj == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }
    json_object_object_add(obj, key, value);
}

/* getters */

cParseJson *cparse_json_get(cParseJson *obj, const char *key)
{
    cParseJson *value = NULL;

    if (obj == NULL || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

#ifdef HAVE_JSON_EXTENDED
    if (json_object_object_get_ex(obj, key, &value))
#else
    if ((value = json_object_object_get(obj, key)) != NULL)
#endif
        return value;

    return NULL;
}

cParseNumber cparse_json_get_number(cParseJson *obj, const char *key, cParseNumber def)
{
    cParseJson *value = NULL;

    if (obj == NULL || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return def;
    }

#ifdef HAVE_JSON_EXTENDED
    if (json_object_object_get_ex(obj, key, &value)) {
        return json_object_get_int64(value);
    }
#else
    if ((value = json_object_object_get(obj, key)) != NULL) {
        return json_object_get_int(value);
    }
#endif

    return def;
}
double cparse_json_get_real(cParseJson *obj, const char *key, double def)
{
    cParseJson *value = NULL;

    if (obj == NULL || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return def;
    }

#ifdef HAVE_JSON_EXTENDED
    if (json_object_object_get_ex(obj, key, &value))
#else
    if ((value = json_object_object_get(obj, key)) != NULL)
#endif
        return json_object_get_double(value);

    return def;
}

bool cparse_json_get_bool(cParseJson *obj, const char *key)
{
    cParseJson *value = NULL;

    if (obj == NULL || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return false;
    }

#ifdef HAVE_JSON_EXTENDED
    if (json_object_object_get_ex(obj, key, &value))
#else
    if ((value = json_object_object_get(obj, key)) != NULL)
#endif
        return json_object_get_boolean(value);

    return false;
}
const char *cparse_json_get_string(cParseJson *obj, const char *key)
{
    cParseJson *value = NULL;

    if (obj == NULL || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

#ifdef HAVE_JSON_EXTENDED
    if (json_object_object_get_ex(obj, key, &value))
#else
    if ((value = json_object_object_get(obj, key)) != NULL)
#endif
        return json_object_get_string(value);

    return NULL;
}

/* array setters */
void cparse_json_array_add_number(cParseJson *arr, cParseNumber value)
{
    if (arr == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }
    json_object_array_add(arr,
#ifdef HAVE_JSON_EXTENDED
                          json_object_new_int64(value)
#else
                          json_object_new_int(value)
#endif
                              );
}

void cparse_json_array_add_real(cParseJson *arr, double real)
{
    if (arr == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }
    json_object_array_add(arr, json_object_new_double(real));
}

void cparse_json_array_add_bool(cParseJson *arr, bool b)
{
    if (arr == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }
    json_object_array_add(arr, json_object_new_boolean(b));
}

void cparse_json_array_add_string(cParseJson *arr, const char *value)
{
    if (arr == NULL || cparse_str_empty(value)) {
        cparse_log_errno(EINVAL);
        return;
    }
    json_object_array_add(arr, json_object_new_string(value));
}

void cparse_json_array_add(cParseJson *arr, cParseJson *value)
{
    if (arr == NULL || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }
    json_object_array_add(arr, value);
}

/* array getters */
cParseNumber cparse_json_array_get_number(cParseJson *arr, size_t index, cParseNumber def)
{
    cParseJson *value = NULL;

    if (arr == NULL) {
        cparse_log_errno(EINVAL);
        return def;
    }

    value = json_object_array_get_idx(arr, index);

    if (value == NULL) {
        return def;
    }

#ifdef HAVE_JSON_EXTENDED
    return json_object_get_int64(value);
#else
    return json_object_get_int(value);
#endif
}
bool cparse_json_array_get_bool(cParseJson *arr, size_t index)
{
    cParseJson *value = NULL;

    if (arr == NULL) {
        cparse_log_errno(EINVAL);
        return false;
    }

    value = json_object_array_get_idx(arr, index);

    return json_object_get_boolean(value);
}

double cparse_json_array_get_real(cParseJson *arr, size_t index, double def)
{
    cParseJson *value = NULL;

    if (arr == NULL) {
        cparse_log_errno(EINVAL);
        return def;
    }

    value = json_object_array_get_idx(arr, index);

    if (value == NULL) {
        return def;
    }

    return json_object_get_double(value);
}

const char *cparse_json_array_get_string(cParseJson *arr, size_t index)
{
    cParseJson *value = NULL;

    if (arr == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    value = json_object_array_get_idx(arr, index);

    if (value == NULL) {
        return NULL;
    }

    return json_object_get_string(value);
}

cParseJson *cparse_json_array_get(cParseJson *arr, size_t index)
{
    if (arr == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }
    return json_object_array_get_idx(arr, index);
}

bool cparse_json_is_array(cParseJson *obj)
{
    if (obj == NULL) {
        return false;
    }
    return json_object_get_type(obj) == json_type_array;
}
size_t cparse_json_array_size(cParseJson *array)
{
    if (array == NULL) {
        return 0;
    }

    return json_object_array_length(array);
}

int cparse_json_num_keys(cParseJson *obj)
{
    if (obj == NULL) {
        return 0;
    }

    return json_object_get_object(obj)->count;
}

void cparse_json_remove(cParseJson *obj, const char *key)
{
    if (obj == NULL || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return;
    }

    json_object_object_del(obj, key);
}

cParseJson *cparse_json_remove_and_get(cParseJson *obj, const char *key)
{
    cParseJson *orig = NULL;

    if (obj == NULL || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

#ifdef HAVE_JSON_EXTENDED
    if (json_object_object_get_ex(obj, key, &orig))
#else
    if ((orig = json_object_object_get(obj, key)) != NULL)
#endif
    {
        orig = json_object_get(orig);
        json_object_object_del(obj, key);
    }

    return orig;
}

/* converters */


cParseNumber cparse_json_to_number(cParseJson *v)
{
    if (v == NULL) {
        cparse_log_errno(EINVAL);
        return 0;
    }

#ifdef HAVE_JSON_EXTENDED
    return json_object_get_int64(v);
#else
    return json_object_get_int(v);
#endif
}

bool cparse_json_to_bool(cParseJson *v)
{
    if (v == NULL) {
        cparse_log_errno(EINVAL);
        return false;
    }
    return json_object_get_boolean(v);
}

double cparse_json_to_real(cParseJson *v)
{
    if (v == NULL) {
        cparse_log_errno(EINVAL);
        return 0;
    }
    return json_object_get_double(v);
}

const char *cparse_json_to_string(cParseJson *v)
{
    if (v == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }
    return json_object_get_string(v);
}

cParseJsonType cparse_json_type(cParseJson *v)
{
    if (v == NULL) {
        return cParseJsonNull;
    }

    switch (json_object_get_type(v)) {
        case json_type_int:
            return cParseJsonNumber;
        case json_type_double:
            return cParseJsonReal;
        case json_type_string:
            return cParseJsonString;
        case json_type_boolean:
            return cParseJsonBoolean;
        case json_type_object:
            return cParseJsonObject;
        case json_type_array:
            return cParseJsonArray;
        default:
        case json_type_null:
            return cParseJsonNull;
    }
}

bool cparse_json_contains(cParseJson *obj, const char *key)
{
    if (obj == NULL || cparse_str_empty(key)) {
        cparse_log_errno(EINVAL);
        return false;
    }

#ifdef HAVE_JSON_EXTENDED
    return json_object_object_get_ex(obj, key, NULL);
#else
    return json_object_object_get(obj, key) != NULL;
#endif
}

const char *cparse_json_to_json_string(cParseJson *obj)
{
    if (obj == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

#ifdef HAVE_JSON_EXTENDED
    return json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PLAIN);
#else
    return json_object_to_json_string(obj);
#endif
}
