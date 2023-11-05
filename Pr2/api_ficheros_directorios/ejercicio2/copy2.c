#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

void copy(int fdo, int fdd) {
	int n_read;
	char buffer[512];
	while ((n_read = read(fdo, buffer, 512)) > 0) {
		/* Transfer data from the buffer onto the output file */
		if (write(fdd, buffer, n_read) < n_read) {
			perror("write");
			close(fdo); 
			close(fdd);
			exit(1);
		}
	}
	if (n_read < 0) {
		perror("read");
		close(fdo); 
		close(fdd);
		exit(1);
	}
}

int main(int argc, char *argv[]) {
	int input, output;
	struct stat statbuf;
	lstat(argv[1],&statbuf);
	if (S_ISREG(statbuf.st_mode)){ // Copy of regular files
		input = open(argv[1], O_RDONLY);
		output = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC);
		copy(input,output);
		close(input);
		close(output);
	}
	else if (S_ISLNK(statbuf.st_mode)) { // If symlink, create new link pointing to same one
		printf("Creando copia del enlade simbólico\n");
		char *buffer = malloc(statbuf.st_size + 1);
		readlink(argv[1],buffer,statbuf.st_size + 1);
		buffer[statbuf.st_size] = '\0';
		symlink(buffer, argv[2]);
	}
	else {
		perror("Can't copy file\n");
		exit(1);
	}
	return 0;
}
