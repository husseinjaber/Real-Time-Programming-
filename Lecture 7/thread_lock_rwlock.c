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

void* reader(void *arg) {
    int id = *((int*) arg);
    int i;

    for (i = 0; i < 5; i++) {
        printf("reader %d want to consult the resource\n", id);
        pthread_rwlock_rdlock(&lock);
        printf("reader %d consults the resource : %c\n", id, data);
        sleep(rand() % 2); // on simule un traitement long
        printf("reader %d finished de reading the resource : %c\n", id, data);
        pthread_rwlock_unlock(&lock);
        sleep(rand() % 3); // le reader fait une pause
    }
    printf("reader %d : is leaving\n", id);
    return NULL;
}

void* writer(void *arg) {
    int id = *((int*) arg);
    int i;

    for (i = 0; i < 5; i++) {
        printf("writer %d want to modify the resource\n", id);
        pthread_rwlock_wrlock(&lock);
        printf("writer %d modify the resource : %c\n", id, data);
        sleep(rand() % 2); // on simule un traitement long
        data++;
        printf("writer %d : now the resource is %c\n", id, data);
        pthread_rwlock_unlock(&lock);
        sleep(rand() % 3); // l'écrivain fait une pause
    }
    printf("writer %d : is leaving\n", id);
    return NULL;
}

int main() {
    int i, nb[8];
    srand(time(NULL)); // initialisation de rand
    pthread_t tid[8];

    pthread_rwlock_init(&lock, NULL);
    for (i = 0; i < 5; i++) {
        nb[i] = i;
        pthread_create(&tid[i], NULL, reader, (void*) &nb[i]);
    }
    for (i = 0; i < 3; i++) {
        nb[i+5] = i;
        pthread_create(&tid[i+5], NULL, writer, (void*) &nb[i+5]);
    }

    for (i = 0; i < 8; i++) {
        pthread_join(tid[i], NULL);
    }
    puts("Reading et modifications are complete");

    pthread_rwlock_destroy(&lock);
    return 0;
}
