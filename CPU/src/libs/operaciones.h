/*
 * operaciones.h
 *
 *  Created on: 07/09/2014
 *      Author: utnso
 */

#ifndef OPERACIONES_H_
#define OPERACIONES_H_

#include "auxiliares.h"
#include <sockets/enviar_recibir_paquete.h>
#include <estructuras_auxiliares.h>

int* param;
//int sockMSP;
//int sockKernel;

void ejecutarLinea(int codigo[]);
int convertirAString(int* bytecode);



#endif /* OPERACIONES_H_ */
