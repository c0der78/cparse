#ifndef CPARSE_PRIVATE_H
#define CPARSE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

struct cparse_object
{
    /* The following must match CPARSE_BASE_OBJ structure */
    char *className;
    char *objectId;
    time_t updatedAt;
    time_t createdAt;

    /* rest of fields */
    CPARSE_JSON *attributes;
    CPARSE_ACL *acl;
};


struct cparse_query
{
    char *className;
    int limit;
    int skip;
    bool trace;
    bool count;
    CPARSE_JSON *where;
    CPARSE_OBJ **results;
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

#define CPARSE_USER_CLASS_NAME "users"

#ifdef __cplusplus
}
#endif

#endif
