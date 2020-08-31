#include "slist.h"

#include <stdlib.h>
#include <stdio.h>


struct slist*
slist_init()
{
    struct slist* sl = malloc(sizeof(struct slist));
    sl->head = NULL;
    sl->tail = NULL;
    return sl;
}

struct slist_node*
slist_node_init(struct slist_node* prev, struct slist_node* next, char* val)
{
    struct slist_node* node = malloc(sizeof(struct slist_node));
    node->string = val;
    node->prev = prev;
    node->next = next;
    return node;
}

void
slist_push_back(struct slist* sl, char* val)
{
    struct slist_node* sn = slist_node_init(sl->tail, NULL, val);
    if(sl->tail)
        sl->tail->next = sn;
    if(!sl->head)
        sl->head = sn;
    sl->tail = sn;
}

int
slist_size(struct slist* sl)
{
    int num = 0;
    struct slist_node* curr = sl->head;
    while (curr) num++;
    return num;
}


void
slist_print(struct slist* sl)
{
    struct slist_node* curr = sl->head;
    while (curr) {
        printf("%s ", curr->string);
        curr = curr->next;
    }
    printf("\n");
}
