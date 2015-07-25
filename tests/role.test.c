#include <stdlib.h>
#include <cparse/defines.h>
#include <cparse/object.h>
#include <cparse/user.h>
#include <cparse/role.h>
#include <stdio.h>
#include <check.h>
#include "parse.test.h"

static void cparse_test_setup()
{
    srand(time(0));
}

static void cparse_test_teardown()
{
    cparse_cleanup_test_objects();
}

START_TEST(test_cparse_role_add_user)
{
    cParseRole *other = NULL, *role = cparse_role_with_name("testing");

    cParseUser *user = cparse_user_with_name(rand_name());

    fail_unless(cparse_user_sign_up(user, "66666", NULL));

    puts("setting public acl");

    cparse_role_set_public_acl(role, true, false);

    puts("setting user acl");

    cparse_role_set_user_acl(role, user, false, false);

    puts("saving role");

    fail_unless(cparse_role_save(role, NULL));

    puts("creating other role");

    other = cparse_role_with_name("developing");

    puts("setting role acl");

    cparse_role_set_role_acl(other, cparse_role_name(role), true, false);

    puts("saving other role");

    fail_unless(cparse_role_save(other, NULL));

    puts("deleting user");

    fail_unless(cparse_user_delete(user, NULL));

    cparse_user_free(user);

    cparse_role_free(other);

    cparse_role_free(role);

}
END_TEST

Suite *cparse_role_suite (void)
{
    Suite *s = suite_create ("Role");

    TCase *tc = tcase_create("Assignable");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_role_add_user);
    suite_add_tcase(s, tc);

    return s;
}

