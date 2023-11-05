#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int mostrar(int fd,int N, int eFlag){
	int n_read = 0;
	char buffer;
	if (N == 0)
		lseek(fd,0,SEEK_SET);
	else if (eFlag == 0) {
		lseek(fd,N,SEEK_SET);
	}
	else
		lseek(fd,-N,SEEK_END);
	while ((n_read = read(fd, &buffer, 1)) > 0) {
		/* Transfer data from the buffer onto the std output */
		if (write(1, &buffer, n_read) < n_read) {
			perror("write");
			close(fd); 
			return 1;
		}
	}
	if (n_read < 0) {
		perror("read");
		close(fd); 
		return 1;
	}
}


int main(int argc, char *argv[]) {
	int opt, N = 0;
	int eFlag = 0;
	int retCode,input,fd;
	input = open(argv[1], O_RDONLY);
	printf("%s file opened, input value: %d\n",argv[1], input);
	while((opt = getopt(argc, argv, "hn:e")) != -1) {
		switch(opt){
			case 'h':
				fprintf(stderr,"Usage: %s <file> [-n N | -e]\n", argv[0]);
				exit(0);
				break;
			case 'n':
				N = atoi(optarg);
				break;
			case 'e':
				eFlag = 1;
				break;
		}
	}
	retCode = mostrar(input,N,eFlag);
	close(input);
	exit(retCode);
}
