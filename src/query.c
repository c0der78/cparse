#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cparse/query.h>
#include <cparse/json.h>
#include "client.h"
#include "private.h"

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

CPARSE_QUERY *cparse_query_with_class_name(const char *className)
{
    CPARSE_QUERY *query = cparse_query_new();

    query->className = strdup(className);

    return query;
}


/* getters/setters */

size_t cparse_query_size(CPARSE_QUERY *query)
{
    return query->size;
}

CPARSE_OBJ *cparse_query_result(CPARSE_QUERY *query, size_t index)
{
    if (!query || !query->results || index > query->size)
        return NULL;

    return query->results[index];
}

bool cparse_query_find_objects(CPARSE_QUERY *query, CPARSE_ERROR **error)
{
    CPARSE_REQUEST *request;
    CPARSE_JSON *data;
    char buf[BUFSIZ + 1];

    /* build the request */
    snprintf(buf, BUFSIZ, "classes/%s", query->className);

    request = cparse_client_request_with_method_and_path(HTTPRequestMethodGet, buf);

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
        CPARSE_JSON *results = cparse_json_get(data, "results");

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

void cparse_query_cancel(CPARSE_QUERY *query)
{

}

int cparse_query_count_objects(CPARSE_QUERY *query, CPARSE_ERROR **error)
{
    return 0;
}

void cparse_query_set_where(CPARSE_QUERY *query, CPARSE_JSON *value)
{
    if (query->where)
        cparse_json_free(query->where);

    query->where = cparse_json_new_reference(value);
}


