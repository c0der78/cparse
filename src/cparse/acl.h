/*!
 * @file
 * @header cParse ACL
 * Functions dealing with object access control lists.
 */
#ifndef CPARSE_ACL_H
/*! @parseOnly */
#define CPARSE_ACL_H

#include <cparse/defines.h>

BEGIN_DECL

/*!
 * Copies one acl to another.
 * @param orig the acl to copy to
 * @param other the acl to copy from
 */
void cparse_acl_copy(cParseACL *orig, cParseACL *other);

/*!
 * Releases an acl from memory.
 * @param acl the acl to free
 */
void cparse_acl_free(cParseACL *acl);

/*!
 * Creates an acl object from its json representation.
 * @param json the json to read from
 * @return an allocated acl with the values from the json
 */
cParseACL *cparse_acl_from_json(cParseJson *json);

/*!
 * Turns an acl object into its json respresentation.
 * @param acl the acl to read from
 * @return an allocated json representation of the acl
 */
cParseJson *cparse_acl_to_json(cParseACL *acl);

/*!
 * tests for a readable property on the acl.
 * @param acl the acl to test
 * @param name the name of the read permission to test
 * @return true if the acl has a readable permission for the name
 */
bool cparse_acl_is_readable(cParseACL *acl, const char *name);

/*!
 * tests for a writable property on the acl.
 * @param acl the acl to test
 * @param name the name of the write permission to test
 * @return true if the acl has a writable permission for the name
 */
bool cparse_acl_is_writable(cParseACL *acl, const char *name);

/*!
 * sets a readable property on the acl.
 * @param acl the acl to set
 * @param name the name of the readable property to set
 * @param value the value of the readable permission
 */
void cparse_acl_set_readable(cParseACL *acl, const char *name, bool value);

/*!
 * sets a writable property on the acl.
 * @param acl the acl to set
 * @param name the name of the writable property to set
 * @param value the value of the writable permission
 */
void cparse_acl_set_writable(cParseACL *acl, const char *name, bool value);

END_DECL

#endif
