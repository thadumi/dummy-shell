//
// Created by thadumi on 5/10/18.
//

#ifndef SHELL_LINKEDL_H
#define SHELL_LINKEDL_H

#include<stdio.h>
#include<stdlib.h>

/* A linked list node */
struct Node  {
    // Any data type can be stored in this node
    void  *data;

    struct Node *next;
};

typedef struct Node* node;

#define llforeach(var, head) node var = (head); \
    while(var != NULL)

/* Function to add a node at the beginning of Linked List.
   This function expects a pointer to the data to be added
   and size of the data type */
void push(struct Node** head_ref, void *new_data, size_t data_size);

/* Function to print nodes in a given linked list. fpitr is used
   to access the function to be used for printing current node data.
   Note that different data types need different specifier in printf() */
void consume(struct Node *node, void (*fptr)(void *, int));

unsigned int lenght(struct Node* head);
#endif //SHELL_LINKEDL_H
