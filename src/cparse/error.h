/*! \file error.h */
#ifndef cParseError_H_
#define cParseError_H_

#include <cparse/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! allocates a new empty error
 * \returns an allocated error
 */
cParseError *cparse_error_new();

/*!
 * allocates an error with a message
 * \returns the allocated error
 */
cParseError *cparse_error_with_message(const char *message);

/*! deallocates an error
 * \param error the error instance to deallocate
 */
void cparse_error_free(cParseError *error);

/*! gets the message for an error
 * \param error the error instance
 * \returns the error message or NULL
 */
const char *cparse_error_message(cParseError *error);

/*! sets the message for an error
 * \param error the error instance
 * \param message the message to set
 */
void cparse_error_set_message(cParseError *error, const char *message);

/*! gets the code for an error
 * \param error the error instance
 * \returns the error code or zero
 */
int cparse_error_code(cParseError *error);

/*! sets the code for an error
 * \param error the error instance
 * \param code the code to set
 */
void cparse_error_set_code(cParseError *error, int code);

#ifdef __cplusplus
}
#endif

#endif
