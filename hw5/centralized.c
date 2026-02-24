#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define ROUND 3

int cmpfunc(const void *a, const void *b) {
    double da = *(double*)a;
    double db = *(double*)b;
    if (da < db) return -1;
    else if (da > db) return 1;
    else return 0;
}

double median(double arr[], int n) {
    // Sortera arrayen
    qsort(arr, n, sizeof(double), cmpfunc);

    if (n % 2 == 1) {
        // ojämnt antal → mittvärde
        return arr[2];
    } else {
        // jämnt antal → medel av två mittersta
        return (arr[n/2 - 1] + arr[n/2]) / 2.0;
    }
}

int main(int argc, char *argv[]){
    double times[5];
    MPI_Init(&argc, &argv);
        
        int rank, size;
        double start, end;

        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);

        MPI_Barrier(MPI_COMM_WORLD);
        if(rank == 0)
            start = MPI_Wtime();

        for(int r = 1; r <= ROUND; r++){

            int value = rank + r ; 
            int all_values[size];//lagrar allas värden
            MPI_Status status;

            if(rank == 0){
                
                all_values[0] = value;
                for(int i = 1; i < size; i++){
                    MPI_Recv(&all_values[i],1, MPI_INT, MPI_ANY_SOURCE, r, MPI_COMM_WORLD, &status);
                }
                #ifdef PRINT
                printf("Process 0 received values: ");
                for (int i = 0; i < size; i++) {
                    printf("%d ", all_values[i]);
                    
                }
                printf("\n");
                #endif

                for(int i = 1; i < size; i++){
                    MPI_Send(&all_values, size, MPI_INT, i, r, MPI_COMM_WORLD);
                }
                

            }
            
            else{
                MPI_Send(&value, 1, MPI_INT, 0, r, MPI_COMM_WORLD);
                #ifdef PRINT
                printf("Process %d sent value %d\n", rank, value);
                #endif

                MPI_Recv(&all_values, size, MPI_INT, 0, r, MPI_COMM_WORLD, &status);
                #ifdef PRINT
                printf("Process %d received values: ", rank);
                for (int i = 0; i < size; i++) {
                    printf("%d ", all_values[i]);
                }
                printf("\n");
                #endif
            }

    
        }
    
        MPI_Barrier(MPI_COMM_WORLD);
                if(rank == 0){
                    end = MPI_Wtime();
                    printf("Total time for %d rounds: %e \n", ROUND ,(end - start));
                }
    
    MPI_Finalize();

    return 0;
}