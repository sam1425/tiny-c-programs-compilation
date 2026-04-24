#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

typedef struct {
 	 char *name;
 	 int priority;
 }food;

typedef struct Edge {
    int target_index;
    int weight;
    struct Edge *next;
} Edge;

typedef struct {
    food data;
    Edge *edges;
    int distance;
    bool visited;
} Node;

void add_edge(Node *source, int target_index, int weight) {
    Edge *new_edge = malloc(sizeof(Edge));
    if (!new_edge) return;

    new_edge->target_index = target_index;
    new_edge->weight = weight;
    new_edge->next = source->edges;
    source->edges = new_edge;
}


int main(void)
{
    srand(time(NULL));
    int total_nodes = 12;

    Node nodes[total_nodes];

    for(int i = 0; i < total_nodes;i++){
        nodes[i].edges = NULL;
        nodes[i].distance = INT_MAX;
        nodes[i].visited = false;
    }
    for (int i = 0; i < total_nodes; i++) {
        int num_connections = rand() % 3;
        for (int j = 0; j < num_connections; j++) {
            int target = rand() % total_nodes;
            if (target != i) {
                int weight = (rand() % 50) + 1;
                add_edge(&nodes[i], target, weight);
            }
        }
    }
       
    printf("```mermaid\ngraph TD\n");
    for (int i = 0; i < total_nodes; i++) {
        Edge *curr = nodes[i].edges;

        if (curr == NULL) {
            printf("    Node%d\n", i);
        }
        while (curr) {
            printf("    Node%d -- %d --> Node%d\n", i, curr->weight, curr->target_index);
            curr = curr->next;
        }
    }
    printf("```\n");

    return 0;
}

// void inserttotable(food *table,char *name,int priority,int table_index)
// {
// 	table[table_index].name     = name;
// 	table[table_index].priority = priority;
// }

    //add_edge(&nodes[i], target_node_index, random_weight);
	//food ensalada[3];
	// uint8_t tablesize = sizeof(ensalada)/sizeof(ensalada[0]);
	// inserttotable(ensalada, "lechuga", 3, 0);
	// inserttotable(ensalada, "tomate", 2, 0);
	// printf("Name:%s\npriority: %i\n",ensalada[0].name,ensalada[0].priority);
 
