#ifndef CPARSE_QUERY_H_
#define CPARSE_QUERY_H_

#include <cparse/defines.h>
#include <cparse/error.h>
#include <cparse/object.h>


#ifdef __cplusplus
extern "C" {
#endif

struct cparse_query
{
    /*CParseCachePolicy cachePolicy;*/
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

/*void cparse_query_clear_all_caches();*/

CPARSE_QUERY *cparse_query_with_class_name(const char *className);

void cparse_query_free(CPARSE_QUERY *query);

void cparse_query_cancel(CPARSE_QUERY *query);

void cparse_query_clear_cache(CPARSE_QUERY *query);

int cparse_query_count_objects(CPARSE_QUERY *query, CPARSE_ERROR **error);

bool cparse_query_find_objects(CPARSE_QUERY *query, CPARSE_ERROR **error);

#define CPARSE_LESS_THAN "$lt"
#define CPARSE_LESS_THAN_EQUAL "$lte"
#define CPARSE_GREATER_THAN "$gt"
#define CPARSE_GREATER_THAN_EQUAL "$gte"
#define CPARSE_NOT_EQUAL "$ne"
#define CPARSE_IN "$in"
#define CPARSE_NOT_IN "$nin"
#define CPARSE_EXISTS "$exists"
#define CPARSE_SELECT "$select"
#define CPARSE_DONT_SELECT "$dontSelect"
#define CPARSE_ALL "$all"

#define CPARSE_ARRAY_KEY "arrayKey"

#ifdef __cplusplus
}
#endif

#endif
