#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "parser.h"
#include "dstr.h"
#include "emit.h"
#include "list.h"

typedef struct curtok{
    token_t * current;
    token_t * peek;
} curtok;


curtok * create_curtok(void);
bool check_peek(t_tok_t);
bool check_token(t_tok_t);
void next_token(void);
void match(t_tok_t);
void statement(void);
void new_line(void);
void expression(void);
void comparison(void);
bool is_operator(void);
void term(void);
void unary(void);
void primary(void);
void  append(LIST *, char *);


extern code_t * code_file;
static curtok * tok = NULL;
static LIST * symbol_table = NULL;     // keep track of variable declared
static LIST * declared_label = NULL;   //keep track of label
static LIST * label_goto = NULL;      //keep of all label goto. To avoid going to non-existing label
static struct dstring * gen_code;    // store the generated c code
static struct dstring * gen_header;  // store the header file adn variable declaration for the c code

extern char MESSAGE[];

curtok * create_curtok(void){
    struct curtok * tok = malloc(sizeof(*tok));
    return tok;
}

bool check_token(t_tok_t token_type){
    return tok->current->token_type == token_type;
}

bool check_peek(t_tok_t token_type){
    return tok->peek->token_type == token_type;
}

void next_token(void){
    if (tok == NULL){
        tok = create_curtok();
        tok->current = get_token(code_file);
        tok->peek = get_token(code_file);
    }
    else{
        destroy_token(tok->current);
        tok->current = tok->peek;
        tok->peek = get_token(code_file);
    }
}

void match(t_tok_t token_type){
    if (check_token(token_type) == false){
        sprintf(MESSAGE, "Expected %d got %d\n %s\n", token_type, tok->current->token_type, tok->current->token_text);
        err_abort(MESSAGE);
    }
    next_token();
}

void run_program(void){
    printf("PROGRAM\n");
    append_headerline(gen_header, "#include <stdio.h>");
    append_headerline(gen_header, "int main(void){");

    //skip beginning newline
    while(check_token(NEWLINE))
        next_token();

    // parse all the statements in the program.
    while(check_token(EOFF) == false){
        statement();
    }

    append_codeline(gen_code, "return 0;");
    append_codeline(gen_code, "}");
    _list * goto_list = label_goto->llist;

    //check that all label referenced in a GOTO is declared.
    while(goto_list != NULL){
        if(is_in(declared_label, goto_list->value) == false){
            sprintf(MESSAGE, "Attempting to GOTO to undeclared label: %s\n",
                    goto_list->value);
            err_abort(MESSAGE);
        }
        goto_list = goto_list->next;
    }
}

void statement(void){

    // "PRINT" (expression | string)
    if(check_token(PRINT)){
        /* printf("STATEMENT-PRINT\n"); */
        next_token();

        if(check_token(STRING))
        {
            append_code(gen_code, "printf(\"%s\\n\",");
            append_code(gen_code, "\"");
            append_code(gen_code, tok->current->token_text);
            append_code(gen_code, "\"");
            append_codeline(gen_code, ");");
            next_token();
        }
        else
        {
            //Expect an expression and print the result as a float
            append_code(gen_code, "printf(\"%.2f\\n\", (float)(");
            expression();
            append_codeline(gen_code, "));");
        }
    }
    else if(check_token(IF)){
        /* printf("STATEMENT-IF\n"); */
        next_token();
        append_code(gen_code, "if(");
        comparison();
        match(THEN);
        new_line();
        append_codeline(gen_code, "){");

        // Zero or more statements in the body;
        while(check_token(ENDIF) == false){
            statement();
        }
        match(ENDIF);
        append_codeline(gen_code, "}");
    }

    else if (check_token(WHILE)){
        /* printf("STATEMENT-WHILE\n"); */
        next_token();
        append_code(gen_code, "while(");
        comparison();
        match(REPEAT);
        new_line();
        append_codeline(gen_code, "){");

        // Zero or more statements in the loop body.
        while(check_token(ENDWHILE)  == false)
            statement();
        match(ENDWHILE);
        append_codeline(gen_code, "}");
    }

    else if(check_token(LABEL)){
        /* printf("LABEL STATEMENT\n"); */
        next_token();

        // Make sure this label doesn't already exist.
        if(is_in(declared_label, tok->current->token_text)){
            sprintf(MESSAGE, "Label already exists: %s\n", tok->current->token_text);
            err_abort(MESSAGE);
        }
        append(declared_label, tok->current->token_text);

        append_code(gen_code, tok->current->token_text);
        append_codeline(gen_code, ":");
        match(IDENT);
    }

    else if(check_token(GOTO)){ 
        /* printf("STATEMENT-GOTO\n"); */
        next_token();
        append(label_goto, tok->current->token_text);
        append_code(gen_code, "goto ");
        append_code(gen_code, tok->current->token_text);
        append_codeline(gen_code, ";");
        match(IDENT);
    }
    else if(check_token(LET)){
        /* printf("STATEMENT-LET\n"); */
        next_token();

        // check if ident exist in symbol table. If not, declare it.
        if(!is_in(symbol_table, tok->current->token_text))
        {
            append(symbol_table, tok->current->token_text);
            append_header(gen_header, "float ");
            append_header(gen_header, tok->current->token_text);
            append_headerline(gen_header, ";");

        }

        append_code(gen_code, tok->current->token_text);
        append_code(gen_code, " = ");
        match(IDENT);
        match(EQ);
        expression();
        append_codeline(gen_code, ";");
    }
    else if(check_token(INPUT)){
        /* printf("STATEMENT-INPUT\n"); */
        next_token();

        //if variable doesn't already exist, declare it
        if(!is_in(symbol_table, tok->current->token_text))
        {
            append(symbol_table, tok->current->token_text);
            append_header(gen_header, "float ");
            append_header(gen_header, tok->current->token_text);
            append_headerline(gen_header, ";");
        }

        //Emit scan and validate input. If input is invalid 
        // set the variable to 0 and clear the input.
        append_code(gen_code, "if(0 == scanf(\"%f\", &");
        append_code(gen_code, tok->current->token_text);
        append_codeline(gen_code, ")) {");
        append_code(gen_code, tok->current->token_text);
        append_codeline(gen_code, " =0;");
        append_codeline(gen_code, "scanf(\"%*s\");");
        append_codeline(gen_code, "}");
        match(IDENT); 
    }
    else{
        sprintf(MESSAGE, "Invalid statement at %s (%d)", 
                tok->current->token_text, tok->current->token_type);
        err_abort(MESSAGE);
    }
    new_line();
}

