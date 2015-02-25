#include <cparse/json.h>
#include <cparse/object.h>
#include <check.h>

static cParseJson *cpv_test;

static void cparse_test_setup()
{
    cpv_test = cparse_json_new();
}

static void cparse_test_teardown()
{
    cparse_json_free(cpv_test);
}

START_TEST(test_cparse_json_set_number)
{
    cparse_json_set_number(cpv_test, "test", 1234);

    fail_unless(cparse_json_get_number(cpv_test, "test", 0) == 1234);
}
END_TEST

START_TEST(test_cparse_json_set_real)
{
    cparse_json_set_real(cpv_test, "test", 1234.4321);

    fail_unless(cparse_json_get_real(cpv_test, "test", 0) == 1234.4321);
}
END_TEST

START_TEST(test_cparse_json_set_string)
{
    cparse_json_set_string(cpv_test, "test", "1234");

    fail_unless(!strcmp(cparse_json_get_string(cpv_test, "test"), "1234"));
}
END_TEST

START_TEST(test_cparse_json_set_object)
{
    cParseJson *obj = cparse_json_new_string("here");

    cparse_json_set(cpv_test, "test", obj);

    fail_unless(cparse_json_get(cpv_test, "test") == obj);

    cparse_json_free(obj);
}
END_TEST

START_TEST(test_cparse_json_set_array)
{
    cParseJson *array = cparse_json_new_array();

    cparse_json_array_add_number(array, 1234);

    cparse_json_set(cpv_test, "test", array);

    fail_unless(cparse_json_array_size(cparse_json_get(cpv_test, "test")) == cparse_json_array_size(array));

    cparse_json_free(array);
}
END_TEST

START_TEST(test_cparse_array_add)
{
    cParseJson *array = cparse_json_new_array();

    cparse_json_array_add_number(array, 1234);

    fail_unless(cparse_json_array_size(array) == 1);

    cparse_json_free(array);

}
END_TEST

Suite *cparse_json_suite (void)
{
    Suite *s = suite_create ("Value");

    /* Core test case */
    TCase *tc = tcase_create ("Value");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_json_set_number);
    tcase_add_test(tc, test_cparse_json_set_real);
    tcase_add_test(tc, test_cparse_json_set_string);
    tcase_add_test(tc, test_cparse_json_set_object);
    tcase_add_test(tc, test_cparse_json_set_array);

    suite_add_tcase(s, tc);

    tc = tcase_create("Array");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_array_add);
    suite_add_tcase(s, tc);

    return s;
}

