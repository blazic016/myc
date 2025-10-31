#include<stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;           
    struct Node *next;
    struct Node *prev; 
} Node;



void printuj(Node *head) {
    while (head != NULL) {
        printf("%d -> ", head->data);
        head = head->next;
    }
    printf("NULL\n");
}


void print_reverse(Node *head)
{
    // prazna lista
    if (head == NULL)
        return;

    Node *current = head;

    printf("NULL <- ");

    // idi na kraj
    while(current->next !=NULL) {
        current = current->next;
    }

    // ispitaj od nazad
    while(current != NULL) {
        printf("%d <- ", current->data);
        current = current->prev;
    }
    
    printf("NULL\n");

}

void basic_sample() {

    // prazna lista
    Node *head = NULL;

    // kreiraj prvi node
    Node *new_node = malloc(sizeof(Node));
    new_node->data = 5;
    new_node->prev = NULL;
    new_node->next = head;  // head = NULL

    // Ne izvrsava se jedino kada se pravi prvi cvor
    if (head != NULL) {
        (head)->prev = new_node;
    }

    head = new_node; // head -> [5] -> NULL

    // kreiraj drugi node
    new_node = (Node*) malloc(sizeof(Node));
    new_node->data = 10;
    new_node->prev = NULL;
    new_node->next = head;
    head = new_node;         // head -> 10 -> 5 -> NULL


    // kreiraj treco node
    new_node = (Node*) malloc(sizeof(Node));
    new_node->data = 15;
    new_node->prev = NULL;
    new_node->next = head;
    head = new_node;         // head -> 15 -> 10 -> 5 -> NULL

    printuj(head);

}

void push_front(Node **head, int value) {

    // prvi čvor ima prev = NULL
    // poslednji čvor ima next = NULL1

    Node *new_node = malloc(sizeof(Node));
    new_node->data = value;
    new_node->prev = NULL;
    new_node->next = *head;

    // Ne izvrsava se jedino kada se pravi prvi cvor
    if (*head != NULL) {
        (*head)->prev = new_node;
    }

    *head = new_node;
}


void pop_front(Node **head) {
    if (*head == NULL) {
        return;
    }

    Node *old_head = *head;     // zapamti stari prvi čvor
    *head = old_head->next;     // pomeri head na sledeći

    if (*head != NULL) {
        (*head)->prev = NULL;   // novi head nema prethodni čvor
    }

    free(old_head);
}


void pop_back(Node **head) {

    if (*head == NULL)
        return;

    Node *current = *head;

    // ako ima samo jedan element
    if (current->next == NULL) {
        free(current);
        *head = NULL;
        return;
    }

    // idi na kraj
    while (current->next != NULL) {
        current = current->next;
    }

    Node *old_tail = current;
    Node *new_tail = old_tail->prev;

    new_tail->next = NULL;

    free(old_tail);
}


void push_back(Node **head, int value)
{
    // prazna lista
    if (head == NULL)
        return;

    Node *current = *head;

    // idi na kraj
    while(current->next !=NULL) {
        current = current->next;
    }

    // dodaj node
    Node *new_node = malloc(sizeof(Node));
    new_node->data = value;
    new_node->prev = current;
    new_node->next = NULL;
    current->next = new_node;

}

int main() {

    // printf("[basic_simple] ");
    // basic_sample();



    /* ---------------------- */

    Node *head = NULL;
    push_front(&head, 40);
    push_front(&head, 50);
    push_front(&head, 60);
    printf("[push_front]  ");
    printuj(head);

    printf("[print_reverse]  ");
    print_reverse(head);

    push_back(&head, 20);
    printf("[print_reverse]  ");
    print_reverse(head);

    printf("[pop_front]\n");
    pop_front(&head);
    printf("[print_reverse]  ");
    print_reverse(head);

    pop_back(&head);
    printf("[print_reverse]  ");
    print_reverse(head);


    return 0;
}