#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define BAD_OUTPUT 7
#define MAGIC_NUMBER 0x6265
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1

int main(int argc, char **argv)
    { // main

    // validate that user has enter 2 arguments (plus the executable name)
    if (argc < 2)
        {
        printf("Usage: ebfComp file1 file2");
        return BAD_ARGS;
        }
    if (argc != 3)
        { // check arg count
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS;
        } // check arg count

    // create a char array to hold magic number
    // and cast to short
    unsigned char magicNumber1[2];
    unsigned short *magicNumberValue1 = (unsigned short *)magicNumber1;

    // create and initialise variables used within code
    int width1 = 0, height1 = 0;
    unsigned int **imageData1;
    long numBytes1;

    // open the input file in read mode
    FILE *inputFile1 = fopen(argv[1], "r");
    // check file opened successfully
    if (!inputFile1)
        { // check file pointer
        printf("ERROR: Bad File Name (%s)\n", argv[1]);
        return BAD_FILE;
        } // check file pointer

    // get first 2 characters which should be magic number
    magicNumber1[0] = getc(inputFile1);
    magicNumber1[1] = getc(inputFile1);

    // checking against the casted value due to endienness.
    if (*magicNumberValue1 != MAGIC_NUMBER)
        { // check magic number
        printf("ERROR: Bad Magic Number (%s)\n", argv[1]);
        return BAD_MAGIC_NUMBER; //Change BAD_FILE to BAD_MAGIC_NUMBER
        } //check magic number

    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    int check = fscanf(inputFile1, "%d %d", &height1, &width1);
    if (check != 2 || height1 < MIN_DIMENSION || width1 < MIN_DIMENSION || height1 > MAX_DIMENSION || width1 > MAX_DIMENSION)
        { // check dimensions
        // close the file as soon as an error is found
        fclose(inputFile1);
        // print appropriate error message and return
        printf("ERROR: Bad Dimensions (%s)\n", argv[1]);
        return BAD_DIM;
        } // check dimensions

       // calculate total size and allocate memory for array
        numBytes1 = height1 * width1;
        imageData1 = (unsigned int **)malloc(height1 * sizeof(unsigned int *));
        if (imageData1 == NULL)
        { // check malloc
            fclose(inputFile1);
            printf("ERROR: Malloc Failed\n");
            return BAD_MALLOC;
        } // check malloc
        for (int i = 0; i < height1; i++)
        {
            imageData1[i] = (unsigned int *)malloc(width1 * sizeof(unsigned int));
            if (imageData1[i] == NULL)
            {
                for (int j = 0; j < i; j++)
                {
                    free(imageData1[j]);
                }
                free(imageData1);
                fclose(inputFile1);
                printf("ERROR: Image Malloc Failed\n");
                return BAD_MALLOC;
            }
        }

    // now we have finished using the inputFile1 we should close it
    fclose(inputFile1);

    // create a char array to hold magic number
    // and cast to short
    unsigned char magicNumber2[2];
    unsigned short *magicNumberValue2 = (unsigned short *)magicNumber2;

    // create and initialise variables used within code
    int width2 = 0, height2 = 0;
    unsigned int *imageData2;
    long numBytes2;

    // open the input file in read mode
    FILE *inputFile2 = fopen(argv[2], "r");
    // check file opened successfully
    if (!inputFile2)
        { // check file pointer
        printf("ERROR: Bad File Name\n");
        return BAD_FILE;
        } // check file pointer

    // get first 2 characters which should be magic number
    magicNumber2[0] = getc(inputFile2);
    magicNumber2[1] = getc(inputFile2);

    // checking against the casted value due to endienness.
    if (*magicNumberValue2 != MAGIC_NUMBER)
        { // check magic number
        printf("ERROR: Bad Magic Number\n");
        return BAD_MAGIC_NUMBER;
        } //check magic number

    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    check = fscanf(inputFile2, "%d %d", &height2, &width2);
    if (check != 2 || height2 < MIN_DIMENSION || width2 < MIN_DIMENSION || height2 > MAX_DIMENSION || width2 > MAX_DIMENSION)
        { // check dimensions
        // close the file as soon as an error is found
        fclose(inputFile2);
        // print appropriate error message and return
        printf("ERROR: Bad Dimensions\n");
        return BAD_DIM;
        } // check dimensions

    // caclulate total size and allocate memory for array
    numBytes2 = height2 * width2;
    imageData2 = (unsigned int *)malloc(numBytes2 * sizeof(unsigned int));

    // if malloc is unsuccessful, it will return a null pointer
    if (imageData2 == NULL)
        { // check malloc
        fclose(inputFile2);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
        } // check malloc

    // read in each grey value from the file
    for (int current = 0; current < numBytes2; current++)
        { // reading in
        check = fscanf(inputFile2, "%u", &imageData2[current]);
        // validate that we have captured 1 pixel value
        if (check != 1)
            { // check inputted data
            // ensure that allocated data is freed before exit.
            free(imageData2);
            fclose(inputFile2);
            printf("ERROR: Bad Data (%s)\n", argv[1]);
            return BAD_DATA;
            } // check inputted data
        } // reading in

    // now we have finished using the inputFile2 we should close it
    fclose(inputFile2);

    // compare the data from the two files:
    
    // start with magic number values
    if (*magicNumberValue1 != *magicNumberValue2)
        { // free and exit
        free(imageData1);
        free(imageData2);
        printf("DIFFERENT\n");
        return SUCCESS;
        } // free and exit

    // check dimensions
    if ((height1 != height2) || (width1 != width2))
        { // free and exit
        free(imageData1);
        free(imageData2);
        printf("DIFFERENT\n");
        return SUCCESS;
        } // free and exit

    // and check the pixel values
    for (int n=0; n<numBytes1; n++)
        {
        if(imageData1[n]!=imageData2[n])
            { // free and exit
            free(imageData1);
            free(imageData2);
            printf("DIFFERENT\n");
            return SUCCESS;
            } // free and exit
        }


    // free allocated memory before exit
    free(imageData1);
    free(imageData2);

    // if we have not exited on different data, must be identical
    printf("IDENTICAL\n");
    return SUCCESS;
    } // main()
