#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int t1,t2,t3,t4;
pthread_barrier_t our_barrier;

void *thread1() {
sleep(2);
printf("Enter value for t1:  ");
scanf("%d",&t1);
pthread_barrier_wait(&our_barrier);
printf("\nvalues entered by the threads are  %d %d %d %d \n",t1,t2,t3,t4);
}


void *thread2() {
sleep(4);

printf("Enter value for t2:  ");
scanf("%d",&t2);
pthread_barrier_wait(&our_barrier);

printf("\nvalues entered by the threads are  %d %d %d %d \n",t1,t2,t3,t4);
}

void *thread3() {

sleep(6);

printf("Enter value for t3:  ");
scanf("%d",&t3);
pthread_barrier_wait(&our_barrier);

printf("\nvalues entered by the threads are  %d %d %d %d \n",t1,t2,t3,t4);
}

void *thread4() {

int temp;
sleep(8);

printf("Enter value for t4:  ");
scanf("%d",&t4);
pthread_barrier_wait(&our_barrier);

printf("\nvalues entered by the threads are  %d %d %d %d \n",t1,t2,t3,t4);
}


main() {

pthread_t thread_id_1,thread_id_2,thread_id_3,thread_id_4;
pthread_attr_t attr;
int ret;
void *res;
pthread_barrier_init(&our_barrier,NULL,4);
ret=pthread_create(&thread_id_1,NULL,&thread1,NULL);
if(ret!=0) {
 printf("Unable to create thread1");
}
ret=pthread_create(&thread_id_2,NULL,&thread2,NULL);

if(ret!=0) {
 printf("Unable to create thread2");
}
ret=pthread_create(&thread_id_3,NULL,&thread3,NULL);

if(ret!=0) {
 printf("Unable to create thread3");
}
ret=pthread_create(&thread_id_4,NULL,&thread4,NULL);

if(ret!=0) {
 printf("Unable to create thread4");
}
printf("\n Created threads \n");
pthread_join(thread_id_1,NULL);
pthread_join(thread_id_2,NULL);
pthread_join(thread_id_3,NULL);
pthread_join(thread_id_4,NULL);
pthread_barrier_destroy(&our_barrier);
}
