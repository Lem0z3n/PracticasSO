#include <sys/types.h>
#include <sys/stat.h>
 #include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFSIZE 512

void main(int argc, char **argv) {
	struct stat sb;
	int fd_in, fd_out, n_read;
	char buffer[BUFSIZE];
	fd_in = open(argv[1], O_RDONLY); /* open the input file */
	if (fd_in < 0){
		perror("open"); exit(1);
	}
	if (lstat(argv[1], &sb) == -1) {
               perror("lstat");
               exit(EXIT_FAILURE);
	}
	switch (sb.st_mode & S_IFMT) {
		case S_IFREG:
				fd_out = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR | S_IWUSR); /* create the output file */
				if (fd_out < 0){
					close(fd_in);
					perror("open"); exit(1);
				}
			while ((n_read = read(fd_in, buffer, BUFSIZE)) > 0) {
				/* Transfer data from the buffer onto the output file */
				if (write(fd_out, buffer, n_read) < n_read) {
					perror("write");
					close(fd_in); close(fd_out);
					exit(1);
				}
				}
				if (n_read < 0) {
					perror("read");
					close(fd_in); close(fd_out);
					exit(1);
				}
		break;
		case S_IFLNK:
		;
			char* link;
			link = malloc(sb.st_size+1);
			readlink(argv[1],link,sb.st_size);
			link = link + '\0';
			if(symlink(link,argv[2]) == -1){
				perror("symlink");
					close(fd_in); close(fd_out);
					exit(1);
			}
		break;
		default:
		 printf("Unable to copy this kind of file.\n");
		 close(fd_in); close(fd_out);
		 exit(1);
		break;
	}
	close(fd_in); close(fd_out);
	exit(0);
	
}