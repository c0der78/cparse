#include <check.h>
#include <stdio.h>
#include <cparse/object.h>
#include <cparse/parse.h>
#include <cparse/json.h>
#include <cparse/error.h>
#include <cparse/query.h>
#include "parse.test.h"

static void cparse_test_setup()
{

}

static void cparse_test_teardown()
{
    cparse_cleanup_test_objects();
}

START_TEST(test_cparse_query_objects)
{
    CPARSE_ERROR *error = NULL;

    fail_unless(cparse_create_and_save_test_object("user1", 1500));

    CPARSE_QUERY *query = cparse_query_with_class_name(TEST_CLASS);

    query->where = cparse_json_new();

    cparse_json_set_string(query->where, "playerName", "user1");

    fail_unless(cparse_query_find_objects(query, &error));

    fail_unless(query->size > 0);

    CPARSE_OBJ *user1 = query->results[0];

    fail_unless(!strcmp(cparse_object_get_string(user1, "playerName"), "user1"));
}
END_TEST

START_TEST(test_cparse_query_where)
{
    CPARSE_ERROR *error = NULL;

    int randScore  = rand() % 100000 + 1;

    fail_unless(cparse_create_and_save_test_object("user1", randScore));

    CPARSE_QUERY *query = cparse_query_with_class_name(TEST_CLASS);

    query->where = cparse_json_new();

    CPARSE_JSON *in = cparse_json_new();

    CPARSE_JSON *inArray = cparse_json_new_array();

    cparse_json_array_add_number(inArray, 127978);

    cparse_json_array_add_number(inArray, randScore);

    cparse_json_array_add_number(inArray, 545666);

    cparse_json_array_add_number(inArray, 255550);

    cparse_json_set(in, "$in", inArray);

    cparse_json_set(query->where, "score", in);

    fail_unless(cparse_query_find_objects(query, &error));

    fail_unless(query->size > 0);

    fail_unless(cparse_object_get_number(query->results[0], "score", 0) == randScore);
}
END_TEST

Suite *cparse_query_suite (void)
{
    Suite *s = suite_create ("Query");

    /* Core test case */
    TCase *tc = tcase_create ("Query");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_query_objects);
    tcase_add_test(tc, test_cparse_query_where);
    suite_add_tcase(s, tc);

    return s;
}
