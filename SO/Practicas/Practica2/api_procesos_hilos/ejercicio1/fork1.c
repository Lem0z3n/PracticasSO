#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
/*programa que crea dos hijos: uno no cambia de ejecutable y otro si */

void createSon(const char * ejecutable, char ** argv_){
	if(fork() == 0){
			printf("I am son %d with father %d\n", getpid() , getppid());
			char * aux = malloc(512);
			sprintf(aux,"/bin/%s", ejecutable);
			execv(aux, argv_); //no se usar execvp
		}
}

int main(int argc, char *argv[])
{
	if(fork() == 0){
		printf("I am son %d with father %d\n", getpid() , getppid());
		createSon(argv[optind],argv);
		waitpid(-1, NULL, 0);
	}else{
		printf("I am parent %d\n", getpid());
		createSon(argv[optind],argv);
		waitpid(-1, NULL, 0);
	}
	return 0;
}
