#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
void* fonc(void* arg){
int i;
for(i=0;i<7;i++){
printf("Tache %d : %d\n", (int) arg, i);
usleep(1000000);
}
}
int main(void)
{
pthread_t tache1, tache2;
pthread_attr_t attr;
struct sched_param param;
pthread_attr_init(&attr);
param.sched_priority = 12;
pthread_setschedparam(pthread_self(),
SCHED_FIFO, &param); //pthread_self() pointe sur le processus en cours d'ex�cution, � l'occurrence la fonction main()
//le processus main() sera ordonnanc� en SHED_FIFO avec une priorit� de 12
pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
param.sched_priority = 10;
pthread_attr_setschedparam(&attr, &param);
pthread_create(&tache1, &attr,
fonc, 1); // la t�che tache1 cr��e, sera ordonnanc�e en SHED_FIFO avec une priorit� de 10
param.sched_priority = 7;
pthread_attr_setschedparam(&attr, &param);
pthread_create(&tache2, &attr, fonc, 2);
// la t�che tache2 cr��e, sera ordonnanc�e en SHED_FIFO avec une priorit� de 7
pthread_attr_destroy(&attr);
pthread_join(tache1, NULL);
pthread_join(tache2, NULL);
return 0;
}
