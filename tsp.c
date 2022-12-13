/**
 * @file tsp.c
 * @authors Camp Steiner, Jeff Luong, Ram Chandu
 *
 * Compile:  gcc -Wall -g -fopenmp -o tsp.o tsp.c 
 * Usage: ./tsp.o <number of threads>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <limits.h>

// Number of cities
#define N 1000

// Matrix to store the distances between the cities
int distances[N][N] = {0};
int global_mincost = 99999999;
// Function to find the minimum distance between the current city
// and the remaining cities
int minDistance(int currCity, int visited[N])
{
    int min = INT_MAX;
    int minIndex;

    for (int i = 0; i < N; i++)
    {
        if (visited[i] == 0 && distances[currCity][i] < min)
        {
            min = distances[currCity][i];
            minIndex = i;
        }
    }

    return minIndex;
}

// Function to find the minimum cost of traveling to all cities
int findMinCost(int visited[N])
{
    int local_minCost = 0;
    int currCity = 0;

    for (int i = 0; i < N; i++)
    {
        visited[currCity] = 1;

        // Find the next closest city
        int nextCity = minDistance(currCity, visited);

        // Add the distance to the minimum cost
        local_minCost += distances[currCity][nextCity];

        // Move to the next city
        currCity = nextCity;
    }

    // Add the distance from the last city to the starting city
    local_minCost += distances[currCity][0];

#pragma omp critical
    {
        if (local_minCost < global_mincost) {
            global_mincost = local_minCost;
        }
    }

    return global_mincost;
}

int main(int argc, char *argv[])
{
    int thread_count = strtol(argv[1], NULL, 10);
    char buffer[8192];
    char *record, *line;
    int i = 0, j = 0;

    FILE *file;
    // Read file
    file = fopen("DistanceMatrix1000_v2.csv", "r");

    // If there is an error in opeing the file, print an error
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return 1;
    }

    while ((line = fgets(buffer, sizeof(buffer), file)) != NULL)
    {
        j = 0;
        record = strtok(line, ",");
        while (record != NULL)
        {
            // Check if record is a valid token
            if (record != "")
            {
                // Place record in array
                distances[i][j] = atoi(record);
                j++;
            }
            record = strtok(NULL, ",");
        }
        i++;
        if (i == 1000)
        {
            // printf("%d \n", i);
            // printf("%d \n", j);
            break;
        }
    }

    fclose(file);

    // printf("\n\nThe cost list is:");

    // for (i = 0; i < N; i++)
    // {
    //     printf("\n");

    //     for (j = 0; j < N; j++)
    //         printf("%d ", distances[i][j]);
    // }
    // printf("\n");

    // Array to keep track of which cities have been visited
    int visited[N] = {0};

    #pragma omp parallel num_threads(thread_count)
    global_mincost = findMinCost(visited);

    printf("Minimum cost: %d\n", global_mincost);

    return 0;
}