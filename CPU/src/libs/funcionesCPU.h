/*
 * funcionesCPU.h
 *
 *  Created on: 07/09/2014
 *      Author: utnso
 */

#ifndef FUNCIONESCPU_H_
#define FUNCIONESCPU_H_

#include <commons/config.h>
#include <commons/log.h>
#include <sys/stat.h>
#include "operaciones.h"

extern char* PATH;
extern int retardo;
extern int quantum;
extern t_log* log_cpu;
extern t_config* config_cpu;
extern t_config_cpu config_struct_cpu;
extern int sockKernel;
extern int sockMsp;
extern t_hilo* tcb;
bool terminoEjecucion;

void leer_configuracion();
void inicializar_configuracion();
t_struct_numero* terminar_y_pedir_tcb(t_hilo* tcb);
void ejecutar_otra_linea(int sockMSP,t_hilo* tcb, int bytecode[4]);


#endif /* FUNCIONESCPU_H_ */
