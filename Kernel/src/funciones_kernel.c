/*
 * funciones_kernel.c
 *
 *  Created on: 17/10/2014
 *      Author: utnso
 */

#include "funciones_kernel.h"

	t_queue* cola_new;
	t_queue* cola_exit;
	t_list* cola_ready;
	t_list* cola_block;
	t_list* cola_exec;
	t_list* consolas;
	sem_t sem_exit;
	sem_t sem_ready;
	pthread_mutex_t mutex_exit;
	pthread_mutex_t mutex_exec;
	pthread_mutex_t mutex_new;
	pthread_mutex_t mutex_ready;
	pthread_mutex_t mutex_block;
	pthread_mutex_t mutex_TIDs;
	pthread_mutex_t mutex_PIDs;
	pthread_mutex_t mutex_log;
	pthread_mutex_t mutex_consolas;
	pthread_mutex_t mutex_consolas_conectadas;
	pthread_mutex_t mutex_cpus_conectadas;
	int cantidad_de_PIDs = 1;
	int cantidad_de_TIDs;

	t_list* lista_abortar;
	pthread_mutex_t mutex_abortar;

void inicializar_semaforos(){
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	sem_init(&sem_new, 1, 0);
	sem_init(&sem_exit, 1, 0);
	pthread_mutex_init(&mutex_PIDs, NULL);
	pthread_mutex_init(&mutex_TIDs, NULL);
	pthread_mutex_init(&mutex_log, NULL);
	pthread_mutex_init(&mutex_consolas, NULL);
	pthread_mutex_init(&mutex_consolas_conectadas, NULL);
	pthread_mutex_init(&mutex_cpus_conectadas, NULL);
	pthread_mutex_init(&mutex_abortar, NULL);
}

void inicializar_colas_new_exit(){
	cola_new = queue_create();
	cola_exit = queue_create();
	consolas = list_create();
	lista_abortar = list_create();
}

bool hay_que_abortar_pid(uint32_t pid_a_abortar){
	bool hay_que_abortarlo(uint32_t* pid){
		return *pid == pid_a_abortar;
	}

	pthread_mutex_lock(&mutex_abortar);
	bool respuesta = list_any_satisfy(lista_abortar, (void*)hay_que_abortarlo);
	pthread_mutex_unlock(&mutex_abortar);

	return respuesta;
}

void agregar_a_abortar(uint32_t pid){
	if(!hay_que_abortar_pid(pid)){
		uint32_t* PID = malloc(sizeof(uint32_t));
		*PID = pid;
		pthread_mutex_lock(&mutex_abortar);
		list_add(lista_abortar, PID);
		pthread_mutex_unlock(&mutex_abortar);
	}
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
	int tid = cantidad_de_TIDs;
	cantidad_de_TIDs++;	// o usar function process_get_thread_id() que provee un id unico
	pthread_mutex_unlock(&mutex_TIDs);
	return tid;
};

/*
void pop_exit(t_data_nodo_exit* data){
	//void *nuevo = queue_pop(cola_exit);
	*data = *(t_data_nodo_exit*)nuevo;
	data = (t_data_nodo_exit*)queue_pop(cola_exit);
	printf("en el pop, data->tcb->tid = %d\n", (data->tcb)->tid);
};*/

t_data_nodo_exit* sacar_de_exit(){
//	consumir_tcb(pop_exit, &sem_exit, &mutex_exit, tcb);
	sem_wait(&sem_exit);
	pthread_mutex_lock(&mutex_exit);
	t_data_nodo_exit* data = (t_data_nodo_exit*)queue_pop(cola_exit);
	pthread_mutex_unlock(&mutex_exit);
	return data;
}

void push_new(t_hilo* tcb){
	queue_push(cola_new, (void*)tcb);
};

void poner_en_new(t_hilo* tcb){
	tcb->cola = NEW;
	producir_tcb(push_new, &sem_new, &mutex_new, tcb);
}

void imprimir_texto(int socket_consola, char* mensaje){
	t_struct_string* impresion = malloc(sizeof(t_struct_string));
	impresion->string = mensaje;
	socket_enviar(socket_consola, D_STRUCT_OUTC, impresion);
	int tamanio_mensaje = strlen(mensaje);
	t_struct_numero* tam = malloc(sizeof(t_struct_numero));
	tam->numero = tamanio_mensaje;
	socket_enviar(socket_consola, D_STRUCT_NUMERO, tam);
	free(impresion);
	free(tam);
}

t_hilo *crear_TCB(uint32_t pid, uint32_t dir_codigo, uint32_t dir_stack, uint32_t tamanio_codigo)
{
	/*
	 * Guardo en el TCB la dir logica (ficticia), asi lo abstraemos de los posibles movimientos en la MSP
	 * TODO: Ver como tratar los ID's de los campos
	 *
	 * NOTA: A la MSP JAMAS le paso el TCB
	 */
	t_hilo *tcb = malloc(sizeof(t_hilo));

	tcb->pid = pid;
	tcb->tid = obtener_tid();
	tcb->kernel_mode = false;	// teniendo en cuenta que TODOS los que vengan por aca seran programas de usuario
	tcb->segmento_codigo = dir_codigo;
	tcb->segmento_codigo_size = tamanio_codigo;
	//tcb->puntero_instruccion = dir_codigo;	// se inicializa con la base del segmento de codigo (segun enunciado)
	tcb->puntero_instruccion = 0;
	tcb->base_stack = dir_stack;
	tcb->cursor_stack = 0; // TODO: Consultar con ayudante si esta bien

	int i;
	for(i=0; i<=4; i++){
		tcb->registros[i] = 0;
	}

	//tcb->cola = NEW;

	return tcb;
}

