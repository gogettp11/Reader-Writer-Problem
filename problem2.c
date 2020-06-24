#include<stdio.h>
#include<stdlib.h> 
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>

pthread_cond_t condition_w,condition_r;
pthread_mutex_t mutex, mutex_c, mutex2;
sem_t sem_r, sem_w;
void *writer();
void *reader();
int library_w = 0;
int library_r = 0;
int waiting_w = 0;

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
    pthread_cond_init(&condition_w,NULL)==-1||sem_init(&sem_r,0,1)==-1){
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

void *writer(){

    
    while(1){

        pthread_mutex_lock(&mutex_c);
        waiting_w++;
        while(library_w!=0||library_r!=0){
            pthread_cond_wait(&condition_w,&mutex_c);
        }
        waiting_w--;
        library_w++;
        printf("writer entered the library\nthere are: %d readers and %d writers\n\n",library_r,library_w);
        pthread_mutex_unlock(&mutex_c);
    
        sleep(1);

        pthread_mutex_lock(&mutex_c);
        library_w--;
        printf("writer left the library\nthere are still: %d readers and %d writers\n\n",library_r,library_w);
        pthread_cond_broadcast(&condition_w);
        pthread_mutex_unlock(&mutex_c);

        sleep(1);
        
    }
}
void *reader(){

    while(1){

        pthread_mutex_lock(&mutex_c);
        while(library_w){
            pthread_cond_wait(&condition_w,&mutex_c);
        }
        library_r++;
        printf("reader entered the library\nthere are: %d readers and %d writers\n\n",library_r,library_w);
        pthread_mutex_unlock(&mutex_c);

        sleep(1);

        //leave
        pthread_mutex_lock(&mutex_c);
        library_r--;
        printf("reader left the library\nthere are still: %d readers and %d writers\n\n",library_r,library_w);
        pthread_cond_signal(&condition_w);
        pthread_mutex_unlock(&mutex_c);

    }
}