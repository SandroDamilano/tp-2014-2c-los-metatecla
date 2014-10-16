/*
 ============================================================================
 Name        : MSP.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "MSP.h"

	//Variables de Archivos
	t_log *logMSP = NULL;
	t_config *archConfigMSP = NULL;

	//Variables de Memoria Principal
	void *memoria_ppal = NULL;
	int memoriaSwapActual = 0;
	int memoriaPpalActual = 0;

	//Variables de estructuras administrativas
	t_list *listaProcesos = NULL;
	t_list *lista_marcos = NULL;

	//Variables para hilos
	pthread_t consola;

	//Semaforos
	pthread_mutex_t mutex_consola;
	pthread_mutex_t mutex_log;

	//Variables para Sockets
	int socketServidorMSP;
	bool escuchandoConexiones = true;

int main(int argc, char *argv[]) {
	//Defino path para poner los archivos del swap. Esta hardcodeado, deberia pasarse por parametro.
	path_swap = "/home/utnso/archivos_swap/";

	//1.Leer archivo de configuracion, archivo de log y iniciar semaforos
	crear_logger(logMSP);
	leerConfiguracion(archConfigMSP, argv[1]);
	inicializar_semaforos();

	//2. Reservar bloque de memoria principal

	memoria_ppal=reservarBloquePpal(tamanio_mem_ppal);

	//TODO log "se creo memoria princial y abaca desde memoria_ppal hasta memoria_ppal+tamanio

	//3. Generar estructuras administrativas

	lista_marcos=dividirMemoriaEnMarcos(memoria_ppal, tamanio_mem_ppal);
	memoriaPpalActual=tamanio_mem_ppal;
	memoriaSwapActual=cant_mem_swap;
	listaProcesos = list_create();

	//TODO log lista de procesos, tabla de paginas y tabla de segmentos

	//4. Abrir conexiones con Kernel y CPU, y levantar Consola MSP

	//Se crea el hilo para la consola
		pthread_create(&consola, NULL, inciarConsola, NULL); //TODO pasar info de la memoria a la consola (4 parametro diferente a null)

		//Se crea socket servidor de la MSP

		socketServidorMSP= socket_crearServidor("127.0.0.1", puertoMSP);

		//TODO LOG Se abren conexiones por sockets para Kernel y CPU

		//Msp a la escucha de nuevas conexiones

	while(escuchandoConexiones){ //TODO log esperando nuevas conexiones

		pthread_t *hiloDeConexion = malloc (sizeof(pthread_t)); // Hilo que va a tratar la nueva conexion
		int *nuevoSocket = malloc(sizeof(int)); //Puntero al socket que va a atender la nueva conexion

		t_info_mem *informacion_memoria= malloc(sizeof(t_info_mem)); //Informacion de memoria que va a utilizar el hilo
		(*informacion_memoria).algoritmo_sust=alg_sustitucion;
		(*informacion_memoria).lista_marcos=lista_marcos;
		(*informacion_memoria).tamanio_mem_ppal=tamanio_mem_ppal;
		(*informacion_memoria).tamanio_swap=cant_mem_swap;

		t_conexion_entrante *nuevaConexion = malloc(sizeof(t_conexion_entrante)); //Informacion para el hilo de conexion
		(*nuevaConexion).hiloDeConexion=hiloDeConexion;
		(*nuevaConexion).informacion_memoria=informacion_memoria;
		(*nuevaConexion).socket=nuevoSocket;

		*nuevoSocket=socket_aceptarCliente(socketServidorMSP);

		pthread_create(hiloDeConexion,NULL,inciarConsola,(void *) nuevaConexion); //TODO donde dice inciarConsola va una funcion que maneje el pedido de la conexion
		pthread_detach(*hiloDeConexion);
	}

		//Espera que se termine el hilo consola
		pthread_join(consola, NULL);

		//TODO Liberar recursos

	return EXIT_SUCCESS;
}

int mismo_pid(int pid1, int pid2){
	return pid1==pid2;
}

int inicializar_semaforos(){

	if(pthread_mutex_init(&mutex_consola,NULL) != 0){
		printf("mutex_consola failed");
		return EXIT_FAILURE;
	}

	if(pthread_mutex_init(&mutex_log,NULL) != 0){
		printf("mutex_log failed");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

uint32_t crearSegmento(int PID, int tamanio_segmento){
	uint32_t direccionBaseDelSegmento;

	//Puntero a funcion mismo_pid
	int (*mismoPID) (int p1, int p2);
	mismoPID = &mismo_pid;

	//1.Verifica si hay memoria disponible
	int cant_mem_actual=memoriaPpalActual+memoriaSwapActual;
	if (tamanio_segmento > 1048576){
		pthread_mutex_lock(&mutex_log);
		printf("Error el tamaño de segmento pedido es mayor a lo soportado (maximo 1048576 bytes).");
		log_error(logMSP,"Error el tamaño de segmento pedido es mayor a lo soportado (maximo 1048576 bytes).");
		pthread_mutex_unlock(&mutex_log);
		return 0;
	}

	if(tamanio_segmento > cant_mem_actual){
		pthread_mutex_lock(&mutex_log);
		printf("Error Memoria llena");
		log_error(logMSP,"Error Memoria Llena");
		pthread_mutex_unlock(&mutex_log);
		return 0;
	}
	//2.Se fija si se existe el proceso

	t_lista_procesos *proceso = malloc(sizeof(t_lista_procesos));
	proceso = list_find(listaProcesos, (void*) (*mismoPID) ((*proceso).pid, PID));

	//3. Se fija donde crear el segmento
	int segmentoEnSwap = 0;
	int segmentoEnMP = 0;

	if(tamanio_segmento<=memoriaSwapActual){
		segmentoEnSwap=tamanio_segmento;
		memoriaSwapActual=memoriaSwapActual-tamanio_segmento;
	} else { if(memoriaSwapActual>0){
				segmentoEnSwap = memoriaSwapActual;
				tamanio_segmento=tamanio_segmento-memoriaSwapActual;
				memoriaSwapActual = 0;
				segmentoEnMP=tamanio_segmento;
				memoriaPpalActual= memoriaPpalActual-tamanio_segmento;
	} else { segmentoEnMP=tamanio_segmento;
			 memoriaPpalActual= memoriaPpalActual-tamanio_segmento;}
	}

	//4.Crea lista de segmentos o agrega nuevo segmento
	if(proceso==NULL){
		(*proceso).pid=PID;
		(*proceso).lista_Segmentos=list_create();
		list_add(listaProcesos,proceso);}

	int tamanioListaSeg=list_size((*proceso).lista_Segmentos);
	t_lista_segmentos *nuevoSegmento = malloc(sizeof(t_lista_segmentos));
	if(tamanioListaSeg<4096){
		(*nuevoSegmento).lista_Paginas=list_create();
		(*nuevoSegmento).numeroSegmento=tamanioListaSeg;
		(*nuevoSegmento).tamanio=segmentoEnSwap+segmentoEnMP;
		pthread_mutex_lock(&mutex_log);
		list_add((*proceso).lista_Segmentos, nuevoSegmento);
		log_info(logMSP,"Se creo el nuevo segmento del proceso: %d y tiene el tamaño: %d",PID,segmentoEnSwap+segmentoEnMP);
		pthread_mutex_unlock(&mutex_log);
	} else {
		pthread_mutex_lock(&mutex_log);
		log_error(logMSP,"Se supera el maximo de segmentos por programa");
		pthread_mutex_unlock(&mutex_log);
		return 0;
	}
	//5.Crea tabla de paginas
	int cantPaginas= (segmentoEnSwap+segmentoEnMP)/256; //no hace falta usar div, porque cantPaginas se queda con la parte entera por ser int
	if (((segmentoEnSwap+segmentoEnMP)%256) > 0){ cantPaginas++;}
	int numeroPag=0;
	while(cantPaginas>0){
		t_lista_paginas *pagina= malloc(sizeof(t_lista_paginas));
		(*pagina).swap=0;
		(*pagina).marcoEnMemPpal=0;
		(*pagina).numeroPagina=numeroPag;
		list_add( (*nuevoSegmento).lista_Paginas , pagina);
		numeroPag++;
		cantPaginas=cantPaginas-1;
	}
    //6. Carga paginas en memoria principal si es necesario
	 if(segmentoEnMP>0){
		 int cantPagCargar=segmentoEnMP/256;
		 if ((segmentoEnMP%256) > 0){ cantPagCargar++;}
		 while(cantPagCargar>0){
			 	//carga paginas en MP funcion a definir cuando se haga el EscribirMemoria y algoritmos de reemplazo
			 cantPagCargar=cantPagCargar-1;
		 }
	 }
	//TODO hacer los FREE()
	//TODO Ver como imprimir direccion base del segmento
	return direccionBaseDelSegmento;
}

