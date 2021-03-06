/*
 * funcionesMSP.h
 *
 *  Created on: 24/09/2014
 *      Author: utnso
 */

#ifndef FUNCIONESMSP_H_
#define FUNCIONESMSP_H_

#define MAX_COUNT_OF_CONFIG_KEYS 5
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
#include <sockets/enviar_recibir_paquete.h>

t_log* logger;

char* path_config, *path_swap;
pthread_mutex_t mutex_log;
int paginas_en_disco;
void* buffer;
char bufferLog[80];

t_list* lista_conexiones;

typedef struct  lista_paginas{
	uint32_t numeroPagina;
	uint32_t marcoEnMemPpal;
	uint32_t swap;
	}t_lista_paginas;

typedef struct  lista_segmentos{
	uint32_t numeroSegmento;
	uint32_t tamanio;
	t_list *lista_Paginas;

}t_lista_segmentos;

typedef struct lista_procesos{
	uint32_t pid;
	t_list *lista_Segmentos;
} t_lista_procesos;

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
	uint32_t marco_libre;
	uint32_t numeroMarco;
	uint32_t bitAlgoritmo;
	uint32_t pid;
	uint32_t segmento;
	uint32_t pagina;
} t_marco;

typedef struct conexion_entrante{
	uint32_t socket;
	//pthread_t *hiloDeConexion; TODO ??
} t_conexion_entrante;


extern uint32_t puertoMSP;
extern uint32_t tamanio_mem_ppal;
extern uint32_t cant_mem_swap;
extern char* alg_sustitucion;
extern t_list *listaProcesos;
extern t_marco *tabla_marcos;
extern uint32_t cant_marcos;
extern bool escuchandoConexiones;
extern void* memoria_ppal;
extern uint32_t punteroClock;
//Semaforos
extern pthread_mutex_t mutex_consola;
extern pthread_mutex_t mutex_log;
extern pthread_mutex_t mutex_reservarMemoria;
extern pthread_mutex_t mutex_crearSegmento;
extern pthread_mutex_t mutex_tablaMarcos;
extern pthread_mutex_t mutex_swap;
extern pthread_mutex_t mutex_memoriaPpal;
////////////Consola//////////////
void *inciarConsola(void *param1);
void indicaciones_consola();
void tabla_segmentos();
void tabla_paginas(uint32_t PID);
void listar_marcos();


///////////Interfaz MSP/////////
uint32_t crearSegmento(uint32_t PID, uint32_t tamanio_segmento); // crea un nuevo segmento para PID del tamanio pedido y devuelta la direcion de memoria base del segmento
uint32_t destruirSegmento(uint32_t PID, uint32_t base_segmento); //destruye el segmento correspodiente a su base del proceso PID
void* solicitar_memoria(uint32_t PID, uint32_t direccion_log, uint32_t tamanio_mem); //muestra desde direcc logica hasta tamanio_mem lo que se encuentra escrito
int escribirMemoria(uint32_t PID, uint32_t direcc_log, void* bytes_escribir, uint32_t tamanio); // para el espacio de direcc de PID escribe hasta tamaño los bytes

///////////CONEXIONES//////////
void handler_conexiones(void);
void handler_solicitudes(t_conexion_entrante* conexion);

///////////Auxiliares//////////

//Excepciones
void PID_not_found_exception(uint32_t PID);
void segment_not_found_exception(uint32_t segmento);
void page_not_found_exception(uint32_t pagina);
void segmentation_fault();

//Manejo de informacion
void* devolverInformacion(void *baseMarco, t_direccion direccion, uint32_t tamanio);
void guardarInformacion(void *baseMarco,t_direccion direccion,void* bytes_escribir, uint32_t tamanio);
void hacerSwap(uint32_t PID, t_direccion direccion, t_lista_paginas *pagina, t_lista_segmentos *segmento, uint32_t numeroMarco);

//Liberar memoria
void liberarProceso(t_lista_procesos *proceso);
void liberarSegmento(t_lista_segmentos *segmento);
void liberarPagina(t_lista_paginas *pagina);

void llegoSenialParaTerminar(int senial);

////////////INICIO////////////
void leer_config(char *path); 	//levanta archivo de configuracion y asigna a las variables correspodientes
void crear_logger();			//Crea archivos de log


//////MEMORIA PRINCIPAL//////
void *reservarBloquePpal(uint32_t tamanioMemoria); //Crea bloque de memoria principal con el tamaño especificado
uint32_t calcularMarcos(uint32_t tamanioMemoria); //Calcula cantidad en que se divide la MP
t_marco *crearTablaDeMarcos(); //Divide el bloque de memoria principal en marcos de tamaño 256bytes y devuelve la lista de ellos
uint32_t buscarMarcoLibre(t_marco *unaTabla);//Busca un marco libre, en caso no encontrar devuelve -1

////////////SWAP////////////
void crearArchivoSwapEnDisco(t_pagina pagina);
t_pagina extraerInfoDeArchSwap(uint32_t pid, uint32_t seg, uint32_t pag);
DIR* abrir_directorio_swap();
t_pagina abrir_archivo_en_direcctorio(uint32_t PID, uint32_t SEG, uint32_t PAG);
void destruir_archivo_swap(int pid, uint32_t segmento, uint32_t pagina);
void destruir_archivo(char* nombre_archivo);
void modificarBitAlgoritmo(uint32_t numero);
uint32_t buscarPaginaMenosUsada();
uint32_t algoritmo_clock();
#endif /* FUNCIONESMSP_H_ */
