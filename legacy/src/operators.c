#include <cparse/types.h>


struct cparse_op_array
{
    const char *operation;
    CPARSE_JSON *objects;
};

struct cparse_op_decrement
{
    int amount;

};

struct cparse_op_increment
{
    int amount;
};


