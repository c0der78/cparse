#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <check.h>
#include <stdio.h>
#include <cparse/object.h>
#include <cparse/parse.h>
#include <cparse/json.h>
#include <cparse/error.h>

#include "private.h"

#include "parse.test.h"

static void cparse_test_setup()
{
}

static void cparse_test_teardown()
{
    cparse_cleanup_test_objects();
}

START_TEST(test_cparse_object_save)
{
    cParseError *error;
    bool rval;
    cParseObject *cp_obj = cparse_new_test_object("user1", 1234);

    fail_unless(cparse_save_test_object(cp_obj));

    fail_unless(cparse_object_id(cp_obj) != NULL);

    cparse_object_set_number(cp_obj, "score", 3243);

    rval = cparse_object_save(cp_obj, &error);

    if (!rval) {
        printf("Update error: %s", cparse_error_message(error));
    }

    fail_unless(rval);
}
END_TEST

void test_cparse_object_callback(cParseObject *obj, cParseError *error, void *param)
{
    if (error) {
        printf("callback error: %s\n", cparse_error_message(error));
    }
    fail_unless(error == NULL);

    if (obj == NULL) {
        printf("no object on callback!\n");
    } else {
        fail_unless(cparse_object_id(obj) != NULL);
    }
}

void test_cparse_object_update_callback(cParseObject *obj, cParseError *error, void *param)
{
    test_cparse_object_callback(obj, error, param);

    fail_unless(cparse_object_get_number(obj, "score", 0) == 987);
}

START_TEST(test_cparse_object_fetch)
{
    cParseObject *cp_obj = cparse_new_test_object("user1", 1234);

    /* first create reference object */
    cParseObject *obj = cparse_new_test_object("user2", 1444);

    cParseJson *data = NULL;

    cParseError *error = NULL;

    /* check we fail if no object id */
    fail_if(cparse_object_fetch(obj, &error));

    if (error) {
        puts(cparse_error_message(error));
        cparse_error_free(error);
    }

    fail_unless(cparse_save_test_object(obj));

    fail_unless(cparse_object_id(obj) != NULL);

    /* now set refrence on other object */

    cparse_object_set_reference(cp_obj, "partner", obj);

    fail_unless(cparse_save_test_object(cp_obj));

    data = cparse_object_get(cp_obj, "partner");

    fail_unless(cparse_json_num_keys(data) == 3);

    /* now fetch the reference */

    error = NULL;

    fail_unless(cparse_object_fetch(cp_obj, &error));

    fail_unless(error == NULL);

    data = cparse_object_get(cp_obj, "partner");

    fail_unless(cparse_json_num_keys(data) > 3);

    fail_unless(cparse_object_fetch_in_background(cp_obj, test_cparse_object_callback, NULL));
}
END_TEST

START_TEST(test_cparse_object_refresh)
{
    cParseError *error = NULL;
    bool rval;
    cParseObject *obj = cparse_new_test_object("user2", 1444), *obj2;

    fail_unless(cparse_save_test_object(obj));

    obj2 = cparse_new_test_object("user3", 1234);

    cparse_cleanup_test_object(obj2);

    rval = cparse_object_refresh(obj, NULL);

    if (!rval) {
        printf("refresh error: %s\n", cparse_error_message(error));
    }

    fail_unless(rval);

    obj2->objectId = strdup(obj->objectId);

    rval = cparse_object_refresh(obj2, &error);

    if (!rval) {
        printf("Refresh error: %s\n", cparse_error_message(error));
    }

    fail_unless(rval);

    fail_unless(cparse_object_get_number(obj2, "score", 0) == 1444);

    fail_unless(cparse_object_refresh_in_background(obj2, test_cparse_object_callback, NULL));
}
END_TEST

START_TEST(test_cparse_object_save_in_background)
{
    cParseObject *cp_obj = cparse_new_test_object("user1", 1234);

    fail_unless(cparse_object_save_in_background(cp_obj, test_cparse_object_callback, NULL));
}
END_TEST


START_TEST(test_cparse_object_set_value)
{
    cParseObject *cp_obj = cparse_object_with_class_name(TEST_CLASS);

    cparse_object_set_number(cp_obj, "score", 1234);

    fail_unless(cparse_object_attribute_size(cp_obj) == 1);

    fail_unless(cparse_object_get_number(cp_obj, "score", 0) == 1234);

    cparse_object_free(cp_obj);
}
END_TEST

static void cparse_test_count_callback(cParseObject *obj, const char *key, cParseJson *value, void *param)
{
    if (param) {
        size_t *p = (size_t *)param;
        ++(*p);
        param = p;
    }
}

START_TEST(test_cparse_object_count_attributes)
{
    size_t count = 0;

    cParseObject *cp_obj = cparse_object_with_class_name(TEST_CLASS);

    cparse_object_set_string(cp_obj, "teststr", "1234");

    cparse_object_set_real(cp_obj, "testreal", 1234.5678);

    fail_unless(cparse_object_attribute_size(cp_obj) == 2);

    cparse_object_foreach_attribute(cp_obj, cparse_test_count_callback, &count);

    fail_unless(count == 2);

    cparse_object_free(cp_obj);
}
END_TEST

