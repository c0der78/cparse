#include <check.h>
#include <stdio.h>
#include <cparse/object.h>
#include <cparse/parse.h>
#include <cparse/json.h>
#include <cparse/error.h>

#include "parse.test.h"

static void cparse_test_setup()
{
}

static void cparse_test_teardown()
{
    cparse_cleanup_test_objects();
}

START_TEST(test_cparse_object_save)
{
    CPARSE_OBJ *cp_obj = cparse_new_test_object("user1", 1234);

    fail_unless(cparse_save_test_object(cp_obj));

    fail_unless(cp_obj->objectId != NULL);
}
END_TEST

START_TEST(test_cparse_object_fetch)
{
    CPARSE_OBJ *cp_obj = cparse_new_test_object("user1", 1234);

    /* first create reference object */
    CPARSE_OBJ *obj = cparse_new_test_object("user2", 1444);

    fail_unless(cparse_save_test_object(obj));

    fail_unless(obj->objectId != NULL);

    /* now set refrence on other object */

    cparse_object_set_reference(cp_obj, "partner", obj);

    CPARSE_JSON *data = cparse_object_get(cp_obj, "partner");

    fail_unless(cparse_save_test_object(cp_obj));

    fail_unless(cparse_json_num_keys(data) == 3);

    /* now fetch the reference */

    CPARSE_ERROR *error = NULL;

    fail_unless(cparse_object_fetch(cp_obj, &error));

    fail_unless(error == NULL);

    data = cparse_object_get(cp_obj, "partner");

    fail_unless(cparse_json_num_keys(data) > 3);
}
END_TEST

void test_cparse_object_callback(CPARSE_OBJ *obj, CPARSE_ERROR *error)
{
    fail_unless(obj->objectId != NULL);
}

START_TEST(test_cparse_object_save_in_background)
{
    CPARSE_OBJ *cp_obj = cparse_new_test_object("user1", 1234);

    pthread_t thread = cparse_object_save_in_background(cp_obj, test_cparse_object_callback);

    pthread_join(thread, NULL); /* wait for thread */

    cparse_cleanup_test_object(cp_obj);
}
END_TEST


START_TEST(test_cparse_object_set_value)
{
    CPARSE_OBJ *cp_obj = cparse_object_with_class_name(TEST_CLASS);

    cparse_object_set_number(cp_obj, "score", 1234);

    fail_unless(cparse_object_attributes(cp_obj) == 1);

    fail_unless(cparse_object_get_number(cp_obj, "score", 0) == 1234);

    cparse_object_free(cp_obj);
}
END_TEST

START_TEST(test_cparse_object_count_attributes)
{
    CPARSE_OBJ *cp_obj = cparse_object_with_class_name(TEST_CLASS);

    cparse_object_set_string(cp_obj, "teststr", "1234");

    cparse_object_set_real(cp_obj, "testreal", 1234.5678);

    fail_unless(cparse_object_attributes(cp_obj) == 2);

    cparse_object_free(cp_obj);
}
END_TEST

START_TEST(test_cparse_object_remove_attribute)
{
    CPARSE_OBJ *cp_obj = cparse_object_with_class_name(TEST_CLASS);

    CPARSE_JSON *value = cparse_json_new_string("1234");

    cparse_object_set(cp_obj, "main", value);

    fail_unless(cparse_object_attributes(cp_obj) == 1);

    CPARSE_JSON *removed = cparse_object_remove(cp_obj, "main");

    fail_unless(removed == value);

    fail_unless(cparse_object_attributes(cp_obj) == 0);

    cparse_object_free(cp_obj);
}
END_TEST

START_TEST(test_cparse_object_to_json)
{
    const char *buf;
    CPARSE_OBJ *cp_obj = cparse_object_with_class_name(TEST_CLASS);

    cparse_object_set_string(cp_obj, "main", "Hello,World");

    buf = cparse_json_to_json_string(cp_obj->attributes);

    fail_unless(!strcmp(buf, "{\"main\":\"Hello,World\"}"));

    cparse_object_set_number(cp_obj, "main", 1234);

    buf = cparse_json_to_json_string(cp_obj->attributes);

    fail_unless(!strcmp(buf, "{\"main\":1234}"));

    cparse_object_free(cp_obj);
}
END_TEST

Suite *cparse_object_suite (void)
{
    Suite *s = suite_create ("Object");

    /* Core test case */
    TCase *tc = tcase_create ("Object");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_object_save);
    tcase_add_test(tc, test_cparse_object_set_value);
    tcase_add_test(tc, test_cparse_object_count_attributes);
    tcase_add_test(tc, test_cparse_object_remove_attribute);
    tcase_add_test(tc, test_cparse_object_to_json);
    tcase_add_test(tc, test_cparse_object_save_in_background);
    tcase_add_test(tc, test_cparse_object_fetch);
    suite_add_tcase(s, tc);

    return s;
}