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
#include <ansisop-panel/panel.h>

pthread_mutex_t mutex_log = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_consola = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {

	// Control de argumentos
	if(!(path_config = argv[1])) {
		path_config = "config.cfg";	// default path, en caso de no ingresarlo
	}
	//Defino path para poner los archivos del swap (path_swap)
	if (!(path_swap = argv[2])) {
		path_swap = "/home/utnso/archivos_swap/";	// default path, en caso de no ingresarlo
	}

	//1. Crear archivo de log, Leer archivo de configuracion e Iniciar semaforos
	crear_logger();
	leer_config(path_config);
	inicializar_semaforos();

	//2. Reservar bloque de memoria principal
	memoria_ppal = reservarBloquePpal(tamanio_mem_ppal);
	printf("pase la mem ppal \n");	//DEBUG

	//3. Generar estructuras administrativas
	lista_marcos = dividirMemoriaEnMarcos(memoria_ppal, tamanio_mem_ppal);
	printf("pase division de marcos\n");	//DEBUG
	memoriaPpalActual = tamanio_mem_ppal;
	memoriaSwapActual = cant_mem_swap;
	listaProcesos = list_create();

	//TODO log lista de procesos, tabla de paginas y tabla de segmentos

	//4. Abrir conexiones con Kernel y CPU, y levantar Consola MSP

	//Se crea el hilo para la consola
/*		pthread_create(&consola, NULL, inciarConsola, NULL); //TODO pasar info de la memoria a la consola (4 parametro diferente a null)

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
		(*informacion_memoria).memoriaPpalActual=memoriaPpalActual;
		(*informacion_memoria).memoriaSwapActual=memoriaSwapActual;

		t_conexion_entrante *nuevaConexion = malloc(sizeof(t_conexion_entrante)); //Informacion para el hilo de conexion
		(*nuevaConexion).hiloDeConexion=hiloDeConexion;
		(*nuevaConexion).informacion_memoria=informacion_memoria;
		(*nuevaConexion).socket=nuevoSocket;

		*nuevoSocket=socket_aceptarCliente(socketServidorMSP);

		pthread_create(hiloDeConexion,NULL,inciarConsola,(void *) nuevaConexion); //TODO donde dice inciarConsola va una funcion que maneje el pedido de la conexion
		pthread_detach(*hiloDeConexion);
	}*/

		//Espera que se termine el hilo consola
		pthread_join(consola, NULL);

		//TODO Liberar recursos

	return EXIT_SUCCESS;
}

int asignarNumeroSegmento(int tamanioListaSegmentos, t_list *listaSegmentos){
	t_lista_segmentos *segmento = malloc(sizeof(t_lista_segmentos));

	bool mismoSegmento(t_lista_segmentos *numeroSegmento){
				return numeroSegmento->numeroSegmento==tamanioListaSegmentos;
			}

	while (list_any_satisfy(listaSegmentos, (void*) (*mismoSegmento)) == true){
		tamanioListaSegmentos=tamanioListaSegmentos+1;
	}
	free(segmento);
	return tamanioListaSegmentos;
}

