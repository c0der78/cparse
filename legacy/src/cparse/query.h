#ifndef CPARSE_QUERY_H_
#define CPARSE_QUERY_H_

#include <cparse/defines.h>
#include <cparse/error.h>
#include <cparse/object.h>


#ifdef __cplusplus
extern "C" {
#endif

/*void cparse_query_clear_all_caches();*/

CPARSE_QUERY *cparse_query_with_class_name(const char *className);

void cparse_query_free(CPARSE_QUERY *query);

/* getters/setters */

size_t cparse_query_size(CPARSE_QUERY *query);

CPARSE_OBJ *cparse_query_result(CPARSE_QUERY *query, size_t index);

void cparse_query_set_where(CPARSE_QUERY *query, CPARSE_JSON *where);

/* functions */

void cparse_query_cancel(CPARSE_QUERY *query);

void cparse_query_clear_cache(CPARSE_QUERY *query);

int cparse_query_count_objects(CPARSE_QUERY *query, CPARSE_ERROR **error);

bool cparse_query_find_objects(CPARSE_QUERY *query, CPARSE_ERROR **error);

#define CPARSE_QUERY_LESS_THAN "$lt"
#define CPARSE_QUERY_LESS_THAN_EQUAL "$lte"
#define CPARSE_QUERY_GREATER_THAN "$gt"
#define CPARSE_QUERY_GREATER_THAN_EQUAL "$gte"
#define CPARSE_QUERY_NOT_EQUAL "$ne"
#define CPARSE_QUERY_IN "$in"
#define CPARSE_QUERY_NOT_IN "$nin"
#define CPARSE_QUERY_EXISTS "$exists"
#define CPARSE_QUERY_SELECT "$select"
#define CPARSE_QUERY_DONT_SELECT "$dontSelect"
#define CPARSE_QUERY_ALL "$all"

#define CPARSE_ARRAY_KEY "arrayKey"

#ifdef __cplusplus
}
#endif

#endif
