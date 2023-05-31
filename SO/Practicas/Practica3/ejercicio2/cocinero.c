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
#include "mesa.h"

 #define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define M 10
#define MAX_BUFFER 1024
#define DATA_TO_PRODUCE 100000

sem_t *sem_mtx;
sem_t *cook_q;
sem_t *sav_q;
int *buffer;

struct t_mesa * mesa;

void putServingsInPot(int servings)
{
	printf("cook adding %i servings \n", servings);
	mesa->comida = servings;
}

void putServingsSafe(int servings){
	while(mesa->comida > 0){
		mesa->cook_waiting++;
		sem_post(sem_mtx);
		sem_wait(cook_q);
		sem_wait(sem_mtx);
	}

	putServingsInPot(M);

	while(mesa->savages_waiting > 0){
		sem_post(sav_q);
		mesa->savages_waiting--;
	}

	sem_post(sem_mtx);
	
}

void cook(void)
{
 while(mesa->finish == 0){
	putServingsSafe(M);
 }
}

int main(int argc, char *argv[])
{
	int shd;
	
	if((shd = shm_open("/shMesa", O_CREAT | O_RDWR, 0700)) == -1)
		handle_error("shm_open");
	ftruncate(shd,sizeof(struct t_mesa));

	mesa = (struct t_mesa*) mmap(NULL, sizeof(struct t_mesa), PROT_WRITE | PROT_READ, MAP_SHARED, shd, 0);

	mesa->cook_waiting = 0;
	mesa->comida = 0;
	mesa->savages_waiting=0;
	mesa->finish = 0;

	close(shd);

	sem_mtx = sem_open("MUTEX", O_CREAT, 0700, 1);
	cook_q = sem_open("COOK", O_CREAT, 0700, 0);
	sav_q = sem_open("SAVAGES", O_CREAT, 0700, 0);


	cook();

	close(shd);
	

	sem_close(sem_mtx);
	sem_close(cook_q);
	sem_close(sav_q);


	sem_unlink("MUTEX");
	sem_unlink("COOK");
	sem_unlink("SAVAGES");
	munmap(mesa,  sizeof(struct t_mesa));

	return 0;
}
