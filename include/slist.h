#ifndef SLIST_H
#define SLIST_H

/* SLIST STRUCTURES                                                           */
/* -------------------------------------------------------------------------- */
/* slist (string list) is a double linked list of char*s                      */
/* wrapped in a structure holding a pointer to the list's head and tail.      */
/* ========================================================================== */

struct slist {
    int len;
    struct slist_node* head;
    struct slist_node* tail;
};

struct slist_node {
    char* string;
    struct slist_node* prev;
    struct slist_node* next;
};



/* SLIST FUNCTIONS                                                            */
/* -------------------------------------------------------------------------- */
/* interface focused on raw strings and slists, slist_nodes should not be     */
/* touched outside of this file.                                              */
/* ========================================================================== */

struct slist*
slist_init(void);

void
slist_free(struct slist*);

struct slist_node*
slist_node_init(struct slist_node*, struct slist_node*, const char* str);

void
slist_node_free(struct slist_node*);

void
slist_push_back(struct slist*, const char* str);

void
slist_pop_back(struct slist*);

void
slist_print(struct slist*);

#endif
