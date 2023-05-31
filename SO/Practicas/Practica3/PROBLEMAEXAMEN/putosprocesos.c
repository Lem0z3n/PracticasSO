#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

 #define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)


void main(){
  pid_t start = getpid();
  pid_t pid;
  int n=0;
  for(int i = 0; i < 3; i++){
    n += 2;
    pid = fork();
    //printf(" padre %i\n hijo %i \n",start, pid);
    //execl("/bin/echo", "/bin/echo", "HELLO",NULL);
    if(pid == 0){
      write(1,"Hello\n",6);
    }
  }
  while (waitpid(NULL)!=-1){};
  printf("valor de N = %i \n", n);
}