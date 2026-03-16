#include <stdio.h>
#include "html.h"
#include <stdlib.h>
#include <string.h>
int main(int argc, char** argv){
    

    if(argc == 1){
        return 0;
    }
    else{
        FILE *f = fopen(argv[1],"r");
        if(f == NULL){
            printf("Failed to open file %s\n",argv[1]);
            return -1;
        }
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

        char *str = malloc(fsize + 1);
        fread(str, fsize, 1, f);
        fclose(f);

        str[fsize] = 0;

        char* path = strdup(argv[1]);
        char* pLastSlash = strrchr(path, '/');
        if(pLastSlash == NULL){
            pLastSlash = strrchr(path, '\\');
        }
        if(pLastSlash == NULL){
            *path = '\\';
            pLastSlash = path;
        }
        *(pLastSlash + 1) = 0;
        parse_file((string){fsize,str},path);
    }
    
    return 0;
}