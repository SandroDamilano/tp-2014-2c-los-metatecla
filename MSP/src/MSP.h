/*
 * MSP.h
 *
 *  Created on: 24/09/2014
 *      Author: utnso
 */

#ifndef MSP_H_
#define MSP_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct lista_procesos{
	int pid;
	int *lista_Segmentos;
} t_lista_procesos;

typedef struct  lista_segmentos{
	int *lista_Paginas;

}t_lista_segmentos;

typedef struct  lista_paginas{
	int *direccion_memoria_ppal;
	boolean swap;
	}t_lista_paginas;

	//Estructuras sujetas a posibles cambio y arreglos mas adelante

#endif /* MSP_H_ */
