/*! \file query.h */
#ifndef CPARSE_QUERY_H_
#define CPARSE_QUERY_H_

#include <cparse/defines.h>
#include <cparse/error.h>
#include <cparse/object.h>


#ifdef __cplusplus
extern "C" {
#endif

/*! creates a new query for an object type
 * \param className the type of object
 * \returns the allocated query
 */
CPARSE_QUERY *cparse_query_with_class_name(const char *className);

/*! deallocates a query
 * \param query the query instance
 */
void cparse_query_free(CPARSE_QUERY *query);

/* getters/setters */

/*! gets the size of the results in the query
 * \param query the query instance
 * \returns the number of results in the query
 */
size_t cparse_query_size(CPARSE_QUERY *query);

/*! gets a result in the query
 * \param query the query instance
 * \param index the index of the result
 * \returns the query result object or NULL
 */
CPARSE_OBJ *cparse_query_result(CPARSE_QUERY *query, size_t index);

/*! sets the where clause of a query
 * \param query the query instance
 * \param where a json object describing the where clause (see https://parse.com/docs/rest#queries)
 */
void cparse_query_set_where(CPARSE_QUERY *query, CPARSE_JSON *where);

/* functions */

void cparse_query_cancel(CPARSE_QUERY *query);

int cparse_query_count_objects(CPARSE_QUERY *query, CPARSE_ERROR **error);

/*! find objects from a query
 * \param query the query instance
 * \param error a pointer to an error object that gets allocated if not successful
 * \returns true if successful
 */
bool cparse_query_find_objects(CPARSE_QUERY *query, CPARSE_ERROR **error);

/* defines for building where clauses */

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
