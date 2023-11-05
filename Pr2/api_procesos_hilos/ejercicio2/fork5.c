#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* programa que crea N hijos siguiendo un grado de dependencias. Usa las
 * versiones de execl y execlp */


int main(int argc, char **argv)
{
	pid_t child_pid1;
	child_pid1 = fork();
	
	
	if (child_pid1 == 0){
		execlp("echo","echo","Soy P1", NULL);
	}
	waitpid(child_pid1);
	int child_pid2 = fork();
	if (child_pid2 == 0)
		execl("/bin/echo", "/bin/echo", "Soy P2", NULL);

	int child_pid5 = fork();	
	if (child_pid5 == 0)
		execlp("echo","echo", "Soy P5", NULL);
	int child_pid7 = fork();
	if (child_pid7 == 0)
		execlp("echo","echo", "Soy P7", NULL);
	waitpid(child_pid2);
	
	int child_pid4 = fork();
	if (child_pid4 == 0)
		execl("/bin/echo", "/bin/echo", "Soy P4", NULL);
	int child_pid3 = fork();
	if (child_pid3 == 0)
		execlp("echo","echo", "Soy P3", NULL);
	waitpid(child_pid4);
	waitpid(child_pid5);
	int child_pid6 = fork();
	if (child_pid6 == 0)
		execl("/bin/echo", "/bin/echo", "Soy P6", NULL);
	waitpid(child_pid6);
	waitpid(child_pid3);
	waitpid(child_pid7);
	int child_pid8 = fork();
	if (child_pid8 == 0)
		execl("/bin/echo", "/bin/echo", "Soy P8", NULL);
	

	return 0;
}
