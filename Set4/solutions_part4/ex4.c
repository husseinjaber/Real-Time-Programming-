#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

void* hello(void *arg)
{
//getpid() will return the id of the process or program(same for all threads).
//pthread_self() will return the is of the thread.
printf("Hello I'm thread %li.\nI'm in the process:%li\n",pthread_self(),getpid());
return NULL;
}

void main()
{
pthread_t *threads;
int id,num;
printf("Enter the number of threads:");
scanf("%d",&num);
threads=(pthread_t*)malloc(sizeof(pthread_t)*num);
for(id=0;id<num;id++)
{
pthread_create(&threads[id],NULL,&hello,NULL);
//sleep(1);
}
for(id=0;id<num;id++)
pthread_join(threads[id],NULL);
}
