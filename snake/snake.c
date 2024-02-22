
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include "queue.h"

#define CANVAS_HEIGHT 12
#define CANVAS_WIDTH 22
#define SNAKE_RENDER_CHAR 219
#define SPACE_CHARACTER 32
#define UPEPR_LEFT_CORNER 201
#define UPPER_RIGHT_CORNER 187
#define BOTTOM_LEFT_CORNER 200
#define BOTTOM_RIGHT_CORNER 188
#define FOOD_CHARACTER 228

typedef struct snake{
    int height_pos;
    int width_pos;
    int head_pos;
    Queue *tail_queue;
    char snake_direction;
    int size;
}Snake;

typedef struct food{
    int food_height;
    int food_width;
    int food_pos;
    int food_offset;
    int num_foods;
}Food;

void renderTitle();
void renderCanvas(int *canvas);
void renderSnake(Snake **p, int *canvas);
void enqueueSnake(Snake **p);
void eraseTrace(Snake **p, int *canvas);
void initSnake(Snake **p);
void moveSnake(Snake **p);
void initFood(Food **p);
void initCanvas(int *canvas);
int checkFoodCollected(Snake *snk, Food *food);
int foodRendered(Food *food);
void renderFood(Food **p, int *canvas);
void renderPoints(Snake *snk);
void renderCredits();
int checkCollision(Snake *snk, int *canvas);
void gameOver(Snake *snk);

int main(){
    srand(time(NULL)); // INITIALIZING RANDOM SEED
    int canvas[CANVAS_HEIGHT*CANVAS_WIDTH]={0};
    initCanvas(canvas); // INITIALIZING CANVAS WITH SPACES
    Snake *snk=NULL;
    Food *food=NULL;
    initSnake(&snk);
    initFood(&food);
    // MAIN GAME LOOP
    while(1){
        system("cls");
        renderTitle();
        renderSnake(&snk, canvas); // DRAW SNAKE
        renderFood(&food, canvas); // DRAW A RANDOM FOOD ON CANVAS
        renderCanvas(canvas);
        renderPoints(snk);
        printQueue(snk->tail_queue);
        renderCredits();

        // CHECK FOR KEY PRESS EVENTS
        if((GetAsyncKeyState('A') & 0x8000) && (snk->snake_direction!='d' || snk->size==1)){
           snk->snake_direction='a'; 
        }
        if((GetAsyncKeyState('W') & 0x8000) && (snk->snake_direction!='s' || snk->size==1)){
           snk->snake_direction='w'; 
        }
        if((GetAsyncKeyState('S') & 0x8000) && (snk->snake_direction!='w' || snk->size==1)){
           snk->snake_direction='s'; 
        }
        if((GetAsyncKeyState('D') & 0x8000) && (snk->snake_direction!='a' || snk->size==1)){
           snk->snake_direction='d'; 
        }
        moveSnake(&snk);
        // CHECK FOR GAME OVER
        if(checkCollision(snk, canvas)){
            break;
        }
        if(!checkFoodCollected(snk, food)){
            // DELETING SNAKE TRACES BASED ON SNAKE SIZE
            eraseTrace(&snk, canvas);
        }

        // CHECKING IF OUT OF BOUNDS
        if(snk->height_pos>=CANVAS_HEIGHT-2){
            snk->height_pos=0;
        }
        if(snk->height_pos<0){
            snk->height_pos=CANVAS_HEIGHT-3;
        }
        if(snk->width_pos>=CANVAS_WIDTH-2){
            snk->width_pos=0;
        }
        if(snk->width_pos<0){
            snk->width_pos=CANVAS_WIDTH-3;
        }

        // DELAY FOR CONTROLLING SNAKE SPEED
        if(snk->snake_direction=='a' || snk->snake_direction=='d'){
            Sleep(10);
        }
        else{
            Sleep(25);
        }
    }

    system("cls");
    gameOver(snk);

    // FREEING MANUALLY ALLOCATED VARIABLES
    freeQueue(&snk->tail_queue);
    free(snk);
    free(food);
    return 0;
}


void renderTitle(){
    printf("\n");
    for(int i=0;i<(CANVAS_WIDTH/2)-5;i++){
        printf(" ");
    }
    printf("SNAKE GAME\n");
}


void initCanvas(int *canvas){
    for(int i=0;i<CANVAS_HEIGHT*CANVAS_WIDTH;i++){
        canvas[i]=SPACE_CHARACTER;
    }
    canvas[0] = UPEPR_LEFT_CORNER;
    canvas[(CANVAS_HEIGHT-1)*CANVAS_WIDTH] = BOTTOM_LEFT_CORNER;
    canvas[CANVAS_WIDTH-1] = UPPER_RIGHT_CORNER;
    canvas[((CANVAS_HEIGHT-1)*CANVAS_WIDTH)+CANVAS_WIDTH-1] = BOTTOM_RIGHT_CORNER;
}


