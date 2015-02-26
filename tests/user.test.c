#include <check.h>
#include <stdio.h>
#include <cparse/user.h>

#include "../src/private.h"
#include "parse.test.h"

extern cParseObject *cparse_current_user_;

static void cparse_test_setup()
{

}

static void cparse_test_teardown()
{
    cparse_cleanup_test_objects();
}

START_TEST(test_cparse_user_sign_up)
{
    cParseError *error = NULL;
    bool rval;
    cParseObject *user = cparse_user_new();

    cparse_user_set_name(user, "user123");

    cparse_object_set_string(user, "bio", "hello, world");

    error = NULL;

    rval = cparse_user_sign_up(user, "Passw0rd!", &error);

    if (error)
        printf("user signup error: %s\n", cparse_error_message(error));

    fail_unless(rval);

    fail_unless(error == NULL);

    fail_unless(cparse_user_session_token(user) != NULL);

    fail_unless(cparse_current_user() == user);

    cparse_user_delete(user, &error);

    if (error)
        printf("user delete error: %s\n", cparse_error_message(error));

    cparse_object_free(user);
}
END_TEST

void cparse_user_sign_up_callback(cParseObject *obj, bool success, cParseError *error)
{
    if (!success)
        printf("callback error: %s\n", cparse_error_message(error));

    fail_unless(success);

    fail_unless(obj != NULL);

    fail_unless(cparse_user_delete(obj, NULL));

    cparse_object_free(obj);
}

START_TEST(test_cparse_user_sign_up_in_background)
{
    cParseObject *obj = cparse_user_with_name("bobby123");

    pthread_t bg = cparse_user_sign_up_in_background(obj, "Password!", cparse_user_sign_up_callback);

    pthread_join(bg, NULL);

}
END_TEST

START_TEST(test_cparse_user_login)
{
    cParseError *error = NULL;

    cParseObject *user123 = cparse_user_new(), *user;

    cparse_user_set_name(user123, "user123");

    cparse_object_set_string(user123, "bio", "hello, world");

    fail_unless(cparse_user_sign_up(user123, "Passw0rd!", &error));

    if (error)
        printf("user signup error: %s\n", cparse_error_message(error));

    fail_unless(error == NULL);

    fail_unless(cparse_current_user() == user123);

    user = cparse_user_login("user123", "Passw0rd!", &error);

    if (error)
        printf("user login error: %s\n", cparse_error_message(error));

    fail_unless(error == NULL);

    fail_unless(user != NULL);

    fail_unless(user == cparse_current_user());

    fail_unless(!strcmp(cparse_user_name(user), "user123"));

    fail_unless(cparse_user_session_token(user) != NULL);

    if (error)
        printf("user delete error: %s\n", cparse_error_message(error));

    cparse_object_free(user);

    fail_unless(cparse_current_user_ == NULL);

    /* check can still get current user */
    user = cparse_current_user();

    fail_unless(user != NULL);

    cparse_object_free(user);

    fail_unless(cparse_user_delete(user123, &error));

    cparse_object_free(user123);

}
END_TEST

void cparse_login_callback(cParseObject *obj, bool success, cParseError *error)
{
    if (!success)
        printf("login callback error: %s\n", cparse_error_message(error));

    fail_unless(success);

    fail_unless(cparse_user_delete(obj, NULL));
}

START_TEST(test_cparse_login_in_background)
{
    pthread_t bg;
    cParseError *error = NULL;
    cParseObject *user123 = cparse_user_with_name("user123");

    fail_unless(cparse_user_sign_up(user123, "Passw0rd!", &error));

    if (error)
        printf("user signup error: %s\n", cparse_error_message(error));

    fail_unless(error == NULL);

    cparse_object_free(user123);

    bg = cparse_user_login_in_background("user123", "Passw0rd!", cparse_login_callback);

    pthread_join(bg, NULL);

    /*cparse_object_free(user123);*/
}
END_TEST

START_TEST(test_cparse_is_user)
{
    cParseObject *obj = cparse_user_new();

    fail_unless(obj->className != NULL && *obj->className);

    fail_unless(cparse_class_name_is_user(obj->className));

    fail_unless(cparse_object_is_user(obj));

    cparse_object_free(obj);
}
END_TEST

Suite *cparse_user_suite (void)
{
    Suite *s = suite_create ("User");

    /* Core test case */
    TCase *tc = tcase_create ("Login");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_user_login);
    tcase_add_test(tc, test_cparse_login_in_background);
    tcase_set_timeout(tc, 30);
    suite_add_tcase(s, tc);

    tc = tcase_create("Sign Up");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_user_sign_up_in_background);
    tcase_add_test(tc, test_cparse_user_sign_up);
    tcase_set_timeout(tc, 30);
    suite_add_tcase(s, tc);

    tc = tcase_create("Other");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_is_user);
    suite_add_tcase(s, tc);

    return s;
}

