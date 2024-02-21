#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define CANVAS_HEIGHT 12
#define CANVAS_WIDTH 22
#define SNAKE_RENDER_CHAR 219
#define SPACE_CHARACTER 32
#define UPEPR_LEFT_CORNER 201
#define UPPER_RIGHT_CORNER 187
#define BOTTOM_LEFT_CORNER 200
#define BOTTOM_RIGHT_CORNER 188

typedef struct snake
{
    int height_pos;
    int width_pos;
    int size;
}Snake;

void renderCanvas(int *canvas);
void renderSnake(Snake *snk, int *canvas, int erase_trace);
void initSnake(Snake **p);
void initCanvas(int *canvas);


int main(){
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD inputRecord;
    DWORD events;
    BOOL running = TRUE;

    // Set console mode to enable input events
    SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

    int canvas[CANVAS_HEIGHT*CANVAS_WIDTH]={0}; // SPACES
    initCanvas(canvas);
    Snake *snk=NULL;
    initSnake(&snk);
    while(1){
        system("cls");
        renderSnake(snk, canvas, 0); // DRAW SNAKE
        renderCanvas(canvas);
        renderSnake(snk, canvas, 1); // ERASE TRACE
        if (ReadConsoleInput(hInput, &inputRecord, 1, &events)) {
            // Filter for key down events only
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
                // Check if 'A' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'A'){
                    snk->width_pos--;
                }
                // Check if 'W' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'W'){
                    snk->height_pos--;
                }
                // Check if 'S' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'S'){
                    snk->height_pos++;
                }
                // Check if 'D' was pressed
                if (inputRecord.Event.KeyEvent.wVirtualKeyCode == 'D'){
                    snk->width_pos++;
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
    free(snk);
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
    temp->size=1;
    *p=temp;
}

void renderSnake(Snake *snk, int *canvas, int erase_trace){
    int snk_head_pos = (CANVAS_WIDTH+1) + (snk->height_pos*CANVAS_WIDTH + snk->width_pos);
    if(erase_trace){
        canvas[snk_head_pos] = SPACE_CHARACTER;
    }
    else{
        canvas[snk_head_pos] = SNAKE_RENDER_CHAR;
    }
}

void renderCanvas(int *canvas){
    for(int i=0;i<CANVAS_HEIGHT*CANVAS_WIDTH;i++){
        printf("%c", canvas[i]);
        if((i+1)%CANVAS_WIDTH==0){
            printf("\n");
        }
    }
}