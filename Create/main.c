#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define BUF_SIZE 1024 //buffersize
#define SHM_KEY 0x0001 //shared memory key - recognizes the shared memory segment

typedef struct { //Struct to save shared memory segment
    int cnt;
    int complete;
    char buf[BUF_SIZE];
} shmsegment;

int write_buffer(char *buffer, int size) {
    static char ch = 'A';
    int filled_count;
    memset(buffer, ch, size - 1);
    buffer[size - 1] = '\0';
    filled_count = strlen(buffer);
    ch++;
    return filled_count;
}

int main() {
    int shmid, i;
    shmsegment *shmp;
    char *buffer;

    shmid = shmget(SHM_KEY, sizeof(shmsegment), IPC_CREAT | 0664);
    if (shmid == -1) {
        perror("Shared memory");
        return 1;
    }

    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *) -1) {
        perror("Shared memory attach error");
        return 1;
    }

    buffer = shmp->buf;
    for (i = 0; i < 5; i++) {
        shmp->cnt = write_buffer(buffer, BUF_SIZE);
        shmp->complete = 0;
        printf("Writing Process: Shared Memory Write: Wrote %d bytes\n", shmp->cnt);
        sleep(3);
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

    printf("Writing Process: Complete\n");
    return 0;
}
