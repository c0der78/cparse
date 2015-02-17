/*! \file json.h */
#ifndef CPARSE_JSON_H_
#define CPARSE_JSON_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <cparse/defines.h>

/* JSON types */
typedef enum
{
    cParseJSONNumber,
    cParseJSONReal,
    cParseJSONString,
    cParseJSONBoolean,
    cParseJSONObject,
    cParseJSONArray,
    cParseJSONNull
} cParseJSONType;

/* value initializers */

/*! allocates a new json object
 * \returns the allocated json object
 */
CPARSE_JSON *cparse_json_new();

/*! increments the reference count on a json object so it is safe to use after deallocation
 * \param orig the original object
 * \returns the object with an incremented reference count
 */
CPARSE_JSON *cparse_json_new_reference(CPARSE_JSON *orig);

/*! creates a number as a json object
 * \param number the number
 * \returns the number as a json object
 */
CPARSE_JSON *cparse_json_new_number(cparse_number);

/*! creates a floating point number as a json object
 * \param number the floating point number
 * \returns the floating point number as a json object
 */
CPARSE_JSON *cparse_json_new_real(double);

/*! creates a bool as a json object
 * \param value the boolean value
 * \returns the value as a json object
 */
CPARSE_JSON *cparse_json_new_bool(bool value);

/*! creates a string as a json object
 * \param value the string value
 * \returns the string as a json object
 */
CPARSE_JSON *cparse_json_new_string(const char *str);

/*! creates an array json object
 * \returns the array json object
 */
CPARSE_JSON *cparse_json_new_array();

/* copy method */

/*! copies one json object into another
 * \param orig the first json object
 * \param other the second json object
 * \param replaceOnConflict a flag indicating if duplicate values should be replaced with the second object
 */
void cparse_json_copy(CPARSE_JSON *orig, CPARSE_JSON *other, bool replaceOnConflict);

/* value cleanup */

/*! deallocates a json object
 * \param value the json object to deallocate
 */
void cparse_json_free(CPARSE_JSON *value);

/* value setters */

/*! sets a json attribute
 * \param obj the json instance
 * \param key the key identifying the value
 * \param value the json object attribute
 */
void cparse_json_set(CPARSE_JSON *obj, const char *key, CPARSE_JSON *value);

/*! sets a number attribute
 * \param obj the json instance
 * \param key the key identifying the value
 * \param value the number attribute
 */
void cparse_json_set_number(CPARSE_JSON *obj, const char *key, cparse_number value);

/*! sets a floating point attribute
 * \param obj the json instance
 * \param key the key identifying the value
 * \param value the floating point attribute
 */
void cparse_json_set_real(CPARSE_JSON *obj, const char *key, double value);

/*! sets a string attribute
 * \param obj the json instance
 * \param key the key identifying the value
 * \param value the string attribute
 */
void cparse_json_set_string(CPARSE_JSON *obj, const char *key, const char *value);

/*! sets a boolean attribute
 * \param obj the json instance
 * \param key the key identifying the value
 * \param value the boolean attribute
 */
void cparse_json_set_bool(CPARSE_JSON *obj, const char *key, bool value);

/* object getters */

/*! gets a json attribute
 * \param obj the json object instance
 * \param key the key identifying the attribute
 */
CPARSE_JSON *cparse_json_get(CPARSE_JSON *obj, const char *key);

/*! gets a number attribute.  if no conversion is possible errno is set to EINVAL.
 * \param obj the json object instance
 * \param key the key identifying the attribute
 * \returns the number of zero if no conversion
 */
cparse_number cparse_json_get_number(CPARSE_JSON *obj, const char *key, cparse_number def);

/*! gets a floating point attribute.  if no conversion is possible errno is set to EINVAL.
 * \param obj the json object instance
 * \param key the key identifying the attribute
 * \returns the floating point or 0.0
 */
double cparse_json_get_real(CPARSE_JSON *obj, const char *key, double def);

/*! gets a boolean attribute
 * \param obj the json object instance
 * \param key the key identifying the attribute
 */
bool cparse_json_get_bool(CPARSE_JSON *obj, const char *key);

/*! gets a string attribute
 * \param obj the json object instance
 * \param key the key identifying the attribute
 */
const char *cparse_json_get_string(CPARSE_JSON *obj, const char *key);

/*! tests if a json object is an array object
 * \param obj the json object instance
 * \returns true if the object is an array
 */
bool cparse_json_is_array(CPARSE_JSON *obj);

/*! gets the type of json object
 * \param obj the object instance
 * \returns a json type of #cParseJSONType
 */
cParseJSONType cparse_json_type(CPARSE_JSON *obj);

/*! gets the number of keys in a json object
 * \param obj the json object instance
 * \returns the number of keys in the object
 */
