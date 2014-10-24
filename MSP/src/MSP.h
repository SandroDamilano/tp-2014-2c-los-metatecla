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
#include "ConsolaMSP.h"
#include "funcionesMSP.h"

//Path para archivos swap
extern char* path_swap;

//Variables de Archivos
extern t_log *logMSP;
t_config *archConfigMSP = NULL;

//Variables de Memoria Principal
void *memoria_ppal = NULL;
int memoriaSwapActual = 0;
int memoriaPpalActual = 0;

//Variables de estructuras administrativas
t_list *listaProcesos = NULL;
t_list *lista_marcos = NULL;

//Variables para hilos
pthread_t consola;

//Semaforos
extern pthread_mutex_t mutex_consola;
extern pthread_mutex_t mutex_log;

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


uint32_t crearSegmento(uint32_t PID, int tamanio_segmento); // crea un nuevo segmento para PID del tamanio pedido y devuelta la direcion de memoria base del segmento
void destruirSegmento(uint32_t PID, uint32_t base_segmento); //destruye el segmento correspodiente a su base del proceso PID
void solicitarMemoria(uint32_t PID, uint32_t direccion_log, int tamanio_mem); //muestra desde direcc logica hasta tamanio_mem lo que se encuentra escrito
//TODO no estamos seguros de que devuelta esta funcion. Consultar
void escribirMemoria(int PID, uint32_t direcc_log, int bytes_escribir, int tamanio); // para el espacio de direcc de PID escribe hasta tamaño los bytes
uint32_t crearDireccion(uint32_t segmento,uint32_t pagina);

int inicializar_semaforos();
#endif /* MSP_H_ */
