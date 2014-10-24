/*
 * planificador.h
 *
 *  Created on: 06/10/2014
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

	#include <ansisop-panel/panel.h>
	#include <commons/log.h>
	#include "general_kernel.h"
	#include "servicios_cpu.h"

	int sockMSP;
	int cantidad_de_PIDs;
	int cantidad_de_TIDs;

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
		uint32_t parametro;
	} t_data_nodo_block;

	//Por ahora estas van aca porque el planificador es el único que las usa
	t_list* cola_ready, cola_block; //Colas de planificación.

	pthread_mutex_t* mutex_ready;

	//Estas 3 variables globales se usan para desbloquear los procesos
	// (participan en la búsqueda del TCB que hay que desbloquear)
	uint32_t tid_a_buscar; //Para encontrar al TCB que hizo la systcall
	uint32_t parametro_a_buscar; //Puede ser un tid o un recurso (semáforo)
	t_evento evento_a_buscar;

	void* main_PLANIFICADOR(void* parametros);

#endif /* PLANIFICADOR_H_ */
