#include "config.h"
#include <stdlib.h>
#include <time.h>
#include <cparse/parse.h>
#include "protocol.h"

const char *const cparse_lib_version = "1.0";

const char *cparse_app_id = NULL;

const char *cparse_api_key = NULL;

extern cParseLogLevel cparse_current_log_level;

const char *const RESERVED_KEYS[] =
{
    KEY_CLASS_NAME, KEY_CREATED_AT, KEY_OBJECT_ID, KEY_UPDATED_AT, KEY_USER_SESSION_TOKEN
};

struct cparse_base_object
{
    char *className;
    char *objectId;
    time_t updatedAt;
    time_t createdAt;
};

void cparse_set_application_id(const char *appId)
{
    cparse_app_id = appId;
}

void cparse_set_api_key(const char *apiKey)
{
    cparse_api_key = apiKey;
}

void cparse_set_log_level(cParseLogLevel value)
{
    cparse_current_log_level = value;
}

