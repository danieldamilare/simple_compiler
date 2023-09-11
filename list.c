#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "list.h"

void print_list(LIST * ll){
    struct list *l = ll->llist;
    while(l->next != NULL){
        printf("%s\n ", l->value);
        l = l->next;
    }
    printf("%s\n", l->value);
}
_list * create_list(char * value){
    struct list * new_list = malloc(sizeof(_list));
    new_list->next = NULL;
    new_list->value = strdup(value);
    return new_list;

}

void append(LIST * list, char * value){
    if(is_in(list, value)) return;
    struct list *new_list = create_list(value);
    if(list->end == NULL){
        list->llist = new_list;
        list->end = new_list;
        return;
    }
    list->end->next = new_list;
    list->end = new_list;
}

LIST * create_llist(void){
    LIST * return_list = malloc(sizeof(LIST));
    return_list->llist = return_list->end = NULL;
    return return_list;
}

bool is_in(LIST * list, char * word){
    _list * inlist = list->llist;

    while(inlist != NULL){
        if(strcmp(inlist->value, word) == 0)
            return true;
        inlist = inlist->next;
    }
    return false;
}

void destroy_list(LIST * list){
    _list * inlist = list->llist;
    _list * next;
    while(inlist != NULL){
        free(inlist->value);
        next = inlist->next;
        free(inlist);
        inlist = next;
    }
}



