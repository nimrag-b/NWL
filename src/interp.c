#include "html.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "lang.h"

//#define ERR_NONE
#define ERR_VERBOSE

#ifdef ERR_VERBOSE
    #define errout(...) printf(__VA_ARGS__)    
#else
    #define errout(...) printf("ERROR")    
#endif

int parse_block(code_block *b);

var* parse_index(code_block* b, var* var);
expr parse_function(code_block* b, string ident);
expr call_func(func *func, expr* args, size_t arg_count);

void strip_whitespace(code_block* b){
    while(isspace(b->body.value[b->body_index])){
        b->body_index++;
    }    
}
char eat(code_block* b){
    strip_whitespace(b);
    //putchar(b->body.value[b->body_index]);
    return b->body.value[b->body_index++];
}
char peek(code_block* b){
    strip_whitespace(b);
    return b->body.value[b->body_index];
}
string scan(code_block* b){
    strip_whitespace(b);
    string s;
    s.value = b->body.value + b->body_index;
    s.length = 0;
    while(isalnum(b->body.value[b->body_index])){
        b->body_index++;
        s.length++;
    } 
    return s;
}



unsigned int hash(string str){
    unsigned int h = 0;
    for (size_t i = 0; i < str.length; i++)
    {

        h = (h << 5) + str.value[i];
    }
    return h % INDEX_MAX;
}

var* new_var(code_block* b, string ident, enum var_type type){
    int h = hash(ident);

    b->indexes[h] = b->var_count;
    var* v = b->variables + b->var_count;
    v->ident = ident;
    v->expr.type = type;
    b->var_count++;

    return v;
}

var* new_array(code_block* b, string ident, enum var_type type){
    var* v = new_var(b,ident,VAR_ARRAY);
    v->expr.arr.type = type;
    v->expr.arr.count = 0;
    v->expr.arr.items = NULL;

    return v;
}

var* init_array(var* v, size_t count){
    if(v->expr.arr.items != NULL){
        free(v->expr.arr.items);
    }
    v->expr.arr.count = count;
    v->expr.arr.items = malloc(sizeof(var) * count);
    for (size_t i = 0; i < count; i++)
    {
        v->expr.arr.items[i].expr.type = v->expr.arr.type;
    }
    
    return v;
}


var* get(code_block* b, string ident){
    var* v = b->variables + b->indexes[hash(ident)];

    if(v->expr.type == VAR_ERROR){
        return get(b->parent,ident);
    }

    return v;
}

int is_binary_op(char ch){
    switch (ch)
    {
    case '+':
    case '-':
    case '/':
    case '*':
        return 1;
    
    default:
        return 0;
    }
}



