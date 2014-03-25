#include <check.h>
#include <stdio.h>
#include <cparse/object.h>
#include <cparse/parse.h>
#include <cparse/json.h>
#include <cparse/error.h>
#include <cparse/query.h>

extern CPARSE_OBJ *cp_obj;

static void cparse_test_setup()
{
    cp_obj = cparse_object_with_class_name("TestCase");
}

static void cparse_test_teardown()
{
    cparse_object_delete(cp_obj, NULL);

    cparse_object_free(cp_obj);
}

START_TEST(test_cparse_query_objects)
{
    cparse_object_set_string(cp_obj, "name", "woody");

    fail_unless(cparse_object_save(cp_obj, NULL));

    CPARSE_QUERY *query = cparse_query_with_class_name("TestCase");

    query->where = cparse_json_new();

    cparse_json_set_string(query->where, "name", "woody");

    fail_unless(cparse_query_find_objects(query, NULL));

    fail_unless(query->size == 1);

    CPARSE_OBJ *woody = query->results[0];

    fail_unless(!strcmp(cparse_object_get_string(woody, "name"), "woody"));
}
END_TEST

Suite *cparse_query_suite (void)
{
    Suite *s = suite_create ("Query");

    /* Core test case */
    TCase *tc = tcase_create ("Query");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_query_objects);
    suite_add_tcase(s, tc);

    return s;
}
