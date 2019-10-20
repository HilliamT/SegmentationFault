//
// Created by jonat on 30/05/2019.
//

#ifndef LINKED_LIST_HASH_TABLE_H
#define LINKED_LIST_HASH_TABLE_H

typedef struct Node {
    char *key;
    void *element;
    struct Node *right_child;
    struct Node *left_child;
} Node;

typedef Node Root;

typedef struct Map {
    Root* root;
} Map;

Map* map_make();

void map_iterate(Map* map, void (*iter)(char *key, void *element, void *context), void *context);

void map_add_node(Map* map, Node *node);

void map_add(Map* map, char *key, void *element);

Node* map_get_node(Map* map, char* key);

void* map_get(Map* map, char* key);

Node* node_make(char *key, void *element);

void map_print_all(Map* map);

void map_destroy(Map* map);

void map_delete(Map* map, char* key);

#endif //LINKED_LIST_HASH_TABLE_H
