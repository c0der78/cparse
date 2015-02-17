/*! \file object.h */
#ifndef CPARSE_OBJECT_H_
#define CPARSE_OBJECT_H_

#include <time.h>
#include <cparse/defines.h>
#include <cparse/acl.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/* initializers */

/*! makes a copy of a parse object
 * \param obj the object instance
 * \returns a copy of the object
 */
CPARSE_OBJ *cparse_object_copy(CPARSE_OBJ *obj);

/*! allocates a parse object with a class name.
 * \param className the type of object to create
 * \returns the allocated object
 */
CPARSE_OBJ *cparse_object_with_class_name(const char *className);

/*! allocates a parse object with json data
 * \param className the type of object to create
 * \param data the data to copy from
 * \returns the allocated object
 */
CPARSE_OBJ *cparse_object_with_class_data(const char *className, CPARSE_JSON *data);

/*! deallocates a parse object
 * \param obj the object instance
 */
void cparse_object_free(CPARSE_OBJ *obj);

/* getters/setters */

/*! gets the memory size of an object
 * \returns the size in bytes
 */
size_t cparse_object_sizeof();

/*! gets the parse object id
 * \param obj the object instance
 * \returns the id or NULL of not set
 */
const char *cparse_object_id(CPARSE_OBJ *obj);

/*! gets the type of parse object
 * \param obj the object instance
 * \returns the object type
 */
const char *cparse_object_class_name(CPARSE_OBJ *obj);

/*! gets the time when the object was created
 * \param obj the object instance
 * \returns the time the object was created or zero if the object has not been saved
 */
time_t cparse_object_created_at(CPARSE_OBJ *obj);

/*! gets the time when the object was updated
 * \param obj the object instance
 * \returns the time the object was updated or zero if the object has not been updated
 */
time_t cparse_object_updated_at(CPARSE_OBJ *obj);

/*! gets the access control list for an object
 * \param obj the object instance
 * \returns the access control list
 */
CPARSE_ACL *cparse_object_acl(CPARSE_OBJ *obj);

/* client/rest methods */

/*! saves a parse object
 * \param obj the object instance
 * \param error a pointer to an error that gets allocated if not successful.
 * \returns true if successful
 */
bool cparse_object_save(CPARSE_OBJ *obj, CPARSE_ERROR **error);

/*! saves a parse object in the background
 * \param obj the object instance
 * \param callback a callback issued after the object is saved
 * \returns the background thread identifier
 */
pthread_t cparse_object_save_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback);

/*! deletes and object
 * \param obj the object instance
 * \param error a pointer to an error that gets allocated if not successful.
 * \returns true if successful
 */
bool cparse_object_delete(CPARSE_OBJ *, CPARSE_ERROR **error);

/*! deletes an object in the background
 * \param obj the object instance
 * \param callback a callback issues after the object is deleted
 * \returns the background thread identifier
 */
pthread_t cparse_object_delete_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback);

/*! refreshes an object' attributes. This does not include other object refereces. (see fetch)
 * \param obj the object instance
 * \param error a pointer to an error that will get allocated if not successful
 * \returns true if successful
 */
bool cparse_object_refresh(CPARSE_OBJ *, CPARSE_ERROR **error);

/*! refreshes an object's attributes in the background. Does not refresh other object refereces. (see fetch)
 * \param obj the object instance
 * \param callback the callback issued after the refresh
 * \returns the background thread identifier
 */
pthread_t cparse_object_refresh_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback);

/*! refreshes an object's attributes, including refereces to other objects.
 * \param obj the object instance
 * \param error a pointer to an error that gets allocated if not successful
 * \returns true if successful
 */
bool cparse_object_fetch(CPARSE_OBJ *, CPARSE_ERROR **error);

/*! refreshes an object's attributes in the background, including references to other objects
 * \param obj the object instance
 * \param callback the callback issues after the fetch
 * \returns a background thread identifier
 */
pthread_t cparse_object_fetch_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback);

/* setters */

/*! tests if the object exists (was saved)
 * \param obj the object instance
 * \returns true if the object exists
 */
bool cparse_object_exists(CPARSE_OBJ *obj);

/*! sets a number attribute on an object
 * \param obj the object instance
 * \param key the key to identify the value
 * \param value the number attribute to set
 */
