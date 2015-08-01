/*! \file defines.h */
#ifndef CPARSE_DEFINES_H
#define CPARSE_DEFINES_H

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

#ifdef __cplusplus
#define BEGIN_DECL extern "C" {
#else
#define BEGIN_DECL
#endif

#ifdef __cplusplus
#define END_DECL }
#else
#define END_DECL
#endif

/*! a type representing an integral number */
#ifdef HAVE_JSON_EXTENDED
typedef long long cParseNumber;
#else
typedef int cParseNumber;
#endif

/*! An error structure */
typedef struct cparse_error cParseError;

/*! A parse object structure */
typedef struct cparse_object cParseObject;

/*! A parse user structure */
typedef struct cparse_object cParseUser;

/*! A parse role structure */
typedef struct cparse_object cParseRole;

/*! A parse query structure */
typedef struct cparse_query cParseQuery;

typedef struct cparse_query_builder cParseQueryBuilder;

/*! An object callback function */
typedef void (*cParseObjectCallback)(cParseObject *obj, cParseError *error, void *param);

/*! a json structure */
typedef struct json_object cParseJson;

/*! frees a node in a list */
typedef void (*cParseListFreeFunk)(void *);

/*! a node in a list */
typedef struct cparse_list_node cParseListNode;

/*! a list of a type */
typedef struct cparse_list cParseList;

/*! types of access for an object */
typedef enum { cParseAccessRead, cParseAccessWrite } cParseAccess;

#endif
