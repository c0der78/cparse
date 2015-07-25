#include <cparse/object.h>
#include <cparse/role.h>
#include <cparse/json.h>
#include <errno.h>
#include <cparse/util.h>
#include "protocol.h"
#include "log.h"
#include "private.h"

const char *const CPARSE_ROLE_CLASS_NAME = "roles";

void (*cparse_role_set_public_acl)(cParseRole *role, bool read, bool write) = cparse_object_set_public_acl;

void (*cparse_role_set_user_acl)(cParseRole *role, cParseUser *user, bool read, bool write) = cparse_object_set_user_acl;

void (*cparse_role_set_role_acl)(cParseRole *role, const char *name, bool read, bool write) = cparse_object_set_role_acl;

void (*cparse_role_free)(cParseRole *role) = cparse_object_free;

bool (*cparse_role_save)(cParseRole *role, cParseError **error) = cparse_object_save;

cParseRole *cparse_role_with_name(const char *name) {
	cParseRole *obj = cparse_object_with_class_name(CPARSE_ROLE_CLASS_NAME);

	cparse_object_set_string(obj, CPARSE_KEY_NAME, name);

	return obj;
}

const char *cparse_role_name(cParseRole *role) {
	return cparse_object_get_string(role, "name");
}

cParseJson *cparse_role_users(cParseRole *role) {
	return cparse_object_get(role, "users");
}

cParseJson *cparse_role_roles(cParseRole *role) {
	return cparse_object_get(role, "roles");
}

void cparse_role_add_user_id(cParseRole *role, const char *userId)
{
	cParseJson *users = NULL;
	cParseJson *objects = NULL;
	cParseJson *pointer = NULL;

	if (!role || cparse_str_empty(userId)) {
		cparse_log_errno(EINVAL);
		return;
	}

	users = cparse_object_get(role, "users");

	if (users == NULL) {
		users = cparse_json_new();
		cparse_json_set_string(users, CPARSE_KEY_OP, CPARSE_KEY_ADD_RELATION);
	}

	objects = cparse_json_get(users, "objects");

	if (objects == NULL) {
		objects = cparse_json_new_array();
	}

	if (!cparse_json_is_array(objects)) {
		cparse_log_error("objects is not an array");
		cparse_json_free(users);
		cparse_json_free(objects);
		return;
	}

	pointer = cparse_json_new();

	cparse_json_set_string(pointer, CPARSE_KEY_OP, CPARSE_TYPE_POINTER);
	cparse_json_set_string(pointer, CPARSE_KEY_CLASS_NAME, CPARSE_CLASS_USER);
	cparse_json_set_string(pointer, CPARSE_KEY_OBJECT_ID, userId);

	cparse_json_array_add(objects, pointer);

	cparse_json_free(pointer);

	cparse_json_set(users, "objects", objects);

	cparse_json_free(objects);

	cparse_object_set(role, "users", users);

	cparse_json_free(users);
}


void cparse_role_add_user(cParseRole *role, cParseUser *user) {
	if (!role || !user) {
		cparse_log_errno(EINVAL);
		return;
	}

	cparse_role_add_user_id(role, user->objectId);
}

