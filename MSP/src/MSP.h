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
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include "sockets/enviar_recibir_paquete.h"
#include "ConsolaMSP.h"
#include "funcionesMSP.h"

extern pthread_mutex_t mutex_consola;
extern pthread_mutex_t mutex_log;

typedef struct  lista_paginas{
	int numeroPagina;
	int marcoEnMemPpal;
	int swap;
	}t_lista_paginas;

typedef struct  lista_segmentos{
	int numeroSegmento;
	int tamanio;
	t_list *lista_Paginas;

}t_lista_segmentos;

typedef struct lista_procesos{
	int pid;
	t_list *lista_Segmentos;
} t_lista_procesos;





	//Estructuras sujetas a posibles cambio y arreglos mas adelante


uint32_t crearSegmento(int PID, int tamanio_segmento); // crea un nuevo segmento para PID del tamanio pedido y devuelta la direcion de memoria base del segmento
void destruirSegmento(int PID, uint32_t base_segmento); //destruye el segmento correspodiente a su base del proceso PID
void solicitarMemoria(int PID, uint32_t direccion_log, int tamanio_mem); //muestra desde direcc logica hasta tamanio_mem lo que se encuentra escrito
//TODO no estamos seguros de que devuelta esta funcion. Consultar
void escribirMemoria(int PID, uint32_t direcc_log, int bytes_escribir, int tamanio); // para el espacio de direcc de PID escribe hasta tamaño los bytes

int inicializar_semaforos();
#endif /* MSP_H_ */
