#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
void* fonc(void* arg){
//data *tid=(data *)arg;
long tid;
tid= (long) arg;
int i;
for(i=0;i<7;i++){
printf("Thread %ld : %d\n", tid, i);
usleep(10000);
}
}
int main(void)
{
pthread_t tache1, tache2;
pthread_attr_t attr;
struct sched_param param;

pthread_attr_init(&attr);
param.sched_priority = 1;
pthread_setschedparam(pthread_self(),SCHED_FIFO, &param);
pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

param.sched_priority = 12;
pthread_attr_setschedparam(&attr, &param);
pthread_create(&tache1, &attr,fonc, (void *) 1); 

param.sched_priority = 10;
pthread_attr_setschedparam(&attr, &param);
pthread_create(&tache2, &attr, fonc, (void *) 2);

pthread_attr_destroy(&attr);
pthread_join(tache1, NULL);
pthread_join(tache2, NULL);
return 0;
}
