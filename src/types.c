#include <cparse/types.h>
#include <cparse/json.h>
#include "protocol.h"
#include <string.h>
#include "private.h"

cParsePointer *cparse_pointer_from_json(cParseJson *data)
{
    cParsePointer *p = NULL;
    const char *type = NULL;

    if (data == NULL) { return NULL; }

    type = cparse_json_get_string(data, CPARSE_KEY_TYPE);

    if (strcmp(type, CPARSE_TYPE_POINTER)) {
        return NULL;
    }

    p = malloc(sizeof(cParsePointer));

    p->className = strdup(cparse_json_get_string(data, CPARSE_KEY_CLASS_NAME));

    p->objectId = strdup(cparse_json_get_string(data, CPARSE_KEY_OBJECT_ID));

    return p;
}

cParseJson *cparse_pointer_to_json(cParsePointer *p)
{
    cParseJson *data = cparse_json_new();

    cparse_json_set_string(data, CPARSE_KEY_CLASS_NAME, p->className);

    cparse_json_set_string(data, CPARSE_KEY_OBJECT_ID, p->objectId);

    cparse_json_set_string(data, CPARSE_KEY_TYPE, CPARSE_TYPE_POINTER);

    return data;
}

void cparse_pointer_free(cParsePointer *p)
{
    if (p->className) {
        free(p->className);
    }

    if (p->objectId) {
        free(p->objectId);
    }

    free(p);
}


