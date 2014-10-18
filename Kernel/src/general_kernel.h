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

	extern t_queue* cola_new, cola_exit;
	extern sem_t* sem_new, sem_exit;
	extern pthread_mutex_t* mutex_new, mutex_exit;

#endif /* GENERAL_KERNEL_H_ */
