/*
 * planificador.h
 *
 *  Created on: 06/10/2014
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

	#include <panel.h>
	#include <commons/log.h>

	typedef struct arg_PLANIFICADOR { // Estructura para pasar argumentos al hilo
		uint32_t quantum;
		char* syscalls_path;
		uint32_t puerto;
		t_log* logger;
	} arg_PLANIFICADOR;

	typedef enum {TCBKM, SYSTCALL, JOIN, SEM} t_evento;

	typedef struct data_nodo_block { //Estructura de la cola de BLOCK
		t_hilo * tcb;
		t_evento evento;
		uint32_t tid;
		uint32_t recurso;
	} t_data_nodo_block;

	//TODO: CONSULTAR UBICACIÓN DE ESTAS VARIABLES GLOBALES
	t_list* cola_ready, cola_block; //Colas de planificación.
	t_queue* cola_new;

	void* main_PLANIFICADOR(void* parametros);

#endif /* PLANIFICADOR_H_ */
