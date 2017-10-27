
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>


int crear_matriz(FILE *archivo, int **matriz);
void imprimir_matriz(int *matriz, int numero);
int productoPuntoS(int *matriz1, int *matriz2, int numero);
void* productoPuntoH(void* p);
int productoPunto_hilos(int *matriz1, int *matriz2, int numero, int hilos);
void* prueba(void* p);

struct param_hilo{
		int *matriz1;
		int *matriz2;
		int base;
		int numero;
		int resultado;
};

int main(int argc, char *argv[]){

	if(argc < 2){
			printf("No tiene suficientes parámetros");
			return(1);
	}

	FILE *archivo1 =fopen(argv[1], "r");
	FILE *archivo2 = fopen( argv[2], "r");
	if (archivo1  == NULL){
		printf("Nombre de archivo 1 incorrecto\n");
		return(1);
	}
	if (archivo2  == NULL ){
		printf("Nombre de archiv 2 incorrecto\n");
		return(1);
	}
	printf(" -- Archivos aceptados --\n\n");
	int *matriz1; //matriz 1
	int *matriz2; //matriz 2
	int numero1 = crear_matriz(archivo1, &matriz1);
	int numero2 = crear_matriz(archivo2, &matriz2);

	if(numero1 != numero2){
		printf("ERROR: las matricesd deben ser del mismo tamaño\n");
		return(1);
	}
	printf(" -- Matrices creadas --\n\n");
	//imprimir_matriz(matriz1, numero1);
	//printf("--------------------------------------------\n");
	//imprimir_matriz(matriz2, numero1);
	int nhilos= atoi(argv[3]);

	if (nhilos > 16){
		printf("ERROR: Numero de hilos mayor al permitido\n");
		return(1);
	}
	int a = productoPunto_hilos(matriz1, matriz2, numero1, nhilos);
	printf(" resultado final:%d\n", a );


	free(matriz1);
	fclose(archivo1);
	free(matriz2);
	fclose(archivo2);
	return 0;
}


void imprimir_matriz(int *matriz, int numero){
	int i;
	for(i = 0; i<numero; i++){
		printf("%d \n", matriz[i]);
	}

}

int crear_matriz(FILE *archivo, int **matriz){
	int numero = 0;
	*matriz = NULL;
	int linea;
	while( fscanf(archivo, "%d", &linea) == 1){
		numero++;
		//printf("%d \n", linea);
		*matriz = (int*) realloc(*matriz, sizeof(int)*numero);
		(*matriz)[numero -1]= linea;
	}
	if(feof(archivo)){
			rewind(archivo);
			return numero;
	}
	rewind(archivo);
	return 0;
}

int productoPuntoS(int *matriz1, int *matriz2, int numero){
	int res = 0;
	int i;
	for(i = 0; i<numero ; i++){
		res = res + (matriz1[i]*matriz2[i]);
	}
	return res;
}

void* productoPuntoH(void* p){

	struct param_hilo* parametros = (struct param_hilo*) p;
	parametros->resultado  = 0;
	int i = parametros->base;
	while(i<parametros->numero){
		parametros->resultado += (parametros->matriz1[i]*parametros->matriz2[i]);
		i++;
	}

	return NULL;
}


//funcion para la creacion de hilos
int productoPunto_hilos(int *matriz1, int *matriz2, int numero, int hilos){
	int res = 0;
	pthread_t id_hilo[16];

	struct param_hilo p[16];

	//iterador creacion de parametros
	int i ;
	for(i = 0; i <hilos; i++){
		//base para empezar a multiplicar
		p[i].base =  ( i*(numero/hilos) );
		//limite
		p[i].numero = ( i*(numero/hilos) ) + ((numero/hilos));
		p[i].matriz1 = matriz1;
		p[i].matriz2 = matriz2;
		//printf("Hilo %d -> base: %d - limite: %d\n", i, p[i].base, p[i].numero );
	}

	p[hilos-1].numero = numero;
	struct timeval inicial, final;
	gettimeofday(&inicial, NULL);
	for(i = 0; i<hilos; i++) pthread_create(&id_hilo[i], NULL, &productoPuntoH, &p[i]);
	for(i = 0; i<hilos; i++) {
		pthread_join(id_hilo[i], NULL);
		res = res + p[i].resultado;
	}
	gettimeofday( &final , NULL);
	double t_tiempo;

	t_tiempo = (final.tv_sec - inicial.tv_sec)*1000.0 + (final.tv_usec - inicial.tv_usec)/ 1000.0;
	printf("- Tiempo total:%g", t_tiempo);


	return res;
}
