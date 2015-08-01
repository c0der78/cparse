#ifndef CPARSE_PROTOCOL_H_
#define CPARSE_PROTOCOL_H_

#define CPARSE_BUF_SIZE 		512

#define CPARSE_HEADER_APP_ID    "X-Parse-Application-Id"

#define CPARSE_HEADER_API_KEY   "X-Parse-REST-API-Key"

#define CPARSE_HEADER_MASTER_KEY   "X-Parse-Master-Key"

#define CPARSE_HEADER_SESSION_TOKEN  "X-Parse-Session-Token"

#define CPARSE_HEADER_REVOCABLE_SESSION "X-Parse-Revocable-Session"

#define CPARSE_KEY_CLASS_NAME   "className"

#define CPARSE_KEY_OBJECT_ID    "objectId"

#define CPARSE_KEY_CREATED_AT   "createdAt"

#define CPARSE_KEY_UPDATED_AT   "updatedAt"

#define CPARSE_KEY_USER_SESSION_TOKEN  "sessionToken"

#define CPARSE_KEY_USER_NAME   "username"

#define CPARSE_KEY_USER_EMAIL  "email"

#define CPARSE_KEY_ACL         "ACL"

#define CPARSE_KEY_USER_PASSWORD "password"

#define CPARSE_KEY_NAME			"name"

#define CPARSE_KEY_RESULTS  	"results"

#define CPARSE_KEY_OP           "__op"

#define CPARSE_KEY_INCREMENT    "Increment"
#define CPARSE_KEY_DECREMENT    "Decrement"
#define CPARSE_KEY_DELETE       "Delete"

#define CPARSE_KEY_OBJECTS      "objects"
#define CPARSE_KEY_ADD          "Add"
#define CPARSE_KEY_ADD_RELATION  "AddRelation"
#define CPARSE_KEY_ADD_UNIQUE   "AddUnique"
#define CPARSE_KEY_REMOVE       "Remove"

#define CPARSE_KEY_TYPE         "__type"

#define CPARSE_KEY_AMOUNT       "amount"

#define CPARSE_KEY_EMAIL_VERIFIED "emailVerified"

extern const char *const CPARSE_RESERVED_KEYS[];

#define CPARSE_OP_INCREMENT     "Increment"

#define CPARSE_OP_DECREMENT     "Decrement"

#define CPARSE_TYPE_OBJECT      "Object"

#define CPARSE_TYPE_POINTER     "Pointer"

#define CPARSE_TYPE_BYTES       "Bytes"

#define CPARSE_TYPE_DATE        "Date"

#define CPARSE_TYPE_GEOPOINT    "GeoPoint"

#define CPARSE_TYPE_FILE        "File"

#define CPARSE_TYPE_RELATION    "Relation"

#define CPARSE_CLASS_USER       "_User"

#define CPARSE_CLASS_ROLE		"_Role"

#define CPARSE_CLASS_SESSION	"_Session"

#define CPARSE_CLASS_INSTALLATION  "_Installation"

#define CPARSE_CLOUD_FUNCTIONS_PATH  "functions"

#define CPARSE_OBJECTS_PATH  	"classes/"

#define CPARSE_USERS_PATH 		"users"

#define CPARSE_ROLES_PATH		"roles"

#define CPARSE_SESSIONS_PATH	"sessions"

#define CPARSE_BATCH_REQUEST_URI  "batch"

#define CPARSE_ACL_PUBLIC	"*"

#define CPARSE_ERROR_INTERNAL  1
#define CPARSE_ERROR_TIMEOUT  124
#define CPARSE_ERROR_EXCEEDED_BURST_LIMIT  155
#define CPARSE_ERROR_OBJECT_NOT_FOUND_FOR_GET  101
#define CPARSE_HTTP_OK  200

#endif