expr parse_literal(code_block* b){
    char ch = peek(b);
    expr ex;
    //printf("literalstart: %c\n",ch);
    //POTENTIAL ERROR - if the number is too large this will overflow.
    if(isdigit(ch)){
        int num = 0;
        while(isdigit(peek(b))){
            num = (num * 10) + (eat(b) - '0');
            if(num < 0){ 
                errout("ERROR: Int too large.\n");
                ex.type = VAR_ERROR;
                return ex;
                //static overflow, throw error
            }
        }

        //check floats
        if(peek(b) == '.'){
            eat(b);
            ex.type = VAR_FLOAT;
            float f = 0;
            int d = 1;
            while(isdigit(peek(b))){

                float v = (float)(eat(b)-'0');
                for (size_t i = 0; i < d; i++)
                {
                    v = v / 10.0f;

                }
                d++;
                f = f + v;
                if(f > 1){
                    errout("ERROR: Float too large.\n");
                    ex.type = VAR_ERROR;
                    return ex;
                }
            }
            if(peek(b) != 'f'){
                errout("ERROR: Expected 'f' at end of float.\n");
                ex.type = VAR_ERROR;
                return ex;
            }
            eat(b);
            ex.fvalue = (float)num + f;
        }
        else{
            ex.type = VAR_INT;
            ex.ivalue = num;
        }

        return ex;
    }

    if(ch == '\"'){
        //handle string
        eat(b);
        ex.type = VAR_STRING;
        char buf[2048];
        int i = 0;
        while(b->body.value[b->body_index] != '\"'){
            if(i >= 2048){
                errout("ERROR: String must not exceed 2048 characters.\n");
                ex.type = VAR_ERROR;
                return ex;
            }
            buf[i++] = b->body.value[b->body_index++];
        }

        if(eat(b) != '\"'){
            errout("ERROR: Invalid string literal.\n");
            ex.type = VAR_ERROR;
            return ex;
        }

        char *c = malloc(sizeof(char) * (i + 1));
        memcpy(c,buf,i);
        c[i] = '\0';
        ex.svalue = (string){i,c};

        return ex;
    }

    if(ch == '\''){
        eat(b);
        ex.type = VAR_CHAR;
        if(b->body.value[b->body_index] != '\\'){ //non escaped
            ex.cvalue = b->body.value[b->body_index++];
        }
        else{
            //handle escape characters
            errout("ERROR: Invalid escape char.\n");
            ex.type = VAR_ERROR;
            return ex;
        }

        if(eat(b) != '\''){
            errout("ERROR: Invalid char literal.\n");
            ex.type = VAR_ERROR;
            return ex;
        }
        return ex;
    }
    
    if(isalpha(ch)){



        string name = scan(b);
        if(peek(b) == '('){
            ex = parse_function(b,name);
        }
        else{

            var *v = get(b,name);
            if(peek(b) == '['){
                v = parse_index(b,v);
            }

            if(v->expr.type == VAR_STRING){
                char* c = malloc(v->expr.svalue.length * sizeof(char));
                memcpy(c,v->expr.svalue.value,v->expr.svalue.length);
                ex.svalue = (string){v->expr.svalue.length,c};
                ex.type = VAR_STRING;
            }
            else{
                ex = v->expr; //copy expr
            }
            
        }
                    
   


        return ex;
    }
    
    errout("ERROR: Invalid literal '%s'.\n",scan(b).value);
    ex.type = VAR_ERROR;
    return ex;
}

expr try_convert(expr ex, enum var_type type){

    if(ex.type == type){
        return ex;
    }
    switch (type)
    {
        case VAR_STRING:
            char *buf;
            int len;
            switch(ex.type){
                case VAR_INT:
                    len = snprintf(NULL, 0, "%i", ex.ivalue);
                    buf = malloc(len + 1);
                    snprintf(buf, 10, "%i", ex.ivalue);
                    ex.svalue = to_string(buf);
                    ex.type = VAR_STRING;
                break;
                case VAR_FLOAT:
                    len = snprintf(NULL, 0, "%g", ex.fvalue);
                    buf = malloc(len + 1);
                    snprintf(buf, len + 1, "%g", ex.fvalue);
                    ex.svalue = to_string(buf);
                    ex.type = VAR_STRING;
                break;
                case VAR_CHAR:
                    buf = malloc(sizeof(char));
                    buf[0] = ex.cvalue;
                    ex.svalue = (string){1,buf};
                    ex.type = VAR_STRING;
                break;

                default:
                    errout("ERROR: Could not convert value to string.\n");
                    ex.type = VAR_ERROR;
                    return ex;
            }
            break;
        case VAR_INT:
            if(ex.type == VAR_FLOAT){ //handle cast to int
                ex.ivalue = (int)ex.fvalue;
                ex.type = VAR_INT;
            }
            else{
                errout("ERROR: Could not cast value to int.\n");
                ex.type = VAR_ERROR;
                return ex;
            }
        break;
        case VAR_FLOAT:
            if(ex.type == VAR_INT){ //handle cast to float
                ex.fvalue = (float)ex.ivalue;
                ex.type = VAR_FLOAT;
            }
            else{
                errout("ERROR: Could not cast value to float.\n");
                ex.type = VAR_ERROR;
                return ex;
            }
        break;
    
    default:
        errout("ERROR: Could not cast value.\n");
        ex.type = VAR_ERROR;
        return ex;
        break;
    }

    return ex;
}

