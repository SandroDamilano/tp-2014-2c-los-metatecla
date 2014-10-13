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
extern int sock_kernel;
extern int sock_msp;

void leer_configuracion();
void inicializar_configuracion();


#endif /* FUNCIONESCPU_H_ */
