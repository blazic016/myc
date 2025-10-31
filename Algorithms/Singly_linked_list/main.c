#include<stdio.h>
#include <stdlib.h>



typedef struct Node {
    int data;           
    struct Node *next;  
} Node;



void printuj(Node *head) {
    while (head != NULL) {
        printf("%d -> ", head->data);
        head = head->next;
    }
    printf("NULL\n");
}


void basic_sample() {

    // prazna lista
    Node *head = NULL;

    // kreiraj prvi node
    Node *new_node = malloc(sizeof(Node));
    new_node->data = 5;
    new_node->next = head;  // head = NULL
    head = new_node;        // head -> [5] -> NULL

    // kreiraj drugi node
    new_node = (Node*) malloc(sizeof(Node));
    new_node->data = 10;
    new_node->next = head;  // head = 5
    head = new_node;        // head -> 10 -> 5 -> NULL

    // kreiraj treci node
    new_node = (Node*) malloc(sizeof(Node));
    new_node->data = 15;
    new_node->next = head;  // head = 10
    head = new_node;        // head -> 15 -> 10 -> 5 -> NULL

    printuj(head);

}


void create_node(Node **head, int value) {
    // ** jer func treba da promeni originalni pokazivac head
    // kad god funkcija treba da promeni originalni pokazivac onda koristis **
    Node *new_node = malloc(sizeof(Node));
    new_node->data = value;
    new_node->next = *head;
    *head = new_node;
}




int main() {

    printf("[basic_simple] ");
    basic_sample();

    /* ---------------------- */

    Node *head = NULL;
    create_node(&head, 40);
    create_node(&head, 50);
    create_node(&head, 60);
    printf("[create_node]  ");
    printuj(head);



    return 0;
}