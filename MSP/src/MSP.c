/*
 ============================================================================
 Name        : MSP.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "ConsolaMSP.h"
#include <pthread.h>


int main(void) {
	pthread_t consola;

	pthread_create(&consola, NULL, inciarConsola, NULL);
	printf("Probando si no se bloquea");
	pthread_join(consola, NULL);
	return EXIT_SUCCESS;
}

