#include <check.h>
#include <cparse/user.h>

static void cparse_test_setup()
{

}

static void cparse_test_teardown()
{

}

START_TEST(test_cparse_user_sign_up)
{
    CPARSE_OBJ *user = cparse_user_new();

    cparse_user_set_name(user, "user123");

    cparse_object_set_string(user, "bio", "hello, world");

    CPARSE_ERROR *error = NULL;

    fail_unless(cparse_user_sign_up(user, "Passw0rd!", &error));

    fail_unless(error == NULL);
}
END_TEST

Suite *cparse_user_suite (void)
{
    Suite *s = suite_create ("User");

    /* Core test case */
    TCase *tc = tcase_create ("User");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_user_sign_up);
    suite_add_tcase(s, tc);

    return s;
}