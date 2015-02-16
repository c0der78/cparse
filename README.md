CParse
======

[![Build Status](https://travis-ci.org/c0der78/arg3json.svg?branch=master)](https://travis-ci.org/c0der78/arg3json)

A C library to use the REST API at [parse.com](http://parse.com).


Setup
=====
- Not required but [Homebrew](http://mxcl.github.com/homebrew/) is pretty handy for install libs
- The build system uses [Cmake](http://cmake.org) for cross platform compiling (OSX: brew install cmake)
- run 'cmake . && make' to compile the library and run the unit tests

Code style
==========
- function names: lower case underscored, prefixed with cparse namespace
- typedefs and defines: upper case underscored, prefixed with CPARSE namespace
- enums and variables: camel case

Dependencies
============

- libcurl for HTTP requests (should be already installed on OSX, otherwise 'brew install curl')
- libjson for JSON parsing ('brew install json')
- libcheck for unit testing ('brew install check')

Example
=======
```
CPARSE_OBJ *obj = cparse_object_with_class_name("Wizards");

cparse_object_set_string(obj, "name", "Harry Potter");

cparse_object_set_number(obj, "age", 24);

cparse_object_set_real(obj, "money", 102.34);

cparse_object_save(obj);

cparse_object_free(obj);

/* Voila, we have saved a Wizard object */

```

Background Operations
=====================
```
void my_nifty_callback(CPARSE_OBJ *obj, CPARSE_ERROR *error)
{
	if(error) {
		log(error);
		return;
	}

	do_some_other_action_with_object(obj);
}

CPARSE_OBJ *obj = cparse_object_with_class_name("Wizards");

cparse_object_set_string(obj, "name", "Harry Potter");

cparse_object_save_in_background(obj, my_nifty_callback);

```