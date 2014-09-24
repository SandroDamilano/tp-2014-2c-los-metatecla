/*
 ============================================================================
 Name        : MSP.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "MSP.h"
#include "ConsolaMSP.h"
#include "funcionesMSP.h"


int main(void) {
	//Variables de Memoria Principal
	void *memoria_ppal = NULL;

	//Variables de estructuras administrativas
	t_lista_procesos *listaProcesos = NULL;

	//Variables para hilos
	pthread_t consola;

	//1.Leer archivo de configuracion
	leerConfiguracion();
	//2. Reservar bloque de memoria principal y dividir en marcos
	memoria_ppal=reservarBloquePpal(tamanio_mem_ppal);
		//log "se creo memoria princial y abaca desde memoria_ppal hasta memoria_ppal+tamanio

	//3. Generar estructuras administrativas
	listaProcesos = list_create();
		//lista de procesos, tabla de paginas y tabla de segmentos
	//4. Abrir conexiones con Kernel y CPU, y levantar Consola MSP
		//Se crea el hilo para la consola
	pthread_create(&consola, NULL, inciarConsola, NULL);
	pthread_join(consola, NULL);
	return EXIT_SUCCESS;
}
