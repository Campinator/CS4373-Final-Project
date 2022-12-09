/**
 * @file matrixvector.c
 * @authors Camp Steiner, Jeff Luong, Ram Chandu
 *
 * Compile:  gcc -Wall -g -o matrixvector.o matrixvector.c
 * Usage: ./matrixvector.o
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <math.h>

#define ARRAYSIZE 512
#define TWODPOINTER(size, row, col) (row * size + col)

// Function headers
double PLUDeterminant(double **a, int n);

int main(int argc, char *argv[])
{
    char f_name[50];

    int arraySize = 12;

    double **a = malloc(arraySize * sizeof(double));
    // Create filename
    // sprintf(f_name, "input-matrix/m%04dx0%04d.bin", arraySize, arraySize);
    sprintf(f_name, "input-matrix/m0032x0032.bin");
    printf("Reading array file %s of size %dx%d\n", f_name, arraySize, arraySize);
    // Open file
    FILE *datafile = fopen(f_name, "rb");
    // Read elelements
    for (int i = 0; i < arraySize; i++)
    {
        a[i] = (double *)malloc(arraySize * sizeof(double));
        for (int j = 0; j < arraySize; j++)
        {
            fread(&a[i][j], sizeof(double), 1, datafile);
            // printf("a[%d][%d]=%f\n", i, j, a[TWODPOINTER(arraySize, i, j)]);
            printf("%f,", a[i][j]);
        }
        printf("\n");
    }
    printf("Matrix has been read.\n\n\n");

    clock_t start = clock();
    double det = PLUDeterminant(a, arraySize);
    clock_t end = clock();
    printf("Determinant: %f in %fs\n\n", det, ((double)end - start)/CLOCKS_PER_SEC);


    return 0;
}


double PLUDeterminant(double **arr, int n)
{   
    // Copy array into local variable so arr doesn't get modified
    int nswaps = 0;
    double **a = malloc(n * sizeof(double));
    for(int i = 0; i < n; i++){
        a[i] = (double *)malloc(n * sizeof(double));
        memcpy(a[i], arr[i], n * sizeof(double));
    }
    // perform PLU decomposition
    for (int k = 0; k < n; k++)
    {
        // pivot
        int i_max = k;
        for (int i = k; i < n; i++)
        {
            if (fabs(a[i][k]) > fabs(a[i_max][k]))
            {
                i_max = i;
            }
        }
        if (i_max != k)
        {
            for (int j = 0; j < n; j++)
            {
                double temp = a[k][j];
                a[k][j] = a[i_max][j];
                a[i_max][j] = temp;
            }
            nswaps++;
        }

        // elimination
        for (int i = k + 1; i < n; i++)
        {
            double factor = a[i][k] / a[k][k];
            for (int j = k; j < n; j++)
            {
                a[i][j] -= factor * a[k][j];
            }
            a[i][k] = factor;
        }
    }

    // print the result
    // for (int i = 0; i < n; i++)
    // {
    //     for (int j = 0; j < n; j++)
    //     {
    //         printf("%lf,", a[i][j]);
    //     }
    //     printf("\n");
    // }

    // Determinant should just be the product of the diagonal now
    double det = 1;
    for (int i = 0; i < n; i++)
    {
        det *= a[i][i];
    }


    if(nswaps % 2 != 0){ // parity for permutations I think?
        det *= -1;
    }
    return det;
}