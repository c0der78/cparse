#include <check.h>
#include <cparse/parse.h>

extern const char *cparse_app_id;

extern const char *cparse_facebook_app_id;

extern bool cparse_offline_messages;

extern bool cparse_error_messages;

static void cparse_test_setup()
{

}

static void cparse_test_teardown()
{

}

START_TEST(test_cparse_set_application_id)
{
    cparse_set_application_id("testId");

    fail_unless(!strcmp(cparse_app_id, "testId") );

}
END_TEST

START_TEST(test_cparse_set_facebook_application_id)
{
    cparse_set_facebook_application_id("testFacebookId");

    fail_unless(!strcmp(cparse_facebook_app_id, "testFacebookId"));
}
END_TEST

START_TEST(test_cparse_offline_messages_enabled)
{
    cparse_offline_messages_enabled(true);

    fail_unless(cparse_offline_messages == true);
}
END_TEST

START_TEST(test_cparse_error_messages_enabled)
{
    cparse_error_messages_enabled(true);

    fail_unless(cparse_error_messages == true);
}
END_TEST

Suite *cparse_parse_suite (void)
{
    Suite *s = suite_create ("Config");

    /* Core test case */
    TCase *tc = tcase_create ("Config");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_set_application_id);
    tcase_add_test(tc, test_cparse_set_facebook_application_id);
    tcase_add_test(tc, test_cparse_offline_messages_enabled);
    tcase_add_test(tc, test_cparse_error_messages_enabled);
    suite_add_tcase(s, tc);

    return s;
}