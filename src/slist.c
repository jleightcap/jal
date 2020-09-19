#include "slist.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct slist*
slist_init(void)
{
    struct slist* sl;
    sl = malloc(sizeof(struct slist));
    sl->len = 0;
    sl->head = NULL;
    sl->tail = NULL;
    return sl;
}

void
slist_free(struct slist* sl)
{
    while (sl->len > 0) {
        slist_pop_back(sl);
    }
    free(sl);
}


struct slist_node*
slist_node_init(struct slist_node* prev, struct slist_node* next, const char* val)
{
    struct slist_node* node;
    node = malloc(sizeof(struct slist_node));
    node->string = malloc(strlen(val) * sizeof(char));
    memcpy(node->string, val, strlen(val));
    node->prev = prev;
    node->next = next;
    return node;
}

void
slist_node_free(struct slist_node* sn)
{
    free(sn->string);
    free(sn);
}

void
slist_push_back(struct slist* sl, const char* val)
{
    struct slist_node* sn;
    sn = slist_node_init(sl->tail, NULL, val);
    if(sl->tail) {
        sl->tail->next = sn;
    }
    if(!sl->head) {
        sl->head = sn;
    }
    sl->tail = sn;
    sl->len++;
}

void
slist_pop_back(struct slist* sl)
{
    assert(sl->len > 0);
    struct slist_node* tmp;
    tmp = sl->tail;
    if(tmp->prev) {
        sl->tail = tmp->prev;
        sl->tail->next = NULL;
    }
    sl->len--;
    slist_node_free(tmp);
}

void
slist_print(struct slist* sl)
{
    if (sl->len == 0) return;
    struct slist_node* curr;
    curr = sl->head;
    for(; curr; curr = curr->next) {
        printf("\'%s\', ", curr->string);
    }
    printf("\n");
}
