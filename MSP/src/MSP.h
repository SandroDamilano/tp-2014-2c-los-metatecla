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
#include "commons/config.h"
#include <stdbool.h>
#include <stdint.h>
#include <sockets/enviar_recibir_paquete.h>

typedef struct  lista_paginas{
	int numeroPagina;
	void *direccion_memoria_ppal;
	bool swap;
	}t_lista_paginas;

typedef struct  lista_segmentos{
	int numeroSegmento;
	t_lista_paginas *lista_Paginas;

}t_lista_segmentos;

typedef struct lista_procesos{
	int pid;
	t_lista_segmentos *lista_Segmentos;
} t_lista_procesos;





	//Estructuras sujetas a posibles cambio y arreglos mas adelante


uint32_t crearSegmento(int PID, int tamanio_segmento); // crea un nuevo segmento para PID del tamanio pedido y devuelta la direcion de memoria base del segmento
void destruirSegmento(int PID, uint32_t base_segmento); //destruye el segmento correspodiente a su base del proceso PID
void solicitarMemoria(int PID, uint32_t direccion_log, int tamanio_mem); //muestra desde direcc logica hasta tamanio_mem lo que se encuentra escrito
//no estamos seguros de que devuelta esta funcion. Consultar
void escribirMemoria(int PID, uint32_t direcc_log, int bytes_escribir, int tamanio); // para el espacio de direcc de PID escribe hasta tamaño los bytes


#endif /* MSP_H_ */
