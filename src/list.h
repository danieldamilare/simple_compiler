#ifndef _LIST_H
#define _LIST_H

typedef struct list{
    char * value;
    struct list * next;
} _list;

typedef struct llist{
    _list * llist;
    _list * end;
} LIST;

void print_list(LIST *);
_list * create_list(char *);
void append(LIST * list, char * value);
LIST  * create_llist(void);
bool is_in(LIST * list, char * word);
void destroy_list(LIST *);

#endif



