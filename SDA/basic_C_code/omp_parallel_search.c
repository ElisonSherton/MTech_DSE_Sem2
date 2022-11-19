#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <omp.h>

// Function to accept a size argument, create an array of random
// numbers between 1 - 10000 of "size" number of elements
int *getrandomarray(int size){
    int *random_array;
    random_array =  malloc(size * sizeof(int));

    for(int iterator = 0; iterator < size; iterator++){
        random_array[iterator] = rand() % 100;
    }
    return random_array;
}

// Function to perform linear search in a sequential manner
int seq_linear_search(int* random_array, int n, int to_find){
	
    // Initialize a variable to hold the index where the element is found
	int element_idx = -1;
    
	for(int iterator = 0; iterator < n; iterator++){
		if(random_array[iterator] == to_find)
			element_idx = iterator + 1;
	}
	
    // Return the index of found element (or -1 if not found)
	return element_idx;
}

// Function to perform linear search in a parallel manner
int omp_linear_search(int* random_array, int n, int to_find, int n_threads){
	
    // Initialize a variable to hold the index where the element is found
	int element_idx = -1;
    
	//Simple OpenMP for loop in parallel
	#pragma omp parallel for num_threads(n_threads)
	for(int iterator = 0; iterator < n; iterator++){
		if(random_array[iterator] == to_find)
			element_idx = iterator + 1;
	}
	// Return the index of found element (or -1 if not found)
	return element_idx;
}

int main(void)
{   
    // Get a different set of random numbers every time
    srand(time(0));

    int size, n_threads, seq_index, par_index;
    double seq_time, omp_time;

    // Take the size of problem and number of threads as user input
    printf("INPUT:\nEnter problem size N: ");
    scanf("%d", &size);
    printf("Enter number of threads n: ");
    scanf("%d", &n_threads);

    // Define the random array
    int *random_array = getrandomarray(size);

    // Search for a random number between 0 - 10 
    // within the randomly generated array
    int to_find = rand() % 100;

    clock_t t;

    // Perform a sequential linear search
    t = clock();
    seq_index = seq_linear_search(random_array, size, to_find);
    seq_time = clock() - t;
    seq_time = ((double)seq_time)/CLOCKS_PER_SEC;

    // Perform an openmp linear search
    t = clock();
    par_index = omp_linear_search(random_array, size, to_find, n_threads);
    omp_time = clock() - t;
    omp_time = ((double)omp_time)/CLOCKS_PER_SEC;

    // Print the stats out
    printf("\nOUTPUT:\nArray Size: %d, Element to find %d", size, to_find);
    printf("\nSequential Search Index: %d, Parallel Search Index %d", seq_index, par_index);
    printf("\nTime taken for sequential search: %f s", seq_time);
    printf("\nTime taken for omp search with %d threads: %f s\nArray: ", n_threads, omp_time);
    
    // Print the array to stdout
	for(int i = 0; i < size; i++){
		printf("%d ", random_array[i]);
	}
	printf("\n");
    return 0;
}