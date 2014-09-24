/*
 * funcionesMSP.h
 *
 *  Created on: 24/09/2014
 *      Author: utnso
 */

#ifndef FUNCIONESMSP_H_
#define FUNCIONESMSP_H_
#include <stdio.h>
#include <stdlib.h>

int puertoMSP = 0, tamanio_mem_ppal = 0, cant_mem_swap = 0;
char* alg_sustitucion;

void leerConfiguracion(); //levanta archivo de configuracion y asigna a las variables correspodientes
void *reservarBloquePpal(int tamanioMemoria); //Crea bloque de memoria principal con el tamaño especificado
#endif /* FUNCIONESMSP_H_ */
