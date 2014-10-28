/*
 * general_kernel.h
 *
 *  Created on: 17/10/2014
 *      Author: utnso
 */

#ifndef GENERAL_KERNEL_H_
#define GENERAL_KERNEL_H_

	#include <commons/collections/queue.h>
	#include <pthread.h>
	#include <semaphore.h>
	#include <ansisop-panel/kernel.h>
	#include <stdlib.h>
	#include <sockets/enviar_recibir_paquete.h>

	extern t_queue* cola_new;
	extern t_queue* cola_exit;
	extern sem_t sem_new;
	extern sem_t sem_exit;
	extern pthread_mutex_t mutex_new;
	extern pthread_mutex_t mutex_exit;
	extern pthread_mutex_t mutex_TIDs;
	extern pthread_mutex_t mutex_PIDs;
	extern pthread_mutex_t mutex_log;
	//TODO Inicializar cantidad de PIDs y TIDs
	extern int cantidad_de_PIDs;
	extern int cantidad_de_TIDs;
	extern int sockMSP;

	t_hilo *crear_TCB(int pid, uint32_t dir_codigo, uint32_t dir_stack, int tamanio_codigo);
	void inicializar_colas_new_exit();
	void push_new(t_hilo* tcb); // Se lo podría poner en el Loader directamente
	void inicializar_semaforos();
	void producir_tcb(void (*funcion)(t_hilo*), sem_t* sem, pthread_mutex_t* mutex, t_hilo* tcb);
	void consumir_tcb(void (*funcion)(t_hilo*), sem_t* sem, pthread_mutex_t* mutex, t_hilo* tcb);
	int obtener_pid();
	int obtener_tid();

#endif /* GENERAL_KERNEL_H_ */