void cparse_object_set_number(CPARSE_OBJ *obj, const char *key, cparse_number value);

/*! sets a real (float) attribute on an object
 * \param obj the object instance
 * \param key the key to identify the value
 * \param value the attribute value to set
 */
void cparse_object_set_real(CPARSE_OBJ *obj, const char *key, double value);

/*! sets a bool attribute on an object
 * \param obj the object instance
 * \param key the key to identify the value
 * \param value the attribute value to set
 */
void cparse_object_set_bool(CPARSE_OBJ *obj, const char *key, bool value);

/*! sets a string attribute on an object
 * \param obj the object instance
 * \param key the key to identify the value
 * \param value the attribute value to set
 */
void cparse_object_set_string(CPARSE_OBJ *obj, const char *key, const char *value);

/*! sets a json object attribute on an object
 * \param obj the object instance
 * \param key the key to identify the value
 * \param value the attribute value to set
 */
void cparse_object_set(CPARSE_OBJ *obj, const char *key, CPARSE_JSON *value);

/*! tests if an object is a user object
 * \param obj the object instance
 * \returns true if the object is a user object
 */
bool cparse_object_is_user(CPARSE_OBJ *obj);

/*! sets a reference attribute to another object
 * \param obj the object instance
 * \param key the key to identify the reference
 * \param ref the object to reference
 */
void cparse_object_set_reference(CPARSE_OBJ *obj, const char *key, CPARSE_OBJ *ref);

/* getters */

/*! get a json attribute for an object
 * \param obj the object instance
 * \param key the key to identify the attribute value
 */
CPARSE_JSON *cparse_object_get(CPARSE_OBJ *, const char *key);

/*! get a number attribute for an object. strings will be parsed, if no conversion exists error number is set to EINVAL
 * \param obj the object instance
 * \param key the key to identify the attribute value
 * \returns the number or zero if not found
 */
cparse_number cparse_object_get_number(CPARSE_OBJ *, const char *key, cparse_number def);

/*! get a double attribute for an object.  strings will be parsed if no conversion exists error number is set to EINVAL
 * \param obj the object instance
 * \param key the key to identify the attribute value
 */
double cparse_object_get_real(CPARSE_OBJ *, const char *key, double def);

/*! get a bool attribute for an object
 * \param obj the object instance
 * \param key the key to identify the attribute value
 */
bool cparse_object_get_bool(CPARSE_OBJ *, const char *key);

/*! get a string attribute for an object
 * \param obj the object instance
 * \param key the key to identify the attribute value
 */
const char *cparse_object_get_string(CPARSE_OBJ *, const char *key);

/*! removes an attribute from an object
 * \param obj the object instance
 * \param key the key to identify the value to remove
 * \returns the removed attribute or NULL if not found
 */
CPARSE_JSON *cparse_object_remove(CPARSE_OBJ *, const char *key);

/*! tests if an object has an attribute
 * \param obj the object instance
 * \param key the key to identify the value to test
 * \returns true if the object contains the attribute
 */
bool cparse_object_contains(CPARSE_OBJ *obj, const char *key);

/* iterator interface */

/*! iterates and objects attributes
 * \param obj the object instance
 * \param foreach the callback for each object attribute
 */
void cparse_object_foreach(CPARSE_OBJ *obj, void (*foreach) (CPARSE_OBJ *o, CPARSE_JSON *attribute));

/*! gets the number of attributes in an object.  this does not include the common attributes - id, type, create at, or updated at.
 * \param obj the object instance
 * \returns the number of attributes
 */
size_t cparse_object_attribute_size(CPARSE_OBJ *);

/*! merges an object's attributes with a json object, duplicate values will be overwritten with the json object.
 * \param obj the object instance
 * \param json the json to merge with the object.
 */
void cparse_object_merge_json(CPARSE_OBJ *obj, CPARSE_JSON *json);

/*! creates a parse object from a json object as attributes
 * \param json the json object to create from
 * \returns the allocated object
 */
CPARSE_OBJ *cparse_object_from_json(CPARSE_JSON *json);

/*! formats an object into a json string
 * \param obj the object instance
 */
const char *cparse_object_to_json_string(CPARSE_OBJ *obj);

#ifdef __cplusplus
}
#endif

#endif
