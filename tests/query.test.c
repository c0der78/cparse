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
    cParseQuery *query;
    cParseJson *where;
    cParseObject *user1;
    cParseError *error = NULL;
    bool rval;

    fail_unless(cparse_create_and_save_test_object("user1", 1500));

    query = cparse_query_with_class_name(TEST_CLASS);

    where = cparse_json_new();

    cparse_json_set_string(where, "playerName", "user1");

    cparse_query_set_where(query, where);

    cparse_json_free(where);

    rval = cparse_query_find_objects(query, &error);

    if (!rval)
        printf("Query error: %s\n", cparse_error_message(error));

    fail_unless(rval);

    fail_unless(cparse_query_size(query) > 0);

    user1 = cparse_query_result(query, 0);

    fail_unless(!strcmp(cparse_object_get_string(user1, "playerName"), "user1"));

    cparse_query_free_results(query);


    cparse_query_free(query);
}
END_TEST

START_TEST(test_cparse_query_where)
{
    cParseError *error = NULL;
    cParseQuery *query;
    cParseJson *inArray;
    cParseObject *result;

    /* create a user with a score */
    int randScore  = rand() % 100000 + 1;

    fail_unless(cparse_create_and_save_test_object("user1", randScore));

    /* create a new test query */
    query = cparse_query_with_class_name(TEST_CLASS);

    /* build an array of scores to find */
    inArray = cparse_json_new_array();

    cparse_json_array_add_number(inArray, 127978);

    cparse_json_array_add_number(inArray, randScore);

    cparse_json_array_add_number(inArray, 545666);

    cparse_json_array_add_number(inArray, 255550);

    /* set the query where clause to the score */
    cparse_query_where_in(query, "score", inArray);

    cparse_json_free(inArray);

    /* find the objects */
    fail_unless(cparse_query_find_objects(query, &error));

    fail_unless(cparse_query_size(query) > 0);

    result = cparse_query_result(query, 0);

    fail_unless(cparse_object_get_number(result, "score", 0) == randScore);

    cparse_query_free_results(query);

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

    tcase_set_timeout(tc, 30);

    suite_add_tcase(s, tc);

    return s;
}
