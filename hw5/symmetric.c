#include "mpi.h"
#include <stdio.h>

#define ROUND 1

int main(int argc, char *argv[]){
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0)
        start = MPI_Wtime();
    
    for(int r = 1; r < ROUND; r++){

        int value = rank + r ; 
        int all_values[size]; //lagrar allas värden
        all_values[0] = value;

        MPI_Status status;   
        
        for(int i = 1; i < size; i++){
            //send sitt eget värde
            MPI_Send(&value, 1, MPI_INT, i, r, MPI_COMM_WORLD);

            
        }
        for(int j = 1; j < size; j++){
            MPI_Recv(&all_values[j], 1, MPI_INT, MPI_ANY_SOURCE, r, MPI_COMM_WORLD, &status )
        }
            
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0)
        end = MPI_Wtime();
        printf("Total time for %d rounds: %e \n", ROUND ,(end - start));


    
    MPI_Finalize();

    return 0;
}