/*!
 * @file
 * @header cParse Role
 * Functions for dealing with roles
 */
#ifndef CPARSE_ROLE_H

/*! @parseOnly */
#define CPARSE_ROLE_H

#include <cparse/defines.h>

BEGIN_DECL

/*! creates a role with a name
 * @param name the name of the role
 * @return the allocated role or NULL
 */
cParseRole *cparse_role_with_name(const char *name);

/*! get the name of a role
 * @param role the role to get from
 * @return the name of the role
 */
const char *cparse_role_name(cParseRole *role);

/*! queries the users for a role
 * @param role the role to query
 * @param error the error to set on failure
 * @return a query containing user objects
 */
cParseQuery *cparse_role_query_users(cParseRole *role, cParseError **error);

/*! query sub roles for a role
 * @param role the role to query
 * @param error the error to set on failure
 * @return a query containing role objects
 */
cParseQuery *cparse_role_query_roles(cParseRole *role, cParseError **error);

/*! adds a user to a role
 * @param role the role to add to
 * @param user the user to add
 */
void cparse_role_add_user(cParseRole *role, cParseUser *user);

/*! adds a sub role to a role
 * @param role the role to add to
 * @param other the sub role to add
 */
void cparse_role_add_role(cParseRole *role, cParseRole *other);

/*! function pointer to set a public acl for a role
 * @see cparse_object_set_public_acl
 */
extern void (*cparse_role_set_public_acl)(cParseRole *role, cParseAccess access, bool value);

/*! function pointer to set a user acl for a role
 * @see cparse_object_set_user_acl
 */
extern void (*cparse_role_set_user_acl)(cParseRole *role, cParseUser *user, cParseAccess access, bool value);

/*! function pointer to set a role acl for a role
 * @see cparse_object_set_user_acl
 */
extern void (*cparse_role_set_role_acl)(cParseRole *role, cParseRole *other, cParseAccess access, bool value);

/*! function pointer to free a role
 * @see cparse_object_free
 */
extern void (*cparse_role_free)(cParseRole *role);

/*! function pointer to save a role
 * @see cparse_object_save
 */
extern bool (*cparse_role_save)(cParseRole *role, cParseError **error);

/*! function pointer to delete a role
 * @see cparse_object_delete
 */
extern bool (*cparse_role_delete)(cParseRole *role, cParseError **error);

/*! function pointer to save a role in the background
 * @see cparse_object_save_in_background
 */
extern bool (*cparse_role_save_in_background)(cParseRole *role, cParseObjectCallback callback, void *param);

/*! function pointer to delete a role in the background
 * @see cparse_object_delete_in_background
 */
extern bool (*cparse_role_delete_in_background)(cParseRole *role, cParseObjectCallback callback, void *param);

END_DECL

#endif
