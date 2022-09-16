#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

struct arrayHandler{
    int arr [9][9];
} array;

int main()
{
    //var declarations
    //====================================================================================

    //used to catch getc() return value
    char charFromInput;

    //countOfNumbers used for detecting if two chars from input stream are
    //being read back to back.
    int countOfNumbers = 0;
    int newLineCounter = 0;
    //used for conversion of character --> int
    int charToInt;

    //vars for validity checking
    const int ROW_SIZE = 9;
    const int COL_HEIGHT = 9;
    int checkForRowSize = 0;

    //used for storage of converted ints into 2d struct array
    int row = 0;
    int col = 0;
    //====================================================================================

    //Handles all storage of values into 2D array
    charFromInput = getc(stdin);
    while(charFromInput != EOF)
    {

        if(charFromInput == '\n')
        {
            newLineCounter++;
        }
        
        //Execute this IF statement if we read a valid character from stdin through
        //redirection operator
        if(!isspace(charFromInput) && charFromInput != '\n')
        {
            newLineCounter = 0;
            checkForRowSize++;
            countOfNumbers++;
            if(countOfNumbers > 1 || isalpha(charFromInput))
            {
                printf("\nInvalid input\n");
                exit(1);
            }

            sscanf(&charFromInput, "%d", &charToInt);

            //Logic to store our converted values into 2D array.
            array.arr[row][col] = charToInt;
            col++;   
        }
        //Use this to index to the next row of 2D array from struct
        if(charFromInput == '\n' && newLineCounter == 1)
        {
            //If we don't have a row of size 9, break
            if(checkForRowSize != 9)
            {
                printf("not enough values in row..\n");
                exit(1);
            }
            
            //if we hit newline and we have a valid number of digits stored in our row, 
            //then we can reset our row checker variable.
            checkForRowSize = 0;
            col = 0;
            //index to the next row to begin storage of next row of values
            row++;
        }
        
        //after above logic, look for second character from stdin
        charFromInput = getc(stdin);
        
        //valid inputs reset countOfNumbers back to zero
        if(isspace(charFromInput) || charFromInput == '\n')
            countOfNumbers = 0;   
    }

    //check if we even have a valud 9x9 matrix
    if(row != COL_HEIGHT)
    {
        printf("not a valid 9x9 matrix");
        exit(1);
    }

    //test print of 2d array from struct
    for(int i = 0; i < ROW_SIZE; i++)
    {
        if(i != 0)
            printf("\n");
        for(int j = 0; j < ROW_SIZE; j++)
            printf("%d", array.arr[i][j]);
    }    
    printf("\n");
    return 0;
}
