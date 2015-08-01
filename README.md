cParse
======

[![Build Status](https://travis-ci.org/c0der78/arg3json.svg?branch=master)](https://travis-ci.org/c0der78/arg3json)

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/c0der78/cparse?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

[![Coverage Status](https://coveralls.io/repos/c0der78/cparse/badge.svg?branch=master&service=github)](https://coveralls.io/github/c0der78/cparse?branch=master)

A C library to use the REST API at [parse.com](http://parse.com).

[View Testing Code Coverage](http://htmlpreview.github.com/?https://github.com/c0der78/cparse/blob/master/coverage/index.html)

[View Documentation](http://htmlpreview.github.com/?https://github.com/c0der78/cparse/blob/master/html/files.html)

Setup
=====
- run `configure && make` to compile the library
- run `make test` to run the unit tests

The **unit tests** will require a parse.test.json file or the environment variables **PARSE_APP_ID** and **PARSE_API_KEY** set.

Dependencies
============

- libcurl for HTTP requests
- libjson (json-c) for JSON parsing
- pthreads
- check for unit testing
- lcov for code coverage

Usage
=====

Set the parse application and api keys with `cparse_set_application_id()` and `cparse_set_api_key()` and starting parsing your objects.

Examples
========
```C
cParseObject *obj = cparse_object_with_class_name("Wizards");

cParseError *error = NULL;

/* set some values */
cparse_object_set_string(obj, "name", "Harry Potter");

cparse_object_set_number(obj, "score", 24);

if(!cparse_object_save(obj, &error))
{
	puts(cparse_error_message(error));

	cparse_error_free(error);
}

cparse_object_free(obj);

/* Voila, we have saved a Wizard object */

```

Background Operations
=====================
```C
void magical_callback(cParseObject *obj, cParseError *error, void *userInfo)
{
	if(error) {
		puts(cparse_error_message(error));
		return;
	}

	do_some_other_action_with_object(obj, userInfo);
}

cParseObject *obj = cparse_object_with_class_name("Wizards");

int userInfo = MAGIC_NUM;

cparse_object_set_string(obj, "name", "Harry Potter");

cparse_object_save_in_background(obj, magical_callback, &userInfo);

```

Querying
========
```C
cParseQuery *query = cparse_query_with_class_name("Wizards");

cparse_query_where_gte(query, "score", cparse_json_new_number(24));

if(!cparse_query_find_objects(query, &error))
{
	puts(cparse_error_message(error));
	cparse_error_free(error);
	return;
}

/* get the second result */
if(cparse_query_size(query) > 1)
{
	cParseObject *obj = cparse_query_get_result(query, 1);
}
```

Building more complex Queries
=============================

A query builder structure can be used to combine multiple conditions.

```C
cParseQuery *query = cparse_query_with_class_name("Wizards");
cParseQueryBuilder *builder = cparse_query_build_new();
cParseJson *names = cparse_json_new_array();

/* query score >= 24 && score < 50 */
cparse_query_build_gte(builder, "score", cparse_json_new_number(24));
cparse_query_build_lt(builder, "score", cparse_json_new_number(50));

/* build an array of names */
cparse_json_add_string(names, "Harry");
cparse_json_add_string(names, "Dumbledore");

/* query by name in array */
cparse_query_build_in(builder, "name", names);

/* assign builder to the query */
cparse_query_build_where(query, builder);

/* free the builder */
cparse_query_build_free(builder);

if(!cparse_query_find_objects(query, &error))
{
	puts(cparse_error_message(error));
	cparse_error_free(error);
	return;
}
```

Users
=====
```C
cParseUser *user = cparse_user_with_name("Ronald");

if(!cparse_user_sign_up(user, "Password!", &error)) {
	puts(cparse_error_message(error));
	cparse_error_free(error);
	return;
}

cparse_user_free(user);

/* have a new user at this point */
user = cparse_user_login("Ronald", "Password!", &error);

if(user == NULL) {
	puts(cparse_error_message(error));
	cparse_error_free(error);
	return;
}

/* user is logged in at this point and can call cparse_current_user() */

loggedInUser = cparse_current_user();

```

Users are objects as well, so you can use any of the object functions if you want.

```C

cParseuser *user = cparse_user_with_name("Ronald");

cparse_object_set_string(user, "last_name", "Weasley");

cparse_object_set_number(user, "birth_year", 1980);

/* etc... */
```


Roles
=====

Roles define access control for a group of users and can inherit from other roles.


```C
cParseRole *role = cparse_role_with_name("Students");
cParseQuery *query = NULL;

/* add a user to the role */
cparse_role_add_user(role, user);

/* add an inherited role */
cparse_role_add_role(role, inherited_role);

/* create the Students role */
if (!cparse_role_save(role, &error)) {
	puts(cparse_error_message(error));
	cparse_error_free(error);
}

/* look up users associated with the role */
query = cparse_role_query_users(role);

if (cparse_query_size(query) > 0) {
	
}

/* look up role heirarchy */
query = cparse_role_query_roles(role);

if (cparse_query_size(query) > 0) {
	
}

```

Access Control
==============

Control read/write access for any object.  Access can for public, user or roles.

```C
cParseObject *obj = cparse_object_with_class_name("Wizards");

/* restrict public write access */
cparse_object_set_public_acl(obj, cParseAccessWrite, false);

/* allow write access for a specific user */
cparse_object_set_user_acl(obj, user, cParseAccessWrite, true);

/* allow write access for users in a role */
cparse_object_set_role_acl(obj, role, cParseAccessWrite, true);
```


TODO
====

- finish query api (arrays, strings, relational, compound, geo)
- implement batch operations
- finish users api (linking, pwd reset, verify email)
- sessions
- files
- c++ wrapper