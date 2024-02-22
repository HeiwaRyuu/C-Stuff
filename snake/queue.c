#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

// int main(int argc, char *argv[]){
//     Queue *queue = NULL;
//     printQueue(queue);
//     printf("Enqueuing...\n");
//     initQueue(&queue);
//     printQueue(queue);
//     enqueue(&queue, 1);   
//     printQueue(queue);
//     enqueue(&queue, 2);   
//     printQueue(queue);
//     enqueue(&queue, 3);   
//     printQueue(queue);
//     enqueue(&queue, 4);   
//     printQueue(queue);
//     enqueue(&queue, 5);   
//     printQueue(queue);
//     printf("\n\n");

//     printf("Dequeueing...\n");
//     dequeue(&queue);
//     printQueue(queue);
//     dequeue(&queue);
//     printQueue(queue);
//     dequeue(&queue);
//     printQueue(queue);
//     printf("\n\n");

//     printf("Freeing Queue...\n");
//     freeQueue(&queue);
//     printQueue(queue);

//     printf("%p\n", queue);

//     return 0;
// }


void initQueue(Queue **p){
    Queue *q = *p;
    if(q!=NULL){
        printf("Already initialized Queue...\n");
        return;
    }
    Queue *aux = (Queue*) malloc(sizeof(Queue));
    if(aux==NULL){
        printf("Error malloc initializing Queue...\n");
        return;
    }
    aux->value=0;
    aux->next=NULL;
    *p=aux;
}


void enqueue(Queue **p, int value){
    Queue *q = *p;
    if(q==NULL){
        printf("Unitialized Queue...\n");
        return;
    }
    while(q->next!=NULL){
        q = q->next;
    }
    Queue *aux = (Queue*) malloc(sizeof(Queue));
    if(aux==NULL){
        printf("Error malloc enqueueing Queue...\n");
        return;
    }
    aux->value=value;
    aux->next=NULL;
    q->next=aux;
    (*p)->value++;
}


void dequeue(Queue **p){
    Queue *q = *p;
    if(q==NULL){
        printf("Unitialized Queue...\n");
        return;
    }
    Queue *aux = q->next;
    q->next = aux->next;
    free(aux);
    q->value--;
}


void printQueue(Queue *q){
    if(q==NULL){
        printf("Unitialized Queue...\n");
        return;
    }
    if(q->next==0){
        printf("Empty Queue!\n");
        return;
    }
    while(q->next!=NULL){
        printf("%d->", q->value);
        q = q->next;
    }
    printf("%d\n", q->value);
}


void freeQueue(Queue **p){
    Queue *q = *p;
    if(q==NULL){
        printf("Unitialized Queue...\n");
        return;
    }
    while(q->value!=0){
        dequeue(p);
    }
    free(q);
    (*p) = NULL;
}