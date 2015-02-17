/*! \file parse.h */
#ifndef CPARSE_PARSE_H_
#define CPARSE_PARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*! levels of logging */
typedef enum
{
    /*! logging is disabled */
    cParseLogNone = 0,
    /*! only error messages will be logged */
    cParseLogError = 1,
    /*! warnings and errors will be logged */
    cParseLogWarn = 2,
    /*! info, warning, and error messages will be logged */
    cParseLogInfo = 3,
    /*! debug, info, warning and error messages will be logged */
    cParseLogDebug = 4,
    /*! trace, debug, info, warning and error messages will be logged */
    cParseLogTrace = 5
} cParseLogLevel;

/*! sets the parse api application id
 * \param appId the application id
 */
void cparse_set_application_id(const char *appId);

/*! sets the parse api key
 * \param apiKey the REST api key
 */
void cparse_set_api_key(const char *apiKey);

/*! sets the logging level
 * \param level the logging level to set
 */
void cparse_set_log_level(cParseLogLevel level);

#ifdef __cplusplus
}
#endif

#endif
