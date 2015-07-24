
#include <cparse/role.h>

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

void cparse_role_add_user(cParseRole *role, cParseUser *user)
{
	cParseJson *users = cparse_object_get(role, "users");

	if (users == NULL) {
		users = cparse_json_new_array();
	}


}

bool cparse_role_save(cParseRole *role, )