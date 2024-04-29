#ifndef CIRCULARLIST_H
#define CIRCULARLIST_H
#include "headers.h"


struct Node
{
    struct process data;
    struct Node *next;
    struct Node *prev;
};

struct CircularList
{
    struct Node *head;
    struct Node *tail;
    struct Node *current;
    int size;
};

struct CircularList *createCircularList() {
    struct CircularList *list = (struct CircularList *)malloc(sizeof(struct CircularList));
    if (list == NULL) {
        // Handle memory allocation error
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->size = 0;
    return list;
}

void insertAtBeginning(struct CircularList *list, struct process newData) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        // Handle memory allocation error
        return;
    }
    newNode->data = newData;
    if (list->head == NULL) {
        newNode->next = newNode;
        newNode->prev = newNode;
        list->head = newNode;
        list->tail = newNode;
        list->current = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = list->tail;
        list->head->prev = newNode;
        list->tail->next = newNode;
        list->head = newNode;
    }
    list->size++;
}

void insertAtEnd(struct CircularList *list, struct process newData) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        // Handle memory allocation error
        return;
    }
    newNode->data = newData;
    if (list->head == NULL) {
        newNode->next = newNode;
        newNode->prev = newNode;
        list->head = newNode;
        list->tail = newNode;
        list->current = newNode;
    } else {
        newNode->next = list->head;
        newNode->prev = list->tail;
        list->head->prev = newNode;
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->size++;
}

void deleteAtBeginning(struct CircularList *list) {
    if (list->head == NULL) {
        // List is empty
        return;
    }
    if (list->head == list->tail) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
        list->current = NULL;
    } else {
        struct Node *temp = list->head;
        list->head = list->head->next;
        list->head->prev = list->tail;
        list->tail->next = list->head;
        free(temp);
    }
    list->size--;
}

void deleteAtEnd(struct CircularList *list) {
    if (list->head == NULL) {
        // List is empty
        return;
    }
    if (list->head == list->tail) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
        list->current = NULL;
    } else {
        struct Node *temp = list->tail;
        list->tail = list->tail->prev;
        list->tail->next = list->head;
        list->head->prev = list->tail;
        free(temp);
    }
    list->size--;
}

int isEmpty(struct CircularList *list) {
    return list->size == 0;
}

int getSize(struct CircularList *list) {
    return list->size;
}

void displayList(struct CircularList *list) {
    if (list->head == NULL) {
        printf("List is empty\n");
        return;
    }
    struct Node *current = list->head;
    printf("Nodes of the circular doubly linked list: \n");
    do {
        // Print current node data
        struct process p = current->data;
        printf("ID: %d, PID: %d, Arrival Time: %d\n", p.id, p.pid, p.arrivaltime); // Modify this to print your process data
        // Move to next node
        current = current->next;
    } while (current != list->head);
    printf("\n");
}

void destroyList(struct CircularList *list) {
    if (list->head == NULL) {
        free(list);
        return;
    }
    struct Node *current = list->head;
    struct Node *next;
    do {
        next = current->next;
        free(current);
        current = next;
    } while (current != list->head);
    free(list);
}

void changeCurrent(struct CircularList *list) {
    list->current = list->current->next;
}

int getCurrent(struct CircularList *list, struct process *item) {
    if (list->current == NULL) {
        return 0;
    }
    *item = list->current->data;
    return 1;
}

int removeCurrent(struct CircularList *list, struct process *item) {
    if (list->current == NULL) {
        return 0;
    }
    *item = list->current->data;
    if (list->current == list->head) {
        list->current = list->current->next;
        deleteAtBeginning(list);
    } else if (list->current == list->tail) {
        list->current = list->current->next;
        deleteAtEnd(list);
    } else {
        struct Node *temp = list->current;
        list->current->prev->next = list->current->next;
        list->current->next->prev = list->current->prev;
        list->current = list->current->next;
        free(temp);
        list->size--;
    }
    return 1;
}
int changeCurrentData(struct CircularList *list, struct process item) {
    if (list->current == NULL) {
        return 0;
    }
    list->current->data = item;
    return 1;
}

#endif
