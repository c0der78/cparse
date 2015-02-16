#ifndef CPARSE_JSON_PRIVATE_H
#define CPARSE_JSON_PRIVATE_H

#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#elif defined(HAVE_JSON_JSON_H)
#include <json/json.h>
#else
#error "json.h header not found, be sure to include config.h"
#endif

#endif
