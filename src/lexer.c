#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

static void skip_comment(code_t *);
static void next_char(code_t *);
static int check_keyword(char *);
static token_t * initialize_token(char *, t_tok_t);
static char peek(code_t *);
static void skip_white_space(code_t *);

char MESSAGE[BUFSIZ];

code_t * initialize_code(char ** str, int row){

    /*
     * Accept str as ragged string as source and row
     * the nuber of the string in str. create the code
     * structure and initialize the member. Return
     * a pointer to the newly created code_t object
     */

    code_t * code = (code_t *) malloc(sizeof(code_t));
    code->source = str;
    code->row_pos = code->col_pos =0;
    code->row_count = row;
    code->current_char = code->source[0][0];
    return code;
}

void destroy_code(code_t * code){
    for(int i = 0; i < code->row_count; i++){
        free(code->source[i]);
    }
    free(code->source);
    free(code);
}

void destroy_token(token_t * token){
    free(token->token_text);
    free(token);
}

static void next_char(code_t * code){

    /*
     * Accept a pointer to code_t * object and skip to
     * the next character in the source code modifying
     * the pointer to the current character 
     */
    int col = code->col_pos;
    int row = code->row_pos;
    col++;
    if (col >= strlen(code->source[row])){
        if (row+1 >= code->row_count){
            code->current_char = '\0';
            return;
        }
        else{
            code->col_pos=0;
            code->current_char = code->source[++code->row_pos][0];
            return;
        }
    }
    else  code->current_char = code->source[row][++code->col_pos];
}

void err_abort(char * message){
    fprintf(stderr, "\x1b[1mError: %s\x1b[0m\n",message);
    exit(1);
}

static char peek(code_t * code){
    /*
     * look at the next character in the source code without 
     * modifying the pointer to the next character 
     */
    int col_pos = code->col_pos + 1;
    int row_pos = code->row_pos;
    if(col_pos >= strlen(code->source[row_pos])){
        if (row_pos+1 >= code->row_count) return '\0';
        else return code->source[row_pos+1][0];
    }
    else return code->source[row_pos][col_pos];
}