int cparse_json_num_keys(CPARSE_JSON *obj);

/*! tests if a json object contains a key
 * \param obj the json object instance
 * \param key the key identifying the attribute
 * \returns true if the object contains the key
 */
bool cparse_json_contains(CPARSE_JSON *obj, const char *key);

/*! removes an attribute from a json object
 * \param obj the json object instance
 * \param key the key identifying the attribute
 * \returns the removed attribute
 */
CPARSE_JSON *cparse_json_remove(CPARSE_JSON *obj, const char *key);

/*! creates a json object from a string
 * \param str the json string
 * \returns a json object or NULL if str was not valid json
 */
CPARSE_JSON *cparse_json_tokenize(const char *str);

/* array setters */

/*! adds a number to an array json object
 * \param array the array instance
 * \param value the number value to add
 */
void cparse_json_array_add_number(CPARSE_JSON *array, cparse_number value);

/*! adds a floating point to an array json object
 * \param array the array instance
 * \param value the floating point value to add
 */
void cparse_json_array_add_real(CPARSE_JSON *array, double value);

/*! adds a boolean to an array json object
 * \param array the array instance
 * \param value the boolean value to add
 */
void cparse_json_array_add_bool(CPARSE_JSON *array, bool value);

/*! adds a string to an array json object
 * \param array the array instance
 * \param value the string value to add
 */
void cparse_json_array_add_string(CPARSE_JSON *array, const char *value);

/*! adds a json object to an array json object
 * \param array the array instance
 * \param value the json object value to add
 */
void cparse_json_array_add(CPARSE_JSON *array, CPARSE_JSON *obj);

/* array getters */

/*! gets a number from a position in a json array.  if no conversion is possible errno is set to EINVAL.
 * \param array the array instance
 * \param index the position in the array
 * \returns the number value or zero if no conversion
 */
cparse_number cparse_json_array_get_number(CPARSE_JSON *array, size_t index);

/*! gets a floating point from a position in a json array.  if no conversion is possible errno is set to EINVAL.
 * \param array the array instance
 * \param index the position in the array
 * \returns the floating point value or zero if no conversion
 */
double cparse_json_array_get_real(CPARSE_JSON *array, size_t index);

/*! gets a string from a position in a json array.
 * \param array the array instance
 * \param index the position in the array
 * \returns the string value or zero if no conversion
 */
const char *cparse_json_array_get_string(CPARSE_JSON *array, size_t index);

/*! gets a boolean from a position in a json array.
 * \param array the array instance
 * \param index the position in the array
 * \returns the boolean value or NULL
 */
bool cparse_json_array_get_bool(CPARSE_JSON *array, size_t index);

/*! gets a json object from a position in a json array.
 * \param array the array instance
 * \param index the position in the array
 * \returns the object value or NULL if not found
 */
CPARSE_JSON *cparse_json_array_get(CPARSE_JSON *array, size_t index);

size_t cparse_json_array_size(CPARSE_JSON *value);

/* converters */

/*! gets the number value of an object. if no conversion possible errno is set to EINVAL
 * \param obj the json object instance
 * \returns the number value or zero
 */
cparse_number cparse_json_to_number(CPARSE_JSON *);

/*! gets the floating point value of an object. if no conversion possible errno is set to EINVAL
 * \param obj the json object instance
 * \returns the floating point value or zero
 */
double cparse_json_to_real(CPARSE_JSON *);

/*! gets the boolean value of an object.
 * \param obj the json object instance
 * \returns the boolean value or zero
 */
bool cparse_json_to_bool(CPARSE_JSON *);

/*! gets the string value of an object.
 * \param obj the json object instance
 * \returns the string value or zero
 */
const char *cparse_json_to_string(CPARSE_JSON *value);

/*! converts a json object to a formated json string
 * \param value the json object instance
 * \returns the json string
 */
const char *cparse_json_to_json_string(CPARSE_JSON *value);

/*! a start block for a json object iterator
 * _example_:
 * \code{.c}
 * cparse_json_object_foreach_start(obj, myKey, myVar)
 * {
 *     do_something(myKey, myVar);
 * }
 * cparse_json_object_foreach_end;
 * \endcode
 * \param obj the object instance
 * \param key the name of the key variable
 * \param val the name of the value variable
 */
#define cparse_json_object_foreach_start(obj, key, val) do { \
        char *key; struct json_object *val; struct lh_entry *entry;\
        for(entry = json_object_get_object(obj)->head; entry && ( ( (key = (char*)entry->k)  || entry) && ( (val = (struct json_object*)entry->v)  || entry)); entry = entry->next )

/*! the end block for a json object iterator */
#define cparse_json_object_foreach_end  } while(0)

#ifdef __cplusplus
}
#endif

#endif
