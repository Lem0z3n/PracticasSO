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

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)


pthread_mutex_t* mutex;
sem_t* colaIzq;
sem_t* colaDrch;

int ciclistasIzq, ciclistasDrch, bidireccion, esperandoIzq, esperandoDrch;
//int bidireccion => -1 sentido derecha 0 bidireccion 1 sentido izquierda


void acceder_puente(int sentido){

  if(sentido == 1){
    while(bidireccion > 0){
      esperandoDrch++;
      sem_wait(colaDrch);
      pthread_mutex_lock(mutex);
    }
    ciclistasDrch++;
    if(ciclistasDrch > 2 && ciclistasIzq == 0)bidireccion = -1;
    pthread_mutex_unlock(mutex);
  }
  else{
    while(bidireccion < 0){
      esperandoIzq++;
      sem_wait(colaIzq);
      sem_wait(mutex);
    }
    ciclistasIzq++;
    if(ciclistasIzq > 2 && ciclistasDrch == 0)bidireccion == 1;
    sem_post(mutex);
  }
}

void salir_puente(int sentido){
  if (sentido == 1){
    if(bidireccion == 0){

      pthread_mutex_lock(mutex);
      ciclistasDrch--;

        pthread_mutex_unlock(mutex);
      sem_post(colaDrch);
    }else if(bidireccion == -1 ){
      if(esperandoIzq > 0) {
        pthread_mutex_lock(mutex);
        bidireccion = 0;
        ciclistasDrch--;
        pthread_mutex_unlock(mutex);
        sem_post(colaIzq);
      }else {
        sem_post(colaDrch);
      }
    }
  }else {
    if(bidireccion == 0 ){
      sem_post(colaIzq);

      pthread_mutex_lock(mutex);
      ciclistasIzq--;

        pthread_mutex_unlock(mutex);
    }else if(bidireccion == 1 ){
      if(esperandoDrch > 0) {
        pthread_mutex_lock(mutex);
        bidireccion = 0;
        ciclistasIzq--;
        pthread_mutex_unlock(mutex);
        sem_post(colaDrch);
      }else {
        sem_post(colaIzq);
      }
    }  
  }
}


int main(int argc, char* args[]){

return 0;
}