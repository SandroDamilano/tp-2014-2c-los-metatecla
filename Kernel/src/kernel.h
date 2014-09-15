/*
 * kernel.h
 *
 *  Created on: 12/09/2014
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#define MAX_COUNT_OF_CONFIG_KEYS 6

	#include <stdio.h>
	#include <stdlib.h>
//	#include <stdint.h>
	#include <string.h>

	#include <stdlib.h>
	#include <sys/stat.h>
	#include <stdlib.h>
	#include <string.h>
	#include <pthread.h>
	#include <semaphore.h>

	#include "loader.h"
	#include <panel/kernel.h>
	#include <panel/panel.h>
	#include <estructuras_auxiliares.h>

	uint32_t puerto, puerto_msp, quantum, tamanio_stack;
	char* ip_msp;
	char* syscalls_path;

	char bufferLog[80];

	// Variables para hilos
	pthread_t thread_KERNEL, thread_LOADER, thread_PLANIFICADOR;
	uint32_t ret_KERNEL, ret_LOADER, ret_PLANIFICADOR;			// return values
	arg_LOADER param_KERNEL, param_LOADER, param_PLANIFICADOR;	// tipo de los argumentos pasados al hilo

	// Funciones ppales
	void leer_config();		// Lee configuracion y asigna a variables.
	void cargar_arg_LOADER();	// Carga estructura para mandar a hilo LOADER
	void cargar_arg_PLANIFICADOR();	// Carga estructura para mandar a hilo PLANIFICADOR

#endif /* KERNEL_H_ */
