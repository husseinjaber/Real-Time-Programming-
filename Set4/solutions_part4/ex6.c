#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
pthread_mutex_t book1;
pthread_mutex_t book2;

pthread_cond_t available;

void* allen(void* arg)
{
pthread_mutex_lock(&book1);
pthread_mutex_lock(&book2);
printf("Allen had collected all books he need!");
pthread_mutex_unlock(&book2);
pthread_mutex_unlock(&book1);
}

void* bob(void* arg)
{ 
pthread_mutex_lock(&book2);
pthread_mutex_lock(&book1);
printf("BOB had collected all books he need!");
pthread_mutex_unlock(&book1);
pthread_mutex_unlock(&book2);
}

void main()
{
pthread_t tid1,tid2;
pthread_create(&tid1,NULL,&allen,NULL);
pthread_create(&tid2,NULL,&bob,NULL);

pthread_join(tid1,NULL);
pthread_join(tid2,NULL);
}
