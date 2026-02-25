#include "mpi.h"
#include <stdio.h>

#define ROUND 3
//#define PRINT

int main(int argc, char *argv[]){
    int rank, size;
    double end, start;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
     
    
    for(int r = 1; r <= ROUND; r++){
        MPI_Status status; 
        int value = rank+r;
        int all_values[size]; 
        for(int i = 0; i < size; i++){
            all_values[i] = -1;
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
        if(rank == 0){
            start = MPI_Wtime();
            all_values[0] = value;
            MPI_Send(&all_values, size, MPI_INT, 1, r, MPI_COMM_WORLD); //skickar till nästa (rank 1)
            #ifdef PRINT
                printf("Process %d sent values: ", rank);
                for (int i = 0; i < size; i++) {
                    printf("%d ", all_values[i]);
                    
                }
                printf("\n");
            #endif
            MPI_Recv(&all_values, size, MPI_INT, (size-1), r, MPI_COMM_WORLD, &status);
               #ifdef PRINT
                printf("Process %d received values: ", rank);
                for (int i = 0; i < size; i++) {
                    printf("%d ", all_values[i]);
                    
                }
                printf("\n");
            #endif
            MPI_Send(&all_values, size, MPI_INT, 1, r, MPI_COMM_WORLD); //skickar final array till nästa (rank 1)


        }

        else{ //rank 1->size-1
            MPI_Recv(&all_values, size, MPI_INT, (rank-1), r, MPI_COMM_WORLD, &status); //recv semi full array
            all_values[rank] = value;
            MPI_Send(&all_values, size, MPI_INT, ((rank+1)%size), r, MPI_COMM_WORLD); //send semi full array
            #ifdef PRINT
                printf("Process %d sent values: ", rank);
                for (int i = 0; i < size; i++) {
                    printf("%d ", all_values[i]);
                    
                }
                printf("\n");
            #endif
            MPI_Recv(&all_values, size, MPI_INT, (rank-1), r, MPI_COMM_WORLD, &status); //recv  full array
            #ifdef PRINT
                printf("Process %d received values: ", rank);
                for (int i = 0; i < size; i++) {
                    printf("%d ", all_values[i]);
                    
                }
                printf("\n");
            #endif
            if(rank < size-1){ //all ranks except the last send the final array to the next 
                MPI_Send(&all_values, size, MPI_INT, ((rank+1)%size), r, MPI_COMM_WORLD); //send full array 
            }

        }

    }
    
    MPI_Barrier(MPI_COMM_WORLD);
        if(rank == 0){
            end = MPI_Wtime();
            printf("Total time for %d rounds: %e \n", ROUND , end - start);
        }

    MPI_Finalize();

    return 0;
}