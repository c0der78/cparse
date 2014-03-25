#ifndef CPARSE_OPERATOR_H_
#define CPARSE_OPERATOR_H_


#ifdef __cplusplus
extern "C" {
#endif

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


#ifdef __cplusplus
}
#endif

#endif
