/**
 * @file matrixvector.c
 * @authors Camp Steiner, Jeff Luong, Ram Chandu
 * 
 * Compile:  gcc -Wall -g -o matrixvector.o matrixvector.c
 * Usage: ./matrixvector.o
 */
#include <stdlib.h>
#include <stdio.h>


#define ARRAYSIZE 16

int main(int argc, char *argv[])
{
    char f_name[50];
    double a[ARRAYSIZE][ARRAYSIZE];
    int i,j;
    double det;
    //Create filename
    sprintf(f_name,"input-matrix/m0016x0016.bin");
    printf("Reading array file %s of size %dx%d\n",f_name,ARRAYSIZE,ARRAYSIZE);
    //Open file
    FILE *datafile=fopen(f_name,"rb");
    //Read elelements
    for (i=0; i< ARRAYSIZE; i++)
        for (j=0; j< ARRAYSIZE; j++)
        {
            fread(&a[i][j],sizeof(double),1,datafile);
            printf("a[%d][%d]=%f\n",i,j,a[i][j]);
        }
            printf("Matrix has been read.\n");
}