token_t * get_token(code_t * code){

    /*
     * Determine the type of token by checking and processing the
     * current character. the type of tokens are: 
     * operator: +, >, -, *, /, >, >=, <, <=, =, EOF, \n  
     * string: Double quotation followed by zero or more characters
     * and a double quotation
     *  numbers: one or more numeric digit with optional decimal point
     *  identifier: alphabetical character followed by zero or more
     *  alphanumeric character
     *  keyword: Exact match of LABEL, GOTO, PRINT INPUT, THEN, ENDIF
     *  WHILE, LET, IF, REPEAT, ENDWHILE
     */

    skip_white_space(code);
    skip_comment(code);
    token_t * token = NULL;
    char * ch = calloc(50, 1);
    size_t curr_size = 50; //current size of the string length
    sprintf(ch, "%c" ,code->current_char);

    switch(ch[0]){
        case '+':
            token = initialize_token(ch, PLUS);
            break;
        case '-':
            token = initialize_token(ch, MINUS);
            break;
        case '*':
            token = initialize_token(ch, ASTERISK);
            break;
        case '/':
            token = initialize_token(ch, SLASH);
            break;
        case '\n':
            token = initialize_token(ch, NEWLINE);
            break;
        case '\0':
            token = initialize_token(ch, EOFF);
            break;

        case '=':
            if (peek(code) == '='){
                next_char(code);
                ch[1] = code->current_char;
                token = initialize_token(ch, EQEQ);
            }
            else{
                token = initialize_token(ch, EQ);
            }
            break;

        case '>':
            if (peek(code) == '='){
                next_char(code);
                ch[1] = code->current_char;
                token = initialize_token(ch, GTEQ);
            }
            else token = initialize_token(ch, GT);
            break;

        case '<':
            if (peek(code) == '='){
                next_char(code);
                ch[1] = code->current_char;
                token = initialize_token(ch, LTEQ);
            }
            else token = initialize_token(ch, LT);
            break;

        case '!':

            if (peek(code) == '='){
                next_char(code);
                ch[1] = code->current_char;
                token = initialize_token(ch, NOTEQ);
            }

            else{
                sprintf(MESSAGE, "Expected !=, got ! %c", peek(code));
                err_abort(MESSAGE);
            }
            break;

        case '"':
            next_char(code);
            {
            int current_pos = 0;
            while(code->current_char != '"'){

                if (code->current_char == '\r' || code->current_char == '\n'
                    || code->current_char == '\t' || code->current_char =='\\' || code->current_char == '%'){
                    err_abort("Illegal character in string.\n");
                }

                if(current_pos >= curr_size){
                    ch =realloc(ch, curr_size*2);
                    curr_size *= 2;
                }

                ch[current_pos++] = code->current_char;
                next_char(code);
            }
            ch[current_pos] = '\0';
            }
            token = initialize_token(ch, STRING);
            break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            {
                int current_pos = 1;
                while(peek(code) >= '0' && peek(code) <= '9' ){
                    next_char(code);
                    ch[current_pos++] = code->current_char;
                }
                if(peek(code) == '.'){
                    next_char(code);
                    ch[current_pos++] = code->current_char;
                    if (peek(code) > '9' || peek(code)< '0')
                        err_abort("Illegal character in number.\n");
                    while(peek(code)>= '0' && peek(code)<= '9'){
                        next_char(code);
                        ch[current_pos++] = code->current_char;
                    }
                }
                ch[current_pos] = '\0';
            }
            token = initialize_token(ch, NUMBER);
            break;

        case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':case 'g':
        case 'h':case 'i':case 'j':case 'k':case 'l':case 'm':case 'n':
        case 'o':case 'p':case 'q':case 'r':case 's':case 't':case 'u':
        case 'v':case 'w':case 'x':case 'y':case 'z':case 'A':case 'B':
        case 'C':case 'D':case 'E':case 'F':case 'G':case 'H':case 'I':
        case 'J':case 'K':case 'L':case 'M':case 'N':case 'O':case 'P':
        case 'Q':case 'R':case 'S':case 'T':case 'U':case 'V':case 'W':
        case 'X':case 'Y':case 'Z':
            {
                int current_pos = 1;
                while((peek(code)>= 'a' && peek(code) <= 'z') || 
                      (peek(code)>= 'A' && peek(code) <= 'Z') ||
                      (peek(code)>= '0' && peek(code) <= '9')){
                    next_char(code);
                    ch[current_pos++] = code->current_char;
                }
                ch[current_pos] = '\0';
            }
            if (check_keyword(ch)){
                token = initialize_token(ch, check_keyword(ch));
            }
            else token = initialize_token(ch, IDENT);
            break;

        default:
             sprintf(MESSAGE, "%s%s\n", "Unknown token: ", ch);
             err_abort(MESSAGE);

    }
    next_char(code);
    return token;
}

static token_t * initialize_token(char * token_text, t_tok_t token_type){
    token_t * token = malloc(sizeof(token_t));
    token->token_text = token_text;
    token->token_type = token_type;
    return token;
}

static void skip_white_space(code_t * code){
    char ch = code->current_char;
    while(ch == ' ' || ch == '\t' || ch == '\r')
    {
        next_char(code);
        ch = code->current_char;
    }
}

static int check_keyword(char * str){

    const char * KEYWORD[] = { "LABEL", "GOTO", "PRINT", "INPUT", "LET",
                               "IF", "THEN", "ENDIF", "WHILE", "REPEAT",
                               "ENDWHILE"};
    const int KEYWORD_LENGTH = 11;
    for(int i = 0; i < KEYWORD_LENGTH; i++){
        if (strcmp(str, KEYWORD[i]) == 0){
            return 101 + i;
        }
    }
    return 0;
}

static void skip_comment(code_t * code){
    /*
     * Read the source code and skip past the comment. Comment starts 
     * with / * and end with * / 
     */

    if (code->current_char == '/' && peek(code) == '*'){ //check if code is a comment;
        next_char(code); //skip past the comment block
        while(1){
            next_char(code);
            if(code->current_char == '*' && peek(code) == '/'){
                next_char(code); // skip to the / line in the comment block
                next_char(code); // skip past the comment
                break;
            }
        } 
    }
}
