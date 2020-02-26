/*
Aplicação da Simulação de Monte Carlo - Modelo de Black Scholes
Compilação em Linux: gcc BlackScholes.c -o BlackScholes -lm 
Execução do programa: ./BlackScholes 
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include<time.h>

double S, E, r, sigma, T;
int M;
long long int NUM_PONTOS;
long int numThreads;

float pi_final=0.0;
pthread_mutex_t mutexSum; //variavel mutexsum do tipo pthread_mutex
int indice;
long long total_all=0;


struct EstruturaAleatorio{
    double x1, x2, w, y1, y2;
    int useLast;
    struct drand48_data random;
};

/*Inicialização do gerador de números aleatórios*/

void initGeradorAleatorio(struct EstruturaAleatorio* state){
    state->random.__init = 0;
    state->useLast = 0;
        
    struct timeval now;
    state->random.__x[0] = now.tv_usec;
}

/*Geração de números aleatórios*/

double RandomNumber(struct EstruturaAleatorio* state){
    if (state->useLast){
        state->y1 = state->y2;
        state->useLast = 0;
    }
  else{
        do{
            drand48_r(&state->random, &state->x1);
            state->x1 = 2.0 * state->x1 - 1.0;
            drand48_r(&state->random, &state->x2);
            state->x2 = 2.0 * state->x2 - 1.0;
            state->w = state->x1 * state->x1 + state->x2 * state->x2;
        }
        while (state->w >= 1.0);
            state->w = sqrt((-2.0 * log(state->w)) / state->w);
            state->y1 = state->x1 * state->w;
            state->y2 = state->x2 * state->w;
            state->useLast = 1;
    }
    return state->y1;
}

/*Calcula: Desvio Padrão*/

double stdDev(double trials[], double mean, int M){
    double stddev = 0.0;
    int i;
    for(i = 0; i < M; i++){
        stddev += pow((trials[i] - mean),2);
    }
    stddev = sqrt(stddev/((double)M-1));
    return stddev;
}


void *blackScholes(void *threadid){
    int i;
    double t, mean = 0.0, stddev, confwidth, confmin, confmax;
    double *trials;
	long long int chunk;
	double pi = 0;
	double j = 0;
	
	chunk = NUM_PONTOS/numThreads; //divide os pedacos de execucao entre as threads disponiveis

    trials = (double *) malloc(M*sizeof(double));

    struct EstruturaAleatorio state;
    initGeradorAleatorio(&state);

    /*Calcula: Trials*/
    for(j=0;j<chunk;j++){
        for(i = 0; i < M; i++){
            t = S*exp((r-((1.0/2.0)*pow(sigma, 2.0)))*T + sigma*sqrt(T)*RandomNumber(&state));

            if((t-E) > 0.0)
                trials[i] = exp((-r)*T)*(t-E);
            else
                trials[i] = 0.0;
        mean += trials[i];
        }

    }
    
    //para tratar a sincronizacao utilizamos o bloqueio mutex
    
	pthread_mutex_lock(&mutexSum);   //bloqueia um objeto mutex
    /*Calcula: Média, Desvio Padrão e o Intervalo de Confiança*/

    mean = mean/(double)M;
    stddev = stdDev(trials, mean, M);
    confwidth = 1.96*stddev/sqrt(M);
    confmin = mean - confwidth;
    confmax = mean + confwidth;
        
	pthread_mutex_unlock(&mutexSum); //libera um objeto mutex
    
    printf("S = %lf\n", S);
    printf("E = %lf\n", E);
    printf("r = %lf\n", r);
    printf("sigma = %lf\n", sigma);
    printf("T = %lf\n", T);
    printf("M = %d\n", M);
    printf("Media = %lf\n", mean);
    printf("Desvio Padrao = %lf\n", stddev);
    printf("Tempo Execucao = %lf\n", t);
    printf("Intervalo de Confianca = (%lf, %lf)\n", confmin, confmax);
    
    return 0;

}

int main(int argc, char **argv){

    double resp;
    FILE *saida;
    saida = fopen("tempo.txt", "a");
    
    if (argc < 8) {
	    printf ("Errado! Utilize: programa <S> <E> <r> <sigma> <T> <M> <threads> <iteracoes>\n\n \tE.g. -> ./programa 100 100 0.05 0.2 1 100 2 1000\n\n");
		exit(1);
	}


    /* TRATANDO AS ENTRADAS */
    
	NUM_PONTOS = strtoul(argv[2], NULL, 10); //n de iteracoes
	long int i;
	numThreads = atol(argv[1]);
	pthread_t threads[numThreads]; //vetor de threads do tipo pthread
	
    S = atof(argv[1]); //valor da acao, preco do ativo
    E = atof(argv[2]); //preco de exercicio da opcao
    r = atof(argv[3]); //taxa de juros livre de risco
    sigma = atof(argv[4]); //volatilidade
    T = atof(argv[5]); //tempo de validade da opcao
    M = atoi(argv[6]); //numero de iteracoes
    
    //Coleta de tempo
    clock_t start_t, end_t;
    double total_t;
    start_t = clock(); //Coleta Inicio

    /* CRIA A THREAD */
	for(i=1;i<=numThreads;i++){
		pthread_create(&threads[i], NULL, blackScholes, (void *)i); 
	}
	
    
    end_t = clock(); //Coleta Final
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    fprintf(saida, "%f\n", total_t);
    return 0;
}
