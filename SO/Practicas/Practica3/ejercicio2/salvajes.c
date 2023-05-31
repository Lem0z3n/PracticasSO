#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include "mesa.h"

 #define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define NUMITER 3
#define MAX_BUFFER 1024
#define DATA_TO_PRODUCE 100000

sem_t *sem_mtx;
sem_t *cookq;
sem_t *savq;
struct t_mesa * mesa;
int done = 0;

void eat(void)
{
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	sleep(rand() % 5);
}

void getServingsFromPot(void)
{
	done++;
	mesa->comida--;
	eat();
	if(done == 100){
		mesa->finish = 1;
	}
}

void getServingsSafe(void){
	if(mesa->cook_waiting > 0)
	 sem_post(cookq);
	sem_wait(sem_mtx);

	while(mesa->comida == 0){
		if(mesa->cook_waiting > 0){
			sem_post(cookq);
			mesa->cook_waiting--;
		}
		mesa->savages_waiting++;
		sem_post(sem_mtx);
		sem_wait(savq);
		sem_wait(sem_mtx);
	}

	getServingsFromPot();
	sem_post(sem_mtx);
}



void savages(void)
{
	while(mesa->finish == 0){
		getServingsSafe();
		done++;
	}
}

int main(int argc, char *argv[])
{
	int shd;

	if((shd = shm_open("/shMesa", O_RDWR, 0700)) == -1)
		handle_error("shm_open");


	mesa = (struct t_mesa *) mmap(NULL, sizeof(struct t_mesa), PROT_READ | PROT_WRITE, MAP_SHARED,shd,0);

	sem_mtx = sem_open("MUTEX",0);
	cookq = sem_open("COOK",0);
	savq  = sem_open("SAVAGES",0);

	//Consumer(buffer);
	savages();


	close(shd);

	sem_close(sem_mtx);
	sem_close(cookq);
	sem_close(savq);

	munmap(mesa,  sizeof(struct t_mesa));
	return 0;
}
