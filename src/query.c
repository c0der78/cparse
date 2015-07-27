#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cparse/query.h>
#include <cparse/util.h>
#include <cparse/json.h>
#include <cparse/types.h>
#include <errno.h>
#include "client.h"
#include "protocol.h"
#include "private.h"
#include "log.h"


#define CPARSE_QUERY_LESS_THAN          "$lt"
#define CPARSE_QUERY_LESS_THAN_EQUAL    "$lte"
#define CPARSE_QUERY_GREATER_THAN       "$gt"
#define CPARSE_QUERY_GREATER_THAN_EQUAL "$gte"
#define CPARSE_QUERY_NOT_EQUAL          "$ne"
#define CPARSE_QUERY_IN                 "$in"
#define CPARSE_QUERY_NOT_IN             "$nin"
#define CPARSE_QUERY_EXISTS             "$exists"
#define CPARSE_QUERY_SELECT             "$select"
#define CPARSE_QUERY_DONT_SELECT        "$dontSelect"
#define CPARSE_QUERY_ALL                "$all"
#define CPARSE_QUERY_RELATED_TO         "$relatedTo"

#define CPARSE_QUERY_WHERE              "where"
#define CPARSE_QUERY_SKIP               "skip"
#define CPARSE_QUERY_LIMIT              "limit"
#define CPARSE_QUERY_KEYS               "keys"
#define CPARSE_QUERY_COUNT              "count"
#define CPARSE_QUERY_RESULTS            "results"

#define CPARSE_ARRAY_KEY                "arrayKey"


extern cParseObject *cparse_object_from_query(cParseQuery *query, cParseJson *data);

void cparse_query_clear_all_caches()
{

}

