#ifndef CPARSE_CONFIG_TEST_H
#define CPARSE_CONFIG_TEST_H

void read_test_config();
void read_env_config();
void die(const char *message);
void cleanup();

bool is_valid_config();

#endif

