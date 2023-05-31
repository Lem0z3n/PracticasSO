#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

/* Forward declaration */
int get_size_dir(char *fname, blksize_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, blksize_t *blocks)
{
        int total = (((*blocks) * 512) / 1000);
        printf("file %s : %d KB\n", fname, total);
        return total;
}


/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, blksize_t *blocks)
{
        DIR * dir;
        struct dirent *dirent;
        struct stat sb;
        int total=0, *blockptr;
        if((dir = opendir(dname)) == NULL){
                perror("opendir");
                exit(EXIT_FAILURE);
        }
        errno = 0;
        while((dirent = readdir(dir)) != NULL){
                if(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0){
                }
                else{
                        if (lstat(dirent->d_name, &sb) == -1) {//no hay alguna manera de no llamar a lstat aqui? puede que usando d_reclen?
                                perror("lstat");
                                exit(EXIT_FAILURE);
                        }
                        switch (sb.st_mode & S_IFMT) {
                        case S_IFDIR:;
                        total = total + get_size_dir(dirent->d_name,&sb.st_blocks);
                        break;
                        case S_IFREG:;
                        total = total + get_size(dirent->d_name, &sb.st_blocks);
                        break;
                        default:
                        break;
                }
                }
        }
        if(errno != 0){
                perror("readdir");
                exit(EXIT_FAILURE);
        }
        printf("directory %s : %d KB\n", dname,total);
        return total;
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[])
{
        struct stat sb;
        int total=0;
        if (argc < 2) {
               fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
               exit(EXIT_FAILURE);
        }

        for(int i = 1; i < argc; i++){
                if (lstat(argv[i], &sb) == -1) {
                perror("lstat");
                exit(EXIT_FAILURE);
                }
                switch (sb.st_mode & S_IFMT) {
                        case S_IFDIR:;
                        total = total + get_size_dir(argv[i],&sb.st_blocks);
                        break;
                        case S_IFREG:;
                        total = total + get_size(argv[i], &sb.st_blocks);
                        break;
                        default:
                        break;
                }
        }
        printf("%d KB\n", total);
        return 0;
}