START_TEST(test_cparse_object_remove_attribute)
{
    cParseJson *removed;

    cParseObject *cp_obj = cparse_object_with_class_name(TEST_CLASS);

    cParseJson *value = cparse_json_new_string("1234");

    cparse_object_set(cp_obj, "main", value);

    fail_unless(cparse_object_attribute_size(cp_obj) == 1);

    removed = cparse_object_remove_and_get(cp_obj, "main");

    fail_unless(removed == value);

    cparse_json_free(value);

    fail_unless(cparse_object_attribute_size(cp_obj) == 0);

    cparse_object_free(cp_obj);
}
END_TEST

START_TEST(test_cparse_object_to_json)
{
    const char *buf;
    cParseObject *cp_obj = cparse_object_with_class_name(TEST_CLASS);

    cparse_object_set_string(cp_obj, "main", "Hello,World");

    buf = cparse_object_to_json_string(cp_obj);

#ifdef HAVE_JSON_EXTENDED
    fail_unless(!strcmp(buf, "{\"main\":\"Hello,World\"}"));
#else
    fail_unless(!strcmp(buf, "{ \"main\": \"Hello,World\" }"));
#endif
    cparse_object_set_number(cp_obj, "main", 1234);

    buf = cparse_object_to_json_string(cp_obj);

#ifdef HAVE_JSON_EXTENDED
    fail_unless(!strcmp(buf, "{\"main\":1234}"));
#else
    fail_unless(!strcmp(buf, "{ \"main\": 1234 }"));
#endif

    cparse_object_free(cp_obj);
}
END_TEST

START_TEST(test_cparse_object_update)
{
    cParseObject *obj = cparse_new_test_object("blah", 1234);
    cParseJson *updates = cparse_json_new();
    cParseError *error = NULL;
    bool rval = false;

    fail_unless(cparse_save_test_object(obj));

    cparse_json_set_number(updates, "score", 789);

    rval = cparse_object_update(obj, updates, &error);

    if (error) {
        puts(cparse_error_message(error));
        cparse_error_free(error);
    }

    cparse_json_free(updates);

    fail_unless(rval);
}
END_TEST

START_TEST(test_cparse_object_update_in_background)
{
    cParseObject *cp_obj = cparse_new_test_object("user1", 1234);
    cParseJson *updates = cparse_json_new();

    fail_unless(cparse_save_test_object(cp_obj));

    cparse_json_set_number(updates, "score", 987);

    fail_unless(cparse_object_update_in_background(cp_obj, updates, test_cparse_object_update_callback, NULL));
}
END_TEST

START_TEST(test_cparse_object_copy)
{
    cParseObject *copy = NULL;
    cParseObject *cp_obj = cparse_new_test_object("userOrig", 4332);

    fail_unless(cparse_save_test_object(cp_obj));

    copy = cparse_object_with_class_name(TEST_CLASS);

    cparse_object_copy(copy, cp_obj);

    fail_unless(!strcmp(cparse_object_class_name(copy), cparse_object_class_name(cp_obj)));

    fail_unless(cparse_object_created_at(copy) == cparse_object_created_at(cp_obj));

    fail_unless(cparse_object_updated_at(copy) == cparse_object_updated_at(cp_obj));

    cparse_object_free(copy);
}
END_TEST

START_TEST(test_cparse_object_delete)
{
    cParseObject *obj = cparse_new_test_object("userAbc", 3122);

    cParseError *error = NULL;

    fail_if(cparse_object_delete(obj, &error));

    fail_unless(cparse_save_test_object(obj));

    if (error) {
        cparse_error_free(error);
        error = NULL;
    }

    fail_unless(cparse_object_delete(obj, &error));
}
END_TEST

Suite *cparse_object_suite(void)
{
    Suite *s = suite_create("Object");

    /* Core test case */
    TCase *tc = tcase_create("Save/Update");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_object_save);
    tcase_add_test(tc, test_cparse_object_save_in_background);
    tcase_add_test(tc, test_cparse_object_update);
    tcase_add_test(tc, test_cparse_object_update_in_background);
    tcase_set_timeout(tc, 30);
    suite_add_tcase(s, tc);

    tc = tcase_create("Attributes");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_object_set_value);
    tcase_add_test(tc, test_cparse_object_count_attributes);
    tcase_add_test(tc, test_cparse_object_remove_attribute);
    tcase_add_test(tc, test_cparse_object_to_json);
    suite_add_tcase(s, tc);

    tc = tcase_create("Refresh/Fetch");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_object_fetch);
    tcase_add_test(tc, test_cparse_object_refresh);
    tcase_set_timeout(tc, 30);
    suite_add_tcase(s, tc);

    tc = tcase_create("Delete/Copy");
    tcase_add_checked_fixture(tc, cparse_test_setup, cparse_test_teardown);
    tcase_add_test(tc, test_cparse_object_delete);
    tcase_add_test(tc, test_cparse_object_copy);
    tcase_set_timeout(tc, 30);
    suite_add_tcase(s, tc);

    return s;
}
