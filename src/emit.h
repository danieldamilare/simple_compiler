#ifndef _EMIT_H
#define _EMIT_H

typedef struct dstring dstr;
void append_code(dstr *, char *);
void append_codeline(dstr *, char *);
void append_header(dstr *, char *);
void append_headerline(dstr *, char *);
void write_file(char * filepath, dstr * header, dstr * code);

#endif

