
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include <stdio.h>

#define THRESHOLD 20
#define MAXSIZE 1000
#define MAXWORKERS 8
double start_time, end_time;
int numWorkers;
int size; 
double times[5];

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


void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int arr[], int low, int high){
	int pivot = arr[high]; //kanske ändra från high?
	
	// Index of smaller element and indicates 
  // the right position of pivot found so far
	int i = low-1;
	
	for(int j = low; j <= high-1; j++){
		if(arr[j] < pivot){
			i++;
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i+1], &arr[high]);
	return i + 1; //returnera index för pivot
}

void quicksort(int arr[], int low, int high){
    if(low < high) {
        int pvt_index = partition(arr, low, high);

        //parallell quicksort
        if(high-low > THRESHOLD) {
            #pragma omp task
            quicksort(arr, low, pvt_index-1);
            #pragma omp task
            quicksort(arr, pvt_index+1, high);

            #pragma omp taskwait //??
        }
        //sekventiell quicksort för små arrayer
        else{
            quicksort(arr, low, pvt_index-1);
            quicksort(arr, pvt_index+1, high);
        }
    }

}

int main(int argc, char *argv[]){

   

        size = (argc > 1)? atoi(argv[1]) : MAXSIZE; //first arg
        numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS; //second arg

        if (size > MAXSIZE) size = MAXSIZE;
        if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

        //omp_set_num_threads(numWorkers);
        
        //create array
    /*for(int run = 0; run <5; run++){
        int arr[size];
        for(int i = 0; i < size; i++){
            arr[i] = rand()%999;
        }*/
        
        int arr[] = {1458, 6, 900, 123, 345, 777, 200, 600, 34, 100};
        // declare last index, get amount of elements in array
        int n = sizeof(arr)/sizeof(arr[0]);

        //print array before sorting
        /*printf("array before quicksort: ");
        for(int i = 0; i < n; i++){
            printf("%d, ", arr[i]);
        }
        printf("\n");*/


        #pragma omp parallel num_threads(numWorkers)
        {
            //int num_threads = omp_get_num_threads();
            //printf("nof threads %d\n", num_threads);

            #pragma omp single
            {   
            start_time = omp_get_wtime();
            quicksort(arr, 0, n-1);   
            
            }    
            
        }

        end_time = omp_get_wtime();
        //times[run] = end_time - start_time;

        printf("it took %g sec\n", (end_time - start_time));
        
        for(int i = 0; i < n; i++){
            printf("%d, ",arr[i]);
        }

    //}
    //double med = median(times, 5);
    //printf("medianen är %g\n", med);
}


