#include <check.h>
#include <stdio.h>
#include <cparse/object.h>
#include <cparse/parse.h>
#include <cparse/json.h>
#include <cparse/error.h>

CPARSE_OBJ *cp_obj = NULL;

static void cparse_test_setup()
{
    cp_obj = cparse_object_with_class_name("TestCase");
}

static void cparse_test_teardown()
{
    cparse_object_delete(cp_obj, NULL);

    cparse_object_free(cp_obj);
}

START_TEST(test_cparse_object_save)
{

    cparse_object_set_number(cp_obj, "score", 1234);

    cparse_object_set_string(cp_obj, "name", "spok");

    CPARSE_ERROR *error = NULL;

    cparse_object_save(cp_obj, &error);

    if (error != NULL)
        printf("%s", error->message);

    fail_unless(error == NULL);

    fail_unless(cp_obj->objectId != NULL);
}
END_TEST

START_TEST(test_cparse_object_fetch)
{
    /* first create reference object */
    CPARSE_OBJ *obj = cparse_object_with_class_name("TestCase");

    cparse_object_set_number(obj, "score", 1234);

    cparse_object_set_string(obj, "name", "spok");

    CPARSE_ERROR *error = NULL;

    cparse_object_save(obj, &error);

    fail_unless(error == NULL);

    fail_unless(obj->objectId != NULL);

    /* now set refrence on other object */

    cparse_object_set_reference(cp_obj, "inner", obj);

    CPARSE_JSON *data = cparse_object_get(cp_obj, "inner");

    cparse_object_save(cp_obj, &error);

    fail_unless(error == NULL);

    fail_unless(cparse_json_num_keys(data) == 3);

    /* now fetch the reference */

    cparse_object_fetch(cp_obj, &error);

    fail_unless(error == NULL);

    data = cparse_object_get(cp_obj, "inner");

    fail_unless(cparse_json_num_keys(data) > 3);

    cparse_object_delete(obj, NULL);
}
END_TEST

void test_cparse_object_callback(CPARSE_OBJ *obj, CPARSE_ERROR *error)
{
    fail_unless(obj->objectId != NULL);
}

START_TEST(test_cparse_object_save_in_background)
{
    cparse_object_set_number(cp_obj, "score", 4567);

    cparse_object_set_string(cp_obj, "status", "saved in background");

    pthread_t thread = cparse_object_save_in_background(cp_obj, test_cparse_object_callback);

    pthread_join(thread, NULL); /* wait for thread */
}
END_TEST


START_TEST(test_cparse_object_set_value)
{
    cparse_object_set_number(cp_obj, "id", 1234);

    fail_unless(cparse_object_attributes(cp_obj) == 1);

    fail_unless(cparse_object_get_number(cp_obj, "id") == 1234);
}
END_TEST

START_TEST(test_cparse_object_count_attributes)
{
    cparse_object_set_string(cp_obj, "teststr", "1234");

    cparse_object_set_real(cp_obj, "testreal", 1234.5678);

    fail_unless(cparse_object_attributes(cp_obj) == 2);
}
END_TEST

START_TEST(test_cparse_object_remove_attribute)
{
    CPARSE_JSON *value = cparse_json_new_string("1234");

    cparse_object_set(cp_obj, "main", value);

    fail_unless(cparse_object_attributes(cp_obj) == 1);

    CPARSE_JSON *removed = cparse_object_remove(cp_obj, "main");

    fail_unless(removed == value);

    fail_unless(cparse_object_attributes(cp_obj) == 0);

    cparse_json_free(value);
}
END_TEST

START_TEST(test_cparse_object_to_json)
{
    const char *buf;

    cparse_object_set_string(cp_obj, "main", "Hello,World");

    buf = cparse_json_to_json_string(cp_obj->attributes);

    fail_unless(!strcmp(buf, "{\"main\":\"Hello,World\"}"));

    cparse_object_set_number(cp_obj, "main", 1234);

    buf = cparse_json_to_json_string(cp_obj->attributes);

    fail_unless(!strcmp(buf, "{\"main\":1234}"));
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