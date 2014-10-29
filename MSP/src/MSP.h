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
#include "sockets/enviar_recibir_paquete.h"
#include "funcionesMSP.h"

//Path para archivos swap
extern char* path_swap;

//Variables de Memoria Principal
void *memoria_ppal = NULL;
int memoriaSwapActual = 0;
int memoriaPpalActual = 0;

//Variables de estructuras administrativas
t_list *listaProcesos = NULL;
t_list *lista_marcos = NULL;

//Variables para hilos
pthread_t consola;

//Variables para Sockets
int socketServidorMSP;
bool escuchandoConexiones = true;

typedef struct  lista_paginas{
	uint32_t numeroPagina;
	uint32_t marcoEnMemPpal;
	int swap;
	}t_lista_paginas;

typedef struct  lista_segmentos{
	uint32_t numeroSegmento;
	uint32_t tamanio;
	t_list *lista_Paginas;

}t_lista_segmentos;

typedef struct lista_procesos{
	uint32_t pid;
	t_list *lista_Segmentos;
} t_lista_procesos;

	//Estructuras sujetas a posibles cambio y arreglos mas adelante
	int inicializar_semaforos();

#endif /* MSP_H_ */
