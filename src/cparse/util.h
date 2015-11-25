/*!
 * @file
 * @header cParse Utilities
 * Various utility functions
 */
#ifndef CPARSE_UTIL_H

/* @parseOnly */
#define CPARSE_UTIL_H

#include <time.h>
#include <cparse/defines.h>

BEGIN_DECL


/*! tests if a string is null or empty
 * @param str the string to test
 * @return non-zero if the string is empty
 */
int cparse_str_empty(const char *str);

/*! parses a date time string
 * @param str the string to parse
 * @return a timestamp
 */
time_t cparse_date_time(const char *str);

/*! replaces an allocated string
 * @param a the string to replace
 * @param b the string to copy from
 */
void cparse_replace_str(char **a, const char *b);

/*! tests if a string starts with another
 * @param a the string to test
 * @param b the string to find
 * @return non-zero if b is a prefix of a
 */
int cparse_str_prefix(const char *a, const char *b);

/*! tests if two strings are equal
 * @param a the string to test
 * @param b the second string to test
 * @return non-zero if the strings are equal
 */
int cparse_str_cmp(const char *a, const char *b);

/*! appends a string to another
 * Note: this function deals with allocated strings only
 * @param pstr	a pointer to the string to append to
 * @param size  the size of string to append to
 * @param append the string to append
 * @param appendSize the size of the append string
 */
bool cparse_str_append(char **pstr, const char *append, size_t size);

/*! adds a reference to an object in json
 * @param data the json data
 * @param ref the object reference
 */
void cparse_json_add_reference(cParseJson *data, cParseObject *ref);

END_DECL

#endif
