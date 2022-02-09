#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // pour fork
#include <sys/types.h> // pour waitpid
#include <sys/wait.h>  // pour waitpid
#include <semaphore.h> // pour les semaphores
#include <fcntl.h>     // pour les flags O_CREAT, O_EXCL, ...

typedef struct {
    int n;
    sem_t * mysem;
    char mysem_name[10];
    sem_t * nextsem;
} th_infos;

void* th(void* p) {
    th_infos info = *(th_infos*)p;
    int i;
    for(i = 0; i < 100; i++) {
        sem_wait(info.mysem);
        printf("%d", info.n);
        fflush(stdout);
        sem_post(info.nextsem);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s [nb_threads]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int num = atoi(argv[1]);
    if (num < 1) {
        fprintf(stderr, "usage: %s [nb_threads > 1]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int i, prev, status;
    pid_t * pids = malloc(num * sizeof(*pids));
    th_infos * infos = malloc(num * sizeof(*infos));
    if (infos == NULL || pids == NULL) {
        fprintf(stderr, "error: malloc\n");
        exit(EXIT_FAILURE);
    }

    // create semaphores
    for (i = 0; i < num; i++) {
        prev = (i>0)? i-1 : num-1;
        infos[i].n = i + 1;
        snprintf(infos[i].mysem_name, 10, "sem%d", i);
        infos[i].mysem = sem_open(infos[i].mysem_name, O_CREAT, S_IRUSR | S_IWUSR, 0);
        infos[prev].nextsem = infos[i].mysem;
    }

    // create processes
    for (i = 0; i < num; i++) {
        pids[i] = fork();

        if (pids[i] == 0) { // enfant
            th(&infos[i]);
            return EXIT_SUCCESS;
        }
        else if (pids[i] < 0) {
            fprintf(stderr, "error: fork %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    sem_post(infos[0].mysem);

    // join processes
    for (i = 0; i < num; i++) {
        waitpid(pids[i], &status, 0);
    }

    // close and unlink semaphores
    for (i = 0; i < num; i++) {
        sem_close(infos[i].mysem);
        sem_unlink(infos[i].mysem_name);
    }

    free(pids);
    free(infos);

    return EXIT_SUCCESS;
}
