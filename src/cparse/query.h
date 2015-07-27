/*! \file query.h */
#ifndef CPARSE_QUERY_H_
#define CPARSE_QUERY_H_

#include <cparse/defines.h>
#include <cparse/error.h>
#include <cparse/object.h>


BEGIN_DECL

/*! creates a new query for an object type
 * \param className the type of object
 * \returns the allocated query
 */
cParseQuery *cparse_query_with_class_name(const char *className);

cParseQuery *cparse_query_for_object(cParseObject *obj);


/*! deallocates a query
 * \param query the query instance
 */
void cparse_query_free(cParseQuery *query);

void cparse_query_free_results(cParseQuery *query);

/* getters/setters */

/*! gets the size of the results in the query
 * \param query the query instance
 * \returns the number of results in the query
 */
size_t cparse_query_size(cParseQuery *query);

/*! gets a result in the query
 * \param query the query instance
 * \param index the index of the result
 * \returns the query result object or NULL
 */
cParseObject *cparse_query_result(cParseQuery *query, size_t index);

/*! sets the where clause of a query
 * \param query the query instance
 * \param where a json object describing the where clause (see https://parse.com/docs/rest#queries)
 */
void cparse_query_set_where(cParseQuery *query, cParseJson *where);

void cparse_query_build_where(cParseQuery *query, cParseQueryBuilder *builder);

/* functions */

void cparse_query_cancel(cParseQuery *query);

int cparse_query_count_objects(cParseQuery *query, cParseError **error);

/*! find objects from a query
 * \param query the query instance
 * \param error a pointer to an error object that gets allocated if not successful
 * \returns true if successful
 */
bool cparse_query_find_objects(cParseQuery *query, cParseError **error);

void cparse_query_where_in(cParseQuery *query, const char *key, cParseJson *inArray);
void cparse_query_where_lte(cParseQuery *query, const char *key, cParseJson *value);
void cparse_query_where_lt(cParseQuery *query, const char *key, cParseJson *value);
void cparse_query_where_gte(cParseQuery *query, const char *key, cParseJson *value);
void cparse_query_where_gt(cParseQuery *query, const char *key, cParseJson *value);
void cparse_query_where_ne(cParseQuery *query, const char *key, cParseJson *value);
void cparse_query_where_nin(cParseQuery *query, const char *key, cParseJson *inArray);
void cparse_query_where_exists(cParseQuery *query, const char *key, cParseJson *value);
void cparse_query_where_select(cParseQuery *query, const char *key, cParseJson *value);
void cparse_query_where_nselect(cParseQuery *query, const char *key, cParseJson *value);
void cparse_query_where_all(cParseQuery *query, const char *key, cParseJson *value);

/* for building where clauses */

cParseQueryBuilder *cparse_query_build_new();

cParseQueryBuilder *cparse_query_build_in(cParseQueryBuilder *query, const char *key, cParseJson *inArray);
cParseQueryBuilder *cparse_query_build_lte(cParseQueryBuilder *query, const char *key, cParseJson *value);
cParseQueryBuilder *cparse_query_build_lt(cParseQueryBuilder *query, const char *key, cParseJson *value);
cParseQueryBuilder *cparse_query_build_gte(cParseQueryBuilder *query, const char *key, cParseJson *value);
cParseQueryBuilder *cparse_query_build_gt(cParseQueryBuilder *query, const char *key, cParseJson *value);
cParseQueryBuilder *cparse_query_build_ne(cParseQueryBuilder *query, const char *key, cParseJson *value);
cParseQueryBuilder *cparse_query_build_nin(cParseQueryBuilder *query, const char *key, cParseJson *inArray);
cParseQueryBuilder *cparse_query_build_exists(cParseQueryBuilder *query, const char *key, cParseJson *value);
cParseQueryBuilder *cparse_query_build_select(cParseQueryBuilder *query, const char *key, cParseJson *value);
cParseQueryBuilder *cparse_query_build_nselect(cParseQueryBuilder *query, const char *key, cParseJson *value);
cParseQueryBuilder *cparse_query_build_all(cParseQueryBuilder *query, const char *key, cParseJson *value);

cParseQueryBuilder *cparse_query_build_related_to(cParseQueryBuilder *query, const char *key, cParseObject *obj);

cParseJson *cparse_query_build_json(cParseQueryBuilder *query);

void cparse_query_build_free(cParseQueryBuilder *query);

END_DECL

#endif
