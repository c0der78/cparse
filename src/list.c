
#include <cparse/list.h>
#include "log.h"
#include "private.h"
#include <errno.h>

cParseList *cparse_list_new(size_t elementSize, cParseListFreeFunk free)
{
	cParseList *list = malloc(sizeof(cParseList));

	if (list == NULL) {
		cparse_log_errno(ENOMEM);
		return NULL;
	}

	list->head = NULL;
	list->tail = NULL;
	list->free_fn = free;
	list->node_size = elementSize;
	list->length = 0;

	return list;
}

void cparse_list_free(cParseList *list)
{
	cParseListNode *node = NULL;
	cParseListNode *next_node = NULL;

	if (list == NULL) {
		return;
	}

	for (node = list->head; node != NULL; node = next_node)
	{
		next_node = node->next;

		if (list->free_fn) {
			list->free_fn(node->data);
		}

		free(node);
	}
	free(list);
}

void cparse_list_prepend(cParseList *list, void *element)
{
	cParseListNode *node = malloc(sizeof(cParseListNode));

	if (node == NULL) {
		cparse_log_errno(ENOMEM);
		return;
	}

	node->data = malloc(list->node_size);

	if (node->data == NULL) {
		cparse_log_errno(ENOMEM);
		return;
	}

	memcpy(node->data, element, list->node_size);

	node->next = list->head;
	list->head = node;

	if (!list->tail) {
		list->tail = list->head;
	}

	list->length++;
}

void cparse_list_append(cParseList *list, void *element)
{
	cParseListNode *node;

	if (list == NULL || element == NULL) {
		cparse_log_errno(EINVAL);
		return;
	}

	node = malloc(sizeof(cParseListNode));

	if (node == NULL) {
		cparse_log_errno(ENOMEM);
		return;
	}

	node->data = malloc(list->node_size);

	if (node->data == NULL) {
		cparse_log_errno(ENOMEM);
		return;
	}

	memcpy(node->data, element, list->node_size);

	if (list->tail == NULL) {
		list->head = list->tail = node;
	} else {
		list->tail->next = node;
		list->tail = node;
	}

	list->length++;
}

size_t cparse_list_size(cParseList *list) {
	return !list ? 0 : list->length;
}

void *cparse_list_get_index(cParseList *list, size_t index) {
	cParseListNode *node;
	size_t pos = 0;

	for (node = list->head; node != NULL; node = node->next, ++pos) {
		if (pos == index) {
			return node->data;
		}
	}
	return NULL;
}

cParseListNode *cparse_list_head(cParseList *list) {
	if (list == NULL) {
		cparse_log_errno(EINVAL);
		return NULL;
	}

	return list->head;
}

cParseListNode *cparse_list_tail(cParseList *list) {
	if (list == NULL) {
		cparse_log_errno(EINVAL);
		return NULL;
	}

	return list->tail;
}

cParseListNode *cparse_list_next(cParseListNode *node) {
	if (node == NULL) {
		cparse_log_errno(EINVAL);
		return NULL;
	}

	return node->next;
}

void *cparse_list_get(cParseListNode *node) {
	if (node == NULL) {
		cparse_log_errno(EINVAL);
		return NULL;
	}

	return node->data;
}

void cparse_list_foreach(cParseList *list, cParseIteratorFunk funk, void *param)
{
	size_t pos = 0;
	cParseListNode *node = NULL;

	if (list == NULL || funk == NULL) {
		cparse_log_errno(EINVAL);
		return;
	}

	for (node = list->head; node != NULL; node = node->next, ++pos) {
		funk(list, pos, node->data, param);
	}
}


