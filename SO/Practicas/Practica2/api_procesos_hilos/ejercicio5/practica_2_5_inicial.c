#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <signal.h>

pthread_cond_t  cond[11];
pthread_mutex_t mutex;
sem_t * semaphore[10];

int main(void)
{
    int fd1,fd2,i,pos;
    char c;
    char buffer[6];
    int condicion[10];

    fd1 = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    write(fd1, "00000", 5);
    for(int i = 0; i < 10; i++){
         condicion[i] = i;
         char  name[12];
         sprintf(name,"SEMAPHORE%i",condicion[i]);
         semaphore[condicion[i]] = sem_open(name, O_CREAT, 0700,0);
    }
    for (i=1; i < 10; i++) {
        if (fork() == 0) {
            /* Child */
            sem_wait(semaphore[condicion[i-1]]);
            fd2 = open("output.txt", O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
            sprintf(buffer, "%d", i*11111);
            //lseek(fd1, pos, SEEK_SET);
            write(fd2, buffer, 5);
            close(fd2);

            sem_post(semaphore[condicion[i]]);
            exit(0);
        }
    }
    sem_post(semaphore[0]);
    sem_wait(semaphore[9]);
    lseek(fd1, 0, SEEK_SET);
    printf("File contents are:\n");
    while (read(fd1, &c, 1) > 0)
        printf("%c", (char) c);
    printf("\n");
    close(fd1);
    exit(0);
}
