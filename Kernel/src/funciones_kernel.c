/*
 * funciones_kernel.c
 *
 *  Created on: 17/10/2014
 *      Author: utnso
 */

#include "funciones_kernel.h"

	t_queue* cola_new;
	t_queue* cola_exit;
	t_list* consolas;
	sem_t sem_exit;
	pthread_mutex_t mutex_exit;
	pthread_mutex_t mutex_TIDs;
	pthread_mutex_t mutex_PIDs;
	pthread_mutex_t mutex_log;
	pthread_mutex_t mutex_consolas;
	int cantidad_de_PIDs = 1;
	int cantidad_de_TIDs;

void inicializar_semaforos(){
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	sem_init(&sem_new, 0, 0);
	sem_init(&sem_exit, 0, 0);
	pthread_mutex_init(&mutex_PIDs, NULL);
	pthread_mutex_init(&mutex_TIDs, NULL);
	pthread_mutex_init(&mutex_log, NULL);
	pthread_mutex_init(&mutex_consolas, NULL);
}

void inicializar_colas_new_exit(){
	cola_new = queue_create();
	cola_exit = queue_create();
	consolas = list_create();
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
	pthread_mutex_lock(&mutex_PIDs);
	int pid = cantidad_de_PIDs++;
	pthread_mutex_unlock(&mutex_PIDs);
	return pid;
};

int obtener_tid(){
	pthread_mutex_lock(&mutex_TIDs);
	cantidad_de_TIDs++;	// o usar function process_get_thread_id() que provee un id unico
	int tid = cantidad_de_TIDs;
	pthread_mutex_unlock(&mutex_TIDs);
	return tid;
};

void pop_exit(t_data_nodo_exit* data){
	void *nuevo = queue_pop(cola_exit);
	*data = *(t_data_nodo_exit*)nuevo;
};

void sacar_de_exit(t_data_nodo_exit* data){
//	consumir_tcb(pop_exit, &sem_exit, &mutex_exit, tcb);
	sem_wait(&sem_exit);
	pthread_mutex_lock(&mutex_exit);
	pop_exit(data);
	pthread_mutex_unlock(&mutex_exit);
}

void push_new(t_hilo* tcb){
	queue_push(cola_new, (void*)tcb);
};

void poner_en_new(t_hilo* tcb){
	producir_tcb(push_new, &sem_new, &mutex_new, tcb);
}

void imprimir_texto(int socket_consola, char* mensaje){
	t_struct_string* impresion = malloc(sizeof(t_struct_string));
	int tamanio_mensaje = strlen(mensaje);
	impresion->string = malloc(tamanio_mensaje);
	memcpy(impresion->string, mensaje, tamanio_mensaje);
	socket_enviar(socket_consola, ENVIAR_IMPRIMIR_TEXTO, impresion);
	free(impresion);
}

t_hilo *crear_TCB(uint32_t pid, uint32_t dir_codigo, uint32_t dir_stack, uint32_t tamanio_codigo)
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
	for(i=0; i<=4; i++){
		tcb->registros[i] = 0;
	}

	tcb->cola = NEW;

	return tcb;
}

