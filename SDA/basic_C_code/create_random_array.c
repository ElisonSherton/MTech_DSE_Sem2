#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

// Function to accept a size argument, create an array of random
// numbers between 1 - 100 of "size" number of elements
int *getrandomarray(int size){
    int *random_array;
    random_array =  malloc(size * sizeof(int));

    for(int iterator = 0; iterator < size; iterator++){
        random_array[iterator] = rand() % 10;
    }
    return random_array;
}

int main(void)
{   
   int size = 10, i, to_find;
   int *random_array = getrandomarray(size);
   scanf("%d", &to_find);

   printf("\nElements of the array: ");
   for(i = 0; i < size; i++)
   {
     printf("%d ",random_array[i]);
   }
   printf("\n");
   return 0;
}