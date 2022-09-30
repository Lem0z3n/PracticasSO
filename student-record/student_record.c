#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include <stdbool.h>
#include "student_record.h"

void fillEntries(student_t* students, char* records, int* nr_records){
int recorded = 0;
char* token;
while(recorded < *nr_records){
    student_t* cur_student;
    token_id_t token_id=STUDENT_ID;
    cur_student = &students[recorded];
    while((token = strsep(&records, ":"))!=NULL) {
			switch(token_id) {
			case STUDENT_ID:
				cur_student->student_id = atoi(token);
				break;
			case NIF:
				strcpy(cur_student->NIF,token);
				break;
			case FIRST_NAME:
				strcpy(cur_student->first_name,token);
				break;
			case LAST_NAME:
				strcpy(cur_student->last_name,token);
				break;
			default:
				break;
			}
			token_id++;
		}
  }
}


student_t* parse_records(char* records[], int* nr_records){
  student_t* students;
  int numrecords = *nr_records;
	int aaaa = 1;  
	int* a = &aaaa;
	char* recordStr;
  students = malloc(sizeof(student_t)* numrecords);
  memset(students,0,sizeof(student_t)*numrecords);
	for(int i = 0; i < numrecords; i++){
		recordStr = malloc(sizeof(records[i]));
		recordStr = *records;
		fillEntries(students,recordStr,a);
	}
  return students;
}

int dump_entries(student_t* entries, int nr_entries, FILE* students){
  int recorded = 0;
  student_t* cur_student;
  while(recorded < nr_entries){
    cur_student = &entries[recorded];
      fprintf(students,"[Entry #%d]\n",recorded);
			fprintf(students,"\tstudent_id=%d\n\tNIF=%s\n\t"
			        "first_name=%s\n\tlast_name=%s\n",
			        cur_student->student_id, cur_student->NIF,
			        cur_student->first_name, cur_student->last_name);
    recorded++;
    }
	return 0;
}

student_t* read_student_file(FILE* students, int* nr_entries){
  char* buffer;
  student_t* arStudents;
  fread(&nr_entries, 4, 1, students);
  int numentries = *nr_entries;
  arStudents = malloc(sizeof(student_t)* numentries);
	buffer = malloc(sizeof(student_t));
  fread(buffer, sizeof(student_t), *nr_entries, students);
  fillEntries(arStudents,buffer,nr_entries);
  return arStudents;
}

char* loadstr(FILE* students){
  int p=0;
  char* sol;
  while(getc(students) != '\0'){
    p++;
  }
  sol = malloc(p);
  fseek(students,-p,SEEK_CUR);
  fread(sol,1,p,students);
  return sol;
}

student_t* checkDB(char* token, student_t* students, int nr_entries){
	student_t* indb = malloc(sizeof(student_t));
	for(int i = 0; (i < nr_entries) && (indb == NULL); i++){
		if(students[i].student_id == atoi(token)) indb = &students[i];
		else if(strcmp(students[i].NIF,token)) indb = &students[i];  
	}
	return indb;
}

int main(int argc, char *argv[])
{
	int opt;
	struct options options;
	FILE* operationsFile;
	student_t* students;
	int nr_entries;
	int* entryPntr = &nr_entries;
	
	/* Initialize default values for options */
	options.outfile=stdout;
	options.output_mode=VERBOSE_MODE;

	/* Parse command-line options */
	while((opt = getopt(argc, argv, "hcalqf:i:n:")) != -1) {
		switch(opt) {
		case 'h':
			fprintf(stderr,"Usage: %s -f file [ -h | -l | -c | -a | -q [-i | -n ID] ]\n",argv[0]);
			exit(0);
		case 'f':
			if ((operationsFile=fopen(optarg ,"w"))==NULL) {
				fprintf(stderr, "The output file %s could not be opened: ",
				        optarg);
				perror(NULL);				
				exit(EXIT_FAILURE);
			}
			break;
		case 'c':
			options.action = CREATE;
			break;
		case 'l':
			options.action = LIST;
			break;
		case 'a':
			options.action = CONCATENATE;
			break;
		case 'q':
			options.action = QUERY;
		default:
			exit(EXIT_FAILURE);
		}
	}
	nr_entries = argc-optind;
	switch (options.action)
	{
	case (CREATE):
		students = malloc(sizeof(parse_records(argv,entryPntr)));
		students = parse_records(argv,entryPntr);
		dump_entries(students,nr_entries,operationsFile);
		break;
	case(LIST):
		students = malloc(sizeof(read_student_file(operationsFile,entryPntr)));
		students = read_student_file(operationsFile,entryPntr);
		int recorded = 0;
  	student_t* cur_student;
		while(recorded < nr_entries){
    cur_student = &students[recorded];
      printf("[Entry #%d]\n",recorded);
			printf("\tstudent_id=%d\n\tNIF=%s\n\t"
			        "first_name=%s\n\tlast_name=%s\n",
			        cur_student->student_id, cur_student->NIF,
			        cur_student->first_name, cur_student->last_name);
    recorded++;
    }
		break;
	case(	CONCATENATE):
		//consultar que no existan antiguos alumnos con mismo id
		students = malloc(sizeof(read_student_file(operationsFile,entryPntr)));
		students = read_student_file(operationsFile,entryPntr);
		char* entry, *token;
		int a = 1;
		int* aptr = &a;
		while((entry = strsep(argv, " "))!=NULL){
			int i = 0;
			token = strsep(argv, ":");
			if(checkDB(token,students,nr_entries) != NULL){
				printf("Id %s allready in database.\n", token);
			}else {
				fillEntries(students, entry, aptr);
				nr_entries++;
				i++;
			}
			printf("%d entries added succesfully",i);
		}
		break;
	case(QUERY):
		//un bucle que busque en students lo pedido si -i o -n e imprima la info.
		char* query;
		student_t* found = malloc(sizeof(student_t));
		found = checkDB(query,students,nr_entries);
		if(found != NULL){
				printf("\tstudent_id=%d\n\tNIF=%s\n\t"
			        "first_name=%s\n\tlast_name=%s\n",
			        found->student_id, found->NIF,
			        found->first_name, found->last_name);
		}else{
			printf("No entries found.\n");
		}
		break;
	default:
		break;
	}
	return 0;
}