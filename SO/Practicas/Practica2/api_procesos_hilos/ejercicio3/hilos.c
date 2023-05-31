#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

 #define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define MAX_T = 10;

struct thread_info {    /* Used as argument to thread_start() */
		pthread_t thread_id;        /* ID returned by pthread_create() */
		int       userID;       /* Application-defined thread # */
		char     *priority;      /* From command-line argument */
};


void *thread_usuario(void *arg)
{
  struct thread_info * tinfo = arg;
	//free(tinfo);
	printf("Soy hilo, %ld\n",pthread_self());
}

int main(int argc, char* argv[])
{
	struct thread_info *tinfo ;
	pthread_attr_t attr;
	int s,stack_size, nThreads;
	void *res;

	if(argc < 2){
		printf("Use: Nº of users.\n");
	}
  nThreads = atoi(argv[1]); //feo
	s = pthread_attr_init(&attr);
	if (s != 0)	handle_error_en(s, "pthread_attr_init");
	tinfo = calloc(nThreads, sizeof(struct thread_info));
	if (tinfo == NULL)handle_error("calloc");
  for(int tnum = 0; tnum < nThreads; tnum++){
		tinfo[tnum].thread_id = tnum;
		if(tnum % 2 == 0){
			tinfo[tnum].priority = "P";
		}else {
			tinfo[tnum].priority = "N";
		}
		s= pthread_create(&tinfo[tnum].thread_id,&attr,&thread_usuario, &tinfo[tnum]);
		if (s != 0)
                   handle_error_en(s, "pthread_create");

	}
	s = pthread_attr_destroy(&attr);
           if (s != 0)
               handle_error_en(s, "pthread_attr_destroy");

	for (int tnum = 0; tnum < nThreads; tnum++) {
				s = pthread_join(tinfo[tnum].thread_id, &res);
				if (s != 0)
						handle_error_en(s, "pthread_join");
				//free(res);      /* Free memory allocated by thread */
	}

	free(tinfo);
	exit(EXIT_SUCCESS);
	return 0;
}
