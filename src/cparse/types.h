/*! \file types.h */
#ifndef CPARSE_TYPES_H_
#define CPARSE_TYPES_H_

#include <cparse/defines.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

cParsePointer *cparse_pointer_from_json(cParseJson *data);

cParseJson *cparse_pointer_to_json(cParsePointer *p);

void cparse_pointer_free(cParsePointer *pointer);

cParseBytes *cparse_bytes_from_json(cParseJson *data);

void cparse_bytes_free(cParseBytes *bytes);

cParseFile *cparse_file_from_json(cParseJson *data);

void cparse_file_free(cParseFile *file);


#ifdef __cplusplus
}
#endif

#endif
