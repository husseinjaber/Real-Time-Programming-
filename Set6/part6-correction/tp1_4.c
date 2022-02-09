/* REPONSE A LA QUESTION
 * Observation :
 * A chaque lancement du programme le nombre de A R G augmente, exemple :
 * premier lancement : ARGARGARGARGARGARG...
 * deuxième lancement : AARRGGAARRGGAARRGG...
 * troisième lancement : AAARRRGGGAAARRR...GGAGAARRRGGAGAARRR...GAGAGARRRGAGAGA...
 * on observe aussi un décalage : les A et G s’entremêlent.
 * Explication :
 * A la fin du premier lancement du programme le sémaphore turnA est à 1.
 * Au deuxième lancement le sémaphore persistant est récupéré. L'appel à
 * sem_post augmente sa valeur de 1 : le 1er thread peut donc afficher deux A
 * coup sur coup, sans attendre le G et donne deux "jetons" au thread R, etc...
 * Au troisième lancement on commence à voir un décalage car après l'affichage
 * du premier G, le thread qui affiche A peut obtenir le sémaphore et affiche
 * son A avant que le 3eme thread affiche le reste des G, etc...
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> // pour les semaphores
#include <fcntl.h>     // pour les flags O_CREAT, O_EXCL, ...

sem_t * turnA;
sem_t * turnR;
sem_t * turnG;

void* a(void* p) {
    int i;
    for(i = 0; i < 100; i++) {
        sem_wait(turnA);
        printf("A");
        fflush(stdout);
        sem_post(turnR);
    }
    return NULL;
}

void* r(void* p) {
    int i;
    for(i = 0; i < 100; i++) {
        sem_wait(turnR);
        printf("R");
        fflush(stdout);
        sem_post(turnG);
    }
    return NULL;
}

void* g(void* p) {
    int i;
    for(i = 0; i < 100; i++) {
        sem_wait(turnG);
        printf("G");
        fflush(stdout);
        sem_post(turnA);
    }
    return NULL;
}

int main() {
    pthread_t ID[2];

    turnA = sem_open("turnA", O_CREAT, S_IRUSR | S_IWUSR, 0);
    turnR = sem_open("turnR", O_CREAT, S_IRUSR | S_IWUSR, 0);
    turnG = sem_open("turnG", O_CREAT, S_IRUSR | S_IWUSR, 0);

    // pour visualiser la valeur du sémaphore
    int valA = 0;
    sem_getvalue(turnA, &valA); // uniquement pour le débogage
    printf("turnA=%d\n", valA); // avec de la chance,
                                // on voit qu'il augmente à chaque exécution

    pthread_create(&ID[0], NULL, r, NULL);
    pthread_create(&ID[1], NULL, g, NULL);

    sem_post(turnA); // ajoute 1 au sémaphore turnA
    a(NULL);

    pthread_join(ID[0], 0);
    pthread_join(ID[1], 0);

    sem_close(turnA);
    sem_close(turnR);
    sem_close(turnG);
    // sem_unlink("turnA"); // le sémaphore est persistant entre deux exécutions
    // sem_unlink("turnR");
    // sem_unlink("turnG");

    return 0;
}
