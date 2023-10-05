#include "dstr.h"

struct dstring{
    size_t len;
    char * content;
    size_t size;
};

void append_string(struct dstring * dest,  char * source, size_t len){
    if (dest->len + len  >= dest-> size){
        char * new_string = realloc(dest->content, dest->size+MIN_STRING_LENGTH);
        if(!new_string) return;
        dest->content = new_string;
        dest->size += MIN_STRING_LENGTH;
    }
    memcpy(dest->content+dest->len, source, len);
    dest->len += len;
}

struct dstring * create_string(void){
    struct dstring * return_str = malloc(sizeof(struct dstring));
    return_str->size = MIN_STRING_LENGTH;
    return_str->len = 0;
    return_str->content = malloc(MIN_STRING_LENGTH);
    return return_str;
}

size_t len(struct dstring * dstr){
    return dstr->len;
}

char * get_content(struct dstring * dstr){
    return dstr->content;
}

void destroy_string(struct dstring * dstr){
    free(dstr->content);
    memset(dstr, 0, sizeof(struct dstring));
}
