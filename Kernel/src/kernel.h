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
	#include "planificador.h"
	#include "general_kernel.h"
	#include <ansisop-panel/kernel.h>
	#include <estructuras_auxiliares.h>
	#include <sockets/enviar_recibir_paquete.h>

	int cantidad_de_TIDs;
	int cantidad_de_PIDs;

	// Vars de config
	uint32_t puerto, quantum, tamanio_stack;
	char* ip_msp, *puerto_msp, *syscalls_path;

	char bufferLog[80];
	char* bufferMSP = NULL;

	// Vars para hilos (KERNEL/LOADER/PLANIFICADOR)
	pthread_t thread_KERNEL, thread_LOADER, thread_PLANIFICADOR;
	uint32_t ret_KERNEL, ret_LOADER, ret_PLANIFICADOR;		// return values
	arg_LOADER param_KERNEL, param_LOADER;	// tipo de los argumentos pasados al hilo KERNEL e hilo LOADER
	arg_PLANIFICADOR param_PLANIFICADOR;	// tipo de los argumentos pasados al hilo PLANIFICADOR

	// Funciones ppales
	void leer_config();				// Lee configuracion y asigna a variables.
	void cargar_arg_LOADER();		// Carga estructura para mandar a hilo LOADER
	void cargar_arg_PLANIFICADOR();	// Carga estructura para mandar a hilo PLANIFICADOR
	void conectar_a_MSP(char *ip, char* puerto);

#endif /* KERNEL_H_ */
