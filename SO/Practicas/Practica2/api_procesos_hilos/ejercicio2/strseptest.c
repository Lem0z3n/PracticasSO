#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


int main(int argc, char* argv[]){
  ;;
 char numExp[5] = "25E2";
 float valor = atof(numExp);
 printf("%f * 2 = %f", valor, valor*2);
  return 0;
}