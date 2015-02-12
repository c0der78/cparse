#ifndef CPARSE_TEST_H_
#define CPARSE_TEST_H_

#define TEST_CLASS "GameScore"

CPARSE_OBJ *cparse_new_test_object(const char *name, int score);
int cparse_cleanup_test_object(CPARSE_OBJ *obj);
int cparse_save_test_object(CPARSE_OBJ *obj);
int cparse_cleanup_test_objects();
int cparse_create_and_save_test_object(const char *name, int score);

#endif
