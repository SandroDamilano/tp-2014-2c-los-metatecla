/*
 * servicios_cpu.c
 *
 *  Created on: 23/10/2014
 *      Author: utnso
 */

#include "servicios_cpu.h"

void copiar_tcb(t_hilo* original, t_hilo* copia){
	copia->tid = original->tid;
	copia->pid = original->pid;
	copia->registros = original->registros;
};

void atender_systcall(t_hilo* tcb, uint32_t dir_systcall){
	t_hilo* tcb_kernel = desbloquear_tcbKernel();
	bloquear_tcbSystcall(tcb, dir_systcall);
	if (*tcb_kernel != NULL){
		copiar_tcb(tcb, tcb_kernel);
		tcb_kernel->puntero_instruccion = dir_systcall;
		encolar_en_ready(tcb_kernel);
	}
	//TODO Avisarle a la cpu que pida otro proceso para ejecutar
};

bool esta_por_systcall(t_data_nodo_block* data){
	return (data->evento == SYSTCALL);
}

t_data_nodo_block* desbloquear_alguno_por_systcall(t_hilo* tcb_kernel){
	return list_remove_by_condition(cola_block, esta_por_systcall);
}

void retornar_de_systcall(t_hilo* tcb_kernel){
	t_hilo* tcb = desbloquear_tcbSystcall(tcb_kernel->tid);
	tcb->registros = tcb_kernel->registros;
	encolar_en_ready(tcb);
	bloquear_tcbKernel(tcb_kernel);
	t_data_nodo_block* data_otro_tcb = desbloquear_alguno_por_systcall(tcb_kernel);

	//TODO Avisarle a la cpu que pida otro proceso para ejecutar

	if (*data_otro_tcb != NULL){
		atender_systcall(data_otro_tcb->tcb, data_otro_tcb->parametro);
	};
}

void crear_nuevo_hilo(t_hilo* tcb_padre){
	//TODO Averiguar qué hay que poner en el nuevo TCB hijo
}

