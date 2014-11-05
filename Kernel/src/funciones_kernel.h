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
	#include <ansisop-panel/kernel.h>
	#include <stdlib.h>
	#include <sockets/enviar_recibir_paquete.h>

	extern t_queue* cola_new;
	extern t_queue* cola_exit;
	extern t_list* consolas; //Lista con info de las consolas activas y sus procesos
	extern sem_t sem_new;
	extern sem_t sem_exit;
	extern pthread_mutex_t mutex_consolas;
	extern pthread_mutex_t mutex_new;
	extern pthread_mutex_t mutex_exit;
	extern pthread_mutex_t mutex_TIDs;
	extern pthread_mutex_t mutex_PIDs;
	extern pthread_mutex_t mutex_log;
	//TODO Inicializar cantidad de PIDs y TIDs
	extern int cantidad_de_PIDs;
	extern int cantidad_de_TIDs;

	//Socket de la MSP
	extern int sockfd_cte;

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

	t_hilo *crear_TCB(uint32_t pid, uint32_t dir_codigo, uint32_t dir_stack, uint32_t tamanio_codigo);
	void inicializar_colas_new_exit();
	void sacar_de_exit(t_data_nodo_exit* tcb);
	void poner_en_new(t_hilo* tcb);// Se lo podría poner en el Loader directamente
//	void push_new(t_hilo* tcb); // Esta no es la que hay que usar
	void inicializar_semaforos();
	void producir_tcb(void (*funcion)(t_hilo*), sem_t* sem, pthread_mutex_t* mutex, t_hilo* tcb);
	void consumir_tcb(void (*funcion)(t_hilo*), sem_t* sem, pthread_mutex_t* mutex, t_hilo* tcb);
	int obtener_pid();
	int obtener_tid();

#endif /* FUNCIONES_KERNEL_H_ */
