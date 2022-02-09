#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS	5

void *print_hello(void *threadid)
{
   int id = (int)threadid;
   printf("Hello World! I'm  thread number: %d!\n", id);
   pthread_exit(NULL);
}

int main()
{
   
pthread_t threads[NUM_THREADS];
int result,t;
   for(t=0;t<NUM_THREADS;t++)
   {
     printf("In main: creating thread %ld\n", t);
     result = pthread_create(&threads[t], NULL, &print_hello, (void *)t);
     if (result)
     {printf("ERROR");
       exit(-1);}
   }
  
   for(t=0;t<NUM_THREADS;t++)
   {
pthread_join(threads[t],NULL);
   }
   pthread_exit(NULL);
}
