#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>

#define PLAYER 'X'
#define COMPUTER 'O'


void init_board(char board[3][3])
{
    for(int i=0; i<3; i++)
    {
        for(int j=0;j<3; j++)
        {
            board[i][j] = ' ';
        }
    }
}


void print_board(char board[3][3])
{
    for(int i=0; i<3; i++)
    {
        for(int j=0;j<3; j++)
        {
            if(j < 2)
            {
                printf(" %c|", board[i][j]);
            }
            else
            {
                printf(" %c", board[i][j]);
            }
        }
        if(i < 2)
        {
            printf("\n--|--|--\n");   
        }
    }
    
    printf("\n\n");
}


void ask_user_input(char board[3][3])
{
    int lower_boundary = 1;
    int upper_boundary = 3;
    int row = -1;
    int column = -1;
    while((row == -1) | (column == -1))
    {
        printf("Please, enter the row number (1-3):\n");
        scanf("%d", &row);
        printf("Please, enter the column number (1-3):\n");
        scanf("%d", &column);
        
        
        if((row==0)|((row!=0)&(row!=-1)) & ((row < lower_boundary) | (row > upper_boundary)))
        {
            printf("Please, enter a valid integer (1-3):\n");
            row=-1;
            print_board(board);
        }
        else if((column==0)|((column!=0)&(column!=-1)) & ((column < lower_boundary) | (column > upper_boundary)))
        {
            printf("Please, enter a valid integer (1-3):\n");
            column=-1;
            print_board(board);
        }
        else
        {
            row -= 1;
            column -= 1;
            if(board[row][column] != ' ')
            {
                printf("The provided row and column [%d][%d] have already been assigned!\n\n", row+1, column+1);
                row = -1;
                print_board(board);
            }
            else
            {
                board[row][column] = PLAYER;    
            }
        }
    }
}


int check_winner(char board[3][3])
{
    char winner = ' ';
    int is_winner = 0;
    
    // check rows
    for(int i=0; i<3; i++)
    {
        if((board[i][0]==board[i][1]) & (board[i][0]==board[i][2]))
        {
            winner = board[i][0];
        }
    }
    
    // check columns
    for(int j=0; j<3; j++)
    {
        if((board[0][j]==board[1][j]) & (board[0][j]==board[2][j]))
        {
            winner = board[0][j];
        }
    }
    
    //check diagonals
    if((board[0][0]==board[1][1]) & (board[0][0]==board[2][2]))
    {
        winner = board[0][0];
    }
    else if((board[0][2]==board[1][1]) & (board[0][2]==board[3][0]))
    {
        winner = board[0][0];
    }
    
    
    if(winner == PLAYER)
    {
        printf("You win!\n");
        is_winner = 1;
    }
    else if(winner == COMPUTER)
    {
        printf("You lose!\n");
        is_winner = 1;
    }
    else
    {
        int board_incomplete = 0;
        for(int i=0; i<3; i++)
        {
            for(int j=0;j<3; j++)
            {
                if(board[i][j] == ' ')
                {
                    board_incomplete = 1;
                    break;
                }
            }
            if (board_incomplete == 1)
            {
                break;
            }
        }
        if(board_incomplete!=1)
        {
            printf("It's a draw!\n");
            is_winner = 1;
        }
    }
    
    return is_winner;
}


void computer_round(char board[3][3])
{
    int board_incomplete = 0;
    for(int i=0; i<3; i++)
    {
        for(int j=0;j<3; j++)
        {
            if(board[i][j] == ' ')
            {
                board_incomplete = 1;
                break;
            }
        }
        if (board_incomplete == 1)
        {
            break;
        }
    }
    
    if(board_incomplete==1)
    {
        srand(time(0));
        int row;
        int column;
        int available = 0;
        
        while(available == 0)
        {
            row = rand() % 3;
            column = rand() % 3;
            if(board[row][column] == ' ')
            {
                board[row][column] = COMPUTER;
                available = 1;
            }
        }
    }
    
}


int main(int argc, int* argv)
{
    char play_again='Y';
    char board[3][3];
    int is_winner=0;
    
    while(play_again=='Y')
    {
        init_board(board);
        while(1)
        {
            ask_user_input(board);
            is_winner = check_winner(board);
            print_board(board);
            if(is_winner)
            {
                break;
            }
            
            computer_round(board);
            print_board(board);
            if(is_winner)
            {
                break;
            }
        }
        printf("\nDo you want to play again? Y/N:\n");
        scanf("%s", &play_again);
        play_again = toupper(play_again);
    }
    
    return 0; 
}