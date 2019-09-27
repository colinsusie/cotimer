#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "clinklist.h"

typedef struct mynode {
    struct mynode *next;
    struct mynode *prev;
    int value;
} mynodet_t;

static void init_my_node(mynodet_t *node, int value) {
    clinklist_init((clinknode_t*)node);
    node->value = value;
}

static void print_my_node(clinknode_t *head) {
    clinknode_t *node = head->next;
    while (node != head) {
        mynodet_t *mynode = (mynodet_t*)node;
        printf("%d ", mynode->value);
        node = node->next;
    }
    printf("\n");
}


int main(int argc, char const *argv[])
{
    clinknode_t head;
    mynodet_t node[10];
    clinklist_init(&head);
    int i;
    for (i = 0; i < 10; ++i) {
        init_my_node(node+i, i);
        clinklist_add_back(&head, (clinknode_t*)(node+i));
    }
    print_my_node(&head);

    for (i = 0; i < 6; ++i) {
        clinklist_remote((clinknode_t*)(node+i));
    }
    print_my_node(&head);
    for (i = 0; i < 6; ++i) {
        clinklist_add_front(&head, (clinknode_t*)(node+i));
    }
    print_my_node(&head);

    clinknode_t head2;
    clinklist_init(&head2);
    clinklist_splice(&head, &head2);
    print_my_node(&head);
    print_my_node(&head2);

    for (i = 0; i < 10; ++i) {
        clinklist_remote(head2.next);
        clinklist_remote(head2.next);
    }
    print_my_node(&head2);
    return 0;
}