uint32_t crearSegmento(uint32_t PID, uint32_t tamanio_segmento){

	uint32_t direccionBaseDelSegmento;

	//funcion local para fijarse el PID
	bool mismoPID(t_lista_procesos *PIDEncontrado){
			return PIDEncontrado->pid==PID;
		}

	//1.Verifica si hay memoria disponible
	int cant_mem_actual=memoriaPpalActual+memoriaSwapActual*256; //AGREGUE *256 PORQUE ME PARECIA MEJOR QUE SOLO DEJAR LA CANTIDAD DE ARCH SWAP

	if (tamanio_segmento > 1048576){
		pthread_mutex_lock(&mutex_log);
		printf("Error el tamaño de segmento pedido es mayor a lo soportado (maximo 1048576 bytes).");
		//log_error(logger,"Error el tamaño de segmento pedido es mayor a lo soportado (maximo 1048576 bytes).");
		pthread_mutex_unlock(&mutex_log);
		return 0;
	}

	if(tamanio_segmento > cant_mem_actual){
		pthread_mutex_lock(&mutex_log);
		printf("Error Memoria llena");
		//log_error(logger,"Error Memoria Llena");
		pthread_mutex_unlock(&mutex_log);
		return 0;
	}
	//2.Se fija si se existe el proceso

	t_lista_procesos* proceso;
	proceso = list_find(listaProcesos,(void*) (*mismoPID));

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
		proceso = malloc(sizeof(t_lista_procesos));
		proceso->pid=PID;
		(*proceso).lista_Segmentos=list_create();
		list_add(listaProcesos,proceso);}

	int tamanioListaSeg=list_size((*proceso).lista_Segmentos);
	t_lista_segmentos *nuevoSegmento = malloc(sizeof(t_lista_segmentos));
	if(tamanioListaSeg<4096){
		(*nuevoSegmento).lista_Paginas=list_create();
		(*nuevoSegmento).numeroSegmento=asignarNumeroSegmento(tamanioListaSeg,(*proceso).lista_Segmentos);
		(*nuevoSegmento).tamanio=segmentoEnSwap+segmentoEnMP;
		pthread_mutex_lock(&mutex_log);
		list_add((*proceso).lista_Segmentos, nuevoSegmento);
		//log_info(logger,"Se creo el nuevo segmento del proceso: %d y tiene el tamaño: %d",PID,segmentoEnSwap+segmentoEnMP);
		printf("Se creo el nuevo segmento del proceso: %d y tiene el tamaño: %d\n", PID, segmentoEnSwap+segmentoEnMP);
		pthread_mutex_unlock(&mutex_log);
	} else {
		pthread_mutex_lock(&mutex_log);
		//log_error(logger,"Se supera el maximo de segmentos por programa");
		printf("Se supera el maximo de segmentos por programa");
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
  direccionBaseDelSegmento = crearDireccion((*nuevoSegmento).numeroSegmento,0);

	//TODO Ver como imprimir direccion base del segmento
  	//FIXME no entiendo, tiene que devolver la direccion base del segmento o la direccion formada por seg, pag y desplazamiento? Es lo mismo?
	return direccionBaseDelSegmento ;
}

void destruirSegmento(uint32_t PID, uint32_t direccBase){
	//1. Traduce direccion base
	t_direccion direccionTraducida = traducirDireccion(direccBase);
	printf("Me llga de direcc traducida segmento %d, pagina %d y desplazamiento %d\n", direccionTraducida.segmento, direccionTraducida.pagina, direccionTraducida.desplazamiento);
	//2. Se fija si la dirrecion base es correcta
	bool mismoPID(t_lista_procesos *PIDEncontrado){
				return PIDEncontrado->pid==PID;
			}
	bool mismoSegmento(t_lista_segmentos *numeroSegmento){
					return numeroSegmento->numeroSegmento==(direccionTraducida).segmento;
				}

	void liberarMemoria(t_lista_paginas *unaPagina){
		if((*unaPagina).swap==0){
			uint32_t numeroDeMarco = (*unaPagina).marcoEnMemPpal;
			bool mismoMarco(t_marco *marco){
									return marco->numeroMarco==numeroDeMarco;
								}
		t_marco *marcoEnMemoria=list_find(lista_marcos,(void*) (*mismoMarco));
		(*marcoEnMemoria).marco_libre=true;
		free((*marcoEnMemoria).memoria);
		}
		else {
		//1. Destruir el archivo
		destruir_archivo_swap(PID, (direccionTraducida).segmento, (*unaPagina).numeroPagina);
	}
	free(unaPagina);
	}

	void liberarSegmento(t_lista_segmentos *segmento){
		free(segmento);
	}

	t_lista_procesos *proceso = malloc(sizeof(t_lista_procesos));
	proceso = list_find(listaProcesos, (void*) (*mismoPID));
	if(proceso != NULL){
		//3. Ingresa la tabla de segmentos del proceso PID y con la traduccion entra al segmento pedido
		t_lista_segmentos *segmento = malloc(sizeof(t_lista_segmentos));
		segmento=list_find((*proceso).lista_Segmentos, (void*) (*mismoSegmento));
		if(segmento != NULL){
			//4. Libera la memoria y elimina la entrada en la tabla de segmentos y su respectiva tabla de paginas
			list_destroy_and_destroy_elements((*segmento).lista_Paginas, (void*) (*liberarMemoria));
			list_remove_and_destroy_by_condition((*proceso).lista_Segmentos,(void*) (*mismoSegmento), (void*) (*liberarSegmento));
			free(segmento);
			//TODO Faltan logs de eleminar segmento
		} else {
			pthread_mutex_lock(&mutex_log);
			//log_error(logger, "El PID: %u, no contiene el numero de segmento: %i por lo tanto no se puede borrar", PID, (direccionTraducida).segmento);
			printf("El PID: %u, no contiene el numero de segmento: %i por lo tanto no se puede borrar", PID, (direccionTraducida).segmento);
			pthread_mutex_unlock(&mutex_log);
		}
	} else {
		pthread_mutex_lock(&mutex_log);
		//log_error(logger, "No existe el PID: %u en el sistema", PID);
		printf("No existe el PID: %u en el sistema", PID);
		pthread_mutex_unlock(&mutex_log);
	}

free(proceso);
}

