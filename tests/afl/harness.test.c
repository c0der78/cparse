#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cparse/object.h>
#include <cparse/error.h>
#include "config.test.h"

#define MAX_REL_ERR 0.000001

cParseObject *obj = NULL;
cParseError *error = NULL;

void harness_cleanup() {
	if (obj != NULL) {
		cparse_object_free(obj);
	}
	if (error != NULL) {
		cparse_error_free(error);
	}
	cleanup();
}

int harness_compare_string(const char *key, const char *value);
int harness_compare_real(const char *key, double value1, double maxRelativeError);
int harness_compare_bool(const char *key, bool value);
int harness_compare_number(const char *key, cParseNumber value);

int main(int argc, char *argv[])
{
	char *buffer = NULL;
	size_t bufsiz = 0;
	ssize_t bytes_read = 0;
	FILE *stream = stdin;
	
	union {
		cParseNumber num;
		double real;
		char str[2048];
	} var;

	puts("AFL test harness");
	
	read_env_config();

	read_test_config();
	
	if (!is_valid_config()) {
		die("test configuration is not valid");
	}
	obj = cparse_object_with_class_name("Fuzzer");	

	atexit(harness_cleanup);

	while((bytes_read = getline(&buffer, &bufsiz, stream)) >= 0) {
		char key[128];
		size_t len = strlen(buffer);

		if (len > 0) {
			buffer[len-1] = '\0';
		}

		if (buffer[0] == '\0') {
			break;
		}

		if (sscanf(buffer, "%s = %d", key, &var.num) == 2) {
			cparse_object_set_number(obj, key, var.num);
		} else if (sscanf(buffer, "%s = %lf", key, &var.real) == 2) {
			cparse_object_set_real(obj, key, var.num);
		} else if (sscanf(buffer, "%s = %[^\n]", key, var.str) == 2) {
			cparse_object_set_string(obj, key, var.str);
		} else if (sscanf(buffer, "%s = %s", key, var.str) == 2 && !strcmp(var.str, "true")) {
			cparse_object_set_bool(obj, key, true);
		} else if (sscanf(buffer, "%s = %s", key, var.str) == 2 && !strcmp(var.str, "false")) {
			cparse_object_set_bool(obj, key, false);
		} else if (sscanf(buffer, "%s == %[^\n]", key, var.str) == 2) {
			if (harness_compare_string(key, var.str)) {
				exit(1);
			}
		} else if (sscanf(buffer, "%s != %[^\n]", key, var.str) == 2) {
			if (!harness_compare_string(key, var.str)) {
				exit(1);
			}
		} else if (sscanf(buffer, "%s == %d", key, &var.num) == 2) {
			if (harness_compare_number(key, var.num)) {
				exit(1);
			}
		} else if (sscanf(buffer, "%s != %d", key, &var.num) == 2) {
			if (!harness_compare_number(key, var.num)) {
				exit(1);
			}
		} else if (sscanf(buffer, "%s == %lf", key, &var.real) == 2) {
			if (harness_compare_real(key, var.real, MAX_REL_ERR)) {
				exit(1);
			}
		} else if (sscanf(buffer, "%s != %lf", key, &var.real) == 2) {
			if (!harness_compare_real(key, var.real, MAX_REL_ERR)) {
				exit(1);
			}
		} else if ((sscanf(buffer, "%s == %s", key, var.str) == 2 && !strcmp(var.str, "true"))  
				|| (sscanf(buffer, "%s != %s", key, var.str) == 2 && !strcmp(var.str, "false"))) {
			if (harness_compare_bool(key, true)) {
				exit(1);
			}
		} else if ((sscanf(buffer, "%s == %s", key, var.str) == 2 && !strcmp(var.str, "false"))
				|| (sscanf(buffer, "%s != %s", key, var.str) == 2 && !strcmp(var.str, "true"))) {
			if (harness_compare_bool(key, false)) {
				exit(1);
			}
		} else if (sscanf(buffer, "rem %s", key) == 1) {
			if (!cparse_object_contains(obj, key)) {
				printf("object does not contain key (%s)\n", key);
			} else {
				cparse_object_remove(obj, key);
			}
		} else if (!strcmp(buffer, "save")) {
			if (!cparse_object_save(obj, &error)) {
				puts(cparse_error_message(error));
				cparse_error_free(error);
				error = NULL;
			}
		} else if (!strcmp(buffer, "delete")) {
			if (!cparse_object_delete(obj, &error)) {
				puts(cparse_error_message(error));
				cparse_error_free(error);
				error = NULL;
			}
		} else if (!strcmp(buffer, "refresh")) {
			if (!cparse_object_refresh(obj, &error)) {
				puts(cparse_error_message(error));
				cparse_error_free(error);
				error = NULL;
			}
		} else if (!strcmp(buffer, "fetch")) {
			if (!cparse_object_fetch(obj, &error)) {
				puts(cparse_error_message(error));
				cparse_error_free(error);
				error = NULL;
			}
		} else {
			printf("No op (%s) in AFL harness\n", buffer);
		}
	}

	return 0;
}

int harness_compare_string(const char *key, const char *value1) {
	const char *value2 = NULL;
	
	if (key == NULL || value1 == NULL) {
		return 1;
	}

	value2 = cparse_object_get_string(obj, key);

	if (value2 == NULL) {
		return 1;
	}

	return strcmp(value1, value2); 
}

int harness_compare_real(const char *key, double value1, double maxRelativeError) {
	double value2 = NAN;

	if (key == NULL || isnan(value1)) {
		return 1;
	}

	value2 = cparse_object_get_real(obj, key, NAN);
	
	if (isnan(value2)) {
		return 1;
	}
	if ( value1 == value2) {
		return 0;
	}

	double relativeError = fabs((value1 - value2) / value2);

	if (relativeError <= maxRelativeError) {
		return 0;
	}

	return 1;
}

int harness_compare_bool(const char *key, bool value) {
	bool value2 = false;

	if (key == NULL) {
		return 1;
	}

	value2 = cparse_object_get_bool(obj, key);

	return value == value2 ? 0 : 1;
}

int harness_compare_number(const char *key, cParseNumber value1)
{
	cParseNumber value2 = 0;

	if (key == NULL) {
		return 1;
	}

	value2 = cparse_object_get_number(obj, key, -1);

	return value1 == value2 ? 0 : 1;
}

