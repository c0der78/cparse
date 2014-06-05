

namespace cparse
{
    namespace protocol
    {
        extern const char *const HOST;

        extern const char *const VERSION;

        extern const char *const HEADER_APP_ID;

        // The HTTP header used for passing your API key to the
        // Parse API.
        extern const char *const HEADER_API_KEY;

        // The HTTP header used for passing your API Master key to the
        // Parse API.
        extern const char *const HEADER_MASTER_KEY;

        // The HTTP header used for passing your authenticated session
        extern const char *const HEADER_SESSION_TOKEN;

        // JSON Keys
        // ----------------------------------------

        // The JSON key used to store the class name of an object
        // in a Pointer datatype.
        extern const char *const KEY_CLASS_NAME;

        // The JSON key used to store the ID of Parse objects
        // in their JSON representation.
        extern const char *const KEY_OBJECT_ID;

        // The JSON key used to store the creation timestamp of
        // Parse objects in their JSON representation.
        extern const char *const KEY_CREATED_AT;

        // The JSON key used to store the last modified timestamp
        // of Parse objects in their JSON representation.
        extern const char *const KEY_UPDATED_AT;

        extern const char *const KEY_USER_SESSION_TOKEN;

        extern const char *const KEY_USER_EMAIL;

        // The JSON key used in the top-level response object
        // to indicate that the response contains an array of objects.
        extern const char *const RESPONSE_KEY_RESULTS;

        extern const char *const KEY_RESULTS;

        // The JSON key used to identify an operator
        extern const char *const KEY_OP;

        extern const char *const KEY_INCREMENT;
        extern const char *const KEY_DECREMENT;
        extern const char *const KEY_DELETE;

        // array ops
        extern const char *const KEY_OBJECTS;
        extern const char *const KEY_ADD;
        extern const char *const KEY_ADD_RELATION;
        extern const char *const KEY_ADD_UNIQUE;
        extern const char *const KEY_REMOVE;

        // The JSON key used to identify the datatype of a special value.
        extern const char *const KEY_TYPE;

        // The JSON key used to specify the numerical value in the
        // increment/decrement API call.
        extern const char *const KEY_AMOUNT;

        extern const char *const RESERVED_KEYS[];

        // Other Constants
        // ----------------------------------------

        // Operation name for incrementing an objects field value remotely
        extern const char *const OP_INCREMENT;

        // Operation name for decrementing an objects field value remotely
        extern const char *const OP_DECREMENT;

        // The data type name for special JSON objects representing a full object
        extern const char *const TYPE_OBJECT;

        // The data type name for special JSON objects representing a reference
        // to another Parse object.
        extern const char *const TYPE_POINTER;

        // The data type name for special JSON objects containing an array of
        // encoded bytes.
        extern const char *const TYPE_BYTES;

        // The data type name for special JSON objects representing a date/time.
        extern const char *const TYPE_DATE;

        // The data type name for special JSON objects representing a
        // location specified as a latitude/longitude pair.
        extern const char *const TYPE_GEOPOINT;

        // The data type name for special JSON objects representing
        // a file.
        extern const char *const TYPE_FILE;

        // The data type name for special JSON objects representing
        // a Relation.
        extern const char *const TYPE_RELATION;

        // The class name for User objects, when referenced by a Pointer.
        extern const char *const CLASS_USER;

        extern const char *const CLASS_INSTALLATION;

        extern const char *const CLOUD_FUNCTIONS_PATH;

        extern const char *const OBJECTS_PATH;

        extern const char *const BATCH_REQUEST_URI;

        extern const unsigned ERROR_INTERNAL;
        extern const unsigned ERROR_TIMEOUT;
        extern const unsigned ERROR_EXCEEDED_BURST_LIMIT;
        extern const unsigned ERROR_OBJECT_NOT_FOUND_FOR_GET;
        extern const unsigned HTTP_OK;
    }
}