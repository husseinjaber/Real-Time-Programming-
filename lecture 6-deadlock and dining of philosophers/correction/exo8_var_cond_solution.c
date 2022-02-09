#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NB_FOURNEE 4
#define NB_GATEAUX 4
#define NB_ENFANTS 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gateaux_prets = PTHREAD_COND_INITIALIZER;
int gateaux = 0;
int stop = 0;

void* enfant(void *arg) { // mange les gateaux
	int id = *((int*) arg);
	int mange = 0;
    while(1) {
		pthread_mutex_lock(&mutex); // on verrouille le mutex pr changer gateaux
        if(gateaux == 0 && stop == 0) {// si y'a rien a faire
			pthread_cond_wait(&gateaux_prets, &mutex); //attente de la condition
			// si la fonction retourne c'est qu'un signal est reçu...
			// si le nombre de gateaux est inférieur au nombre d'enfants,
			// le temps de réagir, il y en aura peut être plus...
		}
        if (stop == 1) { // il est tant de partir
            pthread_mutex_unlock(&mutex); // on déverrouille avant de partir
            break;
        } else if (gateaux > 0) { // il y a un gateau à manger
			gateaux--;
			mange++;
			printf("L'enfant %d a mangé un gateau\n", id);
        } else {  // sinon on fait rien, les autres ont déjà tout mangé
			printf("L'enfant %d n'a pas eu de gateau\n", id);
		}
		pthread_mutex_unlock(&mutex); // on déverrouille le mutex à la fin
		sleep(1);
    }
    return *((void**) &mange);
}

void* parent(void *arg) { // prépare les gateaux
    int i;
    for (i = 0; i < NB_FOURNEE; i++) {
    	pthread_mutex_lock(&mutex); // on verrouille le mutex pr changer gateaux
    	gateaux += NB_GATEAUX;
		printf("Le parent a préparé des gateaux\n");
		//pthread_cond_signal(&gateaux_prets);  // envoie un signal à un seul
		pthread_cond_broadcast(&gateaux_prets); // envoie un signal à tous
		// note : certains enfants peuvent ne pas avoir de gateaux...
		pthread_mutex_unlock(&mutex);
		sleep(2);
    }
    pthread_mutex_lock(&mutex);
    stop = 1;
    pthread_cond_broadcast(&gateaux_prets); // envoie un signal à tous
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
	int i, nb[NB_ENFANTS];
	void* ret[NB_ENFANTS];
    pthread_t tid[NB_ENFANTS + 1];

	for (i = 0; i < NB_ENFANTS; i++) {
		nb[i] = i;
    	pthread_create(&tid[i], NULL, enfant, (void*) &nb[i]);
    }
    pthread_create(&tid[i], NULL, parent, NULL);

	for (i = 0; i < NB_ENFANTS; i++) {
    	pthread_join(tid[i], &ret[i]);
    	printf("L'enfant %d a mangé %d gateaux\n", i, *((int*) &ret[i]));
    }
    pthread_join(tid[i], NULL);

    pthread_mutex_destroy(&mutex);
    return 0;
}
