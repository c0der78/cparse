#include <cparse/object.h>
#include <cparse/role.h>
#include <cparse/json.h>
#include <errno.h>
#include <cparse/util.h>
#include "protocol.h"
#include "log.h"
#include "private.h"

const char *const CPARSE_ROLE_CLASS_NAME = "roles";

extern cParseUser *cparse_object_new();

void (*cparse_role_set_public_acl)(cParseRole *role, bool read, bool write) = cparse_object_set_public_acl;

void (*cparse_role_set_user_acl)(cParseRole *role, cParseUser *user, bool read, bool write) = cparse_object_set_user_acl;

void (*cparse_role_set_role_acl)(cParseRole *role, cParseRole *other, bool read, bool write) = cparse_object_set_role_acl;

void (*cparse_role_free)(cParseRole *role) = cparse_object_free;

bool (*cparse_role_save)(cParseRole *role, cParseError **error) = cparse_object_save;

bool (*cparse_role_delete)(cParseRole *role, cParseError **error) = cparse_object_delete;

cParseRole *cparse_role_with_name(const char *name) {
	cParseRole *obj = cparse_object_new();

	obj->className = strdup(CPARSE_ROLE_CLASS_NAME);

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

static void __cparse_role_add_dependency(cParseRole *role, const char *key, const char *pointerType, const char *id)
{
	cParseJson *users = NULL;
	cParseJson *objects = NULL;
	cParseJson *pointer = NULL;

	if (!role || cparse_str_empty(id) || cparse_str_empty(key) ||
	        (strcmp(key, CPARSE_USER_CLASS_NAME) && strcmp(key, CPARSE_ROLE_CLASS_NAME)) ||
	        (strcmp(pointerType, CPARSE_CLASS_USER) && strcmp(pointerType, CPARSE_CLASS_ROLE))) {
		cparse_log_errno(EINVAL);
		return;
	}

	users = cparse_object_get(role, key);

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

	cparse_json_set_string(pointer, CPARSE_KEY_TYPE, CPARSE_TYPE_POINTER);
	cparse_json_set_string(pointer, CPARSE_KEY_CLASS_NAME, pointerType);
	cparse_json_set_string(pointer, CPARSE_KEY_OBJECT_ID, id);

	cparse_json_array_add(objects, pointer);

	cparse_json_free(pointer);

	cparse_json_set(users, "objects", objects);

	cparse_json_free(objects);

	cparse_object_set(role, key, users);

	cparse_json_free(users);
}


void cparse_role_add_user(cParseRole *role, cParseUser *user) {
	if (!role || !user) {
		cparse_log_errno(EINVAL);
		return;
	}

	__cparse_role_add_dependency(role, CPARSE_USER_CLASS_NAME, CPARSE_CLASS_USER, user->objectId);
}

void cparse_role_add_role(cParseRole *role, cParseRole *other) {
	if (!role || !other) {
		cparse_log_errno(EINVAL);
		return;
	}

	__cparse_role_add_dependency(role, CPARSE_ROLE_CLASS_NAME, CPARSE_CLASS_ROLE, other->objectId);
}