void initSnake(Snake **p){
    Snake *snk = *p;
    if(!(snk==NULL)){
        printf("Snake already initialized!\n");
        return;
    }
    Snake *temp = (Snake*) malloc(sizeof(Snake));
    if(temp==NULL){
        printf("Malloc Error Initializing Snake...\n");
        return;
    }
    temp->height_pos=0;
    temp->width_pos=0;
    temp->head_pos=(CANVAS_WIDTH+1) + (temp->height_pos*CANVAS_WIDTH + temp->width_pos);
    temp->tail_queue = (Queue*) malloc(sizeof(Queue));
    if(temp->tail_queue==NULL){
        printf("Malloc Error Initializing Sanke Queue...\n");
        return;
    }
    temp->tail_queue->value=0;
    temp->tail_queue->next=NULL;
    temp->snake_direction = 's';
    temp->size=1;
    *p=temp;
}


void initFood(Food **p){
    Food *food = *p;
    if(!(food==NULL)){
        printf("Food already initialized!\n");
        return;
    }
    Food *temp = (Food*) malloc(sizeof(Food));
    if(temp==NULL){
        printf("Malloc Error Initializing Food...\n");
        return;
    }
    temp->food_offset = CANVAS_WIDTH + 1;
    temp->num_foods=0;
    *p=temp; // INITIALIZING FOOD WITH NO VALUES (THEY WILL BE INITIALIZED IN RENDER FOOD FUNCTION)
}


void renderSnake(Snake **p, int *canvas){
    Snake *snk = *p;
    snk->head_pos = (CANVAS_WIDTH+1) + (snk->height_pos*CANVAS_WIDTH + snk->width_pos); 
    canvas[snk->head_pos] = SNAKE_RENDER_CHAR;
}


void moveSnake(Snake **p){
    Snake *snk = *p;
    enqueueSnake(&snk);
    switch (snk->snake_direction){
        case 'a':
            snk->width_pos--;
            break;
        case 'w':
            snk->height_pos--;
            break;
        case 's':
            snk->height_pos++;
            break;
        default:
            snk->width_pos++;
            break;
    }
}


void enqueueSnake(Snake **p){
    Snake *snk = *p;
    enqueue(&snk->tail_queue, snk->head_pos);
}


void eraseTrace(Snake **p, int *canvas){
    Snake *snk = *p;
    canvas[snk->tail_queue->next->value] = SPACE_CHARACTER;
    dequeue(&snk->tail_queue);
}


int foodRendered(Food *food){
   if(food->num_foods>0){
        return 1;
   }
   return 0;
}


void renderFood(Food **p, int *canvas){
    Food *food = *p;
    if(foodRendered(food)){
        return;
    }
    food->food_height = rand()%(CANVAS_HEIGHT-2);
    food->food_width = rand()%(CANVAS_WIDTH-2);
    food->food_pos = food->food_offset + ((food->food_height*CANVAS_WIDTH) + food->food_width);
    while(canvas[food->food_pos]==SNAKE_RENDER_CHAR){
        food->food_height = rand()%(CANVAS_HEIGHT-2);
        food->food_width = rand()%(CANVAS_WIDTH-2);
        food->food_pos = food->food_offset + ((food->food_height*CANVAS_WIDTH) + food->food_width);
    }
    canvas[food->food_pos] = FOOD_CHARACTER;
    food->num_foods++;
}


void renderPoints(Snake *snk){
    printf("\n\nYour points: %d\n", snk->size-1);
}


void renderCredits(){
    printf("\n\nBy: Clib/Horsey\n");
}


int checkFoodCollected(Snake *snk, Food *food){
    int future_head_position = (CANVAS_WIDTH+1) + (snk->height_pos*CANVAS_WIDTH + snk->width_pos);
    if(future_head_position==food->food_pos){
        snk->size++;
        food->num_foods--;
        return 1;
    }
    return 0;
}


int checkCollision(Snake *snk, int *canvas){
    int future_head_position = (CANVAS_WIDTH+1) + (snk->height_pos*CANVAS_WIDTH + snk->width_pos);
    if(canvas[future_head_position]==SNAKE_RENDER_CHAR){
        return 1;
    }
    return 0;
}


void renderCanvas(int *canvas){
    for(int i=0;i<CANVAS_HEIGHT*CANVAS_WIDTH;i++){
        printf("%c", canvas[i]);
        if((i+1)%CANVAS_WIDTH==0){
            printf("\n");
        }
    }
}


void gameOver(Snake *snk){
    printf("GAME OVER!\n");
    printf("Your Total Score: %d\n", snk->size-1);
}