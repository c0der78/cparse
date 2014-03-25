#include <cparse/types.h>
#include <cparse/json.h>
#include "protocol.h"
#include <string.h>

CPARSE_PTR *cparse_pointer_from_json(CPARSE_JSON *data)
{
    const char *type = cparse_json_get_string(data, KEY_TYPE);

    if (strcmp(type, TYPE_POINTER))
        return NULL;

    CPARSE_PTR *p = malloc(sizeof(CPARSE_PTR));

    p->className = strdup(cparse_json_get_string(data, KEY_CLASS_NAME));

    p->objectId = strdup(cparse_json_get_string(data, KEY_OBJECT_ID));

    return p;
}

CPARSE_JSON *cparse_pointer_to_json(CPARSE_PTR *p)
{
    CPARSE_JSON *data = cparse_json_new();

    cparse_json_set_string(data, KEY_CLASS_NAME, p->className);

    cparse_json_set_string(data, KEY_OBJECT_ID, p->objectId);

    cparse_json_set_string(data, KEY_TYPE, TYPE_POINTER);

    return data;
}

void cparse_pointer_free(CPARSE_PTR *p)
{
    if (p->className)
        free(p->className);

    if (p->objectId)
        free(p->objectId);

    free(p);
}


