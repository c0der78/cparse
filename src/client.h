#ifndef CPARSE_CLIENT_H_
#define CPARSE_CLIENT_H_

#include <stdlib.h>
#include <curl/curl.h>
#include <cparse/defines.h>
#include "private.h"

struct cparse_client {
    CURL *cURL;
    pthread_mutex_t lock;
    struct curl_slist *headers;
    /* may want a client for an older version simultaneously */
    char *apiVersion;
    int timeout;
    char *sessionToken;
};

BEGIN_DECL

/*! gets the client
 */
cParseClient *cparse_get_client();

END_DECL

#endif
