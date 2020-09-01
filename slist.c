#include "slist.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct slist*
slist_init(void)
{
    struct slist* sl = malloc(sizeof(struct slist));
    sl->len = 0;
    sl->head = NULL;
    sl->tail = NULL;
    return sl;
}

void
slist_free(struct slist* sl)
{
    while (sl->len > 0)
        slist_pop_back(sl);
    free(sl);
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
slist_node_free(struct slist_node* sn)
{
    free(sn);
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
    sl->len++;
}

char*
slist_pop_back(struct slist* sl)
{
    assert(sl->len > 0);
    struct slist_node* tmp = sl->tail;
    char* tmpstr = tmp->string;
    if(sl->tail->prev)
        sl->tail = sl->tail->prev;
    sl->tail->next = NULL;
    free(tmp);
    sl->len--;
    return tmpstr;
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
