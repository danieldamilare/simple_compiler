#ifndef DSTR_H
#define DSTR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MIN_STRING_LENGTH 100

struct dstring;
void append_string(struct dstring * dest, char * source, size_t len);
struct dstring * create_string(void);
size_t len(struct dstring * dstr);
char * get_content(struct dstring * dstr);
void destroy_string(struct dstring * dstr);

#endif

