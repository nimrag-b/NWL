#pragma once


typedef struct string{
    int length;
    char* value;
}string;

int execute(string s, int index);
int compare(string s, char* other);

string to_string(char* src);
