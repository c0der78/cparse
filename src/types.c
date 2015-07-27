#include <cparse/types.h>
#include <cparse/json.h>
#include <cparse/util.h>
#include <errno.h>
#include "protocol.h"
#include "log.h"
#include <string.h>
#include "private.h"


cParseJson *cparse_pointer_from_object(cParseObject *obj)
{
    cParseJson *data = NULL;

    if (obj == NULL || cparse_str_empty(obj->className) || cparse_str_empty(obj->objectId)) {
        cparse_log_errno(EINVAL);
        return NULL;
    }

    data = cparse_json_new();

    cparse_json_set_string(data, CPARSE_KEY_CLASS_NAME, obj->className);

    cparse_json_set_string(data, CPARSE_KEY_OBJECT_ID, obj->objectId);

    cparse_json_set_string(data, CPARSE_KEY_TYPE, CPARSE_TYPE_POINTER);

    return data;
}

bool cparse_json_is_pointer(cParseJson *json)
{
    if (json == NULL) {
        return false;
    }

    if (!cparse_json_contains(json, CPARSE_KEY_TYPE)) {
        return false;
    }

    if (!cparse_str_cmp(cparse_json_get_string(json, CPARSE_KEY_TYPE), CPARSE_TYPE_POINTER)) {
        return false;
    }

    return true;
}



