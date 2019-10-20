#ifndef LINKED_LIST_LINKED_LIST_H
#define LINKED_LIST_LINKED_LIST_H

typedef struct dummy_test {
    int yum;
} dummy_test;

typedef struct ListEntry {
    void* data;
    struct ListEntry* next;
    struct ListEntry* prev;
} ListEntry;

typedef struct LinkedList {
    ListEntry* head;
    ListEntry* tail;
} LinkedList;


void linked_list_initialize(LinkedList* list);

LinkedList* linked_list_make(void);

ListEntry* linked_list_append(LinkedList* list, void* data);

void linked_list_insert_node(ListEntry* position, ListEntry* new_node);

void linked_list_insert_data(ListEntry* position, void* data);

ListEntry* linked_list_get(LinkedList* list, int index);

void linked_list_delete(LinkedList* list, int index);

void linked_list_delete_entry(LinkedList *list, ListEntry *entry);

void linked_list_destroy(LinkedList* list);


/*
 define linked_list_add_def(TYPE, NAME) \
void linked_list_add_ ## NAME (LinkedList* list, TYPE data)

linked_list_add_def(int, int);
linked_list_add_def(dummy_test, dummy_test);
linked_list_add_def(char*, string);
*/
#endif //LINKED_LIST_LINKED_LIST_H
