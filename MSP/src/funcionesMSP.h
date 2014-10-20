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
#include <stdbool.h>
#include <stdint.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include "commons/config.h"
#include "commons/collections/list.h"
#include "commons/log.h"
#include "commons/txt.h"
#include "commons/string.h"

char* path_swap;
t_log* logMSP;
pthread_mutex_t mutex_log;

typedef struct pagina{
	uint32_t PID;
	uint32_t num_pag;
	uint32_t num_segmento;
	char* codigo;
	uint32_t tamanio_buffer;
}t_pagina;

typedef struct marco{
	void *memoria;
	bool marco_libre;
} t_marco;

typedef struct direccion{
	uint32_t segmento;
	uint32_t pagina;
	uint32_t desplazamiento;
} t_direccion;

typedef struct info_memoria{
	int tamanio_mem_ppal;
	int tamanio_swap;
	t_list *lista_marcos;
	char *algoritmo_sust;
} t_info_mem;

typedef struct conexion_entrante{
	int *socket;
	t_info_mem *informacion_memoria;
	pthread_t *hiloDeConexion;
	//TODO Archivo de logeo
} t_conexion_entrante;

extern int puertoMSP, tamanio_mem_ppal, cant_mem_swap;
extern char* alg_sustitucion;



void leerConfiguracion(t_config *config, char *path); //levanta archivo de configuracion y asigna a las variables correspodientes
void crear_logger(t_log *logger);//Crea archivos de logeo
void *reservarBloquePpal(int tamanioMemoria); //Crea bloque de memoria principal con el tamaño especificado
t_list *dividirMemoriaEnMarcos(void *memoria, int tamanioMemoria); //Divide el bloque de memoria principal en marcos de tamaño 256bytes y devuelve la lista de ellos
void crear_archivo_swap_out(t_pagina pagina);
t_pagina leer_y_destruir_archivo_swap_in(int pid);
uint32_t obtenerBaseDelSegmento(uint32_t numeroSegmento);//Devuelve la direccion base del segmento
t_direccion traducirDireccion(uint32_t unaDireccion);//Traduce la direccion y la guarda en un struc nuevo

#endif /* FUNCIONESMSP_H_ */
