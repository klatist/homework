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

#define SHARED 1
#define NUMHONEYBEES 10
#define H 10

int pot = 0;
sem_t pot_lock;
sem_t wake_bear;
int bearSleep = 1;


//producers
void* honeybee(void *arg){
    long honeybeeId = (long) arg;

    while(true){

        sem_wait(&pot_lock);
        if(pot < H){
            pot++;
            printf("bee nr %ld is filling pot\n", honeybeeId);
        }

        if(pot == H && bearSleep){
            printf("bee nr %ld is waking bear\n", honeybeeId);
            bearSleep = 0;
            sem_post(&wake_bear);
        }   
        else{
            printf("bee nr %ld is releasing pot_lock\n", honeybeeId);
            sem_post(&pot_lock);
        }
    }
}

//consumer
void* bear(void *arg){
    while(true){
        
        bearSleep = 1;
        sem_wait(&wake_bear);
        if(pot == H){
            pot = 0;
            printf("nom nom nom yummy honey \n");
            sem_post(&pot_lock);    
        }
        
    }
}

int main(){
    sem_init(&wake_bear, SHARED, 0);
    sem_init(&pot_lock, SHARED, 1);
    
    pthread_t bearId;
    pthread_t honeybees[NUMHONEYBEES];

    for(long i = 0; i < NUMHONEYBEES; i++){
        pthread_create(&honeybees[i], NULL, honeybee, (void*) i);
    }

    pthread_create(&bearId, NULL, bear, NULL);
    
    for (long i = 0; i < NUMHONEYBEES; i++){ //create babyBird threads
        pthread_join(honeybees[i], NULL);
    }
    pthread_join(bearId, NULL);
    
}