void comparison(void){
    /* printf("COMPARISON\n"); */
    expression();

    //There must be at least one comparison operator and another expression
    if (is_operator()){
        append_code(gen_code, tok->current->token_text);
        next_token();
        expression();
    }
    else{
        //error
        sprintf(MESSAGE, "Expected comparison operator at %s\n",
                tok->current->token_text);
        err_abort(MESSAGE);
    }

    //can have 0 or more comparison operator and expressions.
    while(is_operator()){
        /* printf("OPERATOR: %s\n", tok->current->token_text); */
        append_code(gen_code, tok->current->token_text);
        next_token();
        expression();
    }
}


bool is_operator(void){
    return (check_token(GTEQ) ||
            check_token(GT)   ||
            check_token(LT)   ||
            check_token(LTEQ) ||
            check_token(EQEQ) ||
            check_token(NOTEQ));
} 

void expression(void){
    /* printf("EXPRESSION\n"); */
    term();

    //can have 0 or more +/- and expression
    while(check_token(PLUS) || check_token(MINUS)){
        append_code(gen_code, tok->current->token_text);
        next_token();
        term();
    }
}

void term(void){
    unary();

    //can have 0 or more * and expression
    while(check_token(ASTERISK) || check_token(SLASH)){
        append_code(gen_code, tok->current->token_text);
        next_token();
        unary();
    }
}

void unary(void){
    // optional +/-
    if(check_token(PLUS) ||check_token(MINUS))
    {
        append_code(gen_code, tok->current->token_text);
        next_token();
    }
    primary();
}

void primary(void){
    if(check_token(NUMBER))
    {
        append_code(gen_code, tok->current->token_text);
        next_token();
    }
    else if (check_token(IDENT)){
        //ensure the variable already exists
        if(!is_in(symbol_table, tok->current->token_text)){
            sprintf(MESSAGE, "Referencing variable before assignment: %s",
                    tok->current->token_text);
            err_abort(MESSAGE);
        }
        append_code(gen_code, tok->current->token_text);
        next_token();
    }
    else{
        sprintf(MESSAGE, "Unexpected token at %s", tok->current->token_text);
    }
}

void new_line(void){
    match(NEWLINE);
    while(check_token(NEWLINE))
        next_token();
}

void parse(void){
    next_token();
    symbol_table = create_llist();
    declared_label = create_llist();
    label_goto = create_llist();
    gen_code = create_string();
    gen_header = create_string();
    run_program();
    write_file("out.c", gen_header, gen_code);
    system("cc -o out -g out.c");
    destroy_list(symbol_table);
    destroy_list(declared_label);
    destroy_list(label_goto);
}
