#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 2
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

pthread_mutex_t mtx;
pthread_cond_t vipsEsperando, novipsEsperando;
int numDentro, numVipsFuera;

typedef struct {
	int id;
	int vip;
}tclient;

void enter_normal_client(int id) {
	pthread_mutex_lock(&mtx);
	// El cliente vip debe bloquearse mientras que la disco este fuera o haya vips esperando
	while (numDentro == CAPACITY || numVipsFuera > 0){
		printf("Cliente NO VIP %2d se queda a la espera\n", id);
		pthread_cond_wait(&novipsEsperando,&mtx);
	}
	// ya puede entrar
	numDentro++;
	pthread_mutex_unlock(&mtx);
	printf("Cliente NO VIP %2d entra a la disco\n", id);
}

void enter_vip_client(int id) {
	pthread_mutex_lock(&mtx);
	//Ha llegado un VIP, en principio estaría esperando.
	numVipsFuera++;
	//Comprobamos si VIP puede entrar
	while (numDentro == CAPACITY){
		printf("Cliente VIP %2d se queda a la espera\n", id);
		pthread_cond_wait(&vipsEsperando,&mtx);
	}
	// ya puede entrar
	numDentro++;
	numVipsFuera--;
	pthread_mutex_unlock(&mtx);
	printf("Cliente VIP %2d entra a la disco\n", id);
}

void dance(int id, int isvip) {
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip) {
	pthread_mutex_lock(&mtx);
	--numDentro; // Si sale uno, la discoteca tiene 1 hueco y por tanto aviso al siguiente
	printf("Client %2d (%s) is leaving\n", id, VIPSTR(isvip));
	if (numVipsFuera > 0)
		pthread_cond_signal(&vipsEsperando);
	else
		pthread_cond_signal(&novipsEsperando);
	pthread_mutex_unlock(&mtx);
}

void *client(void *arg)
{
	tclient* client = arg;

    if (client->vip == 1)
        enter_vip_client(client->id);
    else
        enter_normal_client(client->id);
    dance(client->id,client->vip);
    disco_exit(client->id,client->vip);

}

int main(int argc, char *argv[]) {
	//Inicialización de mutex y var condicionales
	pthread_mutex_init(&mtx,NULL);
	pthread_cond_init(&vipsEsperando,NULL);
	pthread_cond_init(&novipsEsperando,NULL);
	//Inicialización de var globales
	numDentro = 0;
	numVipsFuera = 0;

	FILE* file = fopen(argv[1],"r");
	int M; 
	fscanf(file,"%d",&M);
	
	pthread_t* clientes[M];
	
	tclient clientesDatos[M];
	for (int i = 0; i < M; ++i){
		clientesDatos[i].id = i;
		fscanf(file,"%d",&clientesDatos[i].vip);
	}
	//Crear hilos y esperarlos antes de borrar mutex y varconds
	for (int i = 0; i < M; ++i)
		pthread_create(&clientes[i],NULL,client,&clientesDatos[i]);

	for (int i = 0; i < M; ++i)
		pthread_join(clientes[i],NULL);

	pthread_mutex_destroy(&mtx);
	pthread_cond_destroy(&vipsEsperando);
	pthread_cond_destroy(&novipsEsperando);
	return 0;
}
