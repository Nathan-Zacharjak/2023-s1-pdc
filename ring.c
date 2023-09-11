#include <stdio.h>
#include <string.h> /* For strlen */
#include <mpi.h> /* For MPI functions, etc */
#include <stdlib.h> /* For random number generation */
#include <time.h> /* For random number seeding */
const int MAX_STRING = 100;

int main(void) {
    int comm_sz; /* Number of processes */
    int my_rank; /* My process rank */

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // 1. Initialise the m-values in each task,
    // by having each task generate a random integer between 10 and 20
    // and using that as its m-value.

    // Initialising the m-values for each task, ensuring the seed is unique for each task
    srand(time(NULL) + my_rank);
    int m_value = rand() % 20 + 10;
    printf("Initial value of task %d is %d\n", my_rank, m_value);

    // 2. Do a left circular shift of the m-values three places to the left
    // by shifting values to the left, 3 times, around the ring.
    // If each task held the value of its rank, the result after execution will be: 3,0,1,2.

    // Figuring out the tasks to send/receive from
    int task_to_send_to = my_rank -1;
    int task_to_receive_from = (my_rank + 1) % comm_sz;
    // This does the "wrap around" from going from the first task to the last task
    // Otherwise, task 0 will try to send to task -1, which doesn't exist
    if (task_to_send_to < 0){
        task_to_send_to = comm_sz - 1;
    }

    // Sending/receiving the values 3 times
    for (int i = 0; i < 3; i++){
        // Sending m_value off to the left task
        printf("Task %d: sending value %d to task %d\n", my_rank, m_value, task_to_send_to);
        MPI_Send( &m_value , 1 , MPI_INT , task_to_send_to , 0 , MPI_COMM_WORLD);

        // Receiving the value sent from the right task
        MPI_Recv( &m_value , 1 , MPI_INT , task_to_receive_from, 0 , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        printf("Task %d: received value %d from task %d\n", my_rank, m_value, task_to_receive_from);
    }
    
    // 3. After that is done, each task outputs its m-value.
    // The outputs must be ordered based on rank.
    // You must use point-to-point communication only to implement this ordering.

    // Alright so we're gonna make task 0 the "master" task
    // so it will receive all the m_values from each other task
    // and ensure each value is printed out in order

    // First we need every task except for task 0 to send its value
    if (my_rank != 0) {
        MPI_Send(&m_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        // Then we need to ensure these messages are received by task 0 in order
        // First just print task 0's value
        printf("The final value of task %d is %d!\n", my_rank, m_value);
        // Then print the other task's values, starting from task 1
        for (int task = 1; task < comm_sz; task++) {
            MPI_Recv(&m_value, 1, MPI_INT, task, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("The final value of task %d is %d!\n", task, m_value);
        }
    }

    MPI_Finalize();
    return 0;
}