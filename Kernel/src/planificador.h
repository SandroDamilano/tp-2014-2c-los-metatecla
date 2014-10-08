/*
 * planificador.h
 *
 *  Created on: 06/10/2014
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

	typedef struct arg_PLANIFICADOR { // Estructura para pasar argumentos al hilo
		uint32_t quantum;
		char* syscalls_path;
		uint32_t puerto;
		t_log* logger;
	} arg_PLANIFICADOR;

	void* main_PLANIFICADOR(void* parametros)

#endif /* PLANIFICADOR_H_ */
