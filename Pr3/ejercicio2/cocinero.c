#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>


#define MAX_BUFFER 1024
#define M 10

int finish = 0;
int *buffer; // Shared buffer
sem_t *emptyPot;
sem_t *fullPot;


/*  Funcionamiento de los semáforos: sem_wait(emptyPot) resta 1 al contador y si es <0 bloquea
	al proceso que lo ha llamado. sem_post(fullPot) suma 1 al conatador y siempre desbloquea
	a uno si los hay bloqueados.
	De este modo, sem_wait(emptyPot) lo pone a -1 y bloquea al cocinero hasta que un salvaje
	le haga sem_post y tras cocinar hará sem_post para desbloquear a algún salvaje.
*/
// cook será esperar a que esté vacío -> reponer -> anunciarlo -> repetir
void cook(int *buffer) {
	while(!finish) {
		sem_wait(emptyPot);
		*buffer = M;
		printf("El cocinero acaba de reponer %d raciones\n",M);
		fflush(stdout);
		sem_post(fullPot);
	}
}

void handler(int signo) {
	finish = 1;
}

int main(int argc, char *argv[]) {
	int shd;

	//The producer creates the file
	shd = open("BUFFER", O_CREAT|O_RDWR, 0666);
	ftruncate(shd, sizeof(int));

	//Maps the file into the process address space
	buffer = (int*) mmap(NULL, sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, shd,0);
	*buffer = 0;

	//The producer creates the semaphores
	//En los problemas Productor-Consumidor, creo dos semáforos. Un ELEMENTS
	//que representa que hay datos y un GAPS que representa que hay huecos
	emptyPot = sem_open("EMPTY", O_CREAT|O_RDWR, 0666,0); // Inicializados a 0
	fullPot = sem_open("FULL",O_CREAT|O_RDWR, 0666,0);
	// Si no se han podido crear, error
	if (emptyPot == SEM_FAILED || fullPot == SEM_FAILED){
		exit(1);
	}
	// Estructura para recibir la señal de kill al cocinero
	struct sigaction action;
	action.sa_handler = *handler;
	sigaction(SIGTERM|SIGINT, &action, NULL);


	cook(buffer); // Ponerle a cocinar
	// Deshacer zona de memoria y cerrar fichero compartido
	munmap(buffer, sizeof(int));
	close(shd);
	unlink("BUFFER");
	// Desligar semáforos y cerrarlos
	sem_unlink("EMPTY");
	sem_unlink("FULL");
	sem_close(emptyPot);
	sem_close(fullPot);
	
	return 0;
}
