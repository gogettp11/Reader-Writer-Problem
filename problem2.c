#include<stdio.h>
#include<stdlib.h> 
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>

pthread_cond_t condition_w,condition_r;
pthread_mutex_t mutex, mutex_c, mutex2;
void *writer();
void *reader();
int library_w = 0;
int library_r = 0;

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
    
    if(pthread_mutex_init(&mutex,NULL)==-1||pthread_mutex_init(&mutex_c,NULL)==-1||
    pthread_cond_init(&condition_r,NULL)==-1||pthread_mutex_init(&mutex2,NULL)==-1||
    pthread_cond_init(&condition_w,NULL)==-1){
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
        library_r++;
    }else if(type=='w'){
        library_w++;
    }else
        perror("wrong writer&reader encodings in code");
    printf("%c entered the library\nthere are: %d readers and %d writers\n\n",type,library_r,library_w);
}
void leave_library(char type){

    if(type=='r'){
        library_r--;
    }else if(type=='w'){
        library_w--;
    }else
        perror("wrong writer&reader encodings in code");
    printf("%c left the library\nthere are still: %d readers and %d writers\n\n",type,library_r,library_w);
}

void *writer(){

    
    while(1){
        pthread_mutex_lock(&mutex_c);
        enter_library('w');
        sleep(1);
        leave_library('w');
        if(library_r==0){
            pthread_cond_broadcast(&condition_r);
            printf("signaled r\n");}
        pthread_cond_wait(&condition_w,&mutex_c);
        printf("wake up!\n");
    }
}
void *reader(){

    while(1){

        sleep(1);
        pthread_mutex_lock(&mutex);
        enter_library('r');
        pthread_mutex_unlock(&mutex);

        sleep(1);

        pthread_mutex_lock(&mutex2);
        leave_library('r');
        if(library_r==0){
            pthread_cond_signal(&condition_w);
            printf("signaled w\n");}
        pthread_cond_wait(&condition_r,&mutex2);
    }
}