expr parse_binary(code_block* b, int min_prec){
    char ch = peek(b);

    expr lhs;
    if(is_binary_op(ch)){
        switch (ch)
        {
        case '+':
            eat(b);
            lhs = parse_binary(b,7);
            if(lhs.type == VAR_INT){
                lhs.ivalue = +lhs.ivalue;
            }
            else if(lhs.type == VAR_FLOAT){
                lhs.fvalue = +lhs.fvalue;
            }
            else{
                lhs.type = VAR_ERROR;
                return lhs;
            }
            break;
        case '-':
            eat(b);
            lhs = parse_binary(b,7);
            if(lhs.type == VAR_INT){
                lhs.ivalue = -lhs.ivalue;
            }
            else if(lhs.type == VAR_FLOAT){
                lhs.fvalue = -lhs.fvalue;
            }
            else{
                lhs.type = VAR_ERROR;
                return lhs;
            }
            break;
        
        default:
            errout("ERROR: Invalid unary operator. Valid values are '-' and '+'.\n");
            lhs.type = VAR_ERROR;
            return lhs;
        }
    }
    else if(ch == '('){
        eat(b);
        lhs = parse_binary(b, 0);
        if(eat(b) != ')'){
            errout("ERROR: unclosed brackets.\n");
            lhs.type = VAR_ERROR;
            return lhs;
        }
    }
    else{
        lhs = parse_literal(b);
        if(lhs.type == VAR_ERROR){
            return lhs;
        }
    }

    while(1){
        ch = peek(b);

        int bpl = -1;
        int bpr;
        switch (ch)
        {
            case '+':
            case '-':
                bpl = 1;
                bpr = 2;
                break;
            case '*':
            case '/':
                bpl = 5;
                bpr = 6;
                break;
        }
        
        if(bpl == -1){
            break;
        }

        if(bpl < min_prec){
            break;
        }

        eat(b); //eat token
        expr rhs = parse_binary(b,bpr);

        if(rhs.type != lhs.type){
            //this is hacky and probably would be better with some kind of binding power evaluation, 
            //but this is the only case im worried about at the moment, so its fine
            if(rhs.type == VAR_FLOAT && lhs.type == VAR_INT){ 
                lhs = try_convert(lhs,VAR_FLOAT);
            }
            else{
                rhs = try_convert(rhs,lhs.type);
            }
            
        }

        switch (lhs.type)
        {
            case VAR_INT:
                switch (ch)
                {
                    case '+':
                        lhs.ivalue += rhs.ivalue;
                        break;
                    case '-':
                        lhs.ivalue -= rhs.ivalue;
                        break;
                    case '*':
                        lhs.ivalue *= rhs.ivalue;
                        break;
                    case '/':
                        lhs.ivalue /= rhs.ivalue;
                        break;
                    default:
                        break;
                }

            break;

            case VAR_FLOAT:

                switch (ch)
                {
                    case '+':
                        lhs.fvalue += rhs.fvalue;
                        break;
                    case '-':
                        lhs.fvalue -= rhs.fvalue;
                        break;
                    case '*':
                        lhs.fvalue *= rhs.fvalue;
                        break;
                    case '/':
                        lhs.fvalue /= rhs.fvalue;
                        break;
                    default:
                        break;
                }

            break;
            case VAR_STRING:
                if(ch == '+'){
                    char* ns = malloc((lhs.svalue.length + rhs.svalue.length)* sizeof(char));
                    memcpy(ns, lhs.svalue.value, lhs.svalue.length);
                    memcpy(ns + lhs.svalue.length, rhs.svalue.value, rhs.svalue.length);
                    free(lhs.svalue.value);
                    free(rhs.svalue.value);
      
                    lhs.svalue = (string){(lhs.svalue.length+rhs.svalue.length),ns};                    
                }
                else{
                    errout("ERROR: Invalid Operation.\n");
                    lhs.type = VAR_ERROR;
                    return lhs;
                }

            break;
        default:
            break;
        }
    }

    return lhs;
}

var* parse_index(code_block* b, var* var){


    if(var->expr.type != VAR_ARRAY){
        return NULL;
    }

    if(eat(b) != '['){
        errout("ERROR: Invalid indexing.");
        return NULL;
    }

    expr ex = parse_binary(b,0);
    if(ex.type != VAR_INT){
        errout("ERROR: Array index Must be an int.");
        return NULL;
    }
    if(ex.ivalue < 0 || ex.ivalue >= var->expr.arr.count){
        errout("ERROR: Index out of bounds.");
        return NULL;
    }

    if(eat(b) != ']'){
        errout("ERROR: Unclosed square bracket.");
        return  NULL;
    }

    return var->expr.arr.items + ex.ivalue;
}