cParseQuery *cparse_query_new()
{
    cParseQuery *query = malloc(sizeof(cParseQuery));

    if (query == NULL) {
        cparse_log_errno(ENOMEM);
        return NULL;
    }

    query->className = NULL;
    query->urlPath = NULL;
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
    if (query == NULL) {
        return;
    }
    if (query->className)
    {
        free(query->className);
    }

    if (query->urlPath)
    {
        free(query->urlPath);
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
    size_t i = 0;

    if (query == NULL) {
        return;
    }

    for (i = 0; i < query->size; i++)
    {
        cparse_object_free(query->results[i]);
        query->results[i] = NULL;
    }
}

cParseQuery *cparse_query_with_class_name(const char *className)
{
    char buf[CPARSE_BUF_SIZE + 1] = {0};
    cParseQuery *query = NULL;

    if (cparse_str_empty(className)) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    query = cparse_query_new();

    query->className = strdup(className);

    snprintf(buf, CPARSE_BUF_SIZE, "%s%s", CPARSE_OBJECTS_PATH, className);

    query->urlPath = strdup(buf);

    return query;
}

cParseQuery *cparse_query_for_object(cParseObject *obj) {
    cParseQuery *query = NULL;

    if (obj == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    query = cparse_query_new();

    query->className = strdup(obj->className);
    query->urlPath = strdup(obj->urlPath);

    return query;
}

/* getters/setters */

size_t cparse_query_size(cParseQuery *query)
{
    return query ? query->size : 0;
}

cParseObject *cparse_query_result(cParseQuery *query, size_t index)
{
    if (!query || !query->results || index > query->size) {
        return NULL;
    }

    return query->results[index];
}

bool cparse_query_find_objects(cParseQuery *query, cParseError **error)
{
    cParseRequest *request = NULL;
    cParseJson *data = NULL;
    char buf[CPARSE_BUF_SIZE + 1] = {0};

    if (query == NULL) {
        cparse_log_set_errno(error, EINVAL);
        return false;
    }

    /* build the request */

    request = cparse_client_request_with_method_and_path(cParseHttpRequestMethodGet, query->urlPath);

    if (query->where)
    {
        cparse_client_request_add_data(request, CPARSE_QUERY_WHERE, cparse_json_to_json_string(query->where));
    }

    if (query->limit > 0)
    {
        snprintf(buf, CPARSE_BUF_SIZE, "%d", query->limit);
        cparse_client_request_add_data(request, CPARSE_QUERY_LIMIT, buf);
    }

    if (query->skip > 0)
    {
        snprintf(buf, CPARSE_BUF_SIZE, "%d", query->skip);
        cparse_client_request_add_data(request, CPARSE_QUERY_SKIP, buf);
    }

    if (query->keys)
    {
        cparse_client_request_add_data(request, CPARSE_QUERY_KEYS, query->keys);
    }

    if (query->count)
    {
        snprintf(buf, CPARSE_BUF_SIZE, "%d", query->count);
        cparse_client_request_add_data(request, CPARSE_QUERY_COUNT, buf);
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
        query->size = cparse_json_get_number(data, CPARSE_QUERY_COUNT, 0);
    }
    else
    {
        cParseJson *results = cparse_json_get(data, CPARSE_QUERY_RESULTS);

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

            if (query->results == NULL) {
                cparse_log_errno(ENOMEM);
                return false;
            }

            for (i = 0; i < query->size; i++)
            {
                query->results[i] = cparse_object_from_query(query, cparse_json_array_get(results, i));
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
    if (query == NULL || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    if (query->where) {
        cparse_json_free(query->where);
    }

    query->where = cparse_json_new_reference(value);
}

void cparse_query_build_where(cParseQuery *query, cParseQueryBuilder *builder)
{
    if (query == NULL || builder == NULL || builder->json == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    if (query->where) {
        cparse_json_free(query->where);
    }

    query->where = cparse_json_new_reference(builder->json);
}

void cparse_query_where_in(cParseQuery *query, const char *key, cParseJson *inArray)
{
    cParseQueryBuilder *builder = NULL;

    if (query == NULL || cparse_str_empty(key) || inArray == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    builder = cparse_query_build_new();

    cparse_query_build_in(builder, key, inArray);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

void cparse_query_where_lte(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    builder = cparse_query_build_new();

    cparse_query_build_lte(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

void cparse_query_where_lt(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    builder = cparse_query_build_new();

    cparse_query_build_lt(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

void cparse_query_where_gte(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    builder = cparse_query_build_new();

    cparse_query_build_gte(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

void cparse_query_where_gt(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    builder = cparse_query_build_new();

    cparse_query_build_gt(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

void cparse_query_where_ne(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    builder = cparse_query_build_new();

    cparse_query_build_ne(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

void cparse_query_where_nin(cParseQuery *query, const char *key, cParseJson *inArray)
{
    cParseQueryBuilder *builder = NULL;

    if (query == NULL || cparse_str_empty(key) || inArray == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    builder = cparse_query_build_new();

    cparse_query_build_nin(builder, key, inArray);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

void cparse_query_where_exists(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    builder = cparse_query_build_new();

    cparse_query_build_exists(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

void cparse_query_where_select(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    builder = cparse_query_build_new();

    cparse_query_build_select(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

void cparse_query_where_nselect(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    builder = cparse_query_build_new();

    cparse_query_build_nselect(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

void cparse_query_where_all(cParseQuery *query, const char *key, cParseJson *value)
{
    cParseQueryBuilder *builder = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    builder = cparse_query_build_new();

    cparse_query_build_all(builder, key, value);

    cparse_query_build_where(query, builder);

    cparse_query_build_free(builder);
}

cParseQueryBuilder *cparse_query_build_new()
{
    cParseQueryBuilder *builder = malloc(sizeof(cParseQueryBuilder));

    if (builder == NULL) {
        cparse_log_errno(ENOMEM);
        return NULL;
    }

    builder->json = cparse_json_new();

    return builder;
}

cParseQueryBuilder *cparse_query_build_in(cParseQueryBuilder *query, const char *key, cParseJson *inArray)
{
    cParseJson *in = NULL;

    if (query == NULL || cparse_str_empty(key) || inArray == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    in = cparse_json_new();

    cparse_json_set(in, CPARSE_QUERY_IN, inArray);

    cparse_json_set(query->json, key, in);

    return query;
}

cParseQueryBuilder *cparse_query_build_lte(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *lte = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    lte = cparse_json_new();

    cparse_json_set(lte, CPARSE_QUERY_LESS_THAN_EQUAL, value);

    cparse_json_set(query->json, key, lte);

    return query;
}
cParseQueryBuilder *cparse_query_build_lt(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_LESS_THAN, value);

    cparse_json_set(query->json, key, inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_gte(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_GREATER_THAN_EQUAL, value);

    cparse_json_set(query->json, key, inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_gt(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_GREATER_THAN, value);

    cparse_json_set(query->json, key, inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_ne(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_NOT_EQUAL, value);

    cparse_json_set(query->json, key, inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_nin(cParseQueryBuilder *query, const char *key, cParseJson *inArray)
{
    cParseJson *inner = NULL;

    if (query == NULL || cparse_str_empty(key) || inArray == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_NOT_IN, inArray);

    cparse_json_set(query->json, key, inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_exists(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_EXISTS, value);

    cparse_json_set(query->json, key, inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_select(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_SELECT, value);

    cparse_json_set(query->json, key, inner);

    return query;
}

cParseQueryBuilder *cparse_query_build_nselect(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_DONT_SELECT, value);

    cparse_json_set(query->json, key, inner);

    return query;
}
cParseQueryBuilder *cparse_query_build_all(cParseQueryBuilder *query, const char *key, cParseJson *value)
{
    cParseJson *inner = NULL;

    if (query == NULL || cparse_str_empty(key) || value == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    inner = cparse_json_new();

    cparse_json_set(inner, CPARSE_QUERY_ALL, value);

    cparse_json_set(query->json, key, inner);

    return query;
}

cParseJson *cparse_query_build_json(cParseQueryBuilder *query)
{
    if (query == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }
    return query->json;
}

void cparse_query_build_free(cParseQueryBuilder *query)
{
    if (query == NULL) {
        return;
    }

    if (query->json) {
        cparse_json_free(query->json);
    }

    free(query);
}

cParseQueryBuilder *cparse_query_build_related_to(cParseQueryBuilder *query, const char *key, cParseObject *obj)
{
    cParseJson *inner = NULL;
    cParseJson *pointer = NULL;

    if (query == NULL || obj == NULL) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    inner = cparse_json_new();

    pointer = cparse_pointer_from_object(obj);

    cparse_json_set(inner, "object", pointer);

    cparse_json_set_string(inner, "key", key);

    cparse_json_set(query->json, CPARSE_QUERY_RELATED_TO, inner);

    return query;
}

