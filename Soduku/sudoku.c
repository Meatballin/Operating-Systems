#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>

struct arrayHandler{
    int arr [9][9];
} array;

//Function Prototypes
//====================================================================================
void print2DArray(struct arrayHandler array, const int ROW_SIZE);
void tokenize2DArray(const int ROW_SIZE, const int COL_HEIGHT);
void *doThreadRowTesting(void *ptr);
void *doThreadColTesting(void *ptr);

//====================================================================================

int main()
{
    pthread_t threadRowTesting[9];
    pthread_t threadColTesting[9];
    int threadIncArray[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

    const int ROW_SIZE = 9;
    const int COL_HEIGHT = 9;

    tokenize2DArray(ROW_SIZE, COL_HEIGHT);
    //print2DArray(array, ROW_SIZE);
    //initBoolArray(testingHandler);

    printf("\n-------------------------\n");
    for(int i = 0; i < ROW_SIZE; i++)
    {
        pthread_create(&threadRowTesting[i], NULL, doThreadRowTesting, &threadIncArray[i]);
        pthread_create(&threadColTesting[i], NULL, doThreadColTesting, &threadIncArray[i]);
    }
    for(int i = 0; i < ROW_SIZE; i++)
    {
        pthread_join(threadRowTesting[i], NULL);;
        pthread_join(threadColTesting[i], NULL);;
    }
    printf("\n-------------------------\n");
    return 0;
}

//Function to store all input stream values in our 2D array
void tokenize2DArray(const int ROW_SIZE, const int COL_HEIGHT)
{
    //var declarations
    //====================================================================================

    //used to catch getc() return value
    char charFromInput;

    //countOfNumbers used for detecting if two chars from input stream are
    //being read back to back.
    int countOfNumbers = 0;

    //used for conversion of character --> int
    int charToInt;

    int checkForRowSize = 0;

    //used for storage of converted ints into 2d struct array
    int row = 0;
    int col = 0;
    //====================================================================================

    
    //Handles if input starts with blank lines
    charFromInput = getc(stdin);
    if(charFromInput == '\n')
    {
        while(charFromInput == '\n')
            charFromInput = getc(stdin);
        ungetc(charFromInput, stdin);
    }
    ungetc(charFromInput, stdin);
    charFromInput = getc(stdin);

    while(charFromInput != EOF)
    {
        //Execute this IF statement if we read a valid character from stdin through
        //redirection operator
        if(!isspace(charFromInput) && charFromInput != '\n')
        {
        
            checkForRowSize++;
            countOfNumbers++;

            if(countOfNumbers > 1 || isalpha(charFromInput))
            {
                printf("Invalid input\n");
                exit(1);
            }

            sscanf(&charFromInput, "%d", &charToInt);
            //Logic to store our converted values into 2D array.
            array.arr[row][col] = charToInt;
            col++;   
        }
        
        if(charFromInput == '\n')
        {
            //If we don't have a row of size 9, break
            if(checkForRowSize != 9)
            {
                printf("not enough values in row..\n");
                exit(1);
            }

            
            while(charFromInput == '\n')
                charFromInput = getc(stdin);

            if(charFromInput == EOF)
            {
                break;
            }
        
            ungetc(charFromInput, stdin); 
            row++;
            
            checkForRowSize = 0;
            col = 0;
        }

        charFromInput = getc(stdin);
       
        //valid inputs reset countOfNumbers back to zero
        if(isspace(charFromInput) || charFromInput == '\n')
            countOfNumbers = 0;   
    }

    //check if we even have a valud 9x9 matrix
    if(row != COL_HEIGHT - 1)
    {
        printf("not a valid 9x9 matrix");
        exit(1);
    }
}

//function to print our 2D array for testing
void print2DArray(struct arrayHandler array, const int ROW_SIZE)
{
    //test print of 2d array from struct
    for(int i = 0; i < ROW_SIZE; i++)
    {
        if(i != 0)
            printf("\n");
        for(int j = 0; j < ROW_SIZE; j++)
            printf("%d", array.arr[i][j]);
    }  
}


//Function for row checking thread
void *doThreadRowTesting(void *ptr)
{
    //vars
    int row = *(int*)ptr;
    bool boolArray[9];
    int rowArray[9];
    int temp;
    int checkVar;

    //bool arr that is allocated on stack gets fully set to false.
    for(int b = 0; b < 9; b++)
    {
        boolArray[b] = false;
    }

    //initialize our function stack array to our first row of 2D array in struct
    for(int i = 0; i < 9; i++)
    {
        rowArray[i] = array.arr[row][i];
    }

    for(int j = 0; j < 9; j++)
    {
        checkVar = rowArray[j];
        if(!boolArray[checkVar - 1])
            boolArray[checkVar - 1] = true;
        else
            printf("\nYou have an invalid input on row: %d", row + 1);
    }
    
}

void *doThreadColTesting(void *ptr)
{
    //vars
    int col = *(int*)ptr;
    bool boolArray[9];
    int colArray[9];
    int temp;
    int checkVar;

    //bool arr that is allocated on stack gets fully set to false.
    for(int b = 0; b < 9; b++)
    {
        boolArray[b] = false;
    }

    //initialize our function stack array to our first row of 2D array in struct
    for(int i = 0; i < 9; i++)
    {
        colArray[i] = array.arr[i][col];
    }

    for(int j = 0; j < 9; j++)
    {
        checkVar = colArray[j];
        if(!boolArray[checkVar - 1])
            boolArray[checkVar - 1] = true;
        else
            printf("\nYou have an invalid input on column: %d", col + 1);
    }
}