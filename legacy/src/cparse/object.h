#ifndef CPARSE_OBJECT_H_
#define CPARSE_OBJECT_H_

#include <time.h>
#include <cparse/defines.h>
#include <cparse/acl.h>
#include <json-c/json.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cparse_object
{
    CPARSE_JSON *attributes;
    char *className;
    time_t updatedAt;
    time_t createdAt;
    char *objectId;
    CPARSE_ACL *acl;
};

/* initializers */
CPARSE_OBJ *cparse_object_copy(CPARSE_OBJ *obj);
CPARSE_OBJ *cparse_object_with_class_name(const char *className);
CPARSE_OBJ *cparse_object_with_class_data(const char *className, CPARSE_JSON *data);

/* cleanup */
void cparse_object_free(CPARSE_OBJ *obj);

/* client/rest methods */
bool cparse_object_save(CPARSE_OBJ *obj, CPARSE_ERROR **error);
pthread_t cparse_object_save_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback);

bool cparse_object_delete(CPARSE_OBJ *, CPARSE_ERROR **error);
pthread_t cparse_object_delete_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback);

bool cparse_object_refresh(CPARSE_OBJ *, CPARSE_ERROR **error);
pthread_t cparse_object_refresh_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback);

bool cparse_object_fetch(CPARSE_OBJ *, CPARSE_ERROR **error);
pthread_t cparse_object_fetch_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_CALLBACK callback);

/* setters */
void cparse_object_set_number(CPARSE_OBJ *obj, const char *key, long long value);
void cparse_object_set_real(CPARSE_OBJ *obj, const char *key, double value);
void cparse_object_set_bool(CPARSE_OBJ *obj, const char *key, bool value);
void cparse_object_set_string(CPARSE_OBJ *obj, const char *key, const char *value);
void cparse_object_set(CPARSE_OBJ *obj, const char *key, CPARSE_JSON *value);

void cparse_object_set_reference(CPARSE_OBJ *obj, const char *key, CPARSE_OBJ *ref);

/* getters */
CPARSE_JSON *cparse_object_get(CPARSE_OBJ *, const char *key);
long long cparse_object_get_number(CPARSE_OBJ *, const char *key, long long def);
double cparse_object_get_real(CPARSE_OBJ *, const char *key, double def);
bool cparse_object_get_bool(CPARSE_OBJ *, const char *key);
const char *cparse_object_get_string(CPARSE_OBJ *, const char *key);
CPARSE_JSON_ARRAY *cparse_object_get_array(CPARSE_OBJ *, const char *key);

CPARSE_JSON *cparse_object_remove(CPARSE_OBJ *, const char *key);
bool cparse_object_contains(CPARSE_OBJ *obj, const char *key);

/* iterator interface */
void cparse_object_foreach(CPARSE_OBJ *, void (*foreach) (CPARSE_JSON *));

size_t cparse_object_attributes(CPARSE_OBJ *);

void cparse_object_merge_json(CPARSE_OBJ *a, CPARSE_JSON *b);

CPARSE_OBJ *cparse_object_from_json(CPARSE_JSON *obj);

#ifdef __cplusplus
}
#endif

#endif
