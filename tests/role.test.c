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
    cParseError *error = NULL;

    bool rval = false;

    cParseRole *other = NULL, *role = cparse_role_with_name(rand_name());

    cParseUser *user = cparse_user_with_name(rand_name());

    fail_unless(cparse_user_sign_up(user, "66666", NULL));

    cparse_role_set_public_acl(role, cParseAccessRead, true);
    cparse_role_set_public_acl(role, cParseAccessWrite, true);

    cparse_role_add_user(role, user);

    rval = cparse_role_save(role, &error);

    if (error) {
        puts(cparse_error_message(error));
        cparse_error_free(error);
    }

    fail_unless(rval);

    other = cparse_role_with_name(rand_name());

    cparse_role_add_role(other, role);

    cparse_role_set_public_acl(other, cParseAccessRead, true);
    cparse_role_set_public_acl(role, cParseAccessWrite, true);

    fail_unless(cparse_role_save(other, NULL));

    fail_unless(cparse_user_delete(user, NULL));

    fail_unless(cparse_role_delete(role, NULL));

    fail_unless(cparse_role_delete(other, NULL));

    cparse_user_free(user);

    cparse_role_free(other);

    cparse_role_free(role);

}
END_TEST

START_TEST(test_cparse_role_query)
{
    cParseError *error = NULL;

    bool rval = false;

    cParseQuery *query = NULL;

    cParseRole *other = NULL, *role = cparse_role_with_name(rand_name());

    cParseUser *user = cparse_user_with_name(rand_name());

    fail_unless(cparse_user_sign_up(user, "66666", NULL));

    cparse_role_set_public_acl(role, cParseAccessRead, true);
    cparse_role_set_public_acl(role, cParseAccessWrite, true);

    cparse_role_add_user(role, user);

    rval = cparse_role_save(role, &error);

    if (error) {
        puts(cparse_error_message(error));
        cparse_error_free(error);
    }

    fail_unless(rval);

    error = NULL;

    query = cparse_role_query_users(role, &error);

    if (error) {
        puts(cparse_error_message(error));
        cparse_error_free(error);
    }

    fail_unless(cparse_query_size(query) == 1);

    cparse_query_free(query);

    other = cparse_role_with_name(rand_name());

    cparse_role_add_role(other, role);

    cparse_role_set_public_acl(other, cParseAccessRead, true);
    cparse_role_set_public_acl(other, cParseAccessWrite, true);

    fail_unless(cparse_role_save(other, NULL));

    query = cparse_role_query_roles(other, &error);

    if (error) {
        puts(cparse_error_message(error));
        cparse_error_free(error);
    }

    fail_unless(cparse_query_size(query) == 1);

    cparse_query_free(query);

    fail_unless(cparse_user_delete(user, NULL));

    fail_unless(cparse_role_delete(role, NULL));

    fail_unless(cparse_role_delete(other, NULL));

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
    tcase_add_test(tc, test_cparse_role_query);
    suite_add_tcase(s, tc);

    return s;
}

