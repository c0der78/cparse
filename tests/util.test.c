#include <check.h>
#include <stdio.h>
#include <cparse/util.h>

static void cparse_test_setup()
{

}

static void cparse_test_teardown()
{

}

START_TEST(test_cparse_date_time)
{
    const char *testStr = "2011-08-20T02:06:57.931Z";

    time_t value = cparse_date_time(testStr);

    printf("%ld\n", value);

    fail_unless(value == 1313806017);
}
END_TEST

Suite *cparse_util_suite (void)
{
    Suite *s = suite_create ("Util");

    /* Core test case */
    TCase *tc = tcase_create ("Util");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_date_time);
    suite_add_tcase(s, tc);

    return s;
}

