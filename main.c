#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

char ** read_source(FILE *, int *);
code_t * code_file;

int main(int argc, char * argv[]){
    printf("Simple Compiler\n");
    if (argc < 2){
        printf("Error: Compile need source file as argument\n");
        exit(1);
    }
    FILE *source_file = fopen(argv[1], "r");
    int row_count; 
    char ** source = read_source(source_file, &row_count);
    code_file = initialize_code(source, row_count);
    fclose(source_file);
    parse();
    printf("Parsing completed\n");
}

char ** read_source(FILE * file, int * row_count){
    const int INITIAL_CAP = 80;
    char **source = malloc(INITIAL_CAP * sizeof(*source));
    int current_cap = 80;
    int count = 0;
    char * buf = NULL;
    int n = 0;
    while(getline(&buf, (size_t *) &n, file) != -1){
        if(count == current_cap){
            current_cap += INITIAL_CAP;
            char ** temp = realloc(source, current_cap);
            if(temp == NULL) break; //error allocating more memory stop reading file
            source = temp;
        }
        source[count++] = buf;
        buf = NULL;
    }
    *row_count = count;
    return source;
}

