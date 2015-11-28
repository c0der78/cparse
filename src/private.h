#ifndef CPARSE_PRIVATE_H
#define CPARSE_PRIVATE_H

/*! a parse client */
typedef struct cparse_client cParseClient;

/*! a parse response */
typedef struct cparse_client_response cParseResponse;

/*! a parse request */
typedef struct cparse_request cParseRequest;

/*! a list of request headers */
typedef struct cparse_dlist cParseRequestHeader;

/*! a list of request data (for url encoding on get requests) */
typedef struct cparse_dlist cParseRequestData;

struct cparse_object {
    cParseJson *attributes;
    char *className;
    char *urlPath;
    char *objectId;
    time_t updatedAt;
    time_t createdAt;
};


struct cparse_query {
    cParseJson *where;
    cParseObject **results;
    char *className;
    char *urlPath;
    char *keys;
    size_t size;
    int limit;
    int skip;
    bool trace;
    bool count;
};

struct cparse_query_builder {
    cParseJson *json;
};

struct cparse_op_array {
    char *operation;
    cParseJson *objects;
};

struct cparse_op_decrement {
    int amount;
};

struct cparse_op_increment {
    int amount;
};


struct cparse_type_bytes {
    void *data;
    size_t size;
};

struct cparse_type_date {
    time_t value;
};

struct cparse_type_file {
    char *localFileName;
    char *parseFileName;
    char *contentType;
    char *body;
    char *url;
};

struct cparse_type_geopoint {
    double latitude;
    double longitude;
};

struct cparse_type_pointer {
    char *className;
    char *objectId;
};

typedef bool (*cParseObjectAction)(cParseObject *obj, cParseError **error);

/* for background threads */
typedef struct {
    cParseClient *client;
    cParseObject *obj;
    cParseObjectCallback callback; /* the callback passed by user */
    cParseObjectAction action;     /* the method to call in thread */
    void *param;                   /* user defined parameter */
    void (*cleanup)(cParseObject *);
} cParseObjectThread;

/* this runs a object action in the background */

BEGIN_DECL

bool cparse_object_run_in_background(cParseObject *obj, cParseObjectAction action, cParseObjectCallback callback, void *param,
                                     void (*cleanup)(cParseObject *));

END_DECL

#endif
