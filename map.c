//
// Created by jonat on 30/05/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked_list.h"
#include "map.h"

Map* map_make() {

    Map* map = malloc(sizeof(Map));
    map->root = 0;
    return map;
}

Node* map_get_node(Map* map, char* key) {
    Node *curr = map->root;
    while (1) {
        if (curr == 0) {
            return 0;
        } else {
            int i = strcmp(key, curr->key);
            if (i == 0) {
                return curr;
            } else if (i < 0) {
                curr = curr->left_child;
            } else {
                curr = curr->right_child;
            }
        }
    }
}


void* map_get(Map *map, char *key) {
    Node* node = map_get_node(map, key);
    if (node == 0) {
        return 0;
    }
    return node->element;
}

void map_iterate(Map* map, void (*iter)(char *key, void *element, void *context), void *context) {
    if (map->root == 0) {
        return;
    }
    LinkedList* list = linked_list_make();
    linked_list_append(list, map->root);
    while (list->head != list->tail) {
        Node *node = (Node *)linked_list_get(list, 0)->data;
        iter(node->key ,node->element, context);
        if (node->right_child != 0) {
            linked_list_append(list, node->right_child);
        }
        if (node->left_child != 0) {
            linked_list_append(list, node->left_child);
        }
        linked_list_delete_entry(list, linked_list_get(list, 0));
    }
    linked_list_destroy(list);
}

void test_print(char *key, void *elem, void *context) {
    printf("[%s : %p] :   %d\n", key, elem, *(int*)elem);
}

void map_print_all(Map *map) {
    map_iterate(map, &test_print, 0);
}


void map_add_node(Map* map, Node *node) {
    if (map->root == 0) {
        map->root = node;
        return;
    }

    Node** curr = &(map->root);

    while ((*curr) != 0) {
        if (strcmp(node->key, (*curr)->key) < 0) {
            curr = &(*curr)->left_child;
        } else if (strcmp(node->key, (*curr)->key)) {
            curr = &(*curr)->right_child;
        }
    }
    *curr = node;
}

Node* node_make(char *key, void *element) {
    Node* node = malloc(sizeof(Node));
    char *key_copy = malloc((strlen(key) + 1) * sizeof(char));
    strcpy(key_copy, key);
    *node = (Node) {
        .key = key_copy,
        .element = element,
        .left_child = 0,
        .right_child = 0
    };
    return node;
}

void map_add(Map* map, char *key, void *element) {
    map_add_node(map, node_make(key, element));
}


void map_destroy_helper(Node *node) {
    if (node != 0) {
        map_destroy_helper(node->left_child);
        map_destroy_helper(node->right_child);
        free(node->key);
        free(node);
    }
}

void map_destroy(Map* map) {
    map_destroy_helper(map->root);
    free(map);
}
