#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "emit.h"
#include "dstr.h"
#include <string.h>


void append_code(dstr * code, char * statement){
    int len = strlen(statement);
    append_string(code, statement, len);
}

void append_codeline(dstr * code, char * statement){
    int len = strlen(statement);
    append_string(code, statement,len);
    append_string(code, "\n", 1);
}

void append_header(dstr * code, char * statement){
    append_code(code, statement);
}
void append_headerline(dstr * code, char * statement){
    append_codeline(code, statement);
}

void write_file(char * filepath, dstr * header, dstr * code){
    FILE * fl = fopen(filepath, "w");
    if(fl == NULL){
        err_abort("Error compiling....\n");
    }
    fwrite(get_content(header), 1, len(header), fl);
    fwrite(get_content(code), 1, len(code), fl);
    fclose(fl);

}
