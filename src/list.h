#ifndef CPARSE_LIST_H
#define CPARSE_LIST_H

#include <stdlib.h>
#include <string.h>
#include <cparse/defines.h>

typedef void (*cParseListFreeFn)(void *);

struct cparse_list_node
{
	void *data;
	struct cparse_list_node *next;
};

typedef struct cparse_list_node cParseListNode;

struct cparse_list
{
	cParseListNode *head;
	cParseListNode *tail;
	cParseListFreeFn free_fn;
	size_t node_size;
	size_t length;
};

typedef struct cparse_list cParseList;

BEGIN_DECL

cParseList *cparse_list_new(size_t node_size, cParseListFreeFn free);

void cparse_list_free(cParseList *list);

void cparse_list_prepend(cParseList *list, void *element);

void cparse_list_append(cParseList *list, void *element);

END_DECL

#endif
