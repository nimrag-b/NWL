#include <stdio.h>
#include "html.h"
#include <stdlib.h>
int main(int argc, char** argv){
    //string s = to_string("string s = \"Hello\"; string sa = \"World!\"; int val = 5+2*3; out(s + \" \" + sa + val );");
    //execute(s,0);
    if(argc == 1){
        return 0;
    }
    if(argc == 2){
        FILE *f = fopen(argv[1],"r");
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

        char *str = malloc(fsize + 1);
        fread(str, fsize, 1, f);
        fclose(f);

        str[fsize] = 0;

        parse_file((string){fsize,str});
    }
    
    return 0;
}