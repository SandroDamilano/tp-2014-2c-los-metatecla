/*
 * servicios_cpu.h
 *
 *  Created on: 23/10/2014
 *      Author: utnso
 */

#ifndef SERVICIOS_CPU_H_
#define SERVICIOS_CPU_H_

	#include <ansisop-panel/panel.h>
	#include <commons/log.h>
	#include "general_kernel.h"
	#include "planificador.h"

void copiar_tcb(t_hilo* original, t_hilo* copia);
void atender_systcall(t_hilo* tcb, uint32_t dir_systcall);


#endif /* SERVICIOS_CPU_H_ */
