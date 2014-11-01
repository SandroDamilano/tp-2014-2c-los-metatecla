
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
	#include "funciones_kernel.h"
	#include "loader_estructuras.h"

	typedef struct arg_LOADER {	// Estructura para pasar argumentos al hilo
		char* puerto_kernel;
		uint32_t tamanio_stack;
		char* ip_msp;
		char* puerto_msp;
		t_log* logger;
	} arg_LOADER;

	// Vars globales LOADER
	t_struct_string msg;

	// Funciones operaciones con cola
	extern int encolar(t_cola cola, t_hilo* tcb);
	extern void check_new_ready();
	extern void check_exit();

	void* main_LOADER(void* parametros);
	void *get_in_addr(struct sockaddr *sa);

#endif /* LOADER_H_ */
