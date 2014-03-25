#ifndef ARG3_PARSE_H_
#define ARG3_PARSE_H_

#include <cparse/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

    void cparse_set_application_id(const char *appId);

    void cparse_set_api_key(const char *apiKey);

    void cparse_set_facebook_application_id(const char *appId);

    bool cparse_has_facebook_application_id();

    void cparse_offline_messages_enabled(bool value);

    void cparse_error_messages_enabled(bool value);

#ifdef __cplusplus
}
#endif

#endif
