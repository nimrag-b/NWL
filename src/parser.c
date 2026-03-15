#include <ctype.h>
#include <string.h>
#include "html.h"
#include <stdio.h>

string make_string(int len, char* src){
    return (string){len, src};
}

string to_string(char* src){
    return (string){strlen(src), src};
}

/*
tag *id_table[1024];

unsigned int hash(const char *str){
    unsigned int h = 0;
    while (*str){
        h = (h <<5) + *str++;
    }
    return h % 1024;
}

void insert_id(tag tag){
    int h = hash(tag.id);

}
*/

string file;
size_t file_index;

int eof(){
    return file_index > file.length;
}


string read_word(){

    while(isspace(file.value[file_index])){
        file_index++;
    }    

    string s;
    s.value = file.value + file_index;
    s.length = 0;

    //either scan a word, or a series of symbols
    if(isalpha(file.value[file_index])){
        while(isalpha(file.value[file_index])){
            file_index++;
            s.length++;
        }        
    }
    else{
        while(!isalpha(file.value[file_index])){
            file_index++;
            s.length++;
        } 
    }

    return s;
}


int compare(string s, char* other){
    for (size_t i = 0; i < s.length; i++)
    {
        if(s.value[i] != other[i]){
            return -1;
        }
    }
    return 0;
}


void parse_file(string in){
    file = in;
    file_index = 0;
    //code_block_count = 0;
    while (file_index < file.length){
        string word = read_word();

        //html block, eat until it finds an open bracket
        if(compare(word,"html") == 0){

            if(file.value[file_index++] != '{'){ //invalid html block
                return;
            }

            while (file.value[file_index] != '}' && !eof())
            {
                char ch = file.value[file_index++];
                
                switch (ch)
                {
                case '{':
                    execute(file,&file_index);
                    break;
                
                default:
                    putchar(ch); //print out
                    break;
                }
            }
            
        }


    }
}