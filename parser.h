#ifndef _PARSER_H

#define _PARSER_H
#include "lexer.h"
void run_program(void);
void parse(void);

/* external variable */
extern code_t * codefile; // structure for the source code and its current position
/** end of external variable */

#endif
