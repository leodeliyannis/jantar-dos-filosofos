/*
 * Leonardo Deliyannis Constantin
 * jantar.c - uma implementação concorrente do Jantar dos Filósofos
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#define MAX 20

int N, C, D, tmin, tmax;

pthread_mutex_t hashi[MAX];
pthread_t filosofo[MAX];
int id[MAX], refeicoes[MAX];
enum _status { PENSANDO, FAMINTO, COMENDO } status[MAX];

void pegaHashi(int f){
    // bloqueia o próprio hashi
    pthread_mutex_lock(&hashi[f]);
    printf("Filosofo %d esta com fome e pegou o 1o garfo\n", f);
    // bloqueia o hashi do vizinho
    pthread_mutex_lock(&hashi[(f+1)%N]);
    printf("Filosofo %d continua com fome e pegou o 2o garfo\n", f);
}

void soltaHashi(int f){
    // desbloqueia o próprio hashi
    pthread_mutex_unlock(&hashi[f]);
    // desbloqueia o hashi do vizinho
    pthread_mutex_unlock(&hashi[(f+1)%N]);
    printf("Filosofo %d acabou de comer pela %da vez e liberou os garfos\n",
        f, ++refeicoes[f]);
}

void realizaAtividade(int f, const char *s){
    int tempo = (rand() % (1 + tmax - tmin)) + tmin;
    printf("Filosofo %d %s por %.3lfs...\n", f, s, (double) tempo / 1000.0);
    usleep(tempo);
}

void *vidaDeUmFilosofo(void *arg){
    int f = *(int*) arg;
    while(refeicoes[f] < C){
        status[f] = PENSANDO;
        realizaAtividade(f, "pensando");
        status[f] = FAMINTO;
        pegaHashi(f);
        status[f] = COMENDO;
        realizaAtividade(f, "comendo");
        soltaHashi(f);
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv){
    int i;
    if(argc == 1) scanf("%d %d %d %d %d", &N, &C, &D, &tmin, &tmax);
    else if(argc == 6){
        sscanf(argv[1], "%d", &N);
        sscanf(argv[2], "%d", &C);
        sscanf(argv[3], "%d", &D);
        sscanf(argv[4], "%d", &tmin);
        sscanf(argv[5], "%d", &tmax);
    }
    else{
        fprintf(stderr, "uso: %s N C D Min Max", argv[0]);
        fprintf(stderr, "onde:"
                        "    N = quantidade de filosofos"
                        "    C = maximo de refeicoes"
                        "    D = 0 se permite deadlock; 1 caso contrario"
                        "    Min = tempo minimo para dormir ou comer"
                        "    Max = tempo maximo para dormir ou comer"
        );
        return 1;
    }
    srand(time(NULL));
    memset(refeicoes, 0, sizeof(refeicoes));
    for(i = 0; i < N; i++){
        pthread_mutex_init(&hashi[i], NULL);
    }
    for(i = 0; i < N; i++){
        id[i] = i;
        pthread_create(&filosofo[i], NULL, &vidaDeUmFilosofo, (void*) &id[i]);
    }
    while(1);
    return 0;
}
