/*
 * MSP.h
 *
 *  Created on: 24/09/2014
 *      Author: utnso
 */

#ifndef MSP_H_
#define MSP_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "funcionesMSP.h"

//Path para archivos swap
extern char* path_swap;

//Variables de Memoria Principal
void *memoria_ppal = NULL;
uint32_t paginasMemoriaSwapActual = 0;
uint32_t paginasMemoriaPpalActual = 0;
uint32_t cant_marcos=0;
uint32_t punteroClock =0;

//Variables de estructuras administrativas
t_list *listaProcesos = NULL;
t_marco *tabla_marcos = NULL;

//Variables para hilos
pthread_t consola;

//Variables para Sockets
int socketServidorMSP;
bool escuchandoConexiones = true;



	//Estructuras sujetas a posibles cambio y arreglos mas adelante
	int inicializar_semaforos();

#endif /* MSP_H_ */
