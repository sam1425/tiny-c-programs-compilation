#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <regex.h>

int main(void){
    regex_t anormalregex;
    assert(regcomp(&anormalregex, "ab*", REG_EXTENDED) == 0 );

    int result = regexec(&anormalregex,"abb",0,NULL,0);

    if (result == REG_NOERROR){
        printf("match");
    }
    else if (result == REG_NOMATCH){
    }

    return 0;
}
