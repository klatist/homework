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

#define W 10
#define SHARED 1
#define NUMBABYBIRDS 10

int dish = W; //counting semaphore 
sem_t wake_parent; //signaling semaphore
sem_t dish_lock;
int parentWaiting = 1;

sem_init(&wake_parent, SHARED, 0);
sem_init(&dish_lock, SHARED, 1);


void* babyBird(void* arg){
    int birdId = (int) arg;
    while(true){
        sem_wait(&dish_lock); //access till dish
        if(dish == 0){
            if(parentWaiting){
                sem_post(&wake_parent); //passing baton
                parentWaiting = 0; 
            }
            sem_post(&dish_lock);
        }
        else{
            dish--;
            sem_post(&dish_lock); // signalera klar nästa kan äta 
            printf("bird nr %d is eating\n", birdId);
        }     

        //sleep

    }
}

void* papaBird(void* arg){
    while(true){

        sem_wait(&dish_lock);
        
        if(dish != 0){
            parentWaiting = 1;
            sem_post(&dish_lock);
            sem_wait(&wake_parent);
        }
        dish = W; //critrical section
        sem_post(&dish_lock);
    }
}


int main(){

    pthread_t babyBirdId[NUMBABYBIRDS];
    pthread_t papaBirdId;

    for (int i = 0; i < NUMBABYBIRDS; i++){ //create babyBird threads
        pthread_create(&babyBirdId[i], NULL, babyBird, (void*) i);
    }

    pthread_create(&papaBirdId, NULL, papaBird, NULL); //create parentBird thread


    
}