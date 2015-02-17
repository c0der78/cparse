#ifndef CPARSE_DEFINES_H_
#define CPARSE_DEFINES_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_STDBOOL_H
typedef unsigned char bool;

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#else
#include <stdbool.h>
#endif

#ifdef HAVE_JSON_INT64
typedef long long cparse_number;
#else
typedef int cparse_number;
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

/* callbacks */
typedef void (*CPARSE_OBJ_CALLBACK)(CPARSE_OBJ *obj, bool success, CPARSE_ERROR *error);

typedef struct json_object CPARSE_JSON;

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
