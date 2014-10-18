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

	extern t_queue* cola_new;
	extern t_queue* cola_exit;
	extern sem_t* sem_new;
	extern sem_t* sem_exit;
	extern pthread_mutex_t* mutex_new;
	extern pthread_mutex_t* mutex_exit;

#endif /* GENERAL_KERNEL_H_ */
