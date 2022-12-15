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
#include <time.h>

// Number of cities
#define N 1000

// Matrix to store the distances between the cities
int distances[N][N] = {{0}};
// int global_visited_cities[N + 1] = {0};
int *global_visited_cities;
int global_mincost = 99999999;
int global_count = 0;
// Function to find the minimum distance between the current city
// and the remaining cities
int minDistance(int currCity, int visited[N])
{
    // Store the minimum distance and the index of the next city
    int min = INT_MAX;
    int minIndex = 0;

    // Loop through all cities
    for (int i = 0; i < N; i++)
    {
        // Check if the city has not been visited and is not the starting city
        if (visited[i] == 0 && i != 0)
        {
            // Check if the distance is less than the minimum
            if (distances[currCity][i] < min)
            {
                // Update the minimum distance and the index of the next city
                min = distances[currCity][i];
                minIndex = i;
            }
        }
    }

    // Return the index of the next city
    return minIndex;
}

// Function to find the minimum cost of traveling to all cities
int findMinCost(int visited[N])
{
    int local_minCost = 0;
    int currCity = 0;
    int local_visited_cities[N];
    int local_count = 1;

    for (int i = 0; i < N; i++)
    {
        visited[i] = 0;
    }

    for (int i = 0; i < N; i++)
    {
        local_visited_cities[i] = 0;
    }

    for (int i = 0; i < N; i++)
    {
        // printf("i = %d, city = %d\n", i, currCity);
        visited[currCity] = 1;
        local_visited_cities[i] = currCity;
        local_count++;

        // Find the next closest city
        int nextCity = minDistance(currCity, visited);

        // Add the distance to the minimum cost
        local_minCost += distances[currCity][nextCity];

        // Move to the next city
        currCity = nextCity;
    }

    // Add the distance from the last city to the starting city
    local_minCost += distances[currCity][0];

    // Set the visited flah for the starting city to 1
    visited[0] = 1;

    // Find the next closest cty (which should be the starting city)
    int nextCity = minDistance(currCity, visited);

    // Add the distance from the last city to the starting city
    local_minCost += distances[currCity][nextCity];

    // Move to the starting city
    currCity = nextCity;

#pragma omp critical
    {
        if (local_minCost < global_mincost) {
            global_mincost = local_minCost;
            memcpy(global_visited_cities, local_visited_cities, sizeof(local_visited_cities));
            global_count = local_count;
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
    global_visited_cities = malloc(1001 * sizeof(int));

    clock_t start = clock(); // Start the time to time reading the file and the computation

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
            // Place record in array
            distances[i][j] = atoi(record);
            j++;
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

    while ((clock() - start) / CLOCKS_PER_SEC < 60)
    {
        // Array to keep track of which cities have been visited
        int visited[N] = {0};

#pragma omp parallel num_threads(thread_count)

        global_mincost = findMinCost(visited);

    }

    printf("Minimum cost: %d\n", global_mincost);

    printf("The number of cities traversed: %d\n", global_count);

    printf("The list of cities reversed in order: ");
    for (i = 0; i < global_count; i++)
    {
        if (i == global_count - 1)
        {
            printf("%d", global_visited_cities[i]);
        }
        else {
            printf("%d->", global_visited_cities[i]);
        }
    }

    printf("\n");

    return 0;
}