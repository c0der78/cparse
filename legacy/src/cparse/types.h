#ifndef CPARSE_TYPES_H_
#define CPARSE_TYPES_H_

#include <cparse/defines.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cparse_type_bytes
{
    void *data;
    size_t size;
};

struct cparse_type_date
{
    time_t value;
};

struct cparse_type_file
{
    char *localFileName;
    char *parseFileName;
    char *contentType;
    char *body;
    char *url;
};

struct cparse_type_geopoint
{
    double latitude;
    double longitude;
};

struct cparse_type_pointer
{
    char *className;
    char *objectId;
};

CPARSE_PTR *cparse_pointer_from_json(CPARSE_JSON *data);

CPARSE_JSON *cparse_pointer_to_json(CPARSE_PTR *p);

void cparse_pointer_free(CPARSE_PTR *pointer);

CPARSE_BYTES *cparse_bytes_from_json(CPARSE_JSON *data);

void cparse_bytes_free(CPARSE_BYTES *bytes);

CPARSE_FILE *cparse_file_from_json(CPARSE_JSON *data);

void cparse_file_free(CPARSE_FILE *file);


#ifdef __cplusplus
}
#endif

#endif
