#ifndef CPARSE_PRIVATE_H
#define CPARSE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

struct cparse_object
{
    /* rest of fields */
    CPARSE_JSON *attributes;
    CPARSE_ACL *acl;
    char *className;
    char *objectId;
    time_t updatedAt;
    time_t createdAt;
};


struct cparse_query
{
    CPARSE_JSON *where;
    CPARSE_OBJ **results;
    char *className;
    int limit;
    int skip;
    bool trace;
    bool count;
    size_t size;
    char *keys;
};

struct cparse_acl
{
    CPARSE_ACL *next;
    char *name;
    bool read;
    bool write;
};

typedef bool (*CPARSE_OBJ_ACTION)(CPARSE_OBJ *obj, CPARSE_ERROR **error);

/* for background threads */
typedef struct
{
    CPARSE_OBJ *obj;
    CPARSE_OBJ_CALLBACK callback; /* the callback passed by user */
    CPARSE_OBJ_ACTION action; /* the method to call in thread */
    pthread_t thread;
    void (*cleanup)(CPARSE_OBJ *);
} CPARSE_OBJ_THREAD;

/* this runs a object action in the background */
pthread_t cparse_object_run_in_background(CPARSE_OBJ *obj, CPARSE_OBJ_ACTION action, CPARSE_OBJ_CALLBACK callback, void (*cleanup)(CPARSE_OBJ *));

#define CPARSE_USER_CLASS_NAME "users"

#ifdef __cplusplus
}
#endif

#endif
