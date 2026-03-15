#include <ctype.h>
#include <string.h>
#include "html.h"

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
size_t index;

//size_t code_blocks[256]; //index of code block
//size_t code_block_count;


int eof(){
    return index > file.length;
}


string read_word(){

    while(isspace(file.value[index])){
        index++;
    }    

    string s;
    s.value = file.value + index;
    s.length = 0;

    //either scan a word, or a series of symbols
    if(isalpha(file.value[index])){
        while(isalpha(file.value[index])){
            index++;
            s.length++;
        }        
    }
    else{
        while(!isalpha(file.value[index])){
            index++;
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


string parse_file(string in){
    file = in;
    index = 0;
    //code_block_count = 0;
    while (index < file.length){
        char ch = file.value[index];
        index++;
        string word = read_word();

        //html block, eat until it finds an open bracket
        if(compare(word,"html") == 0){
            while (!eof())
            {
                char ch = file.value[index++];
                switch (ch)
                {
                case '{':
                    execute(file,index);
                    break;
                
                default:
                    break;
                }
            }
            
        }
    }
}