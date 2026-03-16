#pragma once

typedef struct string{
    int length;
    char* value;
}string;

enum var_type {
    VAR_ERROR,
    VAR_VOID,
    VAR_INT,
    VAR_FLOAT,
    VAR_BOOL,
    VAR_STRING,
    VAR_CHAR,
    VAR_ARRAY
};

typedef struct expr{
    enum var_type type;
    union
    {
        int ivalue;
        float fvalue;
        char cvalue;
        string svalue;
        struct arr{
            enum var_type type;
            size_t count;
            struct var* items;
        } arr;
    };
    
} expr;



typedef struct var{
    string ident;
    expr expr;
} var;

#define VAR_COUNT 256
#define INDEX_MAX 2048


#define MAX_ARGS 32

typedef struct code_block code_block;

typedef struct func{
    string ident;
    int internal;
    enum var_type return_type;
    enum var_type args[MAX_ARGS];
    size_t arg_count;

    union data
    {
        code_block *body;
        expr (*internal_func)(expr* expr);
    };
    
    
}func;

typedef struct func_table
{
    func functions[INDEX_MAX];
}func_table;





typedef struct code_block{
    string body;
    size_t body_index;
    int indexes[INDEX_MAX];
    int var_count;
    var variables[VAR_COUNT];
    int brackets;
    func_table funcs;
    enum var_type return_type;
    expr return_val;

    code_block* parent;
}code_block;

