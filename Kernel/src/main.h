/*
 * main.h
 *
 *  Created on: 12/09/2014
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#define MAX_COUNT_OF_CONFIG_KEYS 7

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
	#include "funciones_kernel.h"
	#include <panel/kernel.h>
	#include <estructuras_auxiliares.h>
	#include <sockets/enviar_recibir_paquete.h>

	int cantidad_de_TIDs;
	int cantidad_de_PIDs;

	uint32_t direccion_codigo_syscalls;
	uint32_t direccion_stack_syscalls;

	// Vars de config
	uint32_t quantum = 0;
	extern uint32_t tamanio_stack = 0;
	char *ip_msp, *ip_kernel, *syscalls_path;
	uint32_t puerto_kernel, puerto_msp;

	char bufferLog[80];
	char* bufferMSP = NULL;

	// Vars para hilos (KERNEL/LOADER/PLANIFICADOR)
	pthread_t thread_KERNEL, thread_LOADER, thread_PLANIFICADOR;
	uint32_t ret_KERNEL, ret_LOADER, ret_PLANIFICADOR;		// return values
	arg_LOADER param_KERNEL, param_LOADER;	// tipo de los argumentos pasados al hilo KERNEL e hilo LOADER
	arg_PLANIFICADOR param_PLANIFICADOR;	// tipo de los argumentos pasados al hilo PLANIFICADOR

	fd_set master_consolas;
	int consolas_fdmax;
	pthread_mutex_t mutex_master_consolas;

	fd_set master_cpus;
	int cpus_fdmax;
	pthread_mutex_t mutex_master_cpus;
	// Funciones ppales
	void leer_config();				// Lee configuracion y asigna a variables.
	void cargar_arg_LOADER();		// Carga estructura para mandar a hilo LOADER
	void cargar_arg_PLANIFICADOR();	// Carga estructura para mandar a hilo PLANIFICADOR
	void conectar_a_MSP(char *ip, uint32_t puerto);
	void handshake_thread();
	void inicializar_multiplex();
	void handler_nuevas_conexiones(int socket_escucha, int* maxfd);
	fd_set combinar_master_fd(fd_set* master1, fd_set* master2, int maxfd);
	void llegoSenialParaTerminar(int n);

#endif /* MAIN_H_ */
