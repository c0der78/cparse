#ifndef CPARSE_DEFINES_H_
#define CPARSE_DEFINES_H_

#include <json/json.h>
#include <json/json_object_iterator.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char bool;

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

/*typedef enum
{
    kCParseCachePolicyIgnoreCache = 0,
    kCParseCachePolicyCacheOnly,
    kCParseCachePolicyNetworkOnly,
    kCParseCachePolicyCacheElseNetwork,
    kCParseCachePolicyNetworkElseCache,
    kCParseCachePolicyCacheThenNetwork
} CParseCachePolicy;*/

typedef struct cparse_acl CPARSE_ACL;

typedef struct cparse_error CPARSE_ERROR;

typedef struct cparse_object CPARSE_OBJ;

typedef struct cparse_query CPARSE_QUERY;

typedef struct cparse_user CPARSE_USER;

/* callbacks */
typedef void (*CPARSE_USER_CALLBACK)(CPARSE_USER *user, CPARSE_ERROR *error);

typedef void (*CPARSE_OBJ_CALLBACK)(CPARSE_OBJ *obj, CPARSE_ERROR *error);

/* JSON */
typedef enum
{
    kCParseJSONNumber = json_type_int,
    kCParseJSONReal = json_type_double,
    kCParseJSONString = json_type_string,
    kCParseJSONBoolean = json_type_boolean,
    kCParseJSONObject = json_type_object,
    kCParseJSONArray = json_type_array,
    kCParseJSONNull = json_type_null
} CParseJSONType;

typedef array_list CPARSE_JSON_ARRAY;

typedef json_object CPARSE_JSON;

/* types */
typedef struct cparse_type_bytes CPARSE_BYTES;

typedef struct cparse_type_data CPARSE_DATA;

typedef struct cparse_type_file CPARSE_FILE;

typedef struct cparse_type_geopoint CPARSE_GEO_POINT;

typedef struct cparse_type_pointer CPARSE_PTR;

/* operators */

typedef struct cparse_op_array CPARSE_OP_ARRAY;

typedef struct cparse_op_decrement CPARSE_OP_DEC;

typedef struct cparse_op_increment CPARSE_OP_INC;

#ifdef __cplusplus
}
#endif

#endif
