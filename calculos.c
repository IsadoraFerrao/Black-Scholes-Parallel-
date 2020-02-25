#include<stdio.h> //printf
#include<stdlib.h> //alocacao
#include<sys/time.h> //captura tempo
#include<math.h>
//#define PRINT

double *aloca(double *vetor, int n, FILE *entrada){
	int i=0;
	vetor = (double*)malloc(n*sizeof(double));
	while(fscanf(entrada, "%lf ", &vetor[i])!=EOF){
		i++;
	}
	return vetor;
}

double func_media(double *vetor, int n){
	int i;
	double media = 0;
	for(i=0; i<n; i++){
		media += vetor[i];
	}
	media = media/n;
	return media;
}

double func_desv_padrao(double *vetor, int n, double media){
	int i;
	double soma = 0, variancia = 0, desvio = 0;
	for(i=0; i<n; i++){
		soma += pow(vetor[i]-media,2);
	}
	variancia = soma/(n-1);
	desvio = sqrt(variancia);
	return desvio;
}

double func_desv_absoluto(double media, double desvio_padrao){
	double desv_abs;
	desv_abs = (desvio_padrao/media)*100;
	return desv_abs;
}

//gcc -o programa Calculo.c -lm
//./programa 30 arquivo.txt 

int main(int argc, char **argv){
	double *vetor, media, desvio_padrao, desvio_absoluto;
	int i, n;
	FILE *entrada, *saida;
	n = atoi(argv[1]);
	entrada = fopen(argv[2], "r");
	saida = fopen("Calculo.txt", "a");

	//aloca e inicializa
	vetor = aloca(vetor, n, entrada);

		#ifdef PRINT
			//imprime o vetor inicializado
			printf("VETOR INICIALIZADO:\n");
			for(i=0; i<n; i++){
				printf("%lf\n", vetor[i]);
			}
			printf("\n");
		#endif
	fprintf(saida, "%s\t\t", argv[2]);
	media = func_media(vetor, n);
	fprintf(saida, "Media = %lf\t", media);
	desvio_padrao = func_desv_padrao(vetor, n, media);
	fprintf(saida, "Desvio Padrao = %lf\t", desvio_padrao);
	desvio_absoluto = func_desv_absoluto(media, desvio_padrao);
	fprintf(saida, "Desvio Absoluto = %lf\n", desvio_absoluto);
	
	return 0;
	fclose(entrada);
	fclose(saida);
	free(vetor);
}