int parse_assign(code_block* b, var* var){
    if(eat(b) != '='){ //must have an equals here
        return -1;
    }

    if(var->expr.type == VAR_ARRAY){
        if(eat(b) != '['){
            return -1;
        }

        expr ex = parse_binary(b,0);
        if(ex.type != VAR_INT){
            return -1;
        }

        init_array(var,ex.ivalue);

        if(eat(b) != ']'){
            return -1;
        }
        return 0;
    }

    expr ex = parse_binary(b,0);
    if(ex.type == VAR_ERROR){
        return -1;
    }

    if(var->expr.type != ex.type){
        ex = try_convert(ex,var->expr.type);
        if(ex.type == VAR_ERROR){
            return -1;
        }

    }

    if(var->expr.type == VAR_STRING){ //make sure to free the string if its assigned
        if(var->expr.svalue.length > 0){
            free(var->expr.svalue.value);
        }

    }

    var->expr = ex;

    return 0;
}


int parse_index_assign(code_block* b, var* var){
    var = parse_index(b,var);
    if(var == NULL){

        return -1;
    }
    return parse_assign(b,var);
    
}


expr parse_function(code_block* b, string ident){
    expr ex[32];
    expr ret;
    ret.type = VAR_VOID;
    int i = 0;

    if(eat(b) != '('){
        ret.type = VAR_ERROR;
        errout("ERROR: No opening paren for function.\n");
        return ret;
    }
    while (peek(b) != ')')
    {
        ex[i++] = parse_binary(b,0);
        if(peek(b) ==')'){
            break;
        }
        if(peek(b) != ','){
            ret.type = VAR_ERROR;
            errout("ERROR: invalid parameters in function.\n");
            return ret;
        }
        //parse operands
    }


    eat(b); //closing paren

    func* fn = get_func(b,ident);

    if(fn->ident.length != ident.length){
        ret.type = VAR_ERROR;
        errout("ERROR: Invalid function.\n");
        return ret;
    }

    ret = call_func(fn,ex, i);

    return ret;
}

int parse_new(code_block* b, string ident, enum var_type type){

    var* v;
    //array declaration
    if(peek(b) == '['){
        eat(b);
        if(eat(b) != ']'){
            return -1;
        }

        ident = scan(b);

        v = new_array(b,ident,type);
    }
    else{
        v = new_var(b,ident,type);
    }


    if(peek(b) == '='){
        return parse_assign(b,v);
    }

    return 0;
}


int parse_if(code_block* b){
    if(eat(b) != '('){
        return -1;
    }
    expr ex = parse_binary(b,0);

    if(eat(b) != ')'){
        return -1;
    }

    ex = try_convert(ex,VAR_INT);

    if(ex.ivalue != 0){
        parse_block(b);  
    }
    else{
        if(eat(b) != '{'){
            return -1;
        }
        int depth = 1;
        while(depth > 0){
            char ch = eat(b);
            if(ch == '{'){
                depth++;
            }
            if(ch == '}'){
                depth--;
            }
        }
    }

    int idx = b->body_index;
    string word = scan(b);
    if(compare(word,"else") == 0){
        if(ex.ivalue != 0){
            if(eat(b) != '{'){
                return -1;
            }
            int depth = 1;
            while(depth > 0){
                char ch = eat(b);
                if(ch == '{'){
                    depth++;
                }
                if(ch == '}'){
                    depth--;
                }
            }
        }
        else{
            parse_block(b);  
        }
    }
    else{
        b->body_index = idx;
    }

    return 0;
}


