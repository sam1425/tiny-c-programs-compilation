//look up table
//
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_DICCIONARY_NODES 20

typedef struct Node {
  struct node *next;
  int value;
  char *key;
}Node;

Node *table[MAX_DICCIONARY_NODES];


unsigned int hash(int key)
{return (unsigned int)key % MAX_DICCIONARY_NODES}


void insert(int key, const char *value) {
    unsigned int index = hash(key);
    Node *next_node = table[index];

    while(next_node)
    {
        if(next_node->key == key)
        {
            free(next_node->value)
            next_node->value = strdup(value);
            return;
        }
        next_node->next;
    }
    Node *new_node = malloc(sizeof(Node));
    new_node->key = key;
    new_node->value = strdup(value);
    new_node->next = table[index];
    table[index] = new_node;
}


int dicctionary[20];

int main(void)
{
  return 0;
}
