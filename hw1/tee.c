#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//create threads
pthread_t readId;
pthread_t fileWriteId;
pthread_t outWriteId;

pthread_cond_t cv; 
pthread_mutex_t mutex;
pthread_mutex_t done_lock;

int readyToRead;
char buffer[100];
bool done = false;

void* Read(void* arg){
    
    while(!done){
        char line[100];
        //printf("type something :");
        //fflush(stdout);
        fgets(line, sizeof(line), stdin);
        //printf("%s",line);
        pthread_mutex_lock(&mutex);
        while(readyToRead != 0){
            pthread_cond_wait(&cv, &mutex);
        }
        readyToRead = 2;
        strcpy(buffer, line);
        pthread_cond_broadcast(&cv);
        pthread_mutex_unlock(&mutex);

        
        //lås och spara buffer i critical section
        //ready to read och broadcast
        //vänta att båsa har läst innan nästa skrivs

    }
    return NULL;

}

void* FileWrite(void* arg){
    
    char* filename = (char*)arg;
    FILE* file_ptr = fopen(filename, "w");
    char local_bfr[100];

    //vänta på att buffern fylls
    while(!done)
    {
        pthread_mutex_lock(&mutex);
        while(readyToRead == 0){
            pthread_cond_wait(&cv, &mutex);
        }
        strcpy(local_bfr, buffer);
        readyToRead -= 1;
        if(readyToRead == 0){
            pthread_cond_signal(&cv);
        }
        pthread_mutex_unlock(&mutex);

        fprintf(file_ptr, "%s", local_bfr); 

        //printf("writes to file, %s", local_bfr);

        //writes line to file's internal buffer
        //may not need fflush... ?
        fflush(file_ptr); //forces all the content in the buffer to be written to disk (make content visible instantly)
    
    }  
    
    fclose(file_ptr);
    printf("closed file");
    return NULL;

}

void* OutWrite(void* arg){

    char local_bfr[100];
    while(!done)
    {
        pthread_mutex_lock(&mutex);
        while(readyToRead == 0){
            pthread_cond_wait(&cv, &mutex);
        }
        strcpy(local_bfr, buffer);
        readyToRead -= 1;
        if(readyToRead == 0){
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
    printf("run main with file %s\n", filename);

    pthread_create(&readId, NULL, Read, NULL);
    pthread_create(&fileWriteId, NULL, FileWrite, filename);
    pthread_create(&outWriteId, NULL, OutWrite, NULL);
    
    pthread_join(readId, NULL);
    pthread_join(fileWriteId, NULL);
    pthread_join(outWriteId, NULL);
    return 0;
    //pthread_exit(NULL);
}