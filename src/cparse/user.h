/*!
 * @file
 * @header cParse User
 * Functions for dealing with user objects
 */
#ifndef CPARSE_USER_H

/* @parseOnly */
#define CPARSE_USER_H

#include <cparse/defines.h>
#include <cparse/query.h>

BEGIN_DECL

/*! gets the currently logged in user
 * @return the current logged in user instance or NULL
 */
cParseUser *cparse_current_user(cParseError **error);

/*! allocates a new user object
 * @return the allocated user instance
 */
cParseUser *cparse_user_new();

/*! allocates a new user with a user name
 * @param username the username for the user
 * @return the allocated user instance
 */
cParseUser *cparse_user_with_name(const char *username);

/*! logs in as a user
 * @param username the username of the user
 * @param password the password for the user
 * @param error a pointer to an error that gets allocated if unsuccessful. can be NULL
 * @return the user object if successful, NULL otherwise
 */
cParseUser *cparse_user_login(const char *username, const char *password, cParseError **error);

/*! logs in as a user in the background
 * @param username the username of the user
 * @param password the password of the user
 * @param callback the callback issued if successful
 * @return an identifier for the background thread
 */
cparse_thread cparse_user_login_in_background(const char *username, const char *password, cParseObjectCallback callback,
                                              void *param);

/*! logs out the current user */
void cparse_user_logout();

/*! creates a new query for a user object
 * @return the query instance
 */
cParseQuery *cparse_user_query_new();

/*! Creates a new user on the backend
 * @param user the user instance
 * @param password the user's password
 * @param error a pointer to an error that will get allocated if unsuccessful. Can be NULL.
 * @return true if successful.
 */
bool cparse_user_sign_up(cParseUser *user, const char *password, cParseError **error);

/*! Creates a new user in the background
 * @param user the user instance
 * @param password the user's password
 * @param callback the callback issued after signing up
 * @return an identifier for the background thread
 */
cparse_thread cparse_user_sign_up_in_background(cParseUser *user, const char *password, cParseObjectCallback callback,
                                                void *param);

/* getters/setters */

/*! gets the user name
 * @param user the user instance
 * @return the name as a string or NULL
 */
const char *cparse_user_name(cParseUser *user);

/*! sets a user's name
 * @param user the user instance
 * @param value the string value
 */
void cparse_user_set_name(cParseUser *user, const char *value);

/*! gets a user's email
 * @param user the user instance
 * @return the email as a string or NULL
 */
const char *cparse_user_email(cParseUser *user);

/*! gets a user's session token
 * @param user the user instance
 * @return the session token as a string or NULL
 */
const char *cparse_user_session_token(cParseUser *user);

/*! validates a session token against the backend
 * @param sessionToken the session token to validate
 * @param error a pointer to an error that will get allocated if unsuccessful. Can be NULL.
 * @return true if successful
 */
cParseUser *cparse_user_validate(const char *sessionToken, cParseError **error);

/*! tests if the user has an 'emailVerified' parameter and its true. If the parameter
 * does not exists a refresh from the server will be attempted.
 * @param user the user instance
 * @param error a pointer to an error that will get allocated if unsuccessful. Can be NULL.
 * @return true if successful
 */
bool cparse_user_validate_email(cParseUser *user, cParseError **error);

/*! resets a user's password
 * @param user the user to reset
 * @param error the error to set on failure
 * @return true if the users password was reset
 */
bool cparse_user_reset_password(cParseUser *user, cParseError **error);

/*! resets a user's password in the background
 * @param user the user to reset
 * @param callback the optional callback function
 * @param param the optional user info for the callback function
 * @return a thread identifier
 */
cparse_thread cparse_user_reset_password_in_background(cParseUser *user, cParseObjectCallback callback, void *param);

/*! releases a user object from memory
 * @see cparse_object_free
 */
extern void (*cparse_user_free)(cParseUser *user);

/*! deletes a user
 * @see cparse_object_delete
 */
extern bool (*cparse_user_delete)(cParseUser *obj, cParseError **error);

/*! deletes a user in the background
 * @see cparse_object_delete_in_background
 */
extern cparse_thread (*cparse_user_delete_in_background)(cParseUser *obj, cParseObjectCallback callback, void *param);

/*! fetch a user
 * @see cparse_object_fetch
 */
extern bool (*cparse_user_fetch)(cParseUser *obj, cParseError **error);

/*! fetch a user in the background
 * @see cparse_object_fetch_in_background
 */
extern cparse_thread (*cparse_user_fetch_in_background)(cParseUser *obj, cParseObjectCallback callback, void *param);

/*! refresh a user
 * @see cparse_object_refresh
 */
extern bool (*cparse_user_refresh)(cParseUser *obj, cParseError **error);

/*! refresh a user in the background
 * @see cparse_object_refresh_in_background
 */
extern cparse_thread (*cparse_user_refresh_in_background)(cParseUser *user, cParseObjectCallback callback, void *param);


END_DECL

#endif
