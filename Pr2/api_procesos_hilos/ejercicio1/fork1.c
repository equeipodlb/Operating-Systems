#include <stdio.h>
#include <stdlib.h>

/*programa que crea dos hijos: uno no cambia de ejecutable y otro si */


int main(int argc, char **argv)
{
	pid_t child_pid1, child_pid2;
	child_pid1 = fork();
	
	if (child_pid1 > 0) {
		// ESTE codigo lo ejecuta SOLO el padre
		printf("Soy el proceso padre %d \n" , getpid());
		waitpid(child_pid1);
	} 
	else if (child_pid1 == 0){
		printf("Soy el proceso hijo %d .Mi padre es el proceso %d \n" , getpid(),getppid());
	}

	child_pid2 = fork();
	if (child_pid2 > 0) {
		// ESTE codigo lo ejecuta SOLO el padre
		printf("Soy el proceso padre %d \n" , getpid());
		waitpid(child_pid2);
	} 
	else if (child_pid2 == 0){
		printf("Soy el proceso hijo %d .Mi padre es el proceso %d \n" , getpid(),getppid());
		execvp(argv[1],&argv[1]);
	}

	// En un principio, este codigo lo ejecutan AMBOS PROCESOS
	// (salvo que alguno haya hecho un return, exit,execxx...)
	return 0;
}
