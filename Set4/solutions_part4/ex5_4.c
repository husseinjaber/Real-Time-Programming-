#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
pthread_mutex_t lock;
struct thread_data
{
   int limit;
   long sum;
};
void* count(void *arg)
{
   int i;
   struct thread_data *data;
   data = (struct thread_data*)arg;
pthread_mutex_lock(&lock);
for(i=0;i< (data->limit);i++)
data->sum++;
pthread_mutex_unlock(&lock);
   pthread_exit(NULL);
return NULL;
}
int main()
{
pthread_t threads[5];
int result,t;
struct thread_data *data=(struct thread_data*)malloc(sizeof(struct thread_data));
data->limit=1000000;
data->sum=0;
for(t=0;t<5;t++) 
{
  result=pthread_create(&threads[t], NULL,&count, (void *)data); //data is the address--->&(*data).
  if(result)
  {
    printf("ERROR\n");
    exit(-1);
    }
}
for(t=0;t<5;t++) 
pthread_join(threads[t],NULL);
printf("The sum is: %ld\n",data->sum);
pthread_exit(NULL);
}
