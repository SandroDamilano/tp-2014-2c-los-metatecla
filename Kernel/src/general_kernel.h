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

	extern t_queue* cola_new;
	extern t_queue* cola_exit;
	extern sem_t* sem_new;
	extern sem_t* sem_exit;
	extern pthread_mutex_t* mutex_new;
	extern pthread_mutex_t* mutex_exit;
	extern pthread_mutex_t* mutex_TIDs;
	extern pthread_mutex_t* mutex_PIDs;
	extern pthread_mutex_t* mutex_log;
	extern int cantidad_de_PIDs = 0;
	extern int cantidad_de_TIDs = 0;
	extern int sockMSP;

	void inicializar_semaforos();
	void producir_tcb(void (*funcion)(t_hilo*), sem_t* sem, pthread_mutex_t* mutex, t_hilo* tcb);
	void consumir_tcb(void (*funcion)(t_hilo*), sem_t* sem, pthread_mutex_t* mutex, t_hilo* tcb);

#endif /* GENERAL_KERNEL_H_ */
