#ifndef SLIST_H
#define SLIST_H

#include <stdlib.h>

struct slist {
    struct slist_node* head;
    struct slist_node* tail;
};

struct slist_node {
    char* string;
    struct slist_node* prev;
    struct slist_node* next;
};

/*
 *
 *
 * =============== */

struct
slist* slist_init();

struct slist_node*
slist_node_init(struct slist_node* prev, struct slist_node* next, char* val);

void
slist_push_back(struct slist* sl, char* val);

int
slist_size(struct slist* sl);


void
slist_print(struct slist* sl);

#endif
