#pragma once
#include <stddef.h>
#include "lang.h"



int execute(string s, size_t *index);
int compare(string s, char* other);

string to_string(char* src);
void parse_file(string in, char* folder);

void make_internal(func_table* table);
func* get_func(code_block* b, string ident);
void add_func(func_table* table, func func);