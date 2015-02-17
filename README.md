cParse
======

[![Build Status](https://travis-ci.org/c0der78/arg3json.svg?branch=master)](https://travis-ci.org/c0der78/arg3json)

A C library to use the REST API at [parse.com](http://parse.com).

[View Testing Code Coverage](http://htmlpreview.github.com/?https://github.com/c0der78/cparse/blob/master/coverage/index.html)


Setup
=====
- run 'configure && make' to compile the library

Code style
==========
- function names: lower case underscored, prefixed with cparse namespace
- typedefs and defines: upper case underscored, prefixed with CPARSE namespace
- enums and variables: camel case

Dependencies
============

- libcurl for HTTP requests
- libjson (json-c) for JSON parsing
- check for unit testing


Configuration
=============

Its up to the developer to set the parse application and api keys with `cparse_set_application_id()` and `cparse_set_api_key()`

Example
=======
```
CPARSE_OBJ *obj = cparse_object_with_class_name("Wizards");

cparse_object_set_string(obj, "name", "Harry Potter");

cparse_object_set_number(obj, "score", 24);

cparse_object_save(obj);

cparse_object_free(obj);

/* Voila, we have saved a Wizard object */

```

Background Operations
=====================
```
void my_nifty_callback(CPARSE_OBJ *obj, bool success, CPARSE_ERROR *error)
{
	if(!success) {
		log(error);
		return;
	}

	do_some_other_action_with_object(obj);
}

CPARSE_OBJ *obj = cparse_object_with_class_name("Wizards");

cparse_object_set_string(obj, "name", "Harry Potter");

cparse_object_save_in_background(obj, my_nifty_callback);

```