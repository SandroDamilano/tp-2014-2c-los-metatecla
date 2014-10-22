/*
 * general_kernel.c
 *
 *  Created on: 17/10/2014
 *      Author: utnso
 */

#include "general_kernel.h"

//TODO usar en algún lado esta función!!
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

int obtener_pid(){
	return cantidad_de_PIDs++;
};

int obtener_tid(){
	return cantidad_de_TIDs++;
};

t_hilo *crear_TCB(int pid, uint32_t dir_codigo, uint32_t dir_stack, int tamanio_codigo)
{
	/*
	 * Guardo en el TCB la dir logica (ficticia), asi lo abstraemos de los posibles movimientos en la MSP
	 * TODO: Ver como tratar los ID's de los campos
	 *
	 * NOTA: A la MSP JAMAS le paso el TCB
	 */
	t_hilo *tcb = (t_hilo*)malloc(sizeof(t_hilo));

	tcb->pid = pid;
	tcb->tid = obtener_tid();
	tcb->kernel_mode = false;	// teniendo en cuenta que TODOS los que vengan por aca seran programas de usuario
	tcb->segmento_codigo = dir_codigo;
	tcb->segmento_codigo_size = tamanio_codigo;
	tcb->puntero_instruccion = dir_codigo;	// se inicializa con la base del segmento de codigo (segun enunciado)
	tcb->base_stack = dir_stack;
	tcb->cursor_stack = 0; // TODO: Consultar con ayudante si esta bien
	tcb->registros = (0,0,0,0,0);
	tcb->cola = NEW;

	return tcb;
}

