/*
 * funcionesMSP.h
 *
 *  Created on: 24/09/2014
 *      Author: utnso
 */

#ifndef FUNCIONESMSP_H_
#define FUNCIONESMSP_H_

#define MAX_COUNT_OF_CONFIG_KEYS 4
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
//#include "ConsolaMSP.h"
#include <estructuras_auxiliares.h>


char* path_config, *path_swap;
pthread_mutex_t mutex_log;
int paginas_en_disco;
void* buffer;
char bufferLog[80];

typedef struct pagina{
	uint32_t PID;
	uint32_t num_pag;
	uint32_t num_segmento;
	char* codigo;
	uint32_t tamanio_buffer;
	FILE* archivo;
	char* nombre_archivo;
}t_pagina;

typedef struct marco{
	void *memoria;
	bool marco_libre;
	uint32_t numeroMarco;
} t_marco;

typedef struct direccion{
	uint32_t segmento;
	uint32_t pagina;
	uint32_t desplazamiento;
} t_direccion;

typedef struct info_memoria{
	uint32_t tamanio_mem_ppal;
	uint32_t tamanio_swap;
	t_list *lista_marcos;
	char *algoritmo_sust;
	uint32_t memoriaSwapActual;
	uint32_t memoriaPpalActual;
} t_info_mem;

typedef struct conexion_entrante{
	uint32_t *socket;
	t_info_mem *informacion_memoria;
	pthread_t *hiloDeConexion;
	//TODO Archivo de logeo
} t_conexion_entrante;

extern uint32_t puertoMSP;
extern uint32_t tamanio_mem_ppal;
extern uint32_t cant_mem_swap;
extern char* alg_sustitucion;

//Semaforos
extern pthread_mutex_t mutex_consola;
extern pthread_mutex_t mutex_log;

void *inciarConsola(void *param1);
void indicaciones_consola();


uint32_t crearSegmento(uint32_t PID, uint32_t tamanio_segmento); // crea un nuevo segmento para PID del tamanio pedido y devuelta la direcion de memoria base del segmento
void destruirSegmento(uint32_t PID, uint32_t base_segmento); //destruye el segmento correspodiente a su base del proceso PID
void solicitarMemoria(uint32_t PID, uint32_t direccion_log, int tamanio_mem); //muestra desde direcc logica hasta tamanio_mem lo que se encuentra escrito
//TODO no estamos seguros de que devuelta esta funcion. Consultar
void escribirMemoria(uint32_t PID, uint32_t direcc_log, void* bytes_escribir, uint32_t tamanio); // para el espacio de direcc de PID escribe hasta tamaño los bytes
uint32_t crearDireccion(uint32_t segmento,uint32_t pagina);

void leer_config(char *path); 	//levanta archivo de configuracion y asigna a las variables correspodientes
void crear_logger();			//Crea archivos de log
void *reservarBloquePpal(int tamanioMemoria); //Crea bloque de memoria principal con el tamaño especificado
t_list *dividirMemoriaEnMarcos(void *memoria, int tamanioMemoria); //Divide el bloque de memoria principal en marcos de tamaño 256bytes y devuelve la lista de ellos
uint32_t obtenerBaseDelSegmento(uint32_t numeroSegmento);	//Devuelve la direccion base del segmento
t_direccion traducirDireccion(uint32_t unaDireccion);		//Traduce la direccion y la guarda en un struc nuevo
uint32_t crearDireccion(uint32_t segmento, uint32_t pagina);

char *traducirABinario(uint32_t direccion, int cantidad_bits) ;
uint32_t traducirADecimal(char *binario, int cantidad_bits);


void swap_out(t_pagina pagina);
t_pagina swap_in(int pid, int seg, int pag);
DIR* abrir_directorio_swap();
t_pagina buscar_archivo(int PID, int SEG, int PAG, DIR* dir);

void destruir_archivo(char* nombre_archivo);
void destruir_archivo_swap(int pid, uint32_t segmento, uint32_t pagina);

void PID_not_found_exception(uint32_t PID);
void segment_not_found_exception(uint32_t segmento);
void page_not_found_exception(uint32_t pagina);
void segmentation_fault();

#endif /* FUNCIONESMSP_H_ */
