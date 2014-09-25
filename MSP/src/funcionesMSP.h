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
#include <commons/config.h>
#include <commons/collections/list.h>
#include <stdbool.h>

typedef struct marco{
	void *memoria;
	bool marco_libre;
} t_marco;

extern int puertoMSP, tamanio_mem_ppal, cant_mem_swap;
extern char* alg_sustitucion;



void leerConfiguracion(); //levanta archivo de configuracion y asigna a las variables correspodientes
void *reservarBloquePpal(int tamanioMemoria); //Crea bloque de memoria principal con el tamaño especificado
t_list *dividirMemoriaEnMarcos(void *memoria, int tamanioMemoria); //Divide el bloque de memoria principal en marcos de tamaño 256bytes y devuelve la lista de ellos
#endif /* FUNCIONESMSP_H_ */
