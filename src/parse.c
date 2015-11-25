#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <cparse/parse.h>
#include "protocol.h"

const char *const cparse_lib_version = "1.0";

const char *cparse_app_id = NULL;

const char *cparse_api_key = NULL;

extern cParseLogLevel cparse_current_log_level;

bool cparse_revocable_sessions = false;

const char *const CPARSE_RESERVED_KEYS[] = {CPARSE_KEY_CLASS_NAME, CPARSE_KEY_CREATED_AT, CPARSE_KEY_OBJECT_ID, CPARSE_KEY_UPDATED_AT,
                                            CPARSE_KEY_USER_SESSION_TOKEN};

void cparse_set_application_id(const char *appId)
{
    cparse_app_id = strdup(appId);
}

void cparse_set_api_key(const char *apiKey)
{
    cparse_api_key = strdup(apiKey);
}

void cparse_set_log_level(cParseLogLevel value)
{
    cparse_current_log_level = value;
}

void cparse_enable_revocable_sessions(bool value)
{
    cparse_revocable_sessions = value;
}
