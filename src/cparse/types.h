/*!
 * @file
 * @header cParse Types
 * Functions for dealing with json representation types
 */
#ifndef CPARSE_TYPES_H

/* @parseOnly */
#define CPARSE_TYPES_H

#include <cparse/defines.h>
#include <time.h>

BEGIN_DECL

/*! creates a pointer json representation from an object
 * @param obj the object to read from
 * @return a json representation of a pointer to an object
 */
cParseJson *cparse_pointer_from_object(cParseObject *obj);

/*! tests if a json object is a pointer representation
 * @param json the json to test
 * @return true if the json is a pointer representation
 */
bool cparse_json_is_pointer(cParseJson *json);

/*! tests if a json object is a byte array
 * @param json the json to test
 * @return true if the json represents a byte array
 */
bool cparse_json_is_bytes(cParseJson *json);

/*! tests if a json object is a file
 * @param json the json to test
 * @return true if the json represents a file
 */
bool cparse_json_is_file(cParseJson *json);

END_DECL

#endif
