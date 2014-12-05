
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
	#include <panel/panel.h>

	#include <sockets/enviar_recibir_paquete.h>
	#include <estructuras_auxiliares.h>
	#include <serialize/functions.h>
	#include "funciones_kernel.h"
	#include "loader_estructuras.h"
	#include "planificador.h"

	typedef struct arg_LOADER {	// Estructura para pasar argumentos al hilo
		uint32_t puerto_kernel;
		uint32_t tamanio_stack;
		char* ip_msp;
		uint32_t puerto_msp;
		t_log* logger;
	} arg_LOADER;

	/*
	// Vars globales LOADER
	t_struct_string msg;

	// Funciones operaciones con cola
	extern int encolar(t_cola cola, t_hilo* tcb);
	extern void check_new_ready();
	extern void check_exit();
	*/

	t_queue* cola_new;
	t_list* consolas;
	pthread_mutex_t mutex_consolas;
	pthread_mutex_t mutex_new;
	pthread_mutex_t mutex_TIDs;
	pthread_mutex_t mutex_PIDs;
	pthread_mutex_t mutex_log;
	int cantidad_de_PIDs;
	int cantidad_de_TIDs;
	sem_t sem_abort;

	fd_set master_consolas;
	int consolas_fdmax;
	pthread_mutex_t mutex_master_consolas;

	int socket_MSP;

	int sock_a_eliminar;

	bool espera_por_inn;

	void handler_consola(int sock_consola);
	int crear_nuevo_tcb(char* codigo, int tamanio, int sock_consola);
	void destruir_seg_codigo(uint32_t pid, uint32_t dir_codigo);
	void agregar_consola(uint32_t pid, uint32_t tid, int sock);
	void eliminar_consola(int sock);
	bool es_la_consola(t_data_nodo_consolas* data);
	int obtener_cpu_ejecutando_la_consola(int sock_consola);
	bool tiene_al_tcbKernel(t_data_nodo_exec* data);
	uint32_t obtener_pid_consola(int sock);

	t_data_nodo_consolas* sacar_data_consola(int sock);
	t_data_nodo_consolas* obtener_data_consola(int sock);
	void eliminar_proceso(uint32_t pid);
	bool la_cpu_esta_ejecutando_el_pid(int sockCPU, uint32_t pid);

#endif /* LOADER_H_ */
