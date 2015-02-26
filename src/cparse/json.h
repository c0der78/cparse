/*! \file json.h */
#ifndef cParseJson_H_
#define cParseJson_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <cparse/defines.h>

/* JSON types */
typedef enum
{
    cParseJsonNumber,
    cParseJsonReal,
    cParseJsonString,
    cParseJsonBoolean,
    cParseJsonObject,
    cParseJsonArray,
    cParseJsonNull
} cParseJsonType;

/* value initializers */

/*! allocates a new json object
 * \returns the allocated json object
 */
cParseJson *cparse_json_new();

/*! increments the reference count on a json object so it is safe to use after deallocation
 * \param orig the original object
 * \returns the object with an incremented reference count
 */
cParseJson *cparse_json_new_reference(cParseJson *orig);

/*! creates a number as a json object
 * \param number the number
 * \returns the number as a json object
 */
cParseJson *cparse_json_new_number(cParseNumber);

/*! creates a floating point number as a json object
 * \param number the floating point number
 * \returns the floating point number as a json object
 */
cParseJson *cparse_json_new_real(double);

/*! creates a bool as a json object
 * \param value the boolean value
 * \returns the value as a json object
 */
cParseJson *cparse_json_new_bool(bool value);

/*! creates a string as a json object
 * \param value the string value
 * \returns the string as a json object
 */
cParseJson *cparse_json_new_string(const char *str);

/*! creates an array json object
 * \returns the array json object
 */
cParseJson *cparse_json_new_array();

/* copy method */

/*! copies one json object into another
 * \param orig the first json object
 * \param other the second json object
 * \param replaceOnConflict a flag indicating if duplicate values should be replaced with the second object
 */
void cparse_json_copy(cParseJson *orig, cParseJson *other, bool replaceOnConflict);

/* value cleanup */

/*! deallocates a json object
 * \param value the json object to deallocate
 */
void cparse_json_free(cParseJson *value);

/* value setters */

/*! sets a json attribute
 * \param obj the json instance
 * \param key the key identifying the value
 * \param value the json object attribute
 */
void cparse_json_set(cParseJson *obj, const char *key, cParseJson *value);

/*! sets a number attribute
 * \param obj the json instance
 * \param key the key identifying the value
 * \param value the number attribute
 */
void cparse_json_set_number(cParseJson *obj, const char *key, cParseNumber value);

/*! sets a floating point attribute
 * \param obj the json instance
 * \param key the key identifying the value
 * \param value the floating point attribute
 */
void cparse_json_set_real(cParseJson *obj, const char *key, double value);

/*! sets a string attribute
 * \param obj the json instance
 * \param key the key identifying the value
 * \param value the string attribute
 */
void cparse_json_set_string(cParseJson *obj, const char *key, const char *value);

/*! sets a boolean attribute
 * \param obj the json instance
 * \param key the key identifying the value
 * \param value the boolean attribute
 */
void cparse_json_set_bool(cParseJson *obj, const char *key, bool value);

/* object getters */

/*! gets a json attribute
 * \param obj the json object instance
 * \param key the key identifying the attribute
 */
cParseJson *cparse_json_get(cParseJson *obj, const char *key);

/*! gets a number attribute.  if no conversion is possible errno is set to EINVAL.
 * \param obj the json object instance
 * \param key the key identifying the attribute
 * \returns the number of zero if no conversion
 */
cParseNumber cparse_json_get_number(cParseJson *obj, const char *key, cParseNumber def);

/*! gets a floating point attribute.  if no conversion is possible errno is set to EINVAL.
 * \param obj the json object instance
 * \param key the key identifying the attribute
 * \returns the floating point or 0.0
 */
double cparse_json_get_real(cParseJson *obj, const char *key, double def);

/*! gets a boolean attribute
 * \param obj the json object instance
 * \param key the key identifying the attribute
 */
bool cparse_json_get_bool(cParseJson *obj, const char *key);

/*! gets a string attribute
 * \param obj the json object instance
 * \param key the key identifying the attribute
 */
const char *cparse_json_get_string(cParseJson *obj, const char *key);

/*! tests if a json object is an array object
 * \param obj the json object instance
 * \returns true if the object is an array
 */
