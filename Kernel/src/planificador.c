/*
 * planificador.c
 *
 *  Created on: 06/10/2014
 *      Author: utnso
 */

#include <planificador.h>

void encolar_en_ready(t_hilo* tcb){
	if (tcb->kernel_mode == 1){
		list_add_in_index(cola_ready, 0, (void*)tcb);
	}else{
		list_add(cola_ready, (void*)tcb);
	};
	tcb->cola = READY;
};

t_hilo* obtener_tcb_a_ejecutar(){
	return (t_hilo*) list_remove(cola_ready, 0);
};

//tid y recurso pueden ser NULL dependiendo del tipo de evento
void bloquear_tcb(t_hilo* tcb, t_evento evento, uint32_t tid, uint32_t recurso){
	data_nodo_block* data = malloc(sizeof(data_nodo_block));
	data->tcb = tcb;
	data->evento = evento;
	data->tid = tid;
	data->recurso = recurso;
	list_add(cola_block, (void*)data);
	tcb->cola = BLOCK;
};

void copiar_tcb(t_hilo* original, t_hilo* copia){
	copia->tid = original->tid;
	copia->pid = original->pid;
	copia->registros = original->registros;
};

bool es_el_tcbkernel(void* data){
	return (data->evento == TCBKM);
};

t_hilo* desbloquear_tcbkernel(){
	return list_remove_by_condition(cola_block, es_el_tcbkernel);
};

void atender_systcall(t_hilo* tcb, uint32_t dir_systcall){
	t_hilo* tcb_kernel = desbloquear_tcbkernel();
	bloquear_tcb(tcb, SYSTCALL, NULL, NULL);
	if (*tcb_kernel != NULL){
		copiar_tcb(tcb, tcb_kernel);
		tcb_kernel->puntero_instruccion = dir_systcall;
		encolar_en_ready(tcb_kernel);
	}
	//TODO Avisarle a la cpu que pida otro proceso para ejecutar
};

// TODO Desbloquear un proceso. Depende de la cantidad de colas de bloqueados que haya.

void* main_PLANIFICADOR(void* parametros)
{

return 0;
}
