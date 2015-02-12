#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cparse/query.h>
#include <cparse/json.h>
#include "client.h"

struct cparse_query_builder
{
    CPARSE_QUERY *query;
    CPARSE_JSON *value;
};

void cparse_query_clear_all_caches()
{

}

CPARSE_QUERY *cparse_query_new()
{
    CPARSE_QUERY *query = malloc(sizeof(CPARSE_QUERY));

    /*query->cachePolicy = kCParseCachePolicyIgnoreCache;*/
    query->className = NULL;
    query->limit = 0;
    query->skip = 0;
    query->trace = false;
    query->where = NULL;
    query->results = NULL;
    query->keys = NULL;
    query->count = false;

    return query;
}

CPARSE_QUERY_BUILDER *cparse_query_builder_new(CPARSE_QUERY *query)
{
    CPARSE_QUERY_BUILDER *builder = malloc(sizeof(CPARSE_QUERY_BUILDER));

    builder->query = query;
    builder->value = cparse_json_new();

    return builder;
}

void cparse_query_free(CPARSE_QUERY *query)
{
    if (query->className)
        free(query->className);

    if (query->keys)
        free(query->keys);

    if (query->results)
    {
        /* note: don't delete the objects, just the array */
        free(query->results);
    }

    if (query->where)
        cparse_json_free(query->where);

    free(query);
}

void cparse_query_builder_free(CPARSE_QUERY_BUILDER *value)
{
    if (value)
    {
        cparse_json_free(value->value);

        free(value);
    }
}

CPARSE_QUERY *cparse_query_with_class_name(const char *className)
{
    CPARSE_QUERY *query = cparse_query_new();

    query->className = strdup(className);

    return query;
}

bool cparse_query_find_objects(CPARSE_QUERY *query, CPARSE_ERROR **error)
{
    CPARSE_CLIENT_REQ *request;
    CPARSE_JSON *data;
    char buf[BUFSIZ + 1];
    char params[BUFSIZ + 1] = {0};
    int pos;

    request = cparse_client_request_new();

    /* build the request */
    snprintf(buf, BUFSIZ, "classes/%s", query->className);

    request->path = strdup(buf);

    request->method = HTTPRequestMethodGet;

    pos = 0;

    if (query->where)
    {
        pos = snprintf(&params[pos], BUFSIZ, "where=%s\n", cparse_json_to_json_string(query->where));
    }

    if (query->limit > 0)
    {
        pos = snprintf(&params[pos], BUFSIZ, "limit=%d\n", query->limit);
    }

    if (query->skip > 0)
    {
        pos = snprintf(&params[pos], BUFSIZ, "skip=%d\n", query->skip);
    }

    if (query->keys)
    {
        pos = snprintf(&params[pos], BUFSIZ, "keys=%s\n", query->keys);
    }

    if (query->count)
    {
        pos = snprintf(&params[pos], BUFSIZ, "count=1\n");
    }

    if (params[0])
    {
        request->payload = strdup(params);
    }

    /* do the deed */
    data = cparse_client_request_get_json(request, error);

    cparse_client_request_free(request);

    if (error != NULL && *error != NULL)
    {
        cparse_json_free(data);

        return false;
    }

    if (query->count)
    {
        query->size = cparse_json_get_number(data, "count", 0);
    }
    else
    {
        CPARSE_JSON *results = cparse_json_get(data, "results");

        query->size = cparse_json_array_size(results);

        if (query->size > 0)
        {
            if (query->results)
            {
                /* fine, go ahead and cleanup */
                free(query->results);
            }
            query->results = malloc(cparse_object_sizeof() * query->size);

            for (int i = 0; i < query->size; i++)
            {
                query->results[i] = cparse_object_with_class_data(query->className, cparse_json_array_get(results, i));
            }
        }
    }

    cparse_json_free(data);

    return true;
}

void cparse_query_cancel(CPARSE_QUERY *query)
{

}

/*void cparse_query_clear_cache(CPARSE_QUERY *query)
{

}*/

int cparse_query_count_objects(CPARSE_QUERY *query, CPARSE_ERROR **error)
{
    return 0;
}

void cparse_query_builder_in_array(CPARSE_QUERY_BUILDER *builder, CPARSE_JSON *value)
{
    cparse_json_set(builder->value, CPARSE_QUERY_IN, value);
}

void cparse_query_where(CPARSE_QUERY *query, const char *key, CPARSE_QUERY_BUILDER *builder)
{
    if (query->where)
        cparse_json_free(query->where);

    query->where = cparse_json_new();

    cparse_json_set(query->where, key, builder->value);
}
