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

	//Variables para Sockets
	int socketServidorMSP;
	bool escuchandoConexiones = true;



int main(int argc, char *argv[]) {

	//1.Leer archivo de configuracion y archivo de log
	crear_logger(logMSP);
	leerConfiguracion(archConfigMSP, argv[1]);


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

uint32_t crearSegmento(int PID, int tamanio_segmento){
	//1.Verifica si hay memoria disponible
	int cant_mem_actual=memoriaPpalActual+memoriaSwapActual;
	if(tamanio_segmento > cant_mem_actual){
		printf("Error Memoria llena");
		log_error(logMSP,"Error Memoria Llena");
		return 0;
	}
	//2.Se fija si se existe el proceso

	t_lista_procesos *proceso = malloc(sizeof(t_lista_procesos));
	proceso = list_find(listaProcesos, (*proceso).pid == PID); //FIXME el segundo parametro no sabemos como ponerlo.
	//3.Crea o agrega nuevo segmento
	if(proceso==NULL){
		(*proceso).pid=PID;
		(*proceso).lista_Segmentos=list_create();
		if(list_size(listaProcesos)<4096){
		list_add(listaProcesos,proceso);
		log_info(logMSP,"Se creo el nuevo segmento del proceso: %d",PID);}
		else { log_error(logMSP,"Supera El Maximo de segmentos por programa");}
	}
	int tamanioListaSeg=list_size((*proceso).lista_Segmentos);

	t_lista_segmentos *nuevoSegmento = malloc(sizeof(t_lista_segmentos));
	(*nuevoSegmento).lista_Paginas=list_create();
	(*nuevoSegmento).numeroSegmento=tamanioListaSeg;
	(*nuevoSegmento).tamanio=tamanio_segmento;
	list_add((*proceso).lista_Segmentos, nuevoSegmento);
	//4.Crea tabla de paginas
	int cantPaginas=div(tamanio_segmento,256);
	if ((tamanio_segmento%256) > 0){ cantPaginas++;}
	int numeroPag=0;
	while(cantPaginas>0){
		t_lista_paginas *pagina= malloc(sizeof(t_lista_paginas));
		(*pagina).swap=0;
		(*pagina).marcoEnMemPpal=0;
		(*pagina).numeroPagina=numeroPag;
		list_add((*nuevoSegmento).lista_Paginas, pagina);
		numeroPag++;
		cantPaginas=cantPaginas-1;
	}

	//TODO hacer los FREE()
	//TODO Ver como imprimir direccion base del segmento
	return direccionBaseDelSegmento;
}

