#ifndef CPARSE_DATA_LIST_H_
#define CPARSE_DATA_LIST_H_

#include <cparse/defines.h>

typedef struct cparse_dlist cParseDataList;

/*! a simple key value linked list */
struct cparse_dlist {
    struct cparse_dlist *next;
    char *key;
    char *value;
};

BEGIN_DECL

cParseDataList *cparse_dlist_new();

void cparse_dlist_free(cParseDataList *list);

void cparse_dlist_remove(cParseDataList **list, bool (*funk)(cParseDataList *));

bool cparse_dlist_is_data(cParseDataList *data);

bool cparse_dlist_is_value(cParseDataList *data);

END_DECL

#endif
