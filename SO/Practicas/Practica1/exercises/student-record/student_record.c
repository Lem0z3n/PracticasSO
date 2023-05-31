#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include <stdbool.h>
#include "student_record.h"

student_t* fillEntries(char* records){
char* token;
//printf("filling entry, %s \n", records);
student_t* cur_student = malloc(sizeof(student_t));
token_id_t token_id=STUDENT_ID;
while((token = strsep(&records, ":"))!=NULL) {
	switch(token_id) {
	case STUDENT_ID:
		cur_student->student_id = atoi(token);
		break;
	case NIF:
		strcpy(cur_student->NIF,token);
		break;
	case FIRST_NAME:
		cur_student->first_name = malloc(strlen(token));
		strcpy(cur_student->first_name,token);
		break;
	case LAST_NAME:
		cur_student->last_name = malloc(strlen(token));
		strcpy(cur_student->last_name,token);
		break;
	default:
		break;
	}
	token_id++;
}
return cur_student;
}


student_t* parse_records(char* records[], int* nr_records){

	student_t* students;
  students = malloc(sizeof(student_t)*(*nr_records));
  memset(students,0,sizeof(student_t));
	for(int i=0; i< *nr_records; i++){
		students[i] = *fillEntries(records[optind+i]); 
	}
	return students;
}

int dump_entries(student_t* entries, int nr_entries, FILE* students){
	for(int i = 0; i < nr_entries; i++){
		char * aux = malloc(sizeof(int));
		sprintf(aux,"%d",entries[i].student_id);
		fwrite(aux, sizeof(int), 1, students);
		fwrite(entries[i].first_name, strlen(entries[i].first_name), 1, students);
		fwrite(entries[i].last_name, strlen(entries[i].last_name), 1, students);
		fwrite(entries[i].NIF, strlen(entries[i].NIF), 1, students);
		
	}
	//fwrite(entries, sizeof(student_t), nr_entries, students);
	return 0;
}

char* loadstr(FILE* students){
  int p=2; //la posicion inical del puntero y el salto de linea
  char* sol;
  while(getc(students) != '\n'){
    p++;
  }
  sol = malloc(p);
  fseek(students,-(p),SEEK_CUR);
  fread(sol,1,p-1,students); //para no leer el salto de linea
	fseek(students,2,SEEK_CUR); //movemos el cursor despues del salto de linea.
  return sol;
}

student_t* read_student_file(FILE* students, int* nr_entries){
  student_t* arStudents;
	int nums = (*nr_entries);
  arStudents = malloc(sizeof(student_t)*nums);
  for(int i=0; i < nums; i++){
		arStudents[i] = *fillEntries(loadstr(students));
	}
  return arStudents;
}

int checkDB(char* token, student_t* students, int nr_entries){
	int resl=-1;
	student_t* indb = malloc(sizeof(student_t)+16);
	indb->student_id= -1;
	for(int i = 0; i < nr_entries; i++){
		if(students[i].student_id == atoi(token)){
			 indb = &students[i];
			 resl = i;
			 }
		else if(strcmp(students[i].NIF,token)==0) {
			indb = &students[i];  
			resl = i;
		}
	}
	//free(indb);
	return resl;
}

int getNrEntries(FILE* operationsFile){
	fseek(operationsFile,SEEK_SET,0);
	char* aux = malloc(sizeof(int));
	fread(aux, sizeof(int), 1, operationsFile);
	return atoi(aux);
}

