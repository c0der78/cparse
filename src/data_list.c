#include <stdlib.h>
#include <errno.h>
#include "data_list.h"
#include "log.h"

/*! allocates a new key value list */
cParseDataList *cparse_dlist_new()
{
    cParseDataList *list = malloc(sizeof(cParseDataList));

    if (list == NULL) {
        cparse_log_errno(ENOMEM);
        return NULL;
    }

    list->next = NULL;
    list->key = NULL;
    list->value = NULL;
    return list;
}

/*! deallocates a key value list */
void cparse_dlist_free(cParseDataList *list)
{
    if (!list) {
        return;
    }

    if (list->key) {
        free(list->key);
    }
    if (list->value) {
        free(list->value);
    }
    free(list);
}

void cparse_dlist_remove(cParseDataList **list, bool (*funk)(cParseDataList *))
{
    cParseDataList *data = NULL, *prev_data = NULL, *next_data = NULL;

    if (list == NULL) {
        cparse_log_errno(EINVAL);
        return;
    }

    for (prev_data = NULL, data = *list, next_data = NULL; data != NULL; data = next_data) {
        next_data = data->next;

        /* body has no key */
        if (funk(data)) {
            if (prev_data) {
                prev_data->next = data->next;
            } else {
                *list = data->next;
            }
            cparse_dlist_free(data);
        }

        prev_data = data;
    }

    return;
}

bool cparse_dlist_is_value(cParseDataList *data)
{
    return data->key == NULL;
}

bool cparse_dlist_is_data(cParseDataList *data)
{
    return data->key != NULL;
}
