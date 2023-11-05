#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/*programa que crea dos hijos: uno no cambia de ejecutable y otro si */

pid_t child_pid1;

void handler(int signum) {
	if (kill(child_pid1,SIGKILL) == 0)
		printf("Señal de kill enviada de forma exitosa\n");
	else
		printf("Error al enviar SIGKILL\n");
}


int main(int argc, char **argv)
{
	int wstatus;
	child_pid1 = fork();
	
	if (child_pid1 > 0) {
		signal(SIGINT, SIG_IGN);
		// ESTE codigo lo ejecuta SOLO el padre
		struct sigaction action;
		action.sa_handler = *handler;
		action.sa_flags = SA_RESTART;
		sigaction(SIGALRM, &action, NULL);
		

		alarm(5);
		waitpid(child_pid1,&wstatus,0);
		printf("Valor de estado %d\n", wstatus);
		// COMPROBAR CAUSA DE FINALIZACIÓN DEL HIJO
		if (WIFEXITED(wstatus))
			printf("El proceso hijo ha terminado de forma normal\n");
		else if (WIFSIGNALED(wstatus))
			printf("El proceso hijo ha terminado mediante kill()\n");
			
	} 
	else if (child_pid1 == 0){
		printf("Soy el proceso hijo %d. Mi padre es el proceso %d \n" , getpid(),getppid());
		execvp(argv[1],&argv[1]);
	}

	// En un principio, este codigo lo ejecutan AMBOS PROCESOS
	// (salvo que alguno haya hecho un return, exit,execxx...)
	sleep(5);
	return 0;
}
