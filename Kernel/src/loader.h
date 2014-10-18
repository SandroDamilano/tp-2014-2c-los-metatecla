
#ifndef LOADER_H_
#define LOADER_H_

	#define DELAY_CHECK_NEW_READY_SEC 1
	#define DELAY_CHECK_NEW_READY_USEC 0

    #include <stdlib.h>
	#include <stdio.h>
	#include <errno.h>
	#include <unistd.h>
	#include <string.h>

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <sys/wait.h>
	#include <signal.h>

	#include <commons/collections/queue.h>
	#include <commons/process.h>
	#include <commons/log.h>
	#include <ansisop-panel/panel.h>

	#include <sockets/enviar_recibir_paquete.h>
	#include <estructuras_auxiliares.h>
	#include <serialize/functions.h>
	#include "general_kernel.h"

	typedef struct arg_LOADER {	// Estructura para pasar argumentos al hilo
		uint32_t puerto;
		char* ip_msp;
		uint32_t puerto_msp;
		uint32_t tamanio_stack;
		t_log* logger;
	} arg_LOADER;

	typedef struct ID {	// Estructura que contiene el ID del proceso o hilo
		u_int32_t identificador;
	} t_id;

	typedef struct Mensaje {	// Estructura para la impresion por pantalla
		char* out;
	} mensaje;

	// Vars globales LOADER
//	t_queue* cola_new, cola_ready, cola_exit; Los puse en general_kernel.h
	mensaje msg;
	t_hilo info_mem_MSP;
	t_id pid, tid;

	// Funciones operaciones con cola
	extern int encolar(t_cola cola, t_hilo* tcb);
	extern void check_new_ready();
	extern void check_exit();

	void* main_LOADER(void* parametros);

#endif /* LOADER_H_ */
