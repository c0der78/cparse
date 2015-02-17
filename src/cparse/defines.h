/*! \file defines.h */
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

/*! a type representing an integral number */
#ifdef HAVE_JSON_EXTENDED
typedef long long cparse_number;
#else
typedef int cparse_number;
#endif

/*! A parse Access Control List structure */
typedef struct cparse_acl CPARSE_ACL;

/*! An error structure */
typedef struct cparse_error CPARSE_ERROR;

/*! A parse object structure */
typedef struct cparse_object CPARSE_OBJ;

/*! A parse query structure */
typedef struct cparse_query CPARSE_QUERY;

/*! An object callback function */
typedef void (*CPARSE_OBJ_CALLBACK)(CPARSE_OBJ *obj, bool success, CPARSE_ERROR *error);

/*! a json structure */
typedef struct json_object CPARSE_JSON;

/* types */

/*! a parse bytes structure */
typedef struct cparse_type_bytes CPARSE_BYTES;

/*! a parse data structure */
typedef struct cparse_type_data CPARSE_DATA;

/*! a parse file structure */
typedef struct cparse_type_file CPARSE_FILE;

/*! a parse geo point structure */
typedef struct cparse_type_geopoint CPARSE_GEO_POINT;

/*! a parse pointer structure */
typedef struct cparse_type_pointer CPARSE_PTR;

/* operators */

/*! an array operator */
typedef struct cparse_op_array CPARSE_OP_ARRAY;

/*! a decrement operator */
typedef struct cparse_op_decrement CPARSE_OP_DEC;

/*! a increment operator */
typedef struct cparse_op_increment CPARSE_OP_INC;

#ifdef __cplusplus
}
#endif

#endif
