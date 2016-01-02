
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <cparse/json.h>
#include <time.h>
#include <cparse/parse.h>
#include "parse.test.h"
#include "config.test.h"

Suite *cparse_parse_suite();
Suite *cparse_json_suite();
Suite *cparse_object_suite();
Suite *cparse_query_suite();
Suite *cparse_util_suite();
Suite *cparse_user_suite();
Suite *cparse_client_suite();
Suite *cparse_acl_suite();
Suite *cparse_role_suite();

extern int cparse_cleanup_test_objects();

int main(void)
{
    int number_failed;
    SRunner *sr;

    srand(time(0));

    read_env_config();
    read_test_config();

    if (!is_valid_config()) {
        die("test configuration not valid");
    }

#ifdef DEBUG
    cparse_set_log_level(cParseLogTrace);
#else
    cparse_set_log_level(cParseLogError);
#endif

    sr = srunner_create(cparse_parse_suite());
    srunner_add_suite(sr, cparse_json_suite());
    srunner_add_suite(sr, cparse_object_suite());
    srunner_add_suite(sr, cparse_query_suite());
    srunner_add_suite(sr, cparse_util_suite());
    srunner_add_suite(sr, cparse_user_suite());
    srunner_add_suite(sr, cparse_client_suite());
    srunner_add_suite(sr, cparse_acl_suite());
    srunner_add_suite(sr, cparse_role_suite());
    srunner_run_all(sr, CK_ENV);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    cparse_global_cleanup();

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}



