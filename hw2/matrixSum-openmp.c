/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c 
     ./matrixSum-openmp size numWorkers

*/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include <stdio.h>
double start_time, end_time;


#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size; 
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);
typedef struct { /*struct to save the min/max value and its position*/
  int row;
  int col;
  int val;
}ValuePosition;

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


/* read command line, initialize, and create threads */

double times[5];
double start_time, end_time;

int main(int argc, char *argv[]) {
  for(int run = 0; run < 5; run++){
    int i, j, total=0;
    ValuePosition matrixMin = {0, 0, -1}; //initilize shared matri
    ValuePosition matrixMax = {0, 0, -1};


    /* read command line args if any */
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
    if (size > MAXSIZE) size = MAXSIZE;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

    omp_set_num_threads(numWorkers);

    /* initialize the matrix */
    for (i = 0; i < size; i++) {
      //printf("[ ");
      for (j = 0; j < size; j++) {
        matrix[i][j] = rand()%99;
        //printf(" %d", matrix[i][j]);
      }
      //printf(" ]\n");
    }

    

  start_time = omp_get_wtime();
  #pragma omp parallel 
  {
    ValuePosition privateMin = {.row = 0, .col = 0, .val = 1000}; //each thread has a private min and max
    ValuePosition privateMax = {.row = 0, .col = 0, .val = -1};


  #pragma omp for reduction (+:total) private(j) 
    for (i = 0; i < size; i++){
      for (j = 0; j < size; j++){
        total += matrix[i][j];
        int matrixVal = matrix[i][j];

        if(matrixVal < privateMin.val ){ //set private min
          privateMin.val = matrixVal;
          privateMin.row = i;
          privateMin.col = j;
        }
        if(matrixVal > privateMax.val){ //set private max
          privateMax.val = matrixVal;
          privateMax.row = i;
          privateMax.col = j;
        }
      } 
    }

  // implicit barrier
  #pragma omp critical //each thread compare their min and max to the global value and update if needed 
  {
    if(matrixMin.val > privateMin.val || matrixMin.val == -1){
      matrixMin = privateMin;
    }
    if(matrixMax.val < privateMax.val || matrixMax.val == -1){
      matrixMax = privateMax;
    }
  }
  }


    end_time = omp_get_wtime();
    times[run] = end_time - start_time;
    
    //printf("the min value is %d at position [%d][%d]\n", matrixMin.val, matrixMin.row, matrixMin.col);
    //printf("the max value is %d at position [%d][%d]\n", matrixMax.val, matrixMax.row, matrixMax.col); 
    //printf("the total is %d\n", total);
    printf("it took %g seconds\n", end_time - start_time);

  }
    double med = median(times, 5);
    printf("medianen är %f\n", med);
}
