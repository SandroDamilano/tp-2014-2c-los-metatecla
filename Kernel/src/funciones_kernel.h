/*
 * funciones_kernel.h
 *
 *  Created on: 17/10/2014
 *      Author: utnso
 */

#ifndef FUNCIONES_KERNEL_H_
#define FUNCIONES_KERNEL_H_

	#include <commons/collections/queue.h>
	#include <pthread.h>
	#include <semaphore.h>
	#include <panel/kernel.h>
	#include <stdlib.h>
	#include <signal.h>
	#include <sockets/enviar_recibir_paquete.h>

	extern t_queue* cola_new;
	extern t_queue* cola_exit;
	extern t_list* cola_ready;
	extern t_list* cola_block;
	extern t_list* consolas; //Lista con info de las consolas activas y sus procesos
	extern t_list* cola_exec;
	extern sem_t sem_new;
	extern sem_t sem_exit;
	extern sem_t sem_ready;
	extern sem_t sem_abort;
	extern pthread_mutex_t mutex_consolas;
	extern pthread_mutex_t mutex_new;
	extern pthread_mutex_t mutex_exit;
	extern pthread_mutex_t mutex_exec;
	extern pthread_mutex_t mutex_ready;
	extern pthread_mutex_t mutex_block;
	extern pthread_mutex_t mutex_TIDs;
	extern pthread_mutex_t mutex_PIDs;

	extern pthread_mutex_t mutex_log;
	extern pthread_mutex_t mutex_cpus_conectadas;
	extern pthread_mutex_t mutex_consolas_conectadas;

	extern t_list* lista_abortar;
	extern pthread_mutex_t mutex_abortar;
	extern bool espera_por_inn;

	//TODO Inicializar cantidad de PIDs y TIDs
	extern int cantidad_de_PIDs;
	extern int cantidad_de_TIDs;

	//Socket de la MSP
	extern int socket_MSP;

	// Variables de uso para los multiplexores del loader y el planificador
	extern fd_set master_consolas;
	extern int consolas_fdmax;
	extern pthread_mutex_t mutex_master_consolas;

	extern fd_set master_cpus;
	extern int cpus_fdmax;
	extern pthread_mutex_t mutex_master_cpus;

	typedef struct data_nodo_consolas {
		int socket;
		uint32_t pid;
		uint32_t tid;
	} t_data_nodo_consolas;

	typedef enum {TERMINAR, ABORTAR} t_fin;

	typedef struct data_nodo_exit {
		t_hilo * tcb;
		t_fin fin;
	} t_data_nodo_exit;

	typedef struct data_nodo_exec {
		t_hilo * tcb;
		int sock;
	} t_data_nodo_exec;

	t_hilo *crear_TCB(uint32_t pid, uint32_t dir_codigo, uint32_t dir_stack, uint32_t tamanio_codigo);
	void inicializar_colas_new_exit();
	//void sacar_de_exit(t_data_nodo_exit* tcb);
	t_data_nodo_exit* sacar_de_exit();
	void poner_en_new(t_hilo* tcb);// Se lo podría poner en el Loader directamente
//	void push_new(t_hilo* tcb); // Esta no es la que hay que usar
	void inicializar_semaforos();
	void producir_tcb(void (*funcion)(t_hilo*), sem_t* sem, pthread_mutex_t* mutex, t_hilo* tcb);
	void consumir_tcb(void (*funcion)(t_hilo*), sem_t* sem, pthread_mutex_t* mutex, t_hilo* tcb);
	int obtener_pid();
	int obtener_tid();
	void imprimir_texto(int socket_consola, char* mensaje);
	bool hay_que_abortar_pid(uint32_t pid);
	void agregar_a_abortar(uint32_t pid);

#endif /* FUNCIONES_KERNEL_H_ */
