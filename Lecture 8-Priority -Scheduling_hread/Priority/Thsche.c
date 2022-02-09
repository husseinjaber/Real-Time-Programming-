#include <unistd.h> /* Symbolic Constants */
#include <sys/types.h> /* Primitive System Data Types */
#include <errno.h> /* Errors */
#include <stdio.h> /* Input/Output */
#include <stdlib.h> /* General Utilities */
#include <pthread.h> /* POSIX Threads */
#include <string.h> /* String handling */
#include <sched.h>
#include <time.h>

/* prototype for thread routine */
void print_message_function ( void *ptr );
void print_message_function1 ( void *ptr );
/* struct to hold data to be passed to a thread
this shows how multiple data items can be passed to a thread */

typedef struct str_thdata
{
int thread_no;
int thread_value;
char message[100];
} thdata;

struct timespec start, finish,start1,finish1,start2,finish2;

int main() {
pthread_t thread1, thread2; /* thread variables */
thdata data1, data2; /* structs to be passed to threads */
/* initialize data to pass to thread 1 */
data1.thread_no = 1;
data1.thread_value = 0;
strcpy(data1.message, "Hello!");
/* initialize data to pass to thread 2 */
data2.thread_no = 2;
data2.thread_value = 10000;
strcpy(data2.message, "Hi!");
double elapsed,elapsed1,elapsed2;
clock_gettime(CLOCK_MONOTONIC, &start);
/* create threads 1 and 2 */
clock_gettime(CLOCK_MONOTONIC, &start1);
pthread_create (&thread1, NULL, (void *) &print_message_function, (void *) &data1);
clock_gettime(CLOCK_MONOTONIC, &start2);
pthread_create (&thread2, NULL, (void *) &print_message_function1, (void *) &data2);
/* End Threads 1 and 2*/
pthread_join(thread1, NULL);clock_gettime(CLOCK_MONOTONIC, &finish1);
pthread_join(thread2, NULL);clock_gettime(CLOCK_MONOTONIC, &finish2);
/*Time thread1*/
elapsed1 = (finish1.tv_sec - start1.tv_sec);
elapsed1 += (finish1.tv_nsec - start1.tv_nsec) / 1000000000.0;
/*Time thread2*/
elapsed2 = (finish2.tv_sec - start2.tv_sec);
elapsed2 += (finish2.tv_nsec - start2.tv_nsec) / 1000000000.0;
/*Time Total*/
clock_gettime(CLOCK_MONOTONIC, &finish);
elapsed = (finish.tv_sec - start.tv_sec);
elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
printf("thread1 time:%f\nthread2 time:%f\ntotal time:%f\n ",elapsed1,elapsed2,elapsed);
exit(0);
}
/*
print_message_function is used as the start routine for the threads used
it accepts a void pointer
*/

void print_message_function ( void *ptr )
{
thdata *data;
data = (thdata *) ptr; /* type cast to a pointer to thdata */
struct sched_param param;
int priority=10;
/* sched_priority will be the priority of the thread */
param.sched_priority = priority;
/* only supported policy, others will result in ENOTSUP */
int policy = SCHED_RR;
/* scheduling parameters of target thread */
pthread_setschedparam(pthread_self(), policy, &param);
printf("Thread %d says sched policy %d \n", data->thread_no, SCHED_RR);
pthread_getschedparam(pthread_self(),&policy,&param);
printf("Thread %d says %s %d \n", data->thread_no, data->message,policy);
/* do the work */
int i=0;
printf("Thread %d says %s %d \n", data->thread_no, data->message,(int)pthread_self());
for(i=0;i<100;i++)
printf("Thread %d says %d \n", data->thread_no,data->thread_value++);
pthread_exit(0); /* exit */
} /* print_message_function ( void *ptr ) */

void print_message_function1 ( void *ptr )
{
thdata *data;
data = (thdata *) ptr; /* type cast to a pointer to thdata */

struct sched_param param;
int priority=30;
/* sched_priority will be the priority of the thread */
param.sched_priority = priority;
/* only supported policy, others will result in ENOTSUP * /
/* scheduling parameters of target thread */int policy = SCHED_RR;
pthread_setschedparam(pthread_self(), policy, &param);
printf("Thread %d says sched policy %d \n", data->thread_no, SCHED_RR);

pthread_getschedparam(pthread_self(),&policy,&param);

printf("Thread %d says %s %d \n", data->thread_no, data->message,policy);
int p,h;
int m=100;
int i=0;
for (h=100;h>=0;h--){
for (p=100;p>=0;p--){
m=m/50;m=m*50;m=m/50;m=m*50;}}
/* do the work */
printf("Thread %d says %s %d \n", data->thread_no, data->message,(int)pthread_self());
for(i=0;i<100;i++)
printf("Thread %d says %d \n", data->thread_no,data->thread_value++);
pthread_exit(0);/* exit */
}
