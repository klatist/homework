/* matrix summation using pthreads
task a

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

   usage under Linux:
     gcc matrixSum.c -lpthread
     a.out size numWorkers

*/
#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */
#define DEBUG

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
pthread_mutex_t task_lock;

int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived */


/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */
int size; //stripSize;  /* assume size is multiple of numWorkers */
int matrixSum; /* partial sums */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */
int nextRow = 0;

typedef struct { /*struct to save the min/max value and its position*/
  int row;
  int col;
  int val;
}ValuePosition;

ValuePosition matrixMin = {0, 0, -1};
ValuePosition matrixMax = {0, 0, -1};

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  /*pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);*/

  /* initialize mutex total lock */
  pthread_mutex_init(&task_lock, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  //stripSize = size/numWorkers; //delar upp i rader hur många rader varej worker får

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] = rand()%99;
	  }
  }

  /* print the matrix */
#ifdef DEBUG
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }
#endif

  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++){
    pthread_create(&workerid[l], &attr, Worker, (void *) l);
  }
  for (l = 0; l < numWorkers; l++){
    pthread_join(workerid[l], NULL);
  }
   /* get end time */
    end_time = read_timer(); 
    


    printf("The total is %d\n", matrixSum); 
    printf("the min value is %d at position [%d][%d]\n", matrixMin.val, matrixMin.row, matrixMin.col);
    printf("the max value is %d at position [%d][%d]\n", matrixMax.val, matrixMax.row, matrixMax.col);
    printf("The execution time is %g sec\n", end_time - start_time);
    pthread_exit(NULL);
}



/*------------WORKER--------------*/
/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;  //vilken worker/thread
  int total, i, j;
  printf("worker %ld (pthread id %ld) has started\n", myid, pthread_self());

  /*get task/row*/
  ValuePosition min = {min.row= 0, min.col= 0, min.val= matrix[0][0]};
  ValuePosition max = {max.row= 0, max.col= 0, max.val= matrix[0][0]};
  while(true){
  pthread_mutex_lock(&task_lock);
  i = nextRow;
  nextRow++;
  pthread_mutex_unlock(&task_lock);

  if(i >= size){
    break;
  }
#ifdef DEBUG
#endif

  
  /* sum values in my strip */
  total = 0;
  
    for (j = 0; j < size; j++) { //varje element i raden
      total += matrix[i][j]; // utöka totalen med det elementet
      if(matrix[i][j] < min.val){
        min.val = matrix[i][j];
        min.row = i;
        min.col = j;
      }
    
      if(matrix[i][j] > max.val){
        max.val = matrix[i][j];
        max.row = i;
        max.col = j;
      }
    }
  
    pthread_mutex_lock(&task_lock);
    matrixSum += total;
    if (matrixMin.val > min.val || matrixMin.val == -1){
        matrixMin = min;
      }
    if (matrixMax.val < max.val || matrixMax.val == -1){
        matrixMax = max;
      }
    pthread_mutex_unlock(&task_lock);
    
    }
    return NULL;
}
