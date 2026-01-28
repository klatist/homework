#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define BUFFERSIZE 2
//create threads
pthread_t readId;
pthread_t fileWriteId;
pthread_t outWriteId;

pthread_cond_t cv; 
pthread_mutex_t mutex;


int fileDone = 0;
int outDone = 0;
int addLinePosition = 0;
int fetchLinePosition = 0;
char buffer[BUFFERSIZE][100];

bool done = false;

void* Read(void* arg){
    char line[100];

    //read tar in text, lock, lägger på write pos, ökar writepos väcker
    //write, kopierar det på rätt pos, om den andra också har gjort det, ökar read pos 
    //

    while(1){
        fgets(line, sizeof(line), stdin);

        pthread_mutex_lock(&mutex);
         if(strcmp(line, "quit prog\n") == 0){
            fflush(stdout);
            done = true;
            pthread_cond_broadcast(&cv);
            pthread_mutex_unlock(&mutex);
            break;
         }
        /*måsta kolla så att vi inte skriver över */
        while((addLinePosition+1)%BUFFERSIZE == fetchLinePosition%BUFFERSIZE){
            pthread_cond_wait(&cv, &mutex);
        }
        strcpy(buffer[(addLinePosition%BUFFERSIZE)], line); 
        addLinePosition +=1;
        pthread_cond_broadcast(&cv);
        pthread_mutex_unlock(&mutex);

    }
    return NULL;
}

void* FileWrite(void* arg){
    
    char* filename = (char*)arg;
    FILE* file_ptr = fopen(filename, "w");
    char local_bfr[100];

    while(1)
    {   
        pthread_mutex_lock(&mutex);
        while((addLinePosition%BUFFERSIZE == fetchLinePosition%BUFFERSIZE || fileDone == 1 ) && !done) {
            pthread_cond_wait(&cv, &mutex); //waiting to get woken up
        }
        
        if(done){
            pthread_mutex_unlock(&mutex);
            fflush(stdout);
            break;
        }
        strcpy(local_bfr, buffer[fetchLinePosition%BUFFERSIZE]); //copies from right spot in buffer
        fileDone = 1;
        if(fileDone == 1 && outDone == 1){
            fetchLinePosition +=1; 
            fileDone = 0;
            outDone = 0;
        }

        pthread_mutex_unlock(&mutex);
        fprintf(file_ptr, "%s", local_bfr); 
        fflush(file_ptr); //forces all the content in the buffer to be written to disk (make content visible instantly)

    
    }  
    
    fclose(file_ptr);
    printf("closed file\n");
    fflush(stdout);
    return NULL;

}

void* OutWrite(void* arg){

    char local_bfr[100];
    while(1)
    {
        pthread_mutex_lock(&mutex);
        while((addLinePosition%BUFFERSIZE == fetchLinePosition%BUFFERSIZE || outDone == 1) && !done){
            pthread_cond_wait(&cv, &mutex); //waiting to get woken up
        }

        if(done){
            pthread_mutex_unlock(&mutex);
            break;
        }
        strcpy(local_bfr, buffer[fetchLinePosition%BUFFERSIZE]);
        outDone = 1;
        if(fileDone == 1 && outDone == 1){
            fetchLinePosition +=1; 
            fileDone = 0;
            outDone = 0;
            pthread_cond_signal(&cv);
        }
        pthread_mutex_unlock(&mutex);
        printf("print to output: %s", local_bfr);
        fflush(stdout);
        
    }  
    
    return NULL;
}


int main(int argc, char* argv[]){
    char* filename = argv[1];
    printf("running with file %s\n", filename);
    printf("type quit prog to end the program\n");

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cv, NULL);

    pthread_create(&readId, NULL, Read, NULL);
    pthread_create(&fileWriteId, NULL, FileWrite, filename);
    pthread_create(&outWriteId, NULL, OutWrite, NULL);
    
    pthread_join(readId, NULL);
    pthread_join(fileWriteId, NULL);
    pthread_join(outWriteId, NULL);
    printf("quit program main\n");
    fflush(stdout);

    //pthread_exit(NULL);
}