/*
 * planificador.c
 *
 *  Created on: 06/10/2014
 *      Author: utnso
 */

#include "planificador.h"

void encolar_en_ready(t_hilo* tcb){
	pthread_mutex_lock(mutex_ready);
	if (tcb->kernel_mode == 1){
		list_add_in_index(cola_ready, 0, (void*)tcb);
	}else{
		list_add(cola_ready, (void*)tcb);
	};
	pthread_mutex_unlock(mutex_ready);
	tcb->cola = READY;
};

t_hilo* obtener_tcb_a_ejecutar(){
	return (t_hilo*) list_remove(cola_ready, 0);
};

//Este bloquear es GENERAL. Para bloquear, usar los particulares.
void bloquear_tcb(t_hilo* tcb, t_evento evento, uint32_t tid, uint32_t recurso){
	t_data_nodo_block* data = malloc(sizeof(t_data_nodo_block));
	data->tcb = tcb;
	data->evento = evento;
	data->tid = tid;
	data->recurso = recurso;
	list_add(cola_block, (void*)data);
	tcb->cola = BLOCK;
};

void bloquear_tcbKernel(t_hilo* tcb){
	bloquear_tcb(tcb, TCBKM, -1, -1);
}

void bloquear_tcbJoin(t_hilo* tcb, uint32_t tid){
	bloquear_tcb(tcb, JOIN, tid, -1);
}

void bloquear_tcbSystcall(t_hilo* tcb, uint32_t tid){
	bloquear_tcb(tcb, SYSTCALL, tid, -1);
}

void bloquear_tcbSemaforo(t_hilo* tcb, uint32_t sem){
	bloquear_tcb(tcb, SEM, -1, sem);
}

bool es_el_tcbkernel(t_data_nodo_block* data){
	return (data->evento == TCBKM);
};

bool es_el_tcbBuscado(t_data_nodo_block* data){
	return (data->tid == tid_a_buscar & data->recurso == recurso_a_buscar & data->evento == evento_a_buscar);
};

// Este desbloquear es GENERAL. Para desbloqear, hay que usar los particulares.
void desbloquear_proceso(t_evento evento, uint32_t tid, uint32_t recurso){
	t_hilo* tcb_desbloqueado;
	tid_a_buscar = tid;
	recurso_a_buscar = recurso;
	evento_a_buscar = evento;
	tcb_desbloqueado = list_remove_by_condition(cola_block, es_el_tcbBuscado);
	encolar_en_ready(tcb_desbloqueado);
}

t_hilo* desbloquear_tcbkernel(){
	return list_remove_by_condition(cola_block, es_el_tcbkernel);
};

void desbloquear_por_tid(t_evento evento, uint32_t tid){
	desbloquear_proceso(evento, tid, -1);
}

void desbloquear_por_semaforo(t_evento evento, uint32_t sem){
	desbloquear_proceso(evento, -1, sem);
}

void inicializar_ready_block(){
	cola_ready = list_create();
	cola_block = list_create();
}

void pop_new(t_hilo* tcb){
	void* nuevo = queue_pop(cola_new);
	*tcb = *nuevo;
};

//Este hilo se queda haciendo loop hasta que termine la ejecución
void poner_new_a_ready(){
	while(1){
		t_hilo* tcb;
		consumir_tcb(pop_new, sem_new, mutex_new, tcb);
		encolar_en_ready(tcb);
	}
};

void inicializar_semaforo_ready(){
	pthread_mutex_init(mutex_ready, NULL);
};



void boot(char* systcalls_path){
	uint32_t dir_codigo;
	uint32_t dir_stack;
	int tamanio_codigo;

	//Levanta archivo de syst calls
	FILE* syscalls_file = abrir_archivo(systcalls_path, logger, mutex_log);
	tamanio_codigo = calcular_tamanio_archivo(syscalls_file);
	char* syscalls_code = leer_archivo(syscalls_file, tamanio_codigo);

	//TODO mandárselo a la MSP
	//dir_codigo =
	//dir_stack =
	t_hilo* tcb_kernel = crear_TCB(obtener_pid(), dir_codigo, dir_stack, tamanio_codigo);
	tcb_kernel->kernel_mode = true;
	bloquear_tcbKernel(tcb_kernel);
}

void* main_PLANIFICADOR(arg_PLANIFICADOR* parametros)
{
	inicializar_ready_block();
	inicializar_semaforo_ready();
	pthread_t thr_consumidor_new;
	pthread_create(&thr_consumidor_new, NULL, poner_new_a_ready, NULL);

	boot(parametros->syscalls_path);
	// TODO: Boot

	pthread_join(thr_consumidor_new, NULL);

return 0;
}
