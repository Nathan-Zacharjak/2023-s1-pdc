#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char const *argv[]){
    // Serial summation of an array
    // Initialising the array so the ith value = i
    int array[1000] = {0};

    for (int i = 0; i < 1000; i++){
        array[i] = i;
    }
    
    // Now doing a running sum of the array
    int sum = 0;
    for (int i = 0; i < 1000; i++){
        sum += array[i];
    }
    // Printing the result
    printf("The sequential sum is: %d\n", sum);

    // Parallel summation of another array
    // Getting the number of threads as console input argument
    int thread_count = strtol(argv[1], NULL, 10);

    // Initialising the array so i = i in parallel
    int parray[10] = {0};

    #pragma omp parallel for num_threads(thread_count)
    for (int i = 0; i < 1000; i++){
        parray[i] = i;
    }

    // Computing the sum of the array's values in parallel
    // using a reduction clause and parallel for directive
    // Initialising the reduction variable for the running sum
    int psum = 0;
    // Declaring psum as a reduction variable and
    // declaring the parallel for loop
    #pragma omp parallel for num_threads(thread_count)\
        reduction(+: psum)
    // Performing the summation
    for (int i = 0; i < 1000; i++){
        psum += parray[i];
    }

    // Printing the result
    printf("The parallel sum is: %d\n", psum);
    

    return 0;
}