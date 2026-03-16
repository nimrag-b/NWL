#include <stddef.h>
#include "lang.h"
#include <stdio.h>
#include "html.h"
#include <time.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

expr out_internal(expr* args){
    for (size_t i = 0; i < args[0].svalue.length; i++)
    {
        putchar(args[0].svalue.value[i]);
    }

    expr r;
    r.type = VAR_VOID;
    return r;
}

expr outln_internal(expr* args){
    for (size_t i = 0; i < args[0].svalue.length; i++)
    {
        putchar(args[0].svalue.value[i]);
    }
    putchar('\n');

    expr r;
    r.type = VAR_VOID;
    return r;
}

expr datetime_internal(expr* args){
    // Get current time
    time_t t = time(NULL);

    char* time = asctime(localtime(&t));
    size_t len = strlen(time) + 1;
    char* buf = malloc(len* sizeof(char));
    // Get the string of local time
    snprintf(buf,len,"%s",time);
    expr r;
    r.type = VAR_STRING;
    r.svalue = (string){len,buf};
    return r;
}


unsigned int hash_func(string str){
    unsigned int h = 0;
    for (size_t i = 0; i < str.length; i++)
    {
        h = (h << 5) + str.value[i];
    }
    return h % INDEX_MAX;
}

void add_func(func_table* table, func func){
    int h = hash_func(func.ident);

    table->functions[h] = func;
}


func* get_func(code_block* b, string ident){
    func* fn = b->funcs.functions + hash_func(ident);
    if(fn->return_type == VAR_ERROR){
        return get_func(b->parent,ident);
    }
    return fn;
}


void make_internal(func_table* table){
    func outfunc;
    outfunc.arg_count = 1;
    outfunc.args[0] = VAR_STRING;
    outfunc.ident = to_string("out");
    outfunc.return_type = VAR_VOID;
    outfunc.internal = 1;
    outfunc.internal_func = out_internal;

    add_func(table, outfunc);

    func outlnfunc;
    outlnfunc.arg_count = 1;
    outlnfunc.args[0] = VAR_STRING;
    outlnfunc.ident = to_string("outln");
    outlnfunc.return_type = VAR_VOID;
    outlnfunc.internal = 1;
    outlnfunc.internal_func = outln_internal;

    add_func(table, outlnfunc);

    func dtfunc;
    dtfunc.arg_count = 0;
    dtfunc.ident = to_string("datetime");
    dtfunc.return_type = VAR_STRING;
    dtfunc.internal = 1;
    dtfunc.internal_func = datetime_internal;

    add_func(table, dtfunc);
}