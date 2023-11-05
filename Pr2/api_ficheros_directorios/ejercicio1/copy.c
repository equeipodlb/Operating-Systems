#include <sys/types.h>
#include <sys/stat.h>
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

int main(int argc, char *argv[]){
	int input, output;
	input = open(argv[1], O_RDONLY);
	output = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC);
	copy(input,output);
	close(input);
	close(output);
	return 0;
}
