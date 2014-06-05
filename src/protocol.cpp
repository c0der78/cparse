
namespace cparse
{
    namespace protocol
    {
        extern const char *const HOST            = "api.parse.com";

        extern const char *const VERSION         = "1";

        extern const char *const HEADER_APP_ID   = "X-Parse-Application-Id";

        extern const char *const HEADER_API_KEY  = "X-Parse-REST-API-Key";

        extern const char *const HEADER_MASTER_KEY  = "X-Parse-Master-Key";

        extern const char *const HEADER_SESSION_TOKEN = "X-Parse-Session-Token";

        extern const char *const KEY_CLASS_NAME  = "className";

        extern const char *const KEY_OBJECT_ID   = "objectId";

        extern const char *const KEY_CREATED_AT  = "createdAt";

        extern const char *const KEY_UPDATED_AT  = "updatedAt";

        extern const char *const KEY_USER_SESSION_TOKEN = "sessionToken";

        extern const char *const KEY_USER_EMAIL = "email";

        extern const char *const RESPONSE_KEY_RESULTS = "results";
        extern const char *const KEY_RESULTS = RESPONSE_KEY_RESULTS;

        extern const char *const KEY_OP          = "__op";

        extern const char *const KEY_INCREMENT   = "Increment";
        extern const char *const                  KEY_DECREMENT   = "Decrement";
        extern const char *const KEY_DELETE      = "Delete";

        extern const char *const KEY_OBJECTS     = "objects";
        extern const char *const KEY_ADD         = "Add";
        extern const char *const KEY_ADD_RELATION = "AddRelation";
        extern const char *const KEY_ADD_UNIQUE  = "AddUnique";
        extern const char *const KEY_REMOVE      = "Remove";

        extern const char *const KEY_TYPE        = "__type";

        extern const char *const KEY_AMOUNT      = "amount";

        extern const char *const RESERVED_KEYS[] = { KEY_CLASS_NAME, KEY_CREATED_AT, KEY_OBJECT_ID, KEY_UPDATED_AT, KEY_USER_SESSION_TOKEN };

        extern const char *const OP_INCREMENT    = "Increment";

        extern const char *const OP_DECREMENT    = "Decrement";

        extern const char *const TYPE_OBJECT     = "Object";

        extern const char *const TYPE_POINTER    = "Pointer";

        extern const char *const TYPE_BYTES      = "Bytes";

        extern const char *const TYPE_DATE       = "Date";

        extern const char *const TYPE_GEOPOINT   = "GeoPoint";

        extern const char *const TYPE_FILE       = "File";

        extern const char *const TYPE_RELATION   = "Relation";

        extern const char *const CLASS_USER      = "_User";

        extern const char *const CLASS_INSTALLATION = "_Installation";

        extern const char *const CLOUD_FUNCTIONS_PATH = "functions";

        extern const char *const OBJECTS_PATH = "classes";

        extern const char *const BATCH_REQUEST_URI = "batch";

        extern const unsigned ERROR_INTERNAL = 1;
        extern const unsigned ERROR_TIMEOUT = 124;
        extern const unsigned ERROR_EXCEEDED_BURST_LIMIT = 155;
        extern const unsigned ERROR_OBJECT_NOT_FOUND_FOR_GET = 101;
        extern const unsigned HTTP_OK = 200;

    }
}