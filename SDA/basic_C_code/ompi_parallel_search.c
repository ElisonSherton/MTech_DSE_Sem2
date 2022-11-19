#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<stdbool.h>  
#include <time.h>
#include <mpi.h>

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


// Define the main logic here
int main(int argc, char* argv[])
{
	// Define all variables which will be needed
	// pid -> Process Ids; np -> number of processes
	// n_elements_recieved -> How many elements each subthread will get;
	// size -> What is the size of the random array to be created;
	// to_find -> The element to find; mpi_time -> CPU Execution time
	// sublist_index -> The index at which element was found in sublist (threads)
	int pid, np, elements_per_process, n_elements_recieved, size, to_find, sublist_index;
    double mpi_time;
	srand(time(0)); // Use this to define a different set of random numbers each time

    // Take the size of problem and number of threads as user input
    printf("INPUT:\nEnter problem size N: ");
    scanf("%d", &size);
    to_find = rand() % 100;

    int *a = getrandomarray(size);

    // Temporary array for slave process to receive elements
	int a2[size];

	// Create a flag to store if an element was found or not
	int found = -1;

	// This marks the start of the searching procedure
    clock_t t;
    t = clock();

	MPI_Status status;

	// Creation of parallel processes
	MPI_Init(&argc, &argv);

	// find out process ID, and how many processes were started
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &np);

	// Master process
	if (pid == 0) {
		int index, i;
		elements_per_process = size / np;

		// Check how many processes are being run and if they're more than 1
		if (np > 1) {
			// Break the array into chunks and share the work amongst children
			for (i = 1; i < np - 1; i++) {
				index = i * elements_per_process;
				
				// Send how many elements to process, the elements themselves
				// And the index in the original array from where to process to
				// Child threads
				MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&a[index], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&index, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}

			// Since the work might not be divided equally and some elemnts
			// Might persist at the end, spawn a thread and let it process
			// The remaining elements as well.
			index = i * elements_per_process;
			int elements_left = size - index;

			MPI_Send(&elements_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&a[index], elements_left, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&index, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

		}

		// Assign the first partition in the array to be processed by master process
		for (i = 0; i < elements_per_process; i++){
			if (a[i] == to_find){
                found = i;
            }
        }

		// Collect response from other processes if an element is found
		int tmp;
		for (i = 1; i < np; i++) {
			MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			int sender = status.MPI_SOURCE;
			// If an element was found, then modify the index of global found
			// Otherwise let it be -1 only
            if (sender > 0){
			    found = sender;
            }
		}
	}
	// Child processes
	else {
		MPI_Recv(&n_elements_recieved, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&a2, n_elements_recieved, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&sublist_index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		// Iterate over sublist to check if the element is present
		// If yes, then add it's index to the start of sublist index received
		int sublist_found = -1;
		for (int i = 0; i < n_elements_recieved; i++){
			if (a2[i] == to_find){
                sublist_found = sublist_index + i;
            }
        }

		// Send the sublist index to the parent/master process
		MPI_Send(&sublist_found, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	// cleans up all MPI state before exit of process
	MPI_Finalize();

	// The process has completed here 
	mpi_time = clock() - t;
	mpi_time = ((double)mpi_time)/CLOCKS_PER_SEC;

	// Print if the element was found in the array and at what position

	printf("\nOUTPUT:\nArray Size: %d, Element to find %d", size, to_find);
    printf("\nFound at Index: %d", found);
	printf("\nTime taken for search: %f s\nArray: ", mpi_time);
	
	// Print the array to stdout
	for(int i = 0; i < size; i++){
		printf("%d ", a[i]);
	}
	printf("\n");
	return 0;
}
