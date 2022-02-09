#include <pthread.h>
/* définition du tampon */
#define N 10 /* Nb de cases du tampon */
#define NbMess 20 /* Nb de messages échangés */

int NbPleins=0;
int tete=0, queue=0;
int tampon[N];
/* définition des conditions et du mutex */
pthread_cond_t vide;
pthread_cond_t plein;
pthread_mutex_t mutex;
pthread_t tid[2];

void Deposer(int m){
pthread_mutex_lock(&mutex);
if(NbPleins == N) pthread_cond_wait(&plein, &mutex);
tampon[queue]=m;
queue=(queue+1)%N;
NbPleins++;
pthread_cond_signal(&vide);
pthread_mutex_unlock(&mutex);
}

int Prelever(void){
int m;
pthread_mutex_lock(&mutex);
if(NbPleins ==0) pthread_cond_wait(&vide, &mutex);
m=tampon[tete];
tete=(tete+1)%N;
NbPleins--;
pthread_cond_signal(&plein);
pthread_mutex_unlock(&mutex);
return m;
}

void * Prod(void * k) /********** PRODUCTEUR */
{
int i;
int mess;
srand(pthread_self());
for(i=0;i<=NbMess; i++){
usleep(rand()%10000); /* fabrication du message */
mess=rand()%1000;
Deposer(mess);
printf("Mess depose: %d\n",mess);
}
}

void * Cons(void * k) /********** CONSOMMATEUR */
{
int i;
int mess;
srand(pthread_self());
for(i=0;i<=NbMess; i++){
mess=Prelever();
printf("\tMess preleve: %d\n",mess);
usleep(rand()%1000000); /* traitement du message */
}
}


void main() /* M A I N */
{
int i, num;
pthread_mutex_init(&mutex,0);
pthread_cond_init(&vide,0);
pthread_cond_init(&plein,0);
/* creation des threads */
pthread_create(tid, 0, (void * (*)()) Prod, NULL);
pthread_create(tid+1, 0, (void * (*)()) Cons, NULL);
// attente de la fin des threads
pthread_join(tid[0],NULL);
pthread_join(tid[1],NULL);
// libération des ressources
pthread_mutex_destroy(&mutex);
pthread_cond_destroy(&vide);
pthread_cond_destroy(&plein);
exit(0);
}

