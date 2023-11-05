#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	char c;
	int ret;

	if (argc!=2 && argc != 3) {
		fprintf(stderr,"Usage: %s <file_name> <size>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	/* Read file byte by byte */
	int tam;
	if (argc == 3) tam = atoi(argv[2]); else tam = 1;
	while (fread(&c,tam,1,file) != 0) {
		/* Print byte to stdout */
		ret=fwrite(&c,tam,1,stdout);

		if (ret==EOF){
			fclose(file);
			err(3,"putc() failed!!");
		}
	}

	fclose(file);
	return 0;
}
