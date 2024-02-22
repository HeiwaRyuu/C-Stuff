#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#define CANVAS_HEIGHT 12
#define CANVAS_WIDTH 22
#define SNAKE_RENDER_CHAR 219
#define SPACE_CHARACTER 32
#define UPEPR_LEFT_CORNER 201
#define UPPER_RIGHT_CORNER 187
#define BOTTOM_LEFT_CORNER 200
#define BOTTOM_RIGHT_CORNER 188
#define FOOD_CHARACTER 228

typedef struct node{
    int value;
    struct node *next;
}Node;

typedef struct snake{
    int height_pos;
    int width_pos;
    int head_pos;
    int *tail_arr;
    char snake_direction;
    int size;
}Snake;

typedef struct food{
    int food_height;
    int food_width;
    int food_pos;
    int num_foods;
}Food;


void renderCanvas(int *canvas);
void renderSnake(Snake **p, int *canvas);
void enqueueSnake(Snake **p);
void eraseTrace(Snake **p, int *canvas);
void initSnake(Snake **p);
void initFood(Food **p);
void initCanvas(int *canvas);
void checkFoodCollected(Snake *snk, Food *food);
int foodRendered(Food *food);
void renderFood(Food **p, int *canvas);
void renderPoints(Snake *snk);
void printQueue(Snake *snk);
int checkCollision(Snake *snk);
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
    initSnake(&snk);
    initFood(&food);
    while(1){
        system("cls");
        // if(checkCollision(snk) && snk->size>1) break; //NEEDS A FIX
        renderSnake(&snk, canvas); // DRAW SNAKE
        renderFood(&food, canvas); // DRAW A RANDOM FOOD ON CANVAS
        renderCanvas(canvas);
        checkFoodCollected(snk, food);
        renderPoints(snk);
        printQueue(snk);
        if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
            // Filter for key down events only
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
                // Check if 'A' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'A'){
                    enqueueSnake(&snk);
                    eraseTrace(&snk, canvas); // ERASE TRACE
                    snk->width_pos--;
                    snk->snake_direction='a';
                }
                // Check if 'W' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'W'){
                    checkCollision(snk);
                    enqueueSnake(&snk);
                    eraseTrace(&snk, canvas); // ERASE TRACE
                    snk->height_pos--;
                    snk->snake_direction='w';
                }
                // Check if 'S' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'S'){
                    checkCollision(snk);
                    enqueueSnake(&snk);
                    eraseTrace(&snk, canvas); // ERASE TRACE
                    snk->height_pos++;
                    snk->snake_direction='s';
                }
                // Check if 'D' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'D'){
                    checkCollision(snk);
                    enqueueSnake(&snk);
                    eraseTrace(&snk, canvas); // ERASE TRACE
                    snk->width_pos++;
                    snk->snake_direction='d';
                }
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

    free(snk->tail_arr);
    free(snk);
    free(food);
    return 0;
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
    temp->tail_arr = (int*) malloc(sizeof(int)*CANVAS_WIDTH*CANVAS_HEIGHT);
    if(temp->tail_arr==NULL){
        printf("ERROR INITIALIZING SNAKE...\n");
        return;
    }
    temp->tail_arr[0]=temp->head_pos;
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
    temp->food_height = (CANVAS_HEIGHT-2)/2;
    temp->food_width = 0;
    int food_offset = CANVAS_WIDTH + 1;
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
    snk->tail_arr[snk->size-1] = snk->head_pos;
    printQueue(snk);
}


void eraseTrace(Snake **p, int *canvas){
    Snake *snk = *p;
    canvas[snk->tail_arr[0]] = SPACE_CHARACTER;
    for(int i=0;i<snk->size-1;i++){
        snk->tail_arr[i] = snk->tail_arr[i+1];
    }
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
    int food_offset = CANVAS_WIDTH + 1;
    food->food_pos = food_offset + ((food->food_height*CANVAS_WIDTH) + food->food_width);
    while(canvas[food->food_pos]==SNAKE_RENDER_CHAR){
        food->food_height = rand()%(CANVAS_HEIGHT-2);
        food->food_width = rand()%(CANVAS_WIDTH-2);
        int food_offset = CANVAS_WIDTH + 1;
        food->food_pos = food_offset + ((food->food_height*CANVAS_WIDTH) + food->food_width);
    }
    canvas[food->food_pos] = FOOD_CHARACTER;
    food->num_foods++;
}


void renderPoints(Snake *snk){
    printf("\n\nYour points: %d\n", snk->size-1);
}


void checkFoodCollected(Snake *snk, Food *food){
    if(snk->head_pos==food->food_pos){
        snk->size++;
        food->num_foods--;
        enqueueSnake(&snk);
    }
}


int checkCollision(Snake *snk){
    for(int i=snk->size-2;i>0;i--){
        if(snk->tail_arr[i]==snk->head_pos){
            return 1;
        }
    }
    return 0;
}


void printQueue(Snake *snk){
    int i = 0;
    for(;i<snk->size-1;i++){
        printf("%d->", snk->tail_arr[i]);
    }
    printf("%d\n", snk->tail_arr[i]);
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