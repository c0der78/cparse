#include <stdlib.h>
#include <cparse/defines.h>
#include <cparse/object.h>
#include <cparse/user.h>
#include <check.h>
#include <stdio.h>
#include "parse.test.h"

static void cparse_test_setup()
{
    srand(time(0));
}

static void cparse_test_teardown()
{
    cparse_cleanup_test_objects();
}

START_TEST(test_cparse_acl_set)
{
    cParseError *error = NULL;

    bool rval = false;

    cParseObject *obj = cparse_new_test_object("blah", 2433);

    cParseUser *user = cparse_user_with_name(rand_name());

    fail_unless(user != NULL);

    fail_unless(obj != NULL);

    fail_unless(cparse_user_sign_up(user, "676767", NULL));

    cparse_object_set_public_acl(obj, true, true);

    cparse_object_set_user_acl(obj, user, true, true);

    rval = cparse_object_save(obj, &error);

    if (error) {
        puts(cparse_error_message(error));
        cparse_error_free(error);
    }

    fail_unless(rval);

    fail_unless(cparse_user_delete(user, NULL));

    cparse_object_free(obj);

    cparse_object_free(user);
}
END_TEST

Suite *cparse_acl_suite (void)
{
    Suite *s = suite_create ("ACL");

    TCase *tc = tcase_create("Assignable");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_acl_set);
    suite_add_tcase(s, tc);

    return s;
}

