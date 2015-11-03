/*!
 * @file
 * @header cParse Error
 * Functions for dealing with errors
 */
#ifndef CPARSE_ERROR_H

/*! @parseOnly */
#define CPARSE_ERROR_H

#include <cparse/defines.h>

BEGIN_DECL

/*!
 * allocates a new empty error
 * @return an allocated error or NULL
 */
cParseError *cparse_error_new();

/*!
 * allocates an error with a message
 * @param message the message for the error
 * @return the allocated error or NULL
 */
cParseError *cparse_error_with_message(const char *message);

/*!
 * allocates an error with a code and message
 * @param code the error code
 * @param message the error message
 * @return the allocated error
 */
cParseError *cparse_error_with_code_and_message(int code, const char *message);

/*!
 * deallocates an error
 * @param error the error instance to deallocate
 */
void cparse_error_free(cParseError *error);

/*!
 * gets the message for an error
 * @param error the error instance
 * @return the error message or NULL
 */
const char *cparse_error_message(cParseError *error);

/*!
 * sets the message for an error
 * @param error the error instance
 * @param message the message to set
 */
void cparse_error_set_message(cParseError *error, const char *message);

/*!
 * gets the code for an error
 * @param error the error instance
 * @return the error code or zero
 */
int cparse_error_code(cParseError *error);

/*!
 * sets the code for an error
 * @param error the error instance
 * @param code the code to set
 */
void cparse_error_set_code(cParseError *error, int code);

END_DECL

#endif
