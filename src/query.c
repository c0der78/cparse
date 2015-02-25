#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cparse/query.h>
#include <cparse/json.h>
#include "client.h"
#include "protocol.h"
#include "private.h"


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


void cparse_query_clear_all_caches()
{

}

cParseQuery *cparse_query_new()
{
    cParseQuery *query = malloc(sizeof(cParseQuery));

    query->className = NULL;
    query->limit = 0;
    query->skip = 0;
    query->trace = false;
    query->where = NULL;
    query->results = NULL;
    query->keys = NULL;
    query->count = false;
    query->size = 0;

    return query;
}

void cparse_query_free(cParseQuery *query)
{
    if (query->className)
    {
        free(query->className);
    }

    if (query->keys)
    {
        free(query->keys);
    }

    if (query->results)
    {
        /* note: don't delete the objects, just the array */
        free(query->results);
    }

    if (query->where)
    {
        cparse_json_free(query->where);
    }

    free(query);
}

void cparse_query_free_results(cParseQuery *query)
{
    size_t i;

    for (i = 0; query && i < query->size; i++)
    {
        cparse_object_free(query->results[i]);
        query->results[i] = NULL;
    }
}

cParseQuery *cparse_query_with_class_name(const char *className)
{
    char buf[BUFSIZ + 1] = {0};
    cParseQuery *query = cparse_query_new();

    snprintf(buf, BUFSIZ, "%s%s", OBJECTS_PATH, className);

    query->className = strdup(buf);

    return query;
}

/* getters/setters */

size_t cparse_query_size(cParseQuery *query)
{
    return query->size;
}

cParseObject *cparse_query_result(cParseQuery *query, size_t index)
{
    if (!query || !query->results || index > query->size)
        return NULL;

    return query->results[index];
}

bool cparse_query_find_objects(cParseQuery *query, cParseError **error)
{
    cParseRequest *request;
    cParseJson *data;
    char buf[BUFSIZ + 1];

    /* build the request */

    request = cparse_client_request_with_method_and_path(HttpRequestMethodGet, query->className);

    if (query->where)
    {
        cparse_client_request_add_data(request, "where", cparse_json_to_json_string(query->where));
    }

    if (query->limit > 0)
    {
        snprintf(buf, BUFSIZ, "%d", query->limit);
        cparse_client_request_add_data(request, "limit", buf);
    }

    if (query->skip > 0)
    {
        snprintf(buf, BUFSIZ, "%d", query->skip);
        cparse_client_request_add_data(request, "skip", buf);
    }

    if (query->keys)
    {
        cparse_client_request_add_data(request, "keys", query->keys);
    }

    if (query->count)
    {
        snprintf(buf, BUFSIZ, "%d", query->count);
        cparse_client_request_add_data(request, "count", buf);
    }

    /* do the deed */
    data = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (data == NULL)
    {
        return false;
    }

    if (query->count)
    {
        query->size = cparse_json_get_number(data, "count", 0);
    }
    else
    {
        cParseJson *results = cparse_json_get(data, "results");

        query->size = cparse_json_array_size(results);

        if (query->size > 0)
        {
            int i;

            if (query->results)
            {
                /* fine, go ahead and cleanup */
                free(query->results);
            }
            query->results = malloc(cparse_object_sizeof() * query->size);

            for (i = 0; i < query->size; i++)
            {
                query->results[i] = cparse_object_with_class_data(query->className, cparse_json_array_get(results, i));
            }
        }
    }

    cparse_json_free(data);

    return true;
}

void cparse_query_cancel(cParseQuery *query)
{

}

int cparse_query_count_objects(cParseQuery *query, cParseError **error)
{
    return 0;
}

void cparse_query_set_where(cParseQuery *query, cParseJson *value)
{
    if (query->where)
        cparse_json_free(query->where);

    query->where = cparse_json_new_reference(value);
}

void cparse_query_build_where(cParseQuery *query, cParseQueryBuilder *builder)
{
    if (query->where)
        cparse_json_free(query->where);

    query->where = cparse_json_new_reference(builder->json);
}

void cparse_query_where_in(cParseQuery *query, const char *key, cParseJson *inArray)
{
    cParseQueryBuilder *builder = cparse_query_build_new();

    cparse_query_build_in(builder, key, inArray);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

void cparse_query_where_lte(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = cparse_query_build_new();

    cparse_query_build_lte(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}
void cparse_query_where_lt(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = cparse_query_build_new();

    cparse_query_build_lt(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}
void cparse_query_where_gte(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = cparse_query_build_new();

    cparse_query_build_gte(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}
void cparse_query_where_gt(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = cparse_query_build_new();

    cparse_query_build_gt(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}
void cparse_query_where_ne(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = cparse_query_build_new();

    cparse_query_build_ne(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}
void cparse_query_where_nin(cParseQuery *query, const char *key, cParseJson *inArray)
{
    cParseQueryBuilder *builder = cparse_query_build_new();

    cparse_query_build_nin(builder, key, inArray);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}
void cparse_query_where_exists(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = cparse_query_build_new();

    cparse_query_build_exists(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}
void cparse_query_where_select(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = cparse_query_build_new();

    cparse_query_build_select(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}
void cparse_query_where_nselect(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = cparse_query_build_new();

    cparse_query_build_nselect(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}
void cparse_query_where_all(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = cparse_query_build_new();

    cparse_query_build_all(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

cParseQueryBuilder *cparse_query_build_new()
{
    cParseQueryBuilder *builder = malloc(sizeof(cParseQueryBuilder));

    builder->json = cparse_json_new();

    return builder;
}

cParseQueryBuilder *cparse_query_build_in(cParseQueryBuilder *query, const char *key, cParseJson *inArray)
{
    cParseJson *in = cparse_json_new();

    cparse_json_set(in, CPARSE_QUERY_IN, inArray);

    cparse_json_set(query->json, key, in);

    cparse_json_free(in);

    return query;
}

cParseQueryBuilder *cparse_query_build_lte(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *lte = cparse_json_new();

    cparse_json_set(lte, CPARSE_QUERY_LESS_THAN_EQUAL, value);

    cparse_json_set(query->json, key, lte);

    cparse_json_free(lte);

    return query;
}
cParseQueryBuilder *cparse_query_build_lt(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_LESS_THAN, value);

    cparse_json_set(query->json, key, inner);

    cparse_json_free(inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_gte(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_GREATER_THAN_EQUAL, value);

    cparse_json_set(query->json, key, inner);

    cparse_json_free(inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_gt(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_GREATER_THAN, value);

    cparse_json_set(query->json, key, inner);

    cparse_json_free(inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_ne(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_NOT_EQUAL, value);

    cparse_json_set(query->json, key, inner);

    cparse_json_free(inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_nin(cParseQueryBuilder *query, const char *key, cParseJson *inArray)
{
    cParseJson *inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_NOT_IN, inArray);

    cparse_json_set(query->json, key, inner);

    cparse_json_free(inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_exists(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_EXISTS, value);

    cparse_json_set(query->json, key, inner);

    cparse_json_free(inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_select(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_SELECT, value);

    cparse_json_set(query->json, key, inner);

    cparse_json_free(inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_nselect(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_DONT_SELECT, value);

    cparse_json_set(query->json, key, inner);

    cparse_json_free(inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_all(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_ALL, value);

    cparse_json_set(query->json, key, inner);

    cparse_json_free(inner);

    return query;
}

cParseJson *cparse_query_build_json(cParseQueryBuilder *query)
{
    return query->json;
}

void cparse_query_build_free(cParseQueryBuilder *query)
{
    cparse_json_free(query->json);

    free(query);
}