int main(int argc, char *argv[])
{
	int opt;
	struct options options;
	FILE* operationsFile;
	char* filename;
	char defNif[MAX_CHARS_NIF+1];
	student_t* students;
	int nr_entries;
	int* entryPntr = &nr_entries;
	
	/* Initialize default values for options */
	options.outfile=stdout;
	options.output_mode=VERBOSE_MODE;
	strcpy(defNif, "000000000");
	/* Parse command-line options */
	while((opt = getopt(argc, argv, "hcalqf:i:n:")) != -1) {
		switch(opt) {
		case 'h':
			fprintf(stderr,"Usage: %s -f file [ -h | -l | -c | -a | -q [-i | -n ID] ]\n",argv[0]);
			exit(0);
		case 'f':
			filename = malloc(strlen(optarg));
			strcpy(filename,optarg);
			//guardamos el nombre del archivo que debera abrirse con distintos
			//permisos en los distintos modos de ejecucion
			break;
		case 'c':
		if ((operationsFile=fopen(filename ,"w"))==NULL) {
				fprintf(stderr, "The output file %s could not be opened: ",
				        filename);
				perror(NULL);				
				exit(EXIT_FAILURE);
			} else{
				printf("Creating file %s \n", filename);
			}
			options.action = CREATE;
			break;
		case 'l':
		if ((operationsFile=fopen(filename ,"r"))==NULL) {
				fprintf(stderr, "The output file %s could not be opened: ",
				        filename);
				perror(NULL);				
				exit(EXIT_FAILURE);
			} else{
				printf("Reading file %s \n", filename);
			}
			options.action = LIST;
			break;
		case 'a':
		if ((operationsFile=fopen(filename ,"r"))==NULL) {
				fprintf(stderr, "The output file %s could not be opened: ",
				        filename);
				perror(NULL);				
				exit(EXIT_FAILURE);
			} else{
				printf("apending to file %s \n", filename);
			}
			options.action = CONCATENATE;
			break;
		case 'q':
		if ((operationsFile=fopen(filename ,"r"))==NULL) {
				fprintf(stderr, "The output file %s could not be opened: ",
				        filename);
				perror(NULL);				
				exit(EXIT_FAILURE);
			} else{
				printf("Reading file %s \n", filename);
			}
			options.action = QUERY;
			break;
		case 'i':
			options.id = malloc(sizeof(int));
			strcpy(options.NIF,defNif);
			strcpy(options.id, argv[optind-1]);
		 	break;
		case 'n':
			//options.NIF = malloc(strlen(argv[optind]));
			strcpy(options.NIF,argv[optind-1]);
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}
	nr_entries = argc-optind;
	switch (options.action)
	{
	case (CREATE):		
		printf("Creating...\n");
		students = malloc(sizeof(student_t)*nr_entries);
		memset(students, '\0', sizeof(student_t)*nr_entries);
		printf("Parsing %d, entries..\n", *entryPntr);
		students = parse_records(argv, entryPntr);
		fwrite(entryPntr, sizeof(int), 1, operationsFile);
		dump_entries(students,nr_entries,operationsFile);
		printf("%d, Entries filled succesfully.\n", nr_entries);
		break;
	case(LIST):
		*entryPntr = getNrEntries(operationsFile);
		students = malloc(sizeof(student_t)*nr_entries);
		students = read_student_file(operationsFile,entryPntr);
		int recorded = 0;
		student_t* cur_student;
		while(recorded < nr_entries){
			cur_student = &students[recorded];
				printf("[Entry #%d]\n",recorded+1);
				printf("\tstudent_id=%d\n\tNIF=%s\n\t"
								"first_name=%s\n\tlast_name=%s\n",
								cur_student->student_id, cur_student->NIF,
								cur_student->first_name, cur_student->last_name);
			recorded++;
			}
		break;
	case(	CONCATENATE):
		//consultar que no existan antiguos alumnos con mismo id
		*entryPntr = getNrEntries(operationsFile);
		students = malloc(sizeof(student_t)*(nr_entries + (argc - optind)));
		students = read_student_file(operationsFile,entryPntr);
		int i = 0;
		while(optind < argc){ 
			char * arg = malloc(strlen(argv[optind]));
			strcpy(arg,argv[optind]);
			char * token = strsep(&arg, ":");
			if(checkDB(token,students,nr_entries) != -1){
				printf("Id %s allready in database.\n", token);
			}else {
				students[nr_entries]=*fillEntries(argv[optind]);
				nr_entries++;
				i++;
			}
			optind++;
		}
		fclose(operationsFile); //cerramos el archivo para cambiarlo de modo.
		operationsFile=fopen(filename ,"w"); //lo abrimos con modo escritura
		fwrite(students, sizeof(student_t), nr_entries, operationsFile);
		dump_entries(students,nr_entries,operationsFile);
		printf("%d entries added succesfully\n",i);
		break;
	case(QUERY):
		{
		*entryPntr = getNrEntries(operationsFile);
		students = malloc(sizeof(student_t)*nr_entries); //leer el archivo para conseguir nr_entries
		students = read_student_file(operationsFile,entryPntr);
		//un bucle que busque en students lo pedido si -i o -n e imprima la info.
		char* qer;
		if(strcmp(defNif,options.NIF) != 0){	
			qer = malloc(sizeof(options.NIF));
			strcpy(qer,options.NIF);
		}else{
			qer = malloc(sizeof(options.id));
			strcpy(qer,options.id);
		}
		
		//if ternario entre options.
		student_t* found = malloc(sizeof(student_t));
		int index = checkDB(qer,students,nr_entries);
		if( index != -1){
			found = &students[index];
				printf("\t[Entry #%d]\n\t"
							 "student_id=%d\n\tNIF=%s\n\t"
			        "first_name=%s\n\tlast_name=%s\n",
							index+1,
			        found->student_id, found->NIF,
			        found->first_name, found->last_name);
		}else{
			printf("No entries found.\n");
		}
		break;
		}
	default:
		break;
	}
	return 0;
}
