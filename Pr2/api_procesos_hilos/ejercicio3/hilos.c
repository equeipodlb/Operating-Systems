#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct {
	int num;
	char prio;
}tuser;

void *thread_usuario(void *arg) {
	tuser* user = arg;
	int num = user->num;
	char prio = user->prio;
	
	
	printf("Soy el proceso con identificador %ld, usuario numero %d y prioridad %c\n",pthread_self(),num,prio);
	
	pthread_exit(0);
}

int main(int argc, char* argv[])
{
	int N = 10;
	int i;
	pthread_t* array = malloc(N*sizeof(pthread_t));
	tuser* arrayUsers = malloc(N*sizeof(tuser));
	for (i = 0; i < N; ++i){
		arrayUsers[i].num = i;
		if (i % 2 == 0)
			arrayUsers[i].prio = 'P';
		else
			arrayUsers[i].prio = 'N';
	}
	for (i = 0; i < N; ++i){
		pthread_create(&array[i],NULL,thread_usuario,&arrayUsers[i]);
	}
	for (i =0; i< N; ++i){
		pthread_join(array[i],NULL);
	}
	free(arrayUsers);
	free(array);
	return 0;
}
