
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
void initFood(Food **p);
void initCanvas(int *canvas);
int checkFoodCollected(Snake *snk, Food *food);
int foodRendered(Food *food);
void renderFood(Food **p, int *canvas);
void renderPoints(Snake *snk);
int checkCollision(Snake *snk, int *canvas);
void gameOver(Snake *snk);

int main(){
    srand(time(NULL)); // INITIALIZING RANDOM SEED
    // INITIALIZING WINDOWS API INPUT HANDLE
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD inputRecord;
    DWORD events;
    BOOL running = TRUE;

    // Set console mode to enable input events
    SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

    int canvas[CANVAS_HEIGHT*CANVAS_WIDTH]={0}; // SPACES
    initCanvas(canvas);
    Snake *snk=NULL;
    Food *food=NULL;
    int snake_moved=0;
    initSnake(&snk);
    initFood(&food);
    while(1){
        system("cls");
        renderTitle();
        renderSnake(&snk, canvas); // DRAW SNAKE
        renderFood(&food, canvas); // DRAW A RANDOM FOOD ON CANVAS
        renderCanvas(canvas);
        renderPoints(snk);
        printQueue(snk->tail_queue);
        if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
            // Filter for key down events only
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
                // Check if 'A' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'A' && (snk->snake_direction!='d' || snk->size==1)){
                    enqueueSnake(&snk);
                    snk->width_pos--;
                    snk->snake_direction='a';
                    snake_moved=1;
                }
                // Check if 'W' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'W' && (snk->snake_direction!='s' || snk->size==1)){
                    enqueueSnake(&snk);
                    snk->height_pos--;
                    snk->snake_direction='w';
                    snake_moved=1;
                }
                // Check if 'S' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'S' && (snk->snake_direction!='w' || snk->size==1)){
                    enqueueSnake(&snk);
                    snk->height_pos++;
                    snk->snake_direction='s';
                    snake_moved=1;
                }
                // Check if 'D' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'D' && (snk->snake_direction!='a' || snk->size==1)){
                    enqueueSnake(&snk);
                    snk->width_pos++;
                    snk->snake_direction='d';
                    snake_moved=1;
                }
                if(snake_moved){
                    if(checkCollision(snk, canvas)){
                        break;
                    }
                    if(!checkFoodCollected(snk, food) && (snake_moved)){
                        eraseTrace(&snk, canvas); // ERASE TRACE
                    }
                }
                snake_moved = 0;
            }
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
    }

    system("cls");
    gameOver(snk);

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