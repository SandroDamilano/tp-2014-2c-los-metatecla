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
#include "funcionesMSP.h"


int main(void) {
	//Variables de Memoria Principal
	void *memoria_ppal = NULL;
	//Variables para hilos
	pthread_t consola;

	//1.Leer archivo de configuracion
	leerConfiguracion();

	//2. Reservar bloque de memoria principal
	memoria_ppal=reservarBloquePpal(tamanio_mem_ppal);
		//log "se creo memoria princial y abaca desde memoria_ppal hasta memoria_ppal+tamanio
	//3. Generar estructuras administrativas
	//4. Abrir conexiones con Kernel y CPU, y levantar Consola MSP
		//Se crea el hilo para la consola
	pthread_create(&consola, NULL, inciarConsola, NULL);
	pthread_join(consola, NULL);
	return EXIT_SUCCESS;
}
