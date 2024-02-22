typedef struct node{
    int value;
    struct node *next;
}Queue;

void initQueue(Queue **p);
void enqueue(Queue **p, int value);
void dequeue(Queue **p);
void printQueue(Queue *q);
void freeQueue(Queue **p);