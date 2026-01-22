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
int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived */

/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

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
int size, stripSize;  /* assume size is multiple of numWorkers */
int sums[MAXWORKERS]; /* partial sums */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

typedef struct { /*struct to save the min/max value and its position*/
  int row;
  int col;
  int val;
}ValuePosition;

ValuePosition workerMin[MAXWORKERS];
ValuePosition workerMax[MAXWORKERS];

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
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers; //delar upp i rader hur många rader varej worker får

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
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, (void *) l);
  pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;  //vilken worker/thread
  int total, i, j, first, last;

#ifdef DEBUG1
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
  first = myid*stripSize; 
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

  /* sum values in my strip */
  total = 0;
  ValuePosition min = {min.row= first, min.col= 0, min.val= matrix[first][0]};
  ValuePosition max = {min.row= first, min.col= 0, min.val= matrix[first][0]};
  for (i = first; i <= last; i++) //för varej rad
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
  
    
  sums[myid] = total; //i array sums lägg till min total 
  workerMin[myid] = min;
  workerMax[myid] = max;

  Barrier(); 
  if (myid == 0) { //kör i ordning börjar med den första 
    total = 0;
    for (i = 0; i < numWorkers; i++){
      total += sums[i];   //för alla summor lägger till i en total 
      if (workerMin[i].val < min.val){
        min = workerMin[i];
      }
      if (workerMax[i].val > max.val){
        max = workerMin[i];
      }
    }
    /* get end time */
    end_time = read_timer();
    /* print results */
    printf("The total is %d\n", total); 
    printf("the min value is %d at position [%d][%d]\n", min.val, min.row, min.col);
    printf("the max value is %d at position [%d][%d]\n", max.val, max.row, max.col);
    printf("The execution time is %g sec\n", end_time - start_time);
  }
}
