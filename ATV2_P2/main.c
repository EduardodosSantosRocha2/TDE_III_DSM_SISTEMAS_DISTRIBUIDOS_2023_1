#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h> // library semaphore
#define BUF_SIZE 1024
#define SHM_KEY 0x0001

typedef struct {
    int cnt;
    int complete;
    char buf[BUF_SIZE];
} shmsegment;

int write_buffer(char *buffer, int size){
    static char ch = 'A';
    int filled_count;
    memset(buffer, ch, size - 1);
    buffer[size-1] = '\0';
    filled_count = strlen(buffer);
    ch++;
    return filled_count;
}

int main() {
    int shmid, i;
    shmsegment *shmp;
    char *buffer;
    sem_t *semaphore;

    shmid = shmget(SHM_KEY, sizeof(shmsegment), IPC_CREAT | 0664);
    if (shmid == -1) {
        perror("Shared memory");
        return 1;
    }


    semaphore = sem_open("/semaphorep2", O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *) -1) {
        perror("Shared memory attach error");
        return 1;
    }

    buffer = shmp->buf;
    for (i = 0; i < 5; i++) {
        sem_wait(semaphore);
        shmp->cnt = write_buffer(buffer, BUF_SIZE);
        shmp->complete = 0;
        printf("Writing Process: Shared Memory Write: Wrote %d bytes\n", shmp->cnt);
        sem_post(semaphore);
        buffer = shmp->buf;
        sleep(7);
    }

    printf("Writing Process: Wrote %d times\n", i);
    shmp->complete = 1;

    if (shmdt(shmp) == -1) {
        perror("shmdt");
        return 1;
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        return 1;
    }


    sem_close(semaphore);
    sem_unlink("/mysemaphorep2");

    printf("Writing Process: Complete\n");
    return 0;
}
