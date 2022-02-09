#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define N 4 //d�claration d'une constante � utiliser dans la boucle des t�ches
typedef struct { // d�claration du type de la donn�e partag�e
float taille;
float poids;
} type_donneePartagee;
pthread_mutex_t verrou; //d�claration du verrou
type_donneePartagee donneePartagee; //d�claration de la donn�e partag�e
void* tache1(void *arg){ // d�claration du corps de la t�che 1
type_donneePartagee ma_donneePartagee;
int i = 0;
while(i < N){
pthread_mutex_lock(&verrou);
ma_donneePartagee = donneePartagee;
//suite du code avec l'utilisation de la donn�e partag�e lue
pthread_mutex_unlock(&verrou);
printf("La tache %s a lu la donn�e partag�e\n", (char*) arg);
//reste du code
i++;
}
}
void* tache2(void *arg){ // d�claration du corps de la t�che 2
int i = 0;
while(i < N){
usleep(5000000); //attendre 5 secondes
printf("La tache %s s'execute\n", (char*) arg);
//reste du code sans utilisation du mutex
i++;
}
}
void* tache3(void *arg){ // d�claration du corps de la t�che 3
int i = 0;
for(i = 0; i < N; i++){
usleep(3000000); //attendre 3 secondes
pthread_mutex_lock(&verrou);
donneePartagee.taille = 100 + rand()%101;
donneePartagee.poids = 10 + rand()%101;
pthread_mutex_unlock(&verrou);
printf("La tache %s a �crit sur la donn�e partag�e\n", (char*) arg);
//reste du code
}
}
int main(void)
{
srand(200);
pthread_t th1, th2, th3;
pthread_attr_t attr;
pthread_mutexattr_t m_attr;
struct sched_param param;
/*pr�paration de la propri�t� du verrou*/
pthread_mutexattr_init(&m_attr);
pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
pthread_mutexattr_setprioceiling(&m_attr, 15); //le mutex dispose d'une priorit� de 15, qui sera celle de la t�che pthread_mutexattr_setprotocol(&m_attr, PTHREAD_PRIO_PROTECT);
pthread_mutex_init(&verrou, &m_attr);
/*initialisation de la donn�e partag�e*/
donneePartagee.taille = 100 + rand()%101;
donneePartagee.poids = 10 + rand()%101;
/*pr�paration de la propri�t� des t�ches*/
pthread_attr_init(&attr);
pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
param.sched_priority = 15;
pthread_attr_setschedparam(&attr, &param);
pthread_create(&th3, &attr, tache3, "tache3"); //cr�ation t�che 3
param.sched_priority = 12;
pthread_attr_setschedparam(&attr, &param);
pthread_create(&th2, &attr, tache2, "tache2"); //cr�ation t�che 2
param.sched_priority = 9;
pthread_attr_setschedparam(&attr, &param);
pthread_create(&th1, &attr, tache1, "tache1"); //cr�ation t�che1
pthread_mutexattr_destroy(&m_attr);
pthread_attr_destroy(&attr);
pthread_join(th1, NULL);
pthread_join(th2, NULL);
pthread_join(th3, NULL);
return 0;
}