void escribirMemoria(uint32_t PID, uint32_t direcc_log, void* bytes_escribir, uint32_t tamanio){ //PUSE EL BUFFER EN VOID. A LO SUMO ES CHAR*
	//1. traducir direccion y validarla

	t_direccion direccion = traducirDireccion(direcc_log);

	//2.fijarse si la pagina solicitada esta en memoria si no cargarla(haciendo swap etc)
		//2.1 en caso de necesitar swap fijarse y la lista de marcos esta llena en ese caso con LRU o CLOCK elegir la pagina a reemplazar
	bool mismoPID(t_lista_procesos *PIDEncontrado){
					return PIDEncontrado->pid==PID;
				}
	bool mismoSegmento(t_lista_segmentos *numeroSegmento){
						return numeroSegmento->numeroSegmento==direccion.segmento;
				}
	bool mismaPagina(t_lista_paginas *numeroPagina){
							return numeroPagina->numeroPagina==direccion.pagina;
				}
	bool funcionMarcoLibre(t_marco *marco){
		return marco->marco_libre == 1;
	}

	t_lista_procesos* proceso = malloc(sizeof(t_lista_procesos));
	proceso = list_find(listaProcesos,(void*) (*mismoPID));

	if(proceso != NULL){
		t_lista_segmentos *segmento = malloc(sizeof(t_lista_segmentos));
		segmento = list_find(proceso->lista_Segmentos, (void*) (*mismoSegmento));
		if(segmento != NULL){
			t_lista_paginas* pagina = malloc(sizeof(t_lista_paginas));
			pagina = list_find(segmento->lista_Paginas, (void*) (*mismaPagina));
			if(pagina != NULL){
				if(pagina->swap == 0){ //Esta en memoria principal
					bool mismoMarco(t_marco *marco){
						return marco->numeroMarco == (*pagina).marcoEnMemPpal;
					}
					t_marco *marco = malloc(sizeof(t_marco));
					marco=list_find(lista_marcos,(void*) (*mismoMarco)); //Busca el marco de memoria ppal

					guardarInformacion(marco,direccion,bytes_escribir,tamanio);

				} else {//Traemos pagina a memoria ppal
					t_marco *marcoLibre = malloc(sizeof(t_marco));
					marcoLibre= list_find(lista_marcos,(void*) (*funcionMarcoLibre));

					if(marcoLibre!=NULL){//Hay un marco libre en memoria principal donde cargar la pagina
						t_pagina *pagina = malloc(sizeof(t_pagina));
						*pagina= swap_in(PID, direccion.segmento, direccion.pagina);
						memcpy(marcoLibre, pagina->codigo, pagina->tamanio_buffer); //FIXME el tamaño buffer es el tamaño de lo que tiene el archivo adentro ?
						guardarInformacion(marcoLibre,direccion,bytes_escribir,tamanio);
						marcoLibre->marco_libre = 0;
					} else { //TODO Fijarse que algoritmo de reemplazo tiene el arch de config y hacer swap

					}
				}
			} else {
				page_not_found_exception(direccion.pagina);
			}
		} else {
			segment_not_found_exception(direccion.segmento);
		}
	} else {
	PID_not_found_exception(PID);}
	//3.una vez cargada la pagina, escribir donde corresponda los bytes a escribir
}
//FIXME hacer los free



