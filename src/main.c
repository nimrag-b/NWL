#include <stdio.h>
#include "html.h"

int main(){
    string s = to_string("string s = \"Hello\"; string sa = \"World!\"; int val = 5+2*3; out(s + \" \" + sa + val );");
    execute(s,0);
    return 0;
}