int parse_expression(code_block* b, string word){

    int r = 0;

    //printf("word: %s\n",word.value);
    char next = peek(b);
    switch (next)
    {
    case '(':
        if(parse_function(b, word).type == VAR_ERROR){
            r = -1;
        }
        else{
            r = 0;
        }
        return r;
    case '=':
        return parse_assign(b, get(b,word));
    }
    

    
    if(compare(word,"string") == 0){
        return parse_new(b,scan(b),VAR_STRING);
    }
    if(compare(word,"int") == 0){
        return parse_new(b,scan(b),VAR_INT);
    }
    if(compare(word,"float") == 0){
        return parse_new(b,scan(b),VAR_FLOAT);
    }
    if(compare(word,"bool") == 0){
        return parse_new(b,scan(b),VAR_BOOL);
    }
    if(compare(word,"char") == 0){
        return parse_new(b,scan(b),VAR_CHAR);
    }

    if(compare(word,"return") == 0){
        expr ex = parse_binary(b,0);
        if(ex.type == b->return_type){
            b->return_val = ex;
            return 1;
        }
        else{
            errout("ERROR: Invalid return type.\n");
            return -1;
        }
        return 0;
    }
    


    if(next =='['){
        return parse_index_assign(b, get(b,word));          
    }

    //invalid statement
    errout("ERROR: Invalid expression.\n");
    return -1;

}

int parse_statement(code_block* b){

    char ch = peek(b);


    if(ch == '{'){
        return parse_block(b);  
        //b->brackets++;
        //eat(b);
        //return 0;   
    }
    if(ch == '}'){
        b->brackets--;
        eat(b);
        return 0;        
    }
    if(isalpha(ch)){
        string word = scan(b);

        if(compare(word,"if") == 0){
            return parse_if(b);
        }


        int r = parse_expression(b,word);
        if(peek(b) == ';'){
            eat(b);
            return r;
        }
        else{
            errout("ERROR: Missing Semicolon.\n");
            return -1;
        }        
    }

    return 0;
}



void clean_block(code_block *b){
    for (size_t i = 0; i < b->var_count; i++)
    {
        if(b->variables[i].expr.type == VAR_STRING){
            free(b->variables[i].expr.svalue.value);
        }
    }
    
}

int run(code_block *b){

    //printf("running %s\n",b->body.value);
    b->brackets = 1;
    while(b->body_index < b->body.length && b->brackets > 0){
        if(parse_statement(b) < 0){ //error
            //printf("ERROR: Invalid Statement.\n");
            //clean_block(b);
            //return -1;
        }
    }
    clean_block(b);
    return 0;
}

int parse_block(code_block *b){
    if(eat(b) != '{'){
        return -1;
    }
    code_block inner;
    inner.body = b->body;
    inner.body_index = b->body_index;
    inner.parent = b;
    inner.var_count = 0;
    for (size_t i = 0; i < VAR_COUNT; i++)
    {
        inner.variables[i].expr.type = VAR_ERROR;
    }
    for (size_t i = 0; i < INDEX_MAX; i++)
    {
        inner.funcs.functions[i].return_type = VAR_ERROR;
    }
    int r = run(&inner);
    b->body_index = inner.body_index;
    return r;
}


expr call_func(func *func, expr* args, size_t arg_count){



    if(arg_count != func->arg_count){
        errout("ERROR: Incorrect number of arguments in function.\n");
        expr err;
        err.type = VAR_ERROR;
        return err;
    }

    for (size_t i = 0; i < arg_count; i++)
    {
        if(args[i].type != func->args[i]){
            args[i] = try_convert(args[i],func->args[i]);
            if(args[i].type == VAR_ERROR){
                errout("ERROR: Incorrect type for argument %u\n",i);
                expr err;
                err.type = VAR_ERROR;
                return err;                
            }
        }

    }
    

    if(func->internal){
        return func->internal_func(args);
    }

    code_block *b = func->body;
    b->body_index = 0;
    if(run(b) == -1){ //failed
        b->return_val.type = VAR_ERROR;
    }
    return b->return_val;
}

int execute(string s, size_t *index){
    code_block b;
    b.body = s;
    b.body_index = *index;
    b.var_count = 0;
    for (size_t i = 0; i < VAR_COUNT; i++)
    {
        b.variables[i].expr.type = VAR_ERROR;
    }

    for (size_t i = 0; i < INDEX_MAX; i++)
    {
        b.funcs.functions[i].return_type = VAR_ERROR;
    }
    
    b.parent = NULL;
    make_internal(&b.funcs);
    int r = run(&b);
    *index = b.body_index;
    return r;
}