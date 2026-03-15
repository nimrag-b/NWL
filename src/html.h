#pragma once
#include <stddef.h>


typedef struct string{
    int length;
    char* value;
}string;

int execute(string s, size_t *index);
int compare(string s, char* other);

string to_string(char* src);
void parse_file(string in);