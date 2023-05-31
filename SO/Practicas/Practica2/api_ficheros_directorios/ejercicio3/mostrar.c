#include <sys/types.h>
#include <sys/stat.h>
 #include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
	int opt, N = 0, end = 0, fd, filePos = 0;
	while((opt = getopt(argc, argv, "n:e")) != -1){
		switch (opt)
		{
		case 'n':
			N = atoi(argv[3]);
			break;

		case'e':
			N = -N;
			filePos = 2;
			break;
		default:
			break;
		}
	}

	fd = open(argv[optind], O_RDONLY);
	if (fd < 0){
	perror("open"); exit(1);
	}
	lseek(fd,N,filePos);
	char* auxBuf;
	int count=0;
	auxBuf = malloc(1);
	while(read(fd,auxBuf,1) != 0){
		count++;
	}
	lseek(fd,-count,SEEK_CUR);
	auxBuf = malloc(count+1);
	read(fd,auxBuf,count);
	printf("%s\n",auxBuf);
	
return 0;
}
