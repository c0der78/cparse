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
typedef long long cParseNumber;
#else
typedef int cParseNumber;
#endif

/*! A parse Access Control List structure */
typedef struct cparse_acl cParseACL;

/*! An error structure */
typedef struct cparse_error cParseError;

/*! A parse object structure */
typedef struct cparse_object cParseObject;

/*! A parse query structure */
typedef struct cparse_query cParseQuery;

typedef struct cparse_query_builder cParseQueryBuilder;

/*! An object callback function */
typedef void (*cParseObjectCallback)(cParseObject *obj, bool success, cParseError *error);

/*! a json structure */
typedef struct json_object cParseJson;

/* types */

/*! a parse bytes structure */
typedef struct cparse_type_bytes cParseBytes;

/*! a parse data structure */
typedef struct cparse_type_data cParseData;

/*! a parse file structure */
typedef struct cparse_type_file cParseFile;

/*! a parse geo point structure */
typedef struct cparse_type_geopoint cParseGeoPoint;

/*! a parse pointer structure */
typedef struct cparse_type_pointer cParsePointer;

/* operators */

/*! an array operator */
typedef struct cparse_op_array cParseOperatorArray;

/*! a decrement operator */
typedef struct cparse_op_decrement cParseOperatorDecrement;

/*! a increment operator */
typedef struct cparse_op_increment cParseOperatorIncrement;

#ifdef __cplusplus
}
#endif

#endif
