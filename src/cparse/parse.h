#ifndef CPARSE_PARSE_H_
#define CPARSE_PARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    cParseLogError = 1,
    cParseLogWarn = 2,
    cParseLogInfo = 3,
    cParseLogDebug = 4,
    cParseLogTrace = 5
} cParseLogLevel;

void cparse_set_application_id(const char *appId);

void cparse_set_api_key(const char *apiKey);

void cparse_set_log_level(cParseLogLevel level);

#ifdef __cplusplus
}
#endif

#endif
