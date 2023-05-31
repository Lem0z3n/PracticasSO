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

#define CAPACITY 5
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

int turn_disp= 0, vip_turn_disp = 0, clients_inside=0;
int cur_turn = 0, cur_vip_turn = 0, vip_waiting = 0;

pthread_mutex_t m;
pthread_cond_t c_v, c;

struct thread_info {    /* Used as argument to thread_start() */
		pthread_t thread_id;        /* ID returned by pthread_create() */
		int       clientId;       /* Application-defined thread # */
		int vip; 
};

void enter_normal_client(int id)
{
	int turno;
	pthread_mutex_lock(&m);
	turno = turn_disp++;
	while(turno != cur_turn || vip_waiting  > 0 || clients_inside >= CAPACITY ){
		pthread_cond_wait(&c, &m);
	}
	cur_turn++;
	clients_inside++;
	pthread_cond_broadcast(&c);
	pthread_mutex_unlock(&m);
}

void enter_vip_client(int id)
{
	
	int turno;
	pthread_mutex_lock(&m);
	turno = vip_turn_disp++;
	vip_waiting++;
	while(turno != cur_vip_turn || clients_inside >= CAPACITY){
		pthread_cond_wait(&c_v, &m);
	}
	clients_inside++;
	cur_vip_turn++;
	vip_waiting--;
	pthread_cond_broadcast(&c_v);
	pthread_mutex_unlock(&m);
}

void dance(int id, int isvip)
{
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{

	printf("Client %2d (%s) exiting disco\n", id, VIPSTR(isvip));
	pthread_mutex_lock(&m);
	clients_inside--;
	(vip_waiting > 0) ? pthread_cond_broadcast(&c_v) : pthread_cond_broadcast(&c);
	pthread_mutex_unlock(&m);
}

void *client(void *arg)
{

  struct thread_info *tinfo = arg;
		(tinfo->vip == 0) ? enter_vip_client(tinfo->clientId) : enter_normal_client(tinfo->clientId);
	dance(tinfo->clientId, tinfo->vip);
	disco_exit(tinfo->clientId, tinfo->vip);
}

int main(int argc, char *argv[])
{
	int fd, m, s;
	int * vips;
	char in[1];
	struct thread_info *tinfo ;
	pthread_attr_t attr;
	fd = open(argv[optind],O_RDONLY); /* open the input file */
	if (fd< 0){
		perror("open"); exit(1);
	}
	read(fd,in,1);
	m = atoi(in);
	tinfo = calloc(m, sizeof(struct thread_info));
	vips = calloc(m, sizeof(int));
	read(fd, in, 1); //eliminar salto de linea
	for(int i = 0; i < m; i++){
		read(fd, in, 1);
		vips[i] = atoi(in);
		read(fd, in, 1); //eliminar salto de linea
	}
	if (tinfo == NULL)handle_error("calloc");
	read(fd,in,1); //leer salto de linea.
	for(int tnum = 0; tnum < m; tnum++){
		tinfo[tnum].thread_id = tnum;
		tinfo[tnum].clientId = tnum;
		tinfo[tnum].vip = vips[tnum];
		s= pthread_create(&tinfo[tnum].thread_id, NULL , client, &tinfo[tnum]);
		if (s != 0)
			handle_error_en(s, "pthread_create");
	}

	for (int tnum = 0; tnum < m; tnum++) {
				s = pthread_join(tinfo[tnum].thread_id, NULL);
				if (s != 0)
						handle_error_en(s, "pthread_join");
				//free(res);      /* Free memory allocated by thread */
	}
	return 0;
}
