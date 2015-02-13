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

    CPARSE_JSON *where = cparse_json_new();

    cparse_json_set_string(where, "playerName", "user1");

    cparse_query_set_where(query, where);

    cparse_json_free(where);

    fail_unless(cparse_query_find_objects(query, &error));

    fail_unless(cparse_query_size(query) > 0);

    CPARSE_OBJ *user1 = cparse_query_result(query, 0);

    fail_unless(!strcmp(cparse_object_get_string(user1, "playerName"), "user1"));
}
END_TEST

START_TEST(test_cparse_query_where)
{
    CPARSE_ERROR *error = NULL;

    int randScore  = rand() % 100000 + 1;

    fail_unless(cparse_create_and_save_test_object("user1", randScore));

    CPARSE_QUERY *query = cparse_query_with_class_name(TEST_CLASS);

    CPARSE_JSON *inArray = cparse_json_new_array();

    cparse_json_array_add_number(inArray, 127978);

    cparse_json_array_add_number(inArray, randScore);

    cparse_json_array_add_number(inArray, 545666);

    cparse_json_array_add_number(inArray, 255550);

    CPARSE_JSON *in = cparse_json_new();

    cparse_json_set(in, CPARSE_QUERY_IN, inArray);

    cparse_json_free(inArray);

    CPARSE_JSON *score = cparse_json_new();

    cparse_json_set(score, "score", in);

    cparse_json_free(in);

    cparse_query_set_where(query, score);

    cparse_json_free(score);

    fail_unless(cparse_query_find_objects(query, &error));

    fail_unless(cparse_query_size(query) > 0);

    CPARSE_OBJ *result = cparse_query_result(query, 0);

    fail_unless(cparse_object_get_number(result, "score", 0) == randScore);

    cparse_query_free(query);
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
