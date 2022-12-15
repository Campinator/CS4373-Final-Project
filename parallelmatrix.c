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
#include <stdbool.h>

// Function headers
long double PLUDeterminantSerial(double **a, int n, bool lt);
long double PLUDeterminantOMP(double **a, int n, bool lt);

int main(int argc, char *argv[])
{
    char f_name[50];

    int sizes[] = {16, 32, 64, 128, 256, 496, 512, 1000, 1024, 2000, 2048, 3000, 4000, 4096};

    int threads[] = {2, 4, 8, 16, 32, 64, 128};

    omp_set_dynamic(0); // force using thread_num

    for (int t = 0; t < 7; t++)
    {
        omp_set_num_threads(threads[t]);

        printf("\n\n===PARALLEL RUN - %d THREADS===\n", threads[t]);

        for (int i = 0; i < 14; i++)
        {
            int arraySize = sizes[i];

            double **a = malloc(arraySize * sizeof(double));
            // Create filename
            sprintf(f_name, "input-matrix/m%04dx%04d.bin", arraySize, arraySize);
            // sprintf(f_name, "input-matrix/m0256x0256.bin");
            printf("\n(1) Reading array file %s\n", f_name);
            printf("(2) Size %dx%d\n", arraySize, arraySize);
            // Open file
            FILE *datafile = fopen(f_name, "rb");
            // Read elelements
            for (int i = 0; i < arraySize; i++)
            {
                a[i] = (double *)malloc(arraySize * sizeof(double));
                for (int j = 0; j < arraySize; j++)
                {
                    fread(&a[i][j], sizeof(double), 1, datafile);
                    // printf("%f,", a[i][j]);
                }
                // printf("\n");
            }
            // printf("Matrix has been read.\n");
            fclose(datafile);

            double start, end;

            start = omp_get_wtime();
            long double det = PLUDeterminantOMP(a, arraySize, false);
            end = omp_get_wtime();
            printf("(3) Determinant: %.6Le in %fs\n", det, (end - start));

            start = omp_get_wtime();
            long double det10 = PLUDeterminantOMP(a, arraySize, true);
            end = omp_get_wtime();
            printf("(4) Log10 Determinant: %.6Le in %fs\n", det10, (end - start));

            // FILE *f = fopen("out.csv", "w");
            // if (f == NULL)
            // {
            //     printf("Error opening file!\n");
            //     exit(1);
            // }

            // // loop over the values in the matrix
            // for (int i = 0; i < arraySize; i++)
            // {
            //     for (int j = 0; j < arraySize; j++)
            //     {
            //         fprintf(f, "%lf,", a[i][j]);
            //     }
            //     fprintf(f, "\n");
            // }

            // // close the file
            // fclose(f);
        }
    }

    return 0;
}

long double PLUDeterminantSerial(double **arr, int n, bool lt)
{
    // Copy array into local variable so arr doesn't get modified
    int nswaps = 0;
    double **a = malloc(n * sizeof(double));
    for (int i = 0; i < n; i++)
    {
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
    long double det = 1;
    for (int i = 0; i < n; i++)
    {
        if (lt)
        {
            det *= log10(fabs(a[i][i]));
        }
        else
        {
            det *= a[i][i];
        }
    }

    if (!lt && nswaps % 2 != 0)
    { // parity for permutations I think?
        det *= -1;
    }
    return det;
}

long double PLUDeterminantOMP(double **arr, int n, bool lt)
{
    // Copy array into local variable so arr doesn't get modified
    int nswaps = 0;
    double **a = malloc(n * sizeof(double));
    for (int i = 0; i < n; i++)
    {
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
        #pragma omp parallel for
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
    long double det = 1;
    for (int i = 0; i < n; i++)
    {
        if (lt)
        {
            det *= log10(fabs(a[i][i]));
        }
        else
        {
            det *= a[i][i];
        }
    }

    if (!lt && nswaps % 2 != 0)
    { // parity for permutations I think?
        det *= -1;
    }
    return det;
}