#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* programa que crea N hijos siguiendo un grado de dependencias. Usa las
 * versiones de execl y execlp */
const int N = 8;
pid_t Pids[N];
void mostrar(int i){
	char* msg = ("P%i", i+1);
	execLp("/bin/echo", "/bin/echo", msg, NULL );
}

int main(int argc, char **argv)
{
	pid_t pid;
	Pids[0] = fork();
	if(Pids[0] == 0)mostrar(0);
	waitpid(Pids[0],NULL,0);
	Pids[1] = fork();

	if(Pids[1] == 0)mostrar(1);
	Pids[4] = fork();
	
	if(Pids[4] == 0)mostrar(4);

	Pids[6] = fork();

	if(Pids[6] == 0)mostrar(6);
	waitpid(Pids[1],NULL,0);
	Pids[2] =fork();

	if(Pids[2] == 0)mostrar(2);
	Pids[]

 
		
	return 0;
}
