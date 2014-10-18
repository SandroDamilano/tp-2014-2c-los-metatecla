/*
 * general_kernel.c
 *
 *  Created on: 17/10/2014
 *      Author: utnso
 */

#include <general_kernel.h>

void inicializar_semaforos(){
	pthread_mutex_init(mutex_new, NULL);
	pthread_mutex_init(mutex_exit, NULL);
	sem_init(sem_new, 0, 0);
	sem_init(sem_exit, 0, 0);
}

//La idea seria usar estas funciones para hacer push y pop de las colas de new y ready
void producir_tcb(void (*funcion)(t_hilo*), sem_t* sem, pthread_mutex_t* mutex, t_hilo* tcb){
	pthread_mutex_lock(mutex);
	funcion(tcb);
	pthread_mutex_unlock(mutex);
	sem_post(sem);
};

void consumir_tcb(void (*funcion)(t_hilo*), sem_t* sem, pthread_mutex_t* mutex, t_hilo* tcb){
	sem_wait(sem);
	pthread_mutex_lock(mutex);
	funcion(tcb);
	pthread_mutex_unlock(mutex);
};

//TODO: Traer aca la funcion crear_TCB del loader