//FIXME!!! HICE UN COPY PASTE RARO DE ESCRIBIR MEMORIA PORQUE ME PARECIO QUE LA LOGICA ES SIMILAR
//CAMBIEN TODOO LO QUE CONSIDEREN NECESARIO, POR SUPUESTO
char* solicitar_memoria(uint32_t PID, uint32_t direcc_log, uint32_t tamanio){
	char* bytes_solicitados;

	t_direccion direccion = traducirDireccion(direcc_log);
	return NULL;//FIXME

	bool mismoPID(t_lista_procesos *PIDEncontrado){
			return PIDEncontrado->pid==PID;
		}
	bool mismoSegmento(t_lista_segmentos *numeroSegmento){
			return numeroSegmento->numeroSegmento==direccion.segmento;
		}
	bool mismaPagina(t_lista_paginas *numeroPagina){
			return numeroPagina->numeroPagina==direccion.pagina;
		}
	bool funcionMarcoLibre(t_marco *marco){
			return marco->marco_libre == 1;
		}

	t_lista_procesos* proceso = malloc(sizeof(t_lista_procesos));
	proceso = list_find(listaProcesos,(void*) (*mismoPID));

	if(proceso != NULL){
			t_lista_segmentos *segmento = malloc(sizeof(t_lista_segmentos));
			segmento = list_find(proceso->lista_Segmentos, (void*) (*mismoSegmento));
			if(segmento != NULL){
				t_lista_paginas* pagina = malloc(sizeof(t_lista_paginas));
				pagina = list_find(segmento->lista_Paginas, (void*) (*mismaPagina));
				if(pagina != NULL){
					if(pagina->swap == 0){ //Esta en memoria principal

						bool mismoMarco(t_marco *marco){
							return marco->numeroMarco == (*pagina).marcoEnMemPpal;
						}

						t_marco *marco = malloc(sizeof(t_marco));
						marco=list_find(lista_marcos,(void*) (*mismoMarco)); //Busca el marco de memoria ppal

						bytes_solicitados = devolverInformacion(marco, direccion, tamanio);
						return bytes_solicitados;
						//TODO: cuando se terminen de usar los bytes solicitados, AFUERA de esta funcion, liberar memoria.

					} else {//Traemos pagina a memoria ppal
						t_marco *marcoLibre = malloc(sizeof(t_marco));
						marcoLibre= list_find(lista_marcos,(void*) (*funcionMarcoLibre));

						if(marcoLibre!=NULL){//Hay un marco libre en memoria principal donde cargar la pagina
							t_pagina *pagina = malloc(sizeof(t_pagina));
							*pagina= swap_in(PID, direccion.segmento, direccion.pagina);
							memcpy(marcoLibre, pagina->codigo, pagina->tamanio_buffer);

							bytes_solicitados = devolverInformacion(marcoLibre, direccion, tamanio);

							marcoLibre->marco_libre = 0;
							return bytes_solicitados;
						} else { //TODO Fijarse que algoritmo de reemplazo tiene el arch de config y hacer swap

						}
					}
				} else {
					page_not_found_exception(direccion.pagina);
					return NULL;
				}
			} else {
				segment_not_found_exception(direccion.segmento);
				return NULL;
			}
		} else {
		PID_not_found_exception(PID);
		return NULL;}

}

