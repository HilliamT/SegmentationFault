#include <stdlib.h>
#include "linked_list.h"

ListEntry* helper_make_ListEntry() {
    ListEntry *entry = malloc(sizeof(ListEntry));
    entry->next = 0;
    entry->data = 0;
    entry->prev = 0;
    return entry;
}

void linked_list_initialize(LinkedList* list) {
    ListEntry *entry = helper_make_ListEntry();
    *list = (LinkedList) {
        .head = entry,
        .tail = entry
    };
}

LinkedList* linked_list_make(void) {
    LinkedList *list = malloc(sizeof(LinkedList));
    ListEntry *entry = calloc(1, sizeof(ListEntry));
    *list = (LinkedList) {
            .head = entry,
            .tail = entry
    };
    return list;
}

ListEntry* linked_list_append(LinkedList* list, void* data) {
    ListEntry *entry = helper_make_ListEntry();

    entry->prev = list->tail;

    list->tail->data = data;
    list->tail->next = entry;
    list->tail = entry;

    return entry;
}

ListEntry* linked_list_get(LinkedList* list, int index) {
    ListEntry *curr = list->head;
    while (index > 0) {
        curr = curr->next;
        index--;
    }
    return curr;
}

void linked_list_delete(LinkedList *list, int index) {
    if (list->head == list->tail) {
        return;
    }
    if (index == 0) {
        ListEntry* temp = list->head;
        list->head = list->head->next;
        free(temp);
        return;
    }

    ListEntry* curr = list->head;
    ListEntry* prev = list->head;
    while (index > 0) {
        prev = curr;
        curr = curr->next;
        index--;
    }
    prev->next = curr->next;
    curr->next->prev = prev;
    free(curr->data);
    free(curr);
    return;
}

void linked_list_destroy(LinkedList* list) {
    ListEntry* curr = list->head;
    ListEntry* temp;
    while (curr != 0) {
        temp = curr->next;
        // free(curr->data);
        free(curr);
        curr = temp;
    }
    free(list);
}

void linked_list_insert_node(ListEntry* position, ListEntry* new_node) {
    ListEntry* en = position->next;
    position->next = new_node;
    new_node->prev = position;
    en->prev = new_node;
    new_node->next = en;
}

void linked_list_insert_data(ListEntry* position, void* data) {
    ListEntry* new = malloc(sizeof(ListEntry));
    new->data = data;
    linked_list_insert_node(position, new);
}

void linked_list_delete_entry(LinkedList* list, ListEntry *entry) {
    if (entry->prev != 0) {
        entry->prev->next = entry->next;
    } else {
        list->head = entry->next;
    }
    if (entry->next != 0) {
        entry->next->prev = entry->prev;
    }
    free(entry);
}
