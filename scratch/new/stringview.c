#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct {
    const char *data;
    size_t count;
} StringView;

#define SV_Fmt "%.*s"
#define SV_Arg(s) (int)(s).count, (s).data

StringView null_to_view(const char *cstr) {
    return (StringView) {
        .data = cstr,
        .count = strlen(cstr),
    };
}

void trim_left(StringView *sv, size_t n){
    if (n > sv->count) n = sv->count;
    sv->count -= n;
    sv->data  += n;
}
void trim_right(StringView *sv, size_t n){
    if (n > sv->count) n = sv->count;
    sv->count -= n;
}
void trim(StringView *sv, size_t n){
    trim_left(sv,1);
    trim_right(sv,1);
}
void trim_space(StringView *sv){
    while(sv->count > 0 && isspace(sv->data[0])){
        trim_left(sv, 1);
    }
}

int main() {
    FILE *f = fopen(__FILE__, "rb");
    size_t filesize = 1024*1024;
    char *buffer = malloc(filesize);
    size_t size = fread(buffer, 1, filesize, f);

    String_View s = {
        .data = buffer,
        .count = size,
    };

    size_t lineNumbers = 0;
    size_t wordCount = 0;
    //this is null terminated
    char *sam = "Hello, world";
    while(s.count > 0 ){
        if (s.count == 0) break;
    }
    StringView sv = null_to_view(sam);
    /*for(int i = 0; i < 2; i++){
        trim(&sv, 1);
    }*/
    printf(SV_Fmt"\n", SV_Arg(sv));
    free(buffer);
    return 0;
}
