#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <getopt.h>

/** These are the only two global variables allowed in your program **/
static int verbose = 0;
static int use_fork = 0;

/**
arrayHandler: initializes a 2D array to store our Sudoku
@return: an array of size 9x9
**/
struct arrayHandler{
    int arr [9][9];
} array;

/**
subgrids: initializes a 2D array to store our 3x3 subgrids
@return: an array of size 9x9
**/
struct subgrids{
    int threeByThree[9][9];
}subgrid;


/***************** Function Prototypes: *****************/

/**
 void print2DArray: prints our Sudoku stored in a 2D array
 @param: a struct arrayHandler which stores our 2D array, and the row size declared in main
 @return: prints the sudoku table
 **/
void print2DArray(struct arrayHandler array, const int ROW_SIZE);

/**
 void tokenize2DArray:
 **/
void tokenize2DArray(const int ROW_SIZE, const int COL_HEIGHT);
void *doThreadRowTesting(void *ptr);
void *doThreadColTesting(void *ptr);
void *do3x3ThreadTesting(void *ptr);
void setBoolArrayToFalse(bool *array);
void initSubgrids();
void parse_args(int argc, char *argv[]);
/********************************************************/

int main(int argc, char *argv[])
{
    pthread_t threadRowTesting[9];
    pthread_t threadColTesting[9];
    pthread_t threadSubgridTesting[9];
    int threadIncArray[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    
    const int ROW_SIZE = 9;
    const int COL_HEIGHT = 9;

    pid_t p;
    int temp = 0;

    
    tokenize2DArray(ROW_SIZE, COL_HEIGHT);
    initSubgrids();
    
    parse_args(argc, argv);
    
    //BEGIN OF FORK()
    if (use_fork) 
    {
        printf("We are forking child processes as workers.\n");
        printf("\n-------------------------\n");
    
        for(int i = 0; i < 27; i++)
        {
            p = fork();
            if(p == 0)
            {
                if(i >= 0 && i <= 8)
                {
                    doThreadRowTesting(&threadIncArray[i]);
                    exit(1);
                }
                else if (i >= 9 && i <= 17)
                {
                    temp = i % 9;
                    doThreadColTesting(&threadIncArray[temp]);
                    exit(1);
                }
                else
                {
                    temp = i % 18;
                    do3x3ThreadTesting(&threadIncArray[temp]);
                    exit(1);
                }
            
            }
        
        }
        printf("\n-------------------------\n");

    
    } 
    //BEGIN OF THREADS
    else if (verbose) 
    {
        printf("We are using worker threads.\n");
        
    
        printf("\n-------------------------\n");
        for(int i = 0; i < ROW_SIZE; i++)
        {
            pthread_create(&threadRowTesting[i], NULL, doThreadRowTesting, &threadIncArray[i]);
            pthread_create(&threadColTesting[i], NULL, doThreadColTesting, &threadIncArray[i]);
            pthread_create(&threadSubgridTesting[i], NULL, do3x3ThreadTesting, &threadIncArray[i]);
            
        }
        for(int i = 0; i < ROW_SIZE; i++)
        {
            pthread_join(threadRowTesting[i], NULL);
            pthread_join(threadColTesting[i], NULL);
            pthread_join(threadSubgridTesting[i], NULL);
        }
        printf("\n-------------------------\n");
    }
    
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

            //This loop handles multiple new lines in a row
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
        printf("not a valid 9x9 matrix\n");
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
    int checkVar;

    //bool arr that is allocated on stack gets fully set to false.
    setBoolArrayToFalse(boolArray);

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
            printf("You have an invalid input on row: %d\n", row + 1);
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
    setBoolArrayToFalse(boolArray);

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
            printf("You have an invalid input on column: %d\n", col + 1);
    }
}

void *do3x3ThreadTesting(void *ptr)
{
    //currentSection used in terms of output 
    int row = *(int*)ptr;
    bool boolArray[9];
    int checkVar;

    setBoolArrayToFalse(boolArray);
    for(int j = 0; j < 9; j++)
    {
        
        checkVar = subgrid.threeByThree[row][j];
        if(!boolArray[checkVar - 1])
            boolArray[checkVar - 1] = true;
        else
            switch(row){
                case 0:
                    printf("The Top Left subgrid doesn't have the required values.\n");
                    break;
                case 1:
                    printf("The Top Middle subgrid doesn't have the required values.\n");
                    break;
                case 2:
                    printf("The Top Right subgrid doesn't have the required values.\n");
                    break;
                case 3:
                    printf("The Middle Left subgrid doesn't have the required values.\n");
                    break;
                case 4:
                    printf("The Middle subgrid doesn't have the required values.\n");
                    break;
                case 5:
                    printf("The Middle Right subgrid doesn't have the required values.\n");
                    break;
                case 6:
                    printf("The Bottom Left subgrid doesn't have the required values.\n");
                    break;
                case 7:
                    printf("The Bottom Middle subgrid doesn't have the required values.\n");
                    break;
                case 8:
                    printf("The Bottom Right subgrid doesn't have the required values.\n");
                    break;
            }

    }

}

void setBoolArrayToFalse(bool *array)
{
    for(int i = 0; i < 9; i++)
        array[i] = false;
}

void initSubgrids()
{
    int row = 0;
    int col = 0;

    //Loop to populate our new 3x3 subgrid 2D array with entire left side of 
    //tokenized 2D array. This loop covers TL, ML, BL
    for(int f = 0; f < 9; f += 3)
    {
        row = f;
        for(int i = f; i < (f + 3); i++)
        {
            for(int j = 0; j < 3; j++)
            {
                subgrid.threeByThree[row][col] = array.arr[i][j];
                col++;
            }
        }
        col = 0;
        
    }
    col = 0;
    row = 1;
    //Loop to populate new 2D array with TM, M, BM
    for(int f = 0; f < 9; f += 3)
    {
        
        for(int i = f; i < (f + 3); i++)
        {
            for(int j = 3; j < 6; j++)
            {
                subgrid.threeByThree[row][col] = array.arr[i][j];
                col++;
            }
        }
        row += 3;
        col = 0;
    }

    col = 0;
    row = 2;

    //Loop to populate new 2D array with TR, ML, BR
    for(int f = 0; f < 9; f += 3)
    {
        
        for(int i = f; i < (f + 3); i++)
        {
            for(int j = 6; j < 9; j++)
            {
                subgrid.threeByThree[row][col] = array.arr[i][j];
                col++;
            }
        }
        row += 3;
        col = 0;
    }


}

//Takes care of command line arguments
void parse_args(int argc, char *argv[])
{
    int c;
    while (1)
    {
        static struct option long_options[] =
        {
            {"verbose", no_argument,       0, 'v'},
            {"fork",    no_argument,       0, 'f'},
            {0, 0, 0, 0}
        };
        int option_index = 0;
        c = getopt_long (argc, argv, "vf", long_options, &option_index);
        if (c == -1) break;

        switch (c)
        {
            case 'f':
                use_fork = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                exit(1);
        }
    }
}

