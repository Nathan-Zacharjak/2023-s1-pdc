#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void Hello(char* output[]); /* Thread function */

int main(int argc, char const *argv[]){
    /* Get number of threads from command line */
    int thread_count = strtol(argv[1], NULL, 10);
    // Print out number of threads given
    printf("Number of threads = %d\n", thread_count);

    // Set up global string array that each thread will output to
    char* output[8];
    // Run the parallel function
    #pragma omp parallel num_threads(thread_count)
    Hello(output);
    

    return 0;
}   /* main */

void Hello(char* output[]){
    int my_rank =  omp_get_thread_num();
    int thread_count = omp_get_num_threads();
    // Each thread has its own message string, that will
    char message[40];

    sprintf(message, "Hello from thread %d of %d\n", my_rank, thread_count);
    // then get placed into the global string array
    // so all threads can access all other thread's messages
    output[my_rank] = message;

    // Putting an empty single directive here to synchronize
    // all the threads (there is an implicit barrier at the
    // end of omp single)
    #pragma omp single
    {}

    // Now, we only need a single thread to output the
    // result, so we put another omp single to ensure
    // only 1 thread creates multiple threads off of it
    // in the upcoming parallel for directive
    #pragma omp single
    // Finally, now that we know that all threads have
    // definitely placed their output into the global
    // string array, because we synchronized them with
    // the first omp single from before, we can print
    // the contents of the global string array using
    // an omp for directive
    #pragma omp parallel for num_threads(thread_count)
    for (int i = 0; i < thread_count; i++){
        printf("%s", output[i]);
    }
    
}   /* Hello */