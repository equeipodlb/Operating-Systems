#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks) {
	struct stat statbuf;
	lstat(fname,&statbuf);
	if (S_ISDIR(statbuf.st_mode)) // Si es directorio
		get_size_dir(fname,blocks);
	else
		(*blocks) += statbuf.st_blocks; // Si es fichero
}


/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks) {
	DIR *dir = opendir(dname);
	struct dirent *entry;
	while((entry = readdir(dir)) != NULL){
		if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
			continue;
		char path[1024];
		snprintf(path,sizeof(path),"%s/%s",dname,entry->d_name);
		get_size(path,blocks); // Vuelvo a la funcion anterior que comprueba
	}
	closedir(dir);
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[]) {
	for (int i =1; i< argc; ++i) {
		size_t blocks = 0;
		get_size(argv[i],&blocks);
		printf("%ldK %s\n",blocks/2,argv[i]);
	}
}
