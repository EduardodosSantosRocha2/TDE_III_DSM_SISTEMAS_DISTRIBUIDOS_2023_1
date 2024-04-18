#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define SHM_KEY 0x0001

typedef struct {
    int cnt;
    int complete;
    char buf[BUF_SIZE];
} shmsegment;

int main() {
    int shmid;
    shmsegment *shmp;
    shmid = shmget(SHM_KEY, sizeof(shmsegment), 0664);
    if (shmid == -1) {
        perror("Shared memory");
        return 1;
    }

    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *) -1) {
        perror("Shared memory attach error");
        return 1;
    }

    while (shmp->complete != 1) {
        printf("Segment contains:\n\"%s\"\n", shmp->buf);
        if (shmp->cnt == -1) {
            perror("read");
            return 1;
        }
        printf("Reading Process: Shared Memory: Read %d bytes\n", shmp->cnt);
        sleep(7); // 7 segundos
    }

    printf("Reading Process: Reading Done, Detaching Shared Memory\n");
    if (shmdt(shmp) == -1) {
        perror("shmdt");
        return 1;
    }

    printf("Reading Process: Complete\n");
    return 0;
}
