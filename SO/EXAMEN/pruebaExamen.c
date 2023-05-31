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
#include <pthread.h>
#include <string.h>

#define MAXPROC 16

struct task{
  int valid;
  int next[MAXPROC];
  int id;
};



struct task * tasks;

pthread_mutex_t  mut;
pthread_cond_t  ready;

sem_t * semaphore;


void wait_for_predecesors(int id){
 for(int i = 0; i < MAXPROC; i++){
    if(tasks[id].next[i] == 1){
      sem_wait(semaphore);
    }
  }
}

void notify_succesors(int id){
  sem_post(semaphore);
}

void *task_body(void *arg){
  int id = *(int *) arg;
  wait_for_predecesors(id);
  printf("Task %d running\n",id);
  notify_succesors(id);
  return NULL;
}

void print_graph(struct task tasks[], int n){
  int i,j;

  for(i = 0; i <n; i++){
    if (!tasks[i].valid)
    {
      continue;
    }
    printf("%d :  espera a", tasks[i].id);
    for(j = 0; j <n; j++){
      if(tasks[i].next[j])
              printf("  %d,",j);
    }
    printf("\n");
    
  }
}

int parse_tasks( struct task tasks[], int fd){
  char * datos;
  char * buf = malloc(1);
  int pos=0, i=0;
  char * token;
  int nr_entries = 0;


  while(read(fd,buf,1) != 0){
    lseek(fd,-1,SEEK_CUR);
    while(strcmp(buf,"\n") != 0){
      read(fd,buf,1);
      pos++; //leo hasta el salto de linea
    
    }
    lseek(fd,-pos,SEEK_CUR);
    datos = malloc(pos);
    read(fd,datos,pos);
    int j = 0, numPadres = 0;
    while((token = strsep(&datos, ",")) != NULL){
      if(j == 0){
        tasks[i].id = atoi(token);
        nr_entries++;
        tasks[i].valid = 1;
        j++;
      } //troceo y parseo la entrada
      else if(j == 1){
        numPadres = atoi(token);
        j++;
      }
      else if(numPadres > 0){
        tasks[i].next[atoi(token)] = 1; 
      }
    }
    datos = "";
    i++;
    pos = 0;
  }

  return nr_entries;
}



void main(int argc, char * argv[]){
  
  int fd;

  tasks = malloc(sizeof(struct task)*MAXPROC);


  pthread_t *thread_id;


  if(argc < 2){
    printf("you need to specify filename\n");
    exit(EXIT_FAILURE);
  }

  fd = open(argv[1],O_RDONLY);
  if(fd == -1){
    fprintf(stderr, "The output file could not be opened: ");
				perror(NULL);				
				exit(EXIT_FAILURE);
  }

  for(int i = 0; i < MAXPROC; i++){
    tasks[i].valid = 0;
    for(int j = 0; j < MAXPROC; j++){
      tasks[i].next[j] = 0;
    }
  }
  int n = 0;
  n =  parse_tasks(tasks,fd);

  print_graph(tasks,n);

  char * nombreSem = malloc(512);
  semaphore = sem_open("sem", O_CREAT, 0700, 0);


  thread_id = calloc(n, sizeof(int));

  int  s;
  for(int i = 0; i < n; i++){
    thread_id[i] = i;
    s = pthread_create(&thread_id[i],NULL,task_body, &tasks[i].id);
    if(s != 0){
      fprintf(stderr, "ERROR OCURRED CREATING THREADS: ");
				perror(NULL);				
				exit(EXIT_FAILURE);
    }
  }

  /*for(int i = 0; i < n; i++){
    s = pthread_join(thread_id[i],NULL);
    if(s != 0){
      fprintf(stderr, "ERROR OCURRED JOINING THREADS: ");
				perror(NULL);				
				exit(EXIT_FAILURE);
    }
  }*/

  exit(EXIT_SUCCESS);
  
}