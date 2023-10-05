#ifndef _LEXER_H

#define _LEXER_H
typedef struct code_str{
    char ** source; //pointer to the string that store the source code
    int row_pos, col_pos; // current position of the current pointer in the string
    int row_count; //line number of the source code 
    char current_char; //current characeter to be processed by the lexer
} code_t;

typedef enum token_type{
    EOFF = -1,
    NEWLINE = 0,
    NUMBER = 1,
    IDENT = 2,
    STRING = 3,
    
    //KEYWORDS
    LABEL = 101,
    GOTO = 102,
    PRINT = 103,
    INPUT = 104,
    LET = 105,
    IF = 106,
    THEN = 107,
    ENDIF = 108,
    WHILE = 109,
    REPEAT = 110,
    ENDWHILE = 111,

    //OPERATORS.
    EQ = 201,
    PLUS = 202,
    MINUS = 203,
    ASTERISK = 204,
    SLASH = 205,
    EQEQ = 206,
    NOTEQ = 207,
    LT = 208,
    LTEQ = 209,
    GT = 210,
    GTEQ = 211
} t_tok_t; // token type enumerator

typedef struct token {
    char * token_text; // pointer to store the token text
    t_tok_t token_type;
    
} token_t;
/*** function declaration ***/
code_t * initialize_code(char **, int);
token_t * get_token(code_t *);
void destroy_code(code_t * code);
void destroy_token(token_t * token);
void err_abort(char * message);

/*** end of function declaration **/

extern char MESSAGE[];
#endif
