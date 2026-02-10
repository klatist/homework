#ifndef _REENTRANT 
#define _REENTRANT 
#endif 

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <unistd.h>


#define W 10
#define SHARED 1
#define NUMBABYBIRDS 10

int dish = W; //counting semaphore 
sem_t wake_parent; //signaling semaphore
sem_t dish_lock;
int parentWaiting = 1;




void* babyBird(void *arg){
    long birdId = (long) arg;
    while(true){
        sem_wait(&dish_lock); //access till dish
        if(dish == 0){ //there is no worms in dish
            //if(parentWaiting){
                printf("bird nr %ld chirps: CHIRP CHIRP PAPA MORE FOOD\n", birdId);
                //parentWaiting = 0; 
                sem_post(&wake_parent); //passing baton
                
            }
            else{
                printf("bird nr %ld gives up lock\n", birdId);
                sem_post(&dish_lock);
            }
        
        else{ //there is worms in dish
            dish--;
            printf("%d worms left\n", dish);
            sem_post(&dish_lock); // signalera klar nästa kan äta 
            printf("bird nr %ld is eating\n", birdId);
        } 

        printf("bird nr %ld goes to sleep\n", birdId);
        sleep(rand()%5);

    }
}

void* papaBird(void* arg){
    while(true){

        parentWaiting = 1;
        printf("parent is waiting\n");
        sem_wait(&wake_parent);

        if(dish == 0){
            dish = W; //critrical section
            printf("papaBird delivers more worms, dish has %d worms\n", dish);
            sem_post(&dish_lock);
        }
    }

        /*sem_wait(&dish_lock);
        printf("parent looks in dish\n");
        
        if(dish != 0){
            printf("parent is waiting\n");
            parentWaiting = 1;
            sem_post(&dish_lock);
            sem_wait(&wake_parent);
        }
            
        dish = W; //critrical section
        printf("papaBird delivers more worms, dish has %d worms\n", dish);
        sem_post(&dish_lock);*/

    
}


int main(){
    sem_init(&wake_parent, SHARED, 0);
    sem_init(&dish_lock, SHARED, 1);

    pthread_t babyBirdId[NUMBABYBIRDS];
    pthread_t papaBirdId;

    pthread_create(&papaBirdId, NULL, papaBird, NULL); //create parentBird thread

    for (long i = 0; i < NUMBABYBIRDS; i++){ //create babyBird threads
        pthread_create(&babyBirdId[i], NULL, babyBird, (void*) i);
    }

    for (long i = 0; i < NUMBABYBIRDS; i++){ //create babyBird threads
        pthread_join(babyBirdId[i], NULL);
    }
    pthread_join(papaBirdId, NULL);
    

}