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
#define BAD_PERMISSIONS 2
#define MAGIC_NUMBER 0x6265
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1

int main(int argc, char **argv)
{
    // validate that user has enter 2 arguments (plus the executable name)
    if (argc < 2)
    {
        printf("Usage: ebfEcho file1 file2");
        return BAD_ARGS;
    }
    else if (argc != 3)
    { // check arg count
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS;
    } // check arg count

    // create a char array to hold magic number
    // and cast to short
    unsigned char magicNumber[2];
    unsigned short *magicNumberValue = (unsigned short *)magicNumber;

    // create and initialise variables used within code
    int width = 0, height = 0;
    unsigned int **imageData;
    long numBytes;

    // open the input file in read mode
    FILE *inputFile = fopen(argv[1], "r");
    // check file opened successfully
    if (!inputFile)
    { // check file pointer
        printf("ERROR: Bad File Name (%s)\n", argv[1]);
        return BAD_FILE;
    } // check file pointer

    // get first 2 characters which should be magic number
    magicNumber[0] = getc(inputFile);
    magicNumber[1] = getc(inputFile);

    // checking against the casted value due to endienness.
    if (*magicNumberValue != MAGIC_NUMBER)
    { // check magic number
        printf("ERROR: Bad Magic Number (%s)\n", argv[1]);
        return BAD_MAGIC_NUMBER; //BAD_FILE to BAD_MAGIC_NUMBER
    } //check magic number

    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    int check = fscanf(inputFile, "%d %d", &height, &width);
    if (check != 2 || height < MIN_DIMENSION || width < MIN_DIMENSION || height > MAX_DIMENSION || width > MAX_DIMENSION)
    { // check dimensions
        // close the file as soon as an error is found
        fclose(inputFile);
        // print appropriate error message and return
        printf("ERROR: Bad Dimensions (%s)\n", argv[1]);
        return BAD_DIM;
    } // check dimensions

    // calculate total size and allocate memory for array
    numBytes = height * width;
    imageData = (unsigned int **)malloc(height * sizeof(unsigned int *));
    if (imageData == NULL)
    { // check malloc
        fclose(inputFile);
        printf("ERROR: Malloc Failed\n");
        return BAD_MALLOC;
    } // check malloc
    for (int i = 0; i < height; i++)
    {
        imageData[i] = (unsigned int *)malloc(width * sizeof(unsigned int));
        if (imageData[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(imageData[j]);
            }
            free(imageData);
            fclose(inputFile);
            printf("ERROR: Image Malloc Failed\n");
            return BAD_MALLOC;
        }
    }

    // read in each grey value from the file
    int count = 0; // track number of values read
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            check = fscanf(inputFile, "%u", &imageData[i][j]);
            if (check != 1)
            { // check for errors while reading file
                fclose(inputFile);
                for (int x = 0; x < height; x++)
                {
                    free(imageData[x]);
                }
                free(imageData);
                printf("ERROR: Bad Data (%s)\n", argv[1]);
                return BAD_DATA;
            } // check for errors while reading file
            count++;
        }
    }

    // check for data (too much)
    int extraData;
    check = fscanf(inputFile, "%d", &extraData);
    if (check == 1){
        fclose(inputFile);
        for (int x = 0; x < height; x++)
        {
            free(imageData[x]);
        }
        free(imageData);
        printf("ERROR: Bad Data (%s)\n", argv[1]);
        return BAD_DATA;
    }

    // check for data (too low)
    if (count < height * width)
    {
        fclose(inputFile);
        for (int x = 0; x < height; x++)
        {
            free(imageData[x]);
        }
        free(imageData);
        printf("ERROR: Bad Data (%s)\n", argv[1]);
        return BAD_DATA;
    }
    // check for data (too high)
    while (fscanf(inputFile, "%u", &check) == 1)
    {
        count++;
    }
    if (count > height * width)
    {
        fclose(inputFile);
        for (int x = 0; x < height; x++)
        {
            free(imageData[x]);
        }
        free(imageData);
        printf("ERROR: Bad Data (%s)\n", argv[1]);
        return BAD_DATA;
    }

    // close the input file
    fclose(inputFile);

    // open the output file in write mode
    FILE *outputFile = fopen(argv[2], "wb");
    // validate that the file has been opened correctly
    if (outputFile == NULL)
        { // validate output file
        printf("ERROR: Bad File Name (%s)\n");
        return BAD_FILE;
        } // validate output file
    //if (!outputFile)
    //{ // check file pointer
    //   printf("ERROR: Bad Permissions (%s)\n", argv[2]);
    //    return BAD_PERMISSIONS;
    //} // check file pointer 

    // write the magic number to the file
    fwrite(&magicNumber, sizeof(unsigned char), 2, outputFile);

    // write the dimensions to the file
    fprintf(outputFile, "%d %d\n", height, width);

    // write the data to the file
    for (int i = 0; i < height; i++)
    {
        fwrite(imageData[i], sizeof(unsigned int), width, outputFile);
    }

    // free the memory for the image data
    for (int i = 0; i < height; i++)
    {
        free(imageData[i]);
    }
    free(imageData);

    // write the header data in one block
    check = fprintf(outputFile, "eb\n%d %d\n", height, width);
    // and use the return from fprintf to check that we wrote.
    if (check == 0) 
        { // check write
        fclose(outputFile);
        printf("ERROR: Bad Output (%s)\n");
        return BAD_OUTPUT;
        } // check write

    // iterate though the array and print out pixel values
    for (int current = 0; current < numBytes; current++)
        { // writing out
        // if we are at the end of a row ((current+1)%width == 0) then write a newline, otherwise a space.
        check = fprintf(outputFile, "%u%c", imageData[current], ((current + 1) % width) ? ' ' : '\n'); //Changed inputFile to outputFile
        if (check == 0)
            { // check write
            fclose(outputFile);
            free(imageData);
            printf("ERROR: Bad Output (%s)\n");
            return BAD_OUTPUT;
            } // check write
        } // writing out

    // free allocated memory before exit
    free(imageData);
    // close the output file before exit
    fclose(outputFile);

    // print final success message and return
    printf("ECHOED\n");
    return SUCCESS;
    } // main()