#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define num 1000000
volatile unsigned long cnt=0;
pthread_mutex_t lock;

void* count(void* arg)
{
volatile int i;
int id=(int)arg;
pthread_mutex_lock(&lock);
cnt=cnt+(id*num); //Adding the order number to cnt 1000000 times.
pthread_mutex_unlock(&lock);
return NULL;
}

void main()
{
pthread_t tid[5];
int i;
for(i=0;i<5;i++)
pthread_create(&tid[i],NULL,count,(void*)i);

for(i=0;i<5;i++)
pthread_join(tid[i],NULL);

printf("Count is: %ld\n",cnt);
exit(0);
}

