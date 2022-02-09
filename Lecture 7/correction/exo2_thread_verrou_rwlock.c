// Solution : PTHREAD_RWLOCK_PREFER_READER_NP : comportement par défaut
//	          PTHREAD_RWLOCK_PREFER_WRITER_NP : même comportement => c'est un bug !
//		      PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP : priorité aux écrivains,
//                les lecteurs peuvent subir une famine.
//                fonctionne bien tant que les appels sont "non récursifs".
//     solution équitable : pas possible avec les options de rwlock

#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

pthread_rwlock_t lock;
char data = 'A';

void* lecteur(void *arg) {
	int id = *((int*) arg);
	int i;

	for (i = 0; i < 5; i++) {
		printf("Lecteur %d veut consulter la ressource\n", id);
		pthread_rwlock_rdlock(&lock);
		printf("Lecteur %d consulte la ressource : %c\n", id, data);
		sleep(rand() % 2); // on simule un traitement long
		printf("Lecteur %d a fini de consulter la ressource : %c\n", id, data);
		pthread_rwlock_unlock(&lock);
		sleep(rand() % 3); // le lecteur fait une pause
	}
	printf("Lecteur %d : s'en va\n", id);
    return NULL;
}

void* ecrivain(void *arg) {
	int id = *((int*) arg);
	int i;

	for (i = 0; i < 5; i++) {
		printf("Ecrivain %d veut modifier la ressource\n", id);
		pthread_rwlock_wrlock(&lock);
		printf("Ecrivain %d modifie la ressource : %c\n", id, data);
		sleep(rand() % 2); // on simule un traitement long
		data++;
		printf("Ecrivain %d : maintenant la ressource est %c\n", id, data);
		pthread_rwlock_unlock(&lock);
		sleep(rand() % 3); // l'écrivain fait une pause
	}
	printf("Ecrivain %d : s'en va\n", id);
    return NULL;
}

int main() {
	int i, nb[8];
	srand(time(NULL)); // initialisation de rand
    pthread_t tid[8];

	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);
	//pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NP);
	pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);

    pthread_rwlock_init(&lock, &attr);
	for (i = 0; i < 5; i++) {
		nb[i] = i;
    	pthread_create(&tid[i], NULL, lecteur, (void*) &nb[i]);
    }
	for (i = 0; i < 3; i++) {
		nb[i+5] = i;
		pthread_create(&tid[i+5], NULL, ecrivain, (void*) &nb[i+5]);
    }

    for (i = 0; i < 8; i++) {
		pthread_join(tid[i], NULL);
    }
    puts("Consultation et modifications terminées");

	pthread_rwlock_destroy(&lock);
    return 0;
}
