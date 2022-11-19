#include <stdio.h>
#include <omp.h>

int main(){
    int myid, total_thread;

    #pragma omp parallel
    {
        total_thread = omp_get_num_threads();

        myid = omp_get_thread_num();

        printf("Hello from thread %d of %d threads\n", myid, total_thread);
    }

    printf("End of program");
return 0;
}