#ifndef STUDENT_RECORD_H
#define STUDENT_RECORD_H

#include <stdio.h>

#define MAX_CHARS_NIF  9

typedef struct{
    int student_id; 
    char NIF[MAX_CHARS_NIF+1];  
    char* first_name;
    char* last_name;
} student_t;

typedef enum {
	STUDENT_ID=0,
	NIF,
	FIRST_NAME,
	LAST_NAME
} token_id_t;

typedef enum {
	VERBOSE_MODE,
	PIPE_MODE
} output_mode_t;

typedef enum{
	CREATE,
	LIST,
	CONCATENATE,
	QUERY
}Action;

struct options {
	FILE* outfile;
	output_mode_t output_mode;
	Action action;
	char* id;
	char NIF[MAX_CHARS_NIF+1];  
};


#endif