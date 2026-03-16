#include <ctype.h>
#include <string.h>
#include "html.h"
#include <stdio.h>
#include <stdlib.h>



string make_string(int len, char* src){
    return (string){len, src};
}

string to_string(char* src){
    return (string){strlen(src), src};
}

typedef struct openfile{
    string file;
    size_t file_index;
}openfile;

openfile file;

int eof(){
    return file.file_index > file.file.length;
}


string read_word(openfile* f){

    while(isspace(f->file.value[f->file_index])){
        f->file_index++;
    }    

    string s;
    s.value = f->file.value + f->file_index;
    s.length = 0;

    while(isalpha(f->file.value[f->file_index])){
        f->file_index++;
        s.length++;
    }     

    return s;
}


int compare(string s, char* other){
    if(s.length == 0){
        return -1;
    }
    for (size_t i = 0; i < s.length; i++)
    {
        if(s.value[i] != other[i]){
            return -1;
        }
    }
    return 0;
}

string preproc(string in, char *folder){

    openfile pre;
    pre.file = in;
    pre.file_index = 0;

    while (pre.file_index < pre.file.length){

        while(pre.file_index < pre.file.length && isspace(pre.file.value[pre.file_index])){
            pre.file_index++;
        }
        
        if(pre.file.value[pre.file_index] == '#'){
            int startidx = pre.file_index;
            pre.file_index++;
            string word = read_word(&pre);

            while(isspace(pre.file.value[pre.file_index])){
                pre.file_index++;
            }
            if(compare(word, "include") == 0){
                if(pre.file.value[pre.file_index] != '\"'){
                    //error
                }
                pre.file_index++;
                char* path = pre.file.value + pre.file_index;
                int len = 0;
                
                while(pre.file.value[pre.file_index++] != '\"'){
                    len++;
                }
                path[len] = 0;//its gonna get taken out so replacing the end quote is fine

                char* p = malloc(len + strlen(folder) + 1);
                memcpy(p,folder,strlen(folder));
                memcpy(p+strlen(folder),path,len+1);

                //printf("path: %s\n",p);
                int replaced = pre.file_index - startidx;

                FILE *f = fopen(p,"r");
                if(f == NULL){
                    printf("Failed to open file %s\n",p);
                    pre.file.length = 0;
                    return pre.file;
                }

                free(p);
                fseek(f, 0, SEEK_END);
                long fsize = ftell(f);
                fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

                char *str = malloc(pre.file.length + fsize - replaced + 1); //malloc(fsize + 1);
                memcpy(str,pre.file.value,startidx);
                int count = fread(str + startidx, 1, fsize, f);
                memcpy(str + startidx + count, pre.file.value + pre.file_index, pre.file.length - pre.file_index);
                fclose(f);

                str[count + pre.file.length - replaced] = 0;

                

                free(pre.file.value);
                pre.file.value = str;
                pre.file.length = count + pre.file.length - replaced;

                pre.file_index = startidx;

            }
        }

        while(pre.file_index < pre.file.length && pre.file.value[pre.file_index] != '\n'){
            pre.file_index++;
        }
        pre.file_index++;
    }

    return pre.file;

}

void parse_file(string in, char* folder){
    file.file = preproc(in,folder);
    file.file_index = 0;
    //code_block_count = 0;
    while (file.file_index < file.file.length){
        string word = read_word(&file);

        //html block, eat until it finds an open bracket
        if(compare(word,"html") == 0){

            if(file.file.value[file.file_index++] != '{'){ //invalid html block
                return;
            }

            while (file.file.value[file.file_index] != '}' && !eof())
            {
                char ch = file.file.value[file.file_index++];
                
                switch (ch)
                {
                case '{':
                    execute(file.file,&file.file_index);
                    break;
                
                default:
                    putchar(ch); //print out
                    break;
                }
            }
            
        }
        else{
            file.file_index++;
        }


    }
}