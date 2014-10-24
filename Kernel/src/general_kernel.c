/*
 * general_kernel.c
 *
 *  Created on: 17/10/2014
 *      Author: utnso
 */

#include "general_kernel.h"

void inicializar_semaforos(){
	//FIXME creo que en algun lado tenes que usar malloc para los mutex si los usas como puntero. Sino se pueden usar con &
	pthread_mutex_init(mutex_new, NULL);
	pthread_mutex_init(mutex_exit, NULL);
	sem_init(sem_new, 0, 0);
	sem_init(sem_exit, 0, 0);
	pthread_mutex_init(mutex_PIDs, NULL);
	pthread_mutex_init(mutex_TIDs, NULL);
	pthread_mutex_init(mutex_log, NULL);
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
	pthread_mutex_lock(mutex_PIDs);
	int pid = cantidad_de_PIDs++;
	pthread_mutex_unlock(mutex_PIDs);
	return pid;
};

int obtener_tid(){
	pthread_mutex_lock(mutex_TIDs);
	int tid = cantidad_de_TIDs++;
	pthread_mutex_unlock(mutex_TIDs);
	return tid;
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

	int i;
	for(i=0; i==4; i++){
		tcb->registros[i] = 0;
	}

	tcb->cola = NEW;

	return tcb;
}
