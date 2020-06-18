#include<stdio.h>
#include<stdlib.h> 
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>

pthread_cond_t conditionv;
pthread_mutex_t mutex;
sem_t sem_r, sem_w;
void *writer();
void *reader();
int library_w = 0;
int library_r = 0;
int READERS_IN_LIB = 0;

int main(int argc, char*argv[]){

    //reading input and init
    if(argc != 3){
        perror("Wrong arguments.");
        exit(0);
    }
    int writers_num = atoi(argv[1]);
    int readers_num = atoi(argv[2]);
    printf("writers: %d\nreaders: %d\n\n",writers_num,readers_num);
    pthread_t writers[writers_num];
    pthread_t readers[readers_num];
    
    if(sem_init(&sem_w, 0, 1)==-1||sem_init(&sem_r,0,1)==-1||pthread_mutex_init(&mutex,NULL)){
        perror("init error");
        exit(-1);
    }
    

    for(int i=0;i<writers_num;i++){
        pthread_create(&writers[i], NULL, &writer, NULL);
    }
    for(int i=0;i<readers_num;i++){
        pthread_create(&readers[i], NULL, &reader, NULL);
    }
    //waiting for the end that will never come
    for(int i=0;i<writers_num;i++){
        pthread_join(writers[i], NULL);
    }
    for(int i=0;i<readers_num;i++){
        pthread_join(readers[i], NULL);
    }

}
//enter types writer=w and reader=r
void enter_library(char type){

    if(type=='r'){
        pthread_mutex_lock(&mutex);
        library_r++;
        pthread_mutex_unlock(&mutex);
    }else if(type=='w'){
        library_w++;
    }else
        perror("wrong writer&reader encodings in code");
    printf("%c entered the library\nthere are: %d readers and %d writers\n\n",type,library_r,library_w);
}
void leave_library(char type){

    if(type=='r'){
        pthread_mutex_lock(&mutex);
        library_r--;
        pthread_mutex_unlock(&mutex);
    }else if(type=='w'){
        library_w--;
    }else
        perror("wrong writer&reader encodings in code");
    printf("%c left the library\nthere are still: %d readers and %d writers\n\n",type,library_r,library_w);
}

void *writer(){

    
    while(1){
        
        sem_wait(&sem_r);
        enter_library('w');
        sleep(1);
        leave_library('w');
        sem_post(&sem_r);
        sleep(1);
    }
}
void *reader(){

    while(1){
        sleep(1);
        sem_wait(&sem_w);
        READERS_IN_LIB++;
        if(READERS_IN_LIB==1){
            sem_wait(&sem_r);
        }
        sem_post(&sem_w);
        sleep(1);
        enter_library('r');
        sleep(1);
        leave_library('r');
        sem_wait(&sem_w);
        READERS_IN_LIB--;
        if(READERS_IN_LIB==0){
            sem_post(&sem_r);
        }
        sem_post(&sem_w);
    }
}