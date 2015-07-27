#ifndef CPARSE_LIST_H
#define CPARSE_LIST_H

#include <stdlib.h>
#include <string.h>
#include <cparse/defines.h>

typedef bool (*cParseIteratorFunk)(cParseList *, size_t index, void *data, void *param);

BEGIN_DECL

cParseList *cparse_list_new(size_t node_size, cParseListFreeFunk free);

void cparse_list_free(cParseList *list);

void cparse_list_prepend(cParseList *list, void *element);

void cparse_list_append(cParseList *list, void *element);

cParseListNode *cparse_list_head(cParseList *list);

cParseListNode *cparse_list_tail(cParseList *list);

cParseListNode *cparse_list_next(cParseListNode *node);

size_t cparse_list_size(cParseList *list);

void *cparse_list_get_index(cParseList *list, size_t index);

void *cparse_list_get(cParseListNode *node);

void cparse_list_foreach(cParseList *list, cParseIteratorFunk funk, void *param);

END_DECL

#define cparse_list_get_as(T, node)  ((T) cparse_list_get(node))

#endif
