#include "config.h"
#include <string.h>
#include <stdio.h>
#include <cparse/json.h>
#include <cparse/object.h>
#include <assert.h>
#include "json_private.h"

/*initializers */
CPARSE_JSON *cparse_json_new()
{
    return json_object_new_object();
}

CPARSE_JSON *cparse_json_new_reference(CPARSE_JSON *orig)
{
    return json_object_get(orig);
}

CPARSE_JSON *cparse_json_new_number(long long value)
{
    return json_object_new_int64(value);
}

CPARSE_JSON *cparse_json_new_bool(bool value)
{
    return json_object_new_boolean(value);
}

CPARSE_JSON *cparse_json_new_real(double value)
{
    return json_object_new_double(value);
}

CPARSE_JSON *cparse_json_new_string(const char *value)
{
    return json_object_new_string(value);
}

CPARSE_JSON *cparse_json_tokenize(const char *str)
{
    return json_tokener_parse(str);
}

void cparse_json_copy(CPARSE_JSON *orig, CPARSE_JSON *other, bool replaceOnConflict)
{
    json_object_object_foreach(other, key, val)
    {
        if (replaceOnConflict && json_object_object_get_ex(orig, key, NULL))
        {
            json_object_object_del(orig, key);
        }

        json_object_object_add(orig, key, json_object_get(val));
    }
}

CPARSE_JSON *cparse_json_new_array()
{
    return json_object_new_array();
}

/* destructors */
void cparse_json_free(CPARSE_JSON *value)
{
    json_object_put(value);
}

/* setters */

void cparse_json_set_number(CPARSE_JSON *obj, const char *key, long long value)
{
    json_object_object_add(obj, key, json_object_new_int64(value));
}

void cparse_json_set_real(CPARSE_JSON *obj, const char *key, double value)
{
    json_object_object_add(obj, key, json_object_new_double(value));
}

void cparse_json_set_bool(CPARSE_JSON *obj, const char *key, bool value)
{
    json_object_object_add(obj, key, json_object_new_boolean(value));
}

void cparse_json_set_string(CPARSE_JSON *obj, const char *key, const char *value)
{
    json_object_object_add(obj, key, json_object_new_string(value));
}

void cparse_json_set(CPARSE_JSON *obj, const char *key, CPARSE_JSON *value)
{
    json_object_object_add(obj, key, json_object_get(value));
}

/* getters */

CPARSE_JSON *cparse_json_get(CPARSE_JSON *obj, const char *key)
{
    CPARSE_JSON *value = NULL;

    if (json_object_object_get_ex(obj, key, &value))
        return value;

    return NULL;
}

long long cparse_json_get_number(CPARSE_JSON *obj, const char *key, long long def)
{
    CPARSE_JSON *value = NULL;

    if (json_object_object_get_ex(obj, key, &value))
        return json_object_get_int64(value);

    return def;
}
double cparse_json_get_real(CPARSE_JSON *obj, const char *key, double def)
{
    CPARSE_JSON *value = NULL;

    if (json_object_object_get_ex(obj, key, &value))
        return json_object_get_double(value);

    return def;
}

bool cparse_json_get_bool(CPARSE_JSON *obj, const char *key)
{
    CPARSE_JSON *value = NULL;

    if (json_object_object_get_ex(obj, key, &value))
        return json_object_get_boolean(value);

    return false;
}
const char *cparse_json_get_string(CPARSE_JSON *obj, const char *key)
{
    CPARSE_JSON *value = NULL;

    if (json_object_object_get_ex(obj, key, &value))
        return json_object_get_string(value);

    return NULL;
}

/* array setters */
void cparse_json_array_add_number(CPARSE_JSON *arr, long long value)
{
    json_object_array_add(arr, json_object_new_int64(value));
}

void cparse_json_array_add_real(CPARSE_JSON *arr, double real)
{
    json_object_array_add(arr, json_object_new_double(real));
}

void cparse_json_array_add_bool(CPARSE_JSON *arr, bool b)
{
    json_object_array_add(arr, json_object_new_boolean(b));
}

void cparse_json_array_add_string(CPARSE_JSON *arr, const char *value)
{
    json_object_array_add(arr, json_object_new_string(value));
}

void cparse_json_array_add(CPARSE_JSON *arr, CPARSE_JSON *value)
{
    json_object_array_add(arr, json_object_get(value));
}

/* array getters */
long long cparse_json_array_get_number(CPARSE_JSON *arr, size_t index)
{
    CPARSE_JSON *value = json_object_array_get_idx(arr, index);

    return json_object_get_int64(value);
}
bool cparse_json_array_get_bool(CPARSE_JSON *arr, size_t index)
{
    CPARSE_JSON *value = json_object_array_get_idx(arr, index);

    return json_object_get_boolean(value);
}

double cparse_json_array_get_real(CPARSE_JSON *arr, size_t index)
{
    CPARSE_JSON *value = json_object_array_get_idx(arr, index);

    return json_object_get_double(value);
}

const char *cparse_json_array_get_string(CPARSE_JSON *arr, size_t index)
{
    CPARSE_JSON *value = json_object_array_get_idx(arr, index);

    return json_object_get_string(value);
}

CPARSE_JSON *cparse_json_array_get(CPARSE_JSON *arr, size_t index)
{
    return json_object_array_get_idx(arr, index);
}

bool cparse_json_is_arrau(CPARSE_JSON *obj)
{
    return json_object_get_type(obj) == json_type_array;
}
size_t cparse_json_array_size(CPARSE_JSON *array)
{
    return json_object_array_length(array);
}

int cparse_json_num_keys(CPARSE_JSON *obj)
{
    return json_object_get_object (obj)->count;
}

CPARSE_JSON *cparse_json_remove(CPARSE_JSON *obj, const char *key)
{
    CPARSE_JSON *orig = NULL;

    if (json_object_object_get_ex(obj, key, &orig))
    {
        orig = json_object_get(orig);
        json_object_object_del(obj, key);
    }

    return orig;
}

/* converters */


long long cparse_json_to_number(CPARSE_JSON *v)
{
    return json_object_get_int(v);
}

bool cparse_json_to_bool(CPARSE_JSON *v)
{
    return json_object_get_boolean(v);
}

double cparse_json_to_real(CPARSE_JSON *v)
{
    return json_object_get_double(v);
}

const char *cparse_json_to_string(CPARSE_JSON *v)
{
    return json_object_get_string(v);
}

CParseJSONType cparse_json_type(CPARSE_JSON *v)
{
    switch ( json_object_get_type(v) )
    {
    case json_type_int: return kCParseJSONNumber;
    case json_type_double: return kCParseJSONReal;
    case json_type_string: return kCParseJSONString;
    case json_type_boolean: return kCParseJSONBoolean;
    case json_type_object: return  kCParseJSONObject;
    case json_type_array: return kCParseJSONArray;
    case json_type_null: return kCParseJSONNull;
    }
}

bool cparse_json_contains(CPARSE_JSON *obj, const char *key)
{
    return json_object_object_get_ex(obj, key, NULL);
}

const char *cparse_json_to_json_string(CPARSE_JSON *obj)
{
    return json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PLAIN);
}

