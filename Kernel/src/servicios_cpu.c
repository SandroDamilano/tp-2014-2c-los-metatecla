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
	t_hilo* tcb_kernel = desbloquear_tcbkernel();
	bloquear_tcbSystcall(tcb, tcb->tid);
	if (*tcb_kernel != NULL){
		copiar_tcb(tcb, tcb_kernel);
		tcb_kernel->puntero_instruccion = dir_systcall;
		encolar_en_ready(tcb_kernel);
	}
	//TODO Avisarle a la cpu que pida otro proceso para ejecutar
};

void retornar_de_systcall(t_hilo* tcb_kernel){
	//TODO Averiguar dónde puedo guardar la dirección de la systcall en atender_systcall
}

void crear_nuevo_hilo(t_hilo* tcb_padre){
	//TODO Averiguar qué hay que poner en el nuevo TCB hijo
}

