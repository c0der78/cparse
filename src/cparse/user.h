/*! \file user.h */
#ifndef CPARSE_USER_H_
#define CPARSE_USER_H_

#include <cparse/defines.h>
#include <cparse/query.h>
#include <cparse/acl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! gets the currently logged in user
 * \returns the current logged in user instance or NULL
 */
CPARSE_OBJ *cparse_current_user();

/*! allocates a new user object
 * \returns the allocated user instance
 */
CPARSE_OBJ *cparse_user_new();

/*! allocates a new user with a user name
 * \param username the username for the user
 * \returns the allocated user instance
 */
CPARSE_OBJ *cparse_user_with_name(const char *username);

/*! logs in as a user
 * \param username the username of the user
 * \param password the password for the user
 * \param error a pointer to an error that gets allocated if unsuccessful. can be NULL
 * \returns the user object if successful, NULL otherwise
 */
CPARSE_OBJ *cparse_user_login(const char *username, const char *password, CPARSE_ERROR **error);

/*! logs in as a user in the background
 * \param username the username of the user
 * \param password the password of the user
 * \param callback the callback issued if successful
 * \returns an identifier for the background thread
 */
pthread_t cparse_user_login_in_background(const char *username, const char *password, CPARSE_OBJ_CALLBACK callback);

/*! logs out the current user */
void cparse_user_logout();

/*! deletes a user from the backend
 * \param user the user instance
 * \param error a pointer to an error that gets allocated if not successful. can be NULL
 * \returns true if successful
 */
bool cparse_user_delete(CPARSE_OBJ *user, CPARSE_ERROR **error);

/*! creates a new query for a user object
 * \returns the query instance
 */
CPARSE_QUERY *cparse_user_query();

/*! Creates a new user on the backend
 * \param user the user instance
 * \param password the user's password
 * \param error a pointer to an error that will get allocated if unsuccessful. Can be NULL.
 * \returns true if successful.
 */
bool cparse_user_sign_up(CPARSE_OBJ *user, const char *password, CPARSE_ERROR **error);

/* getters/setters */

/*! gets the user name
 * \params user the user instance
 * \returns the name as a string or NULL
 */
const char *cparse_user_name(CPARSE_OBJ *user);

/*! sets a user's name
 * \param user the user instance
 * \param value the string value
 */
void cparse_user_set_name(CPARSE_OBJ *user, char *value);

/*! gets a user's email
 * \param user the user instance
 * \returns the email as a string or NULL
 */
const char *cparse_user_email(CPARSE_OBJ *user);

/*! gets a user's session token
 * \param the user instance
 * \returns the session token as a string or NULL
 */
const char *cparse_user_session_token(CPARSE_OBJ *user);

/*! validates a session token against the backend
 * \param user the user to store the data in
 * \param sessionToken the session token to validate
 * \param error a pointer to an error that will get allocated if unsuccessful. Can be NULL.
 * \returns true if successful
 */
bool cparse_user_validate(CPARSE_OBJ *user, const char *sessionToken, CPARSE_ERROR **error);

/*! tests if the user has an 'emailVerified' parameter and its true. If the parameter
 * does not exists a refresh from the server will be attempted.
 * \param user the user instance
 * \param error a pointer to an error that will get allocated if unsuccessful. Can be NULL.
 * \returns true if successful
 */
bool cparse_user_validate_email(CPARSE_OBJ *user, CPARSE_ERROR **error);

bool cparse_user_refresh(CPARSE_OBJ *obj, CPARSE_ERROR **error);

bool cparse_user_fetch(CPARSE_OBJ *obj, CPARSE_ERROR **error);

bool cparse_user_reset_password(CPARSE_OBJ *user, CPARSE_ERROR **error);

bool cparse_user_delete(CPARSE_OBJ *obj, CPARSE_ERROR **error);

#ifdef __cplusplus
}
#endif

#endif
