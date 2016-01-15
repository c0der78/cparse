#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cparse/parse.h>
#include <cparse/json.h>
#include <cparse/util.h>

extern const char *cparse_app_id;
extern const char *cparse_api_key;

void die(const char *message)
{
    printf("ERROR: %s\n", message);
    abort();
}

void read_test_config()
{
    FILE *file;
    char *text;
    long fsize;
    cParseJson *config;

    file = fopen(ROOT_PATH "/tests/parse.test.json", "rb");
    if (!file) {
        file = fopen(ROOT_PATH "/parse.test.json", "rb");
        if (!file) {
            puts("parse.test.json not found");
            return;
        }
    }

    fseek(file, 0, SEEK_END);
    fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    text = malloc(sizeof(char) * fsize + 1);

    memset(text, 0, sizeof(char) * fsize + 1);

    if (!fread(text, sizeof(char), fsize, file)) {
        die("error reading file");
    }

    fclose(file);

    config = cparse_json_tokenize(text);

    free(text);

    if (cparse_json_contains(config, "parseAppId")) {
        cparse_set_application_id(cparse_json_get_string(config, "parseAppId"));
    } else {
        die("No app id");
    }

    if (cparse_json_contains(config, "parseApiKey")) {
        cparse_set_api_key(cparse_json_get_string(config, "parseApiKey"));
    } else {
        die("No api key");
    }

    cparse_json_free(config);
}

void read_env_config()
{
    const char *val = getenv("PARSE_APP_ID");

    if (val != NULL) {
        cparse_set_application_id(val);
    }

    val = getenv("PARSE_API_KEY");

    if (val != NULL) {
        cparse_set_api_key(val);
    }
}

bool is_valid_config()
{
	return !cparse_str_empty(cparse_app_id) && !cparse_str_empty(cparse_api_key);
}

void cleanup()
{
    if (cparse_app_id) {
        free((char *)cparse_app_id);
    }

    if (cparse_api_key) {
        free((char *)cparse_api_key);
    }
}
