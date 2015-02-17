/*! \file error.h */
#ifndef CPARSE_ERROR_H_
#define CPARSE_ERROR_H_

#include <cparse/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! allocates a new empty error
 * \returns an allocated error
 */
CPARSE_ERROR *cparse_error_new();

/*!
 * allocates an error with a message
 * \returns the allocated error
 */
CPARSE_ERROR *cparse_error_with_message(const char *message);

/*! deallocates an error
 * \param error the error instance to deallocate
 */
void cparse_error_free(CPARSE_ERROR *error);

/*! gets the message for an error
 * \param error the error instance
 * \returns the error message or NULL
 */
const char *cparse_error_message(CPARSE_ERROR *error);

/*! sets the message for an error
 * \param error the error instance
 * \param message the message to set
 */
void cparse_error_set_message(CPARSE_ERROR *error, const char *message);

/*! gets the code for an error
 * \param error the error instance
 * \returns the error code or zero
 */
int cparse_error_code(CPARSE_ERROR *error);

/*! sets the code for an error
 * \param error the error instance
 * \param code the code to set
 */
void cparse_error_set_code(CPARSE_ERROR *error, int code);

#ifdef __cplusplus
}
#endif

#endif