bool cparse_json_is_array(cParseJson *obj);

/*! gets the type of json object
 * \param obj the object instance
 * \returns a json type of #cParseJSONType
 */
cParseJsonType cparse_json_type(cParseJson *obj);

/*! gets the number of keys in a json object
 * \param obj the json object instance
 * \returns the number of keys in the object
 */
int cparse_json_num_keys(cParseJson *obj);

/*! tests if a json object contains a key
 * \param obj the json object instance
 * \param key the key identifying the attribute
 * \returns true if the object contains the key
 */
bool cparse_json_contains(cParseJson *obj, const char *key);

/*! removes an attribute from a json object
 * \param obj the json object instance
 * \param key the key identifying the attribute
 * \returns the removed attribute
 */
void cparse_json_remove(cParseJson *obj, const char *key);

/* !removes an attribute from a json object but returns the value
 * \param obj the json object instance
 * \param key the key identifying the attribute
 * \returns the removed attribute value
 */
cParseJson *cparse_json_remove_and_get(cParseJson *ob, const char *key);

/*! creates a json object from a string
 * \param str the json string
 * \returns a json object or NULL if str was not valid json
 */
cParseJson *cparse_json_tokenize(const char *str);

/* array setters */

/*! adds a number to an array json object
 * \param array the array instance
 * \param value the number value to add
 */
void cparse_json_array_add_number(cParseJson *array, cParseNumber value);

/*! adds a floating point to an array json object
 * \param array the array instance
 * \param value the floating point value to add
 */
void cparse_json_array_add_real(cParseJson *array, double value);

/*! adds a boolean to an array json object
 * \param array the array instance
 * \param value the boolean value to add
 */
void cparse_json_array_add_bool(cParseJson *array, bool value);

/*! adds a string to an array json object
 * \param array the array instance
 * \param value the string value to add
 */
void cparse_json_array_add_string(cParseJson *array, const char *value);

/*! adds a json object to an array json object
 * \param array the array instance
 * \param value the json object value to add
 */
void cparse_json_array_add(cParseJson *array, cParseJson *obj);

/* array getters */

/*! gets a number from a position in a json array.  if no conversion is possible errno is set to EINVAL.
 * \param array the array instance
 * \param index the position in the array
 * \returns the number value or zero if no conversion
 */
cParseNumber cparse_json_array_get_number(cParseJson *array, size_t index);

/*! gets a floating point from a position in a json array.  if no conversion is possible errno is set to EINVAL.
 * \param array the array instance
 * \param index the position in the array
 * \returns the floating point value or zero if no conversion
 */
double cparse_json_array_get_real(cParseJson *array, size_t index);

/*! gets a string from a position in a json array.
 * \param array the array instance
 * \param index the position in the array
 * \returns the string value or zero if no conversion
 */
const char *cparse_json_array_get_string(cParseJson *array, size_t index);

/*! gets a boolean from a position in a json array.
 * \param array the array instance
 * \param index the position in the array
 * \returns the boolean value or NULL
 */
bool cparse_json_array_get_bool(cParseJson *array, size_t index);

/*! gets a json object from a position in a json array.
 * \param array the array instance
 * \param index the position in the array
 * \returns the object value or NULL if not found
 */
cParseJson *cparse_json_array_get(cParseJson *array, size_t index);

size_t cparse_json_array_size(cParseJson *value);

/* converters */

/*! gets the number value of an object. if no conversion possible errno is set to EINVAL
 * \param obj the json object instance
 * \returns the number value or zero
 */
cParseNumber cparse_json_to_number(cParseJson *);

/*! gets the floating point value of an object. if no conversion possible errno is set to EINVAL
 * \param obj the json object instance
 * \returns the floating point value or zero
 */
double cparse_json_to_real(cParseJson *);

/*! gets the boolean value of an object.
 * \param obj the json object instance
 * \returns the boolean value or zero
 */
bool cparse_json_to_bool(cParseJson *);

/*! gets the string value of an object.
 * \param obj the json object instance
 * \returns the string value or zero
 */
const char *cparse_json_to_string(cParseJson *value);

/*! converts a json object to a formated json string
 * \param value the json object instance
 * \returns the json string
 */
const char *cparse_json_to_json_string(cParseJson *value);

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
