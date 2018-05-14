//
// Created by thadumi on 5/10/18.
//

#include "../../../headers/util/collections/linkedl.h"

void push(struct Node** head_ref, void *new_data, size_t data_size)  {
    // Allocate memory for node
    //printf("new node");
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

    new_node->data  = malloc(data_size);
    new_node->next = NULL;

    //printf("%d %s\n", data_size, (char*) new_data);

    // Copy contents of new_data to newly allocated memory.
    // Assumption: char takes 1 byte.
    for (int i=0; i<data_size; i++) {
        *(char *) (new_node->data + i) = *(char *) (new_data + i);
    }

    if((*head_ref) == NULL) {
        // Change head pointer as new node is added at the beginning
        (*head_ref) = new_node;

    } else {
        struct Node* _node = (*head_ref);
        while (_node->next != NULL) {
            _node = _node->next;
        }

        _node->next = new_node;
    }

}

void consume(struct Node *_node, void (*fptr)(void *, int))  {
    int current_index = 0;
    while (_node != NULL) {
        (*fptr)(_node->data, current_index);
        _node = _node->next;
        current_index++;
    }
}

unsigned int lenght(struct Node* head) {
    if(head == NULL) return 0;

    int size = 1;
    while(head = head->next) size++;

    return size;
}
