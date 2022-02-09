#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int nbmen;
int nbmenwait;
int nbwomenwait;
int nbwomen;
sem_t mutex;
sem_t queue;


void *man(void *arg){
  int in=0;
  pthread_t id=pthread_self();
  while(in==0){
    sem_wait(&mutex);
    if(nbwomen==0){
      in=1;
    }
    else{
      nbmenwait++;
      sem_post(&mutex);
      sem_wait(&queue);
    }
  }
  nbmen++;
  sem_post(&mutex);
  printf("[H] %u est dedans avec %d femmes et %d hommes\n",id,nbwomen,nbmen);
  sleep(1);
  sem_wait(&mutex);
  nbmen--;
  if(nbmen==0){
    while(nbwomenwait!=0){
      sem_post(&queue);
      nbwomenwait--;
    }
  }
  sem_post(&mutex);
}

void *woman(void *arg){
  int in=0;
  pthread_t id=pthread_self();
  while(in==0){
    sem_wait(&mutex);
    if(nbmen==0){
      in=1;
    }
    else{
      nbwomenwait++;
      sem_post(&mutex);
      sem_wait(&queue);
    }
  }
  nbwomen++;
  sem_post(&mutex);
  printf("[W] %u est dedans avec %d femmes et %d hommes\n",id,nbwomen,nbmen);
  sleep(1);
  sem_wait(&mutex);
  nbwomen--;
  if(nbwomen==0){
    while(nbmenwait!=0){
      sem_post(&queue);
      nbmenwait--;
    }
  }
  sem_post(&mutex);
}





int main(int argc, char**gg){
  pthread_t pid[10];
  void *status[10];
  nbmen=0;
  nbwomen=0;
  nbmenwait=0;
  nbwomenwait=0;

  sem_init(&mutex,0,1);
  sem_init(&queue,0,0);

  pthread_create(&pid[0],NULL,woman,NULL);
  pthread_create(&pid[1],NULL,woman,NULL);
  pthread_create(&pid[2],NULL,man,NULL);
  pthread_create(&pid[3],NULL,woman,NULL);
  pthread_create(&pid[4],NULL,man,NULL);
  pthread_create(&pid[5],NULL,woman,NULL);
  pthread_create(&pid[6],NULL,man,NULL);
  pthread_create(&pid[7],NULL,woman,NULL);
  pthread_create(&pid[8],NULL,man,NULL);
  pthread_create(&pid[9],NULL,woman,NULL);

  pthread_join(pid[0],NULL);
  pthread_join(pid[1],NULL);
  pthread_create(&pid[0],NULL,woman,NULL);
  pthread_create(&pid[1],NULL,man,NULL);
  pthread_join(pid[2],NULL);
  pthread_join(pid[3],NULL);
  pthread_join(pid[4],NULL);
  pthread_join(pid[5],NULL);
  pthread_join(pid[6],NULL);
  pthread_join(pid[7],NULL);
  pthread_join(pid[8],NULL);
  pthread_join(pid[9],NULL);
}
