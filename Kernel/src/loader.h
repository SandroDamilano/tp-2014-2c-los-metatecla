
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
	#include <commons/log.h>

	#include <estructuras_auxiliares.h>
	#include <sockets/enviar_recibir_paquete.h>
	#include <panel/panel.h>

	typedef struct arg_LOADER { // Estructura para pasar argumentos al hilo
		uint32_t puerto;
		char* ip_msp;
		uint32_t puerto_msp;
		uint32_t tamanio_stack;
		t_log* logger;
	} arg_LOADER;

	typedef struct Mensaje {
		char* out;
	} mensaje;

	// Vars globales LOADER
	t_queue* cola_new, cola_ready, cola_exit;
	mensaje msg;

		// Funciones operaciones con cola
	extern int encolar(t_cola cola, t_hilo* tcb);
	extern void check_new_ready();
	extern void check_exit();


#endif /* LOADER_H_ */
