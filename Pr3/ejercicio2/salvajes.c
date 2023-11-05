#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>


#define NUMITER 3

int *buffer; // Shared buffer

sem_t *emptyPot;
sem_t *fullPot;

void eat(void) {
	unsigned long id = (unsigned long) getpid();
	printf("Savage %lu eating\n", id);
	sleep(rand() % 5);
}
// Los salvajes harán: Si está vacío -> avisar -> esperar a lleno ->
// -> cuando se reponga comer 1 ración -> repetir NUMITER veces

/*  Funcionamiento de los semáforos: empiezan los dos en cero con 0 raciones.
	Por tanto, algún salvaje hará semPost(emptyPot) emptyPot
	sumándole 1 y dejando "pasar" al cocinero, que repondrá y hará semwait(emptyPot)
	A continuación el salvaje hará sem_wait(fullPot), restándole 1 y bloqueándose hasta
	que el cocinero le haga un sem_post(fullPot)
	Cuando hayamos dejado pasar al salvaje, este comerá.
*/
void savages(int *buffer) {
	for (int i = 0; i < NUMITER; i++){
		if (*buffer == 0){
			sem_post(emptyPot);
			sem_wait(fullPot);
		}
		*buffer -= 1;

		eat();
		printf("Quedan %d raciones\n", *buffer);
	}
}

int main(int argc, char *argv[]) {
	int shd;

	// Consumer opens file
	shd = open("BUFFER", O_RDWR);

	//Maps the file into the process address space
	buffer = (int *) mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, shd, 0);

	//Consumer opens semaphores
	emptyPot = sem_open("EMPTY",0);
	fullPot = sem_open("FULL",0);
	
	if (emptyPot == SEM_FAILED || fullPot == SEM_FAILED){
		exit(1);
	}

	savages(buffer);
	// El consumidor no hace unlink, solamente los cierro
	munmap(buffer, sizeof(int));
	close(shd);

	sem_close(emptyPot);
	sem_close(fullPot);

	return 0;
}
