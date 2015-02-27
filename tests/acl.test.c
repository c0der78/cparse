
#include <cparse/defines.h>
#include <cparse/acl.h>
#include <cparse/object.h>
#include <check.h>
#include "parse.test.h"

static void cparse_test_setup()
{
}

static void cparse_test_teardown()
{
    cparse_cleanup_test_objects();
}

START_TEST(test_cparse_acl_set)
{
    cParseACL *acl = cparse_acl_new();

    cParseObject *obj = cparse_new_test_object("blah", 2433);

    cparse_object_set_writable_by(obj, "asdf1234", true);

    cparse_object_set_readable_by(obj, "1234knjsdf", true);

    cparse_acl_free(acl);

    cparse_object_free(obj);
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

