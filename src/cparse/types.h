/*! \file types.h */
#ifndef CPARSE_TYPES_H_
#define CPARSE_TYPES_H_

#include <cparse/defines.h>
#include <time.h>

BEGIN_DECL


cParseJson *cparse_pointer_from_object(cParseObject *obj);

bool cparse_json_is_pointer(cParseJson *json);

bool cparse_json_is_bytes(cParseJson *json);

bool cparse_json_is_file(cParseJson *json);

END_DECL

#endif
