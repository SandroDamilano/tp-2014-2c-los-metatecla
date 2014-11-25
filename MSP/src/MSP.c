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
pthread_mutex_t mutex_reservarMemoria = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_crearSegmento = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_tablaMarcos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_swap = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_memoriaPpal = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {

	// Control de argumentos
	if(!(path_config = argv[1])) {
		path_config = "config.cfg";	// default path, en caso de no ingresarlo
	}
	//Defino path para poner los archivos del swap (path_swap)
	if (!(path_swap = argv[2])) {
		path_swap = "/home/utnso/archivos_swap/";	// default path, en caso de no ingresarlo
	}

	//Liberar Memoria
	void liberarPagina(t_lista_paginas *pagina){
				free(pagina);
			}

	void liberarSegmento(t_lista_segmentos *segmento){
		list_iterate((*segmento).lista_Paginas, (void*) (*liberarPagina));
			free(segmento);
		}

	void liberarProceso(t_lista_procesos *proceso){
		if(!list_is_empty((*proceso).lista_Segmentos))	{
		list_iterate((*proceso).lista_Segmentos, (void*) (*liberarSegmento));}
		free(proceso);
	}

	//1. Crear archivo de log, Leer archivo de configuracion e Iniciar semaforos
	crear_logger();
	leer_config(path_config);
	pthread_mutex_lock(&mutex_log);
	log_info(logger, "Tamaño Memoria Principal: %u Kb, Tamaño memoria Swap: %i Mb, Algoritmo de Sustitucion: %s\n",tamanio_mem_ppal, cant_mem_swap, alg_sustitucion);
	pthread_mutex_unlock(&mutex_log);

	inicializar_semaforos();

	//2. Reservar bloque de memoria principal
	memoria_ppal = reservarBloquePpal(tamanio_mem_ppal);
	cant_marcos = calcularMarcos(tamanio_mem_ppal);
	pthread_mutex_lock(&mutex_log);
	log_debug(logger,"La cantidad de marcos es: %i\n", cant_marcos);
	pthread_mutex_unlock(&mutex_log);
	//3. Generar estructuras administrativas
	tabla_marcos = crearTablaDeMarcos();
	paginasMemoriaPpalActual = tamanio_mem_ppal*1024/256;
	paginasMemoriaSwapActual = cant_mem_swap*1024*1024/256 ;
	listaProcesos = list_create();

	/*********************************************************/
	/*FILE* beso = fopen("/home/utnso/out.bc", "r");

				fseek(beso, 0L, SEEK_END); //Averiguo tamaño del archivo
				long tamanio_archivo = ftell(beso);
				fseek(beso, 0L, SEEK_SET);

	crearSegmento(1, tamanio_archivo);

				char buffer[tamanio_archivo];//Copio el contenido del archivo al buffer

				while(!feof(beso)){
				fread(buffer, 1, sizeof(buffer), beso);
				}

				buffer[tamanio_archivo]= '\0';
	escribirMemoria(1,0,buffer, tamanio_archivo);*/

	/*********************************************************/

	//TODO log lista de procesos, tabla de paginas y tabla de segmentos
	//4. Abrir conexiones con Kernel y CPU, y levantar Consola MSP

	//Se crea el hilo para la consola
		pthread_create(&consola, NULL, inciarConsola, NULL);

		//TODO LOG Se abren conexiones por sockets para Kernel y CPU

		//Se crea un hilo que va a manejar las conexiones
		pthread_t atender_conexiones;
		pthread_create(&atender_conexiones, NULL, (void*) &handler_conexiones, NULL);

		//Espera que se termine el hilo consola
		pthread_join(consola, NULL);
		//pthread_join(atender_conexiones, NULL); TODO sacar de comentario

		//Termina MSP -> Liberar recursos
		list_destroy_and_destroy_elements(listaProcesos,(void*) (*liberarProceso));
		free(memoria_ppal);
		free(tabla_marcos);

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

//PUSE QUE SI ES ERROR, DEVUELVA -1 QUE ES MAS CLARO QUE 0
uint32_t crearSegmento(uint32_t PID, uint32_t tamanio_segmento){

	uint32_t direccionBaseDelSegmento;

	//funcion local para fijarse el PID
	bool mismoPID(t_lista_procesos *PIDEncontrado){
			return PIDEncontrado->pid==PID;
		}

	//1.Verifica si hay memoria disponible
	int cant_mem_actual=paginasMemoriaPpalActual+paginasMemoriaSwapActual;
	if (tamanio_segmento > 1048576){
		pthread_mutex_lock(&mutex_log);
		log_error(logger,"Error el tamaño de segmento pedido es mayor a lo soportado (maximo 1048576 bytes).");
		pthread_mutex_unlock(&mutex_log);
		return -1;
	}

	if(tamanio_segmento > cant_mem_actual){
		pthread_mutex_lock(&mutex_log);
		log_error(logger,"Error Memoria Llena");
		pthread_mutex_unlock(&mutex_log);
		return -1;
	}
	//2.Se fija si se existe el proceso

	t_lista_procesos* proceso;
	proceso = list_find(listaProcesos,(void*) (*mismoPID));

	//3. Se fija donde crear el segmento
	int paginasSegmentoEnSwap = 0;
	int paginasSegmentoEnMP = 0;
	uint32_t cantPaginasNuevoSeg=0;
	cantPaginasNuevoSeg=tamanio_segmento/256;

	pthread_mutex_lock(&mutex_reservarMemoria);
	if((tamanio_segmento%256)>0){cantPaginasNuevoSeg++;};
	if(cantPaginasNuevoSeg<=paginasMemoriaSwapActual){
		paginasSegmentoEnSwap=cantPaginasNuevoSeg;
		paginasMemoriaSwapActual -= cantPaginasNuevoSeg;
	} else { if(paginasMemoriaSwapActual>0){
				paginasSegmentoEnSwap = paginasMemoriaSwapActual;
				cantPaginasNuevoSeg -= paginasMemoriaSwapActual;
				paginasMemoriaSwapActual = 0;
				paginasSegmentoEnMP=cantPaginasNuevoSeg;
				paginasMemoriaPpalActual -= cantPaginasNuevoSeg;
	} else { paginasSegmentoEnMP=cantPaginasNuevoSeg;
			 paginasMemoriaPpalActual -= cantPaginasNuevoSeg;}
	}
	pthread_mutex_unlock(&mutex_reservarMemoria);

	pthread_mutex_lock(&mutex_log);
	if(paginasMemoriaSwapActual == 0){
	log_info(logger,"La memoria Swap se encuentra Llena");}
	if(paginasMemoriaPpalActual == 0){
		log_info(logger,"La memoria Principal se encuentra Llena");}
	pthread_mutex_unlock(&mutex_log);

	//4.Crea lista de segmentos o agrega nuevo segmento
	if(proceso==NULL){
		proceso = malloc(sizeof(t_lista_procesos));
		proceso->pid=PID;
		(*proceso).lista_Segmentos=list_create();
		list_add(listaProcesos,proceso);
		}
	pthread_mutex_lock(&mutex_crearSegmento);
	int tamanioListaSeg=list_size((*proceso).lista_Segmentos);
	t_lista_segmentos *nuevoSegmento = malloc(sizeof(t_lista_segmentos));
	if(tamanioListaSeg<4096){
		(*nuevoSegmento).lista_Paginas=list_create();
		(*nuevoSegmento).numeroSegmento=asignarNumeroSegmento(tamanioListaSeg,(*proceso).lista_Segmentos);
		(*nuevoSegmento).tamanio=tamanio_segmento;
		list_add((*proceso).lista_Segmentos, nuevoSegmento);
		pthread_mutex_lock(&mutex_log);
		log_info(logger,"Se creo el nuevo segmento del proceso: %d y tiene el tamaño: %d",PID,tamanio_segmento);
		pthread_mutex_unlock(&mutex_log);
	} else {
		pthread_mutex_lock(&mutex_log);
		log_error(logger,"Se supera el maximo de segmentos por programa");
		pthread_mutex_unlock(&mutex_log);
		return -1;
	}

	//5.Crea tabla de paginas
	int cantPaginas= (paginasSegmentoEnSwap+paginasSegmentoEnMP);
	int numeroPag=0;
	while(cantPaginas>0){
		t_lista_paginas *pagina= malloc(sizeof(t_lista_paginas));
		(*pagina).swap=1;
		(*pagina).marcoEnMemPpal=0;
		(*pagina).numeroPagina=numeroPag;
		list_add( (*nuevoSegmento).lista_Paginas , pagina);
		numeroPag++;
		cantPaginas=cantPaginas-1;
	}
	pthread_mutex_unlock(&mutex_crearSegmento);

	//6. Carga paginas en memoria principal si es necesario

	if(paginasSegmentoEnMP>0){
		 uint32_t cantPagCargar=paginasSegmentoEnMP;
		 pthread_mutex_lock(&mutex_tablaMarcos);
		 while(cantPagCargar>0){
			 bool mismaPagina(t_lista_paginas *pagina){
			 			return pagina->numeroPagina==cantPagCargar-1;//AGREGUE ESTE -1
			 }

			uint32_t marcoLibre = buscarMarcoLibre(tabla_marcos);
			pthread_mutex_lock(&mutex_log);
			log_info(logger,"Al marco %i, se asigno la pagina %i del segmento %i del PID %i \n",marcoLibre, cantPagCargar,(*nuevoSegmento).numeroSegmento, PID);
			pthread_mutex_unlock(&mutex_log);
			tabla_marcos[marcoLibre].marco_libre=0;
			tabla_marcos[marcoLibre].pid=PID;
			tabla_marcos[marcoLibre].segmento=(*nuevoSegmento).numeroSegmento;
			tabla_marcos[marcoLibre].pagina=cantPagCargar-1;
			t_lista_paginas *paginaACargar;//=malloc(sizeof(t_lista_paginas));
			paginaACargar=list_find((*nuevoSegmento).lista_Paginas,(void*) (*mismaPagina));
			(*paginaACargar).marcoEnMemPpal= marcoLibre;
			(*paginaACargar).swap=0;
			cantPagCargar=cantPagCargar-1;
		 }
		 pthread_mutex_lock(&mutex_tablaMarcos);
		 }
  direccionBaseDelSegmento = crearDireccion((*nuevoSegmento).numeroSegmento,0,0);
	return direccionBaseDelSegmento ;
}


uint32_t destruirSegmento(uint32_t PID, uint32_t direccBase){
	//1. Traduce direccion base
	t_direccion direccionTraducida = traducirDireccion(direccBase);
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
			memset(memoria_ppal+(numeroDeMarco*256),0,256);
			tabla_marcos[numeroDeMarco].marco_libre=1;
			tabla_marcos[numeroDeMarco].bitAlgoritmo=0;
			pthread_mutex_lock(&mutex_log);
			log_info(logger,"El marco %i, ahora se encuentra libre \n",numeroDeMarco);
			pthread_mutex_unlock(&mutex_log);}
		else {
		//1. Destruir el archivo
		destruir_archivo_swap(PID, (direccionTraducida).segmento, (*unaPagina).numeroPagina);
	}
	}

	void liberarSegmento(t_lista_segmentos *segmento){
		list_iterate((*segmento).lista_Paginas, (void*) (*liberarMemoria));
		free(segmento);
	}

	t_lista_procesos *proceso ; //= malloc(sizeof(t_lista_procesos));
	proceso = list_find(listaProcesos, (void*) (*mismoPID));
	if(proceso != NULL){
		//3. Ingresa la tabla de segmentos del proceso PID y con la traduccion entra al segmento pedido
		t_lista_segmentos *segmento; // = malloc(sizeof(t_lista_segmentos));
		segmento=list_find((*proceso).lista_Segmentos, (void*) (*mismoSegmento));
		if(segmento != NULL){
			//4. Libera la memoria y elimina la entrada en la tabla de segmentos y su respectiva tabla de paginas
			pthread_mutex_lock(&mutex_tablaMarcos);
			pthread_mutex_lock(&mutex_log);
			log_info(logger,"Se destruyo el segmento %i del proceso: %d",segmento->numeroSegmento,PID);
			pthread_mutex_unlock(&mutex_log);
			list_remove_and_destroy_by_condition((*proceso).lista_Segmentos,(void*) (*mismoSegmento), (void*) (*liberarSegmento));
			pthread_mutex_unlock(&mutex_tablaMarcos);

			return 0;
		} else {
			pthread_mutex_lock(&mutex_log);
			log_error(logger, "El PID: %u, no contiene el numero de segmento: %i por lo tanto no se puede borrar", PID, (direccionTraducida).segmento);
			pthread_mutex_unlock(&mutex_log);
			return -1;
		}
	} else {
		pthread_mutex_lock(&mutex_log);
		log_error(logger, "No existe el PID: %u en el sistema", PID);
		pthread_mutex_unlock(&mutex_log);
		return -1;
	}

}

//PUSE QUE DEVUELVA -1 SI NO SE PUDO ESCRIBIR, Y 0 SI PUDO. ES PARA VALIDACIONES.
int escribirMemoria(uint32_t PID, uint32_t direcc_log, void* bytes_escribir, uint32_t tamanio){ //PUSE EL BUFFER EN VOID. A LO SUMO ES CHAR*
	//1. traducir direccion y validarla

	t_direccion direccion = traducirDireccion(direcc_log);
	printf("direccion: %i, %i, %i\n", direccion.desplazamiento,direccion.pagina,direccion.segmento);
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

	t_lista_procesos* proceso;// = malloc(sizeof(t_lista_procesos));
	proceso = list_find(listaProcesos,(void*) (*mismoPID));


	if(proceso != NULL){
		t_lista_segmentos *segmento /*= malloc(sizeof(t_lista_segmentos))*/;
		segmento = list_find(proceso->lista_Segmentos, (void*) (*mismoSegmento));
		if(segmento != NULL){
			if((direccion.pagina*256+direccion.desplazamiento+tamanio)<=segmento->tamanio){
				while(tamanio>0){
					t_lista_paginas* pagina /*= malloc(sizeof(t_lista_paginas))*/;
					pagina = list_find(segmento->lista_Paginas, (void*) (*mismaPagina));
					if(pagina != NULL){
						pthread_mutex_lock(&mutex_swap);
						if(pagina->swap==1){ //NO ESTÁ EN MEMORIA
							pthread_mutex_lock(&mutex_tablaMarcos);
							uint32_t numeroDeMarcoLibre= buscarMarcoLibre(tabla_marcos);
								if(numeroDeMarcoLibre!=-1){//Hay un marco libre en memoria principal donde cargar la pagina
									pthread_mutex_lock(&mutex_log);
									log_info(logger,"Al marco %i, se asigno la pagina %i del segmento %i del PID %i \n",numeroDeMarcoLibre, pagina->numeroPagina,segmento->numeroSegmento, PID);
									pthread_mutex_unlock(&mutex_log);
									t_pagina *paginaACargar = malloc(sizeof(t_pagina));
									*paginaACargar= extraerInfoDeArchSwap(PID, direccion.segmento, direccion.pagina);
									t_direccion direccion_base; // Cargo el contenido del archivo en la direccion base: n° de segmento y pagina que corresponde, pero desplazamiento 0
									direccion_base.segmento = segmento->numeroSegmento;
									direccion_base.pagina = pagina->numeroPagina;
									direccion_base.desplazamiento = 0;
									guardarInformacion(memoria_ppal+(numeroDeMarcoLibre*256),direccion_base,paginaACargar->codigo,paginaACargar->tamanio_buffer);
									pagina->marcoEnMemPpal=numeroDeMarcoLibre;
									pagina->swap=0;
									tabla_marcos[numeroDeMarcoLibre].marco_libre = 0;
									tabla_marcos[numeroDeMarcoLibre].pagina=pagina->numeroPagina;
									tabla_marcos[numeroDeMarcoLibre].segmento=segmento->numeroSegmento;
									tabla_marcos[numeroDeMarcoLibre].pid=PID;
									free(paginaACargar);
								}else{
									uint32_t numeroMarcoASwapear;
									if(string_equals_ignore_case(alg_sustitucion,"LRU")){
									numeroMarcoASwapear = buscarPaginaMenosUsada();
									} else { numeroMarcoASwapear = algoritmo_clock();}
									bool paginaASacar(t_lista_paginas *numeroPagina){
										return numeroPagina->numeroPagina==tabla_marcos[numeroMarcoASwapear].pagina;
										}
									bool segmentoASacar(t_lista_segmentos *numeroSegmento){
										return numeroSegmento->numeroSegmento==tabla_marcos[numeroMarcoASwapear].segmento;
										}
									bool pidASacar(t_lista_procesos *PIDEncontrado){
														return PIDEncontrado->pid==tabla_marcos[numeroMarcoASwapear].pid;
													}
									printf("Numero de marco a swapear: %i \n", numeroMarcoASwapear);
									t_lista_procesos *procesoDePaginaASacar;
									t_lista_paginas *paginaSacada;
									t_lista_segmentos *segmentoDePaginaASacar;
									procesoDePaginaASacar = list_find(listaProcesos,(void*) (*pidASacar));
									segmentoDePaginaASacar = list_find(procesoDePaginaASacar->lista_Segmentos, (void*) (*segmentoASacar));
									paginaSacada = list_find(segmentoDePaginaASacar->lista_Paginas, (void*) (*paginaASacar));
									printf("Pagina a sacar: %i\n", paginaSacada->numeroPagina);
									printf("Pagina en swap: %i\n", paginaSacada->swap);
									paginaSacada->swap=1;
									printf("Pagina en swap: %i\n", paginaSacada->swap);
									paginaSacada->marcoEnMemPpal=-1;
									hacerSwap(PID,direccion,pagina,segmento, numeroMarcoASwapear);
									pthread_mutex_lock(&mutex_log);
									log_info(logger,"Se hizo swap en el marco: %i.\n La pagina que salio de memoria principal fue: %i del segmento %i del PID %i \n La pagina que entro en memoria principal fue %i del segmento %i del PID %i \n",numeroMarcoASwapear, paginaSacada->numeroPagina,segmentoDePaginaASacar->numeroSegmento, procesoDePaginaASacar->pid,pagina->numeroPagina,segmento->numeroSegmento,PID);
									pthread_mutex_unlock(&mutex_log);
									}
						pthread_mutex_unlock(&mutex_tablaMarcos);
						}
						modificarBitAlgoritmo(pagina->marcoEnMemPpal);
						pthread_mutex_unlock(&mutex_swap);
						pthread_mutex_lock(&mutex_memoriaPpal);
						if((256-direccion.desplazamiento)>=tamanio){
							printf("Bytes a escribir: %s tamanio: %i \n", bytes_escribir, tamanio);
							guardarInformacion(memoria_ppal+((pagina->marcoEnMemPpal)*256),direccion,bytes_escribir,tamanio);
							tamanio=0;
						} else {
							uint32_t espacioLibre = 256-direccion.desplazamiento;
						 guardarInformacion(memoria_ppal+((pagina->marcoEnMemPpal)*256),direccion,bytes_escribir,espacioLibre);
						 //bytes_escribir=string_substring(bytes_escribir,espacioLibre, tamanio-espacioLibre);
						 void* buffer = malloc(tamanio-espacioLibre);
						 memcpy(buffer, bytes_escribir + espacioLibre, tamanio-espacioLibre);
						 memset(bytes_escribir,0,tamanio);
						 memcpy(bytes_escribir, buffer, tamanio-espacioLibre);
						 free(buffer);
						 direccion.pagina=direccion.pagina+1;
						 direccion.desplazamiento=0;
						 tamanio=tamanio-espacioLibre;
						}
					pthread_mutex_unlock(&mutex_memoriaPpal);
					printf("Libera semaforo \n");
					} else {
						page_not_found_exception(direccion.pagina);
						return -1;}
								}
			} else {segmentation_fault();}
		} else {segment_not_found_exception(direccion.segmento);
				return -1;}
		} else {
	PID_not_found_exception(PID);
	return -1;}

}

void* solicitar_memoria(uint32_t PID, uint32_t direcc_log, uint32_t tamanio){
	void* respuesta = malloc(tamanio);
	memset(respuesta,0,tamanio);
	void* buff = malloc(tamanio);
	memset(buff,0,tamanio);
	int memoriaGuardada = 0;

	t_direccion direccion = traducirDireccion(direcc_log);
	printf("direccion: %i, %i, %i\n", direccion.desplazamiento,direccion.pagina,direccion.segmento);

	bool mismoPID(t_lista_procesos *PIDEncontrado){
			return PIDEncontrado->pid==PID;
		}
	bool mismoSegmento(t_lista_segmentos *numeroSegmento){
			return numeroSegmento->numeroSegmento==direccion.segmento;
		}
	bool mismaPagina(t_lista_paginas *numeroPagina){
			return numeroPagina->numeroPagina==direccion.pagina;
		}

	t_lista_procesos* proceso;
	proceso = list_find(listaProcesos,(void*) (*mismoPID));

	if(proceso != NULL){
		t_lista_segmentos *segmento;
		segmento = list_find(proceso->lista_Segmentos, (void*) (*mismoSegmento));
		if(segmento != NULL){
			if((direccion.pagina*256+direccion.desplazamiento+tamanio)<=segmento->tamanio){
				while(tamanio>0){

					t_lista_paginas* pagina;
					pagina = list_find(segmento->lista_Paginas, (void*) (*mismaPagina));
					if(pagina != NULL){
						pthread_mutex_lock(&mutex_swap);
						if(pagina->swap==1){ //NO ESTÁ EN MEMORIA
							pthread_mutex_lock(&mutex_tablaMarcos);
							uint32_t numeroDeMarcoLibre= buscarMarcoLibre(tabla_marcos);
							if(numeroDeMarcoLibre!=-1){//Hay un marco libre en memoria principal donde cargar la pagina
								pthread_mutex_lock(&mutex_log);
								log_info(logger,"Al marco %i, se asigno la pagina %i del segmento %i del PID %i \n",numeroDeMarcoLibre, pagina->numeroPagina,segmento->numeroSegmento, PID);
								pthread_mutex_unlock(&mutex_log);
								t_pagina paginaACargar;
								printf("extraigo info de archivo\n");
								paginaACargar= extraerInfoDeArchSwap(PID, direccion.segmento, direccion.pagina);
								printf("extraida info de archivo\n");
								t_direccion direccion_base; // Cargo el contenido del archivo en la direccion base: n° de segmento y pagina que corresponde, pero desplazamiento 0
								memcpy(&direccion_base.segmento, &segmento->numeroSegmento, sizeof(uint32_t));
								memcpy(&direccion_base.pagina, &pagina->numeroPagina, sizeof(uint32_t));
								direccion_base.desplazamiento = 0;
								guardarInformacion(memoria_ppal+(numeroDeMarcoLibre*256),direccion_base,paginaACargar.codigo,paginaACargar.tamanio_buffer);
								pagina->marcoEnMemPpal=numeroDeMarcoLibre;
								pagina->swap=0;
								printf("guarde el archivo en memoria\n"); //DEBUG
								tabla_marcos[numeroDeMarcoLibre].marco_libre = 0;//Ocupado
								memcpy(&tabla_marcos[numeroDeMarcoLibre].pagina, &pagina->numeroPagina, sizeof(uint32_t));
								memcpy(&tabla_marcos[numeroDeMarcoLibre].segmento, &segmento->numeroSegmento, sizeof(uint32_t));
								tabla_marcos[numeroDeMarcoLibre].pid=PID;

								}else{
									printf("No hay marcol libre\n");
									uint32_t numeroMarcoASwapear;
									if(string_equals_ignore_case(alg_sustitucion,"LRU")){
											numeroMarcoASwapear = buscarPaginaMenosUsada();
										} else { numeroMarcoASwapear = algoritmo_clock();}
									bool paginaASacar(t_lista_paginas *numeroPagina){
										printf("Numero de pagina en Marco: %i Pagina: %i \n",tabla_marcos[numeroMarcoASwapear].pagina, numeroPagina->numeroPagina);
												return numeroPagina->numeroPagina==tabla_marcos[numeroMarcoASwapear].pagina;
												}
									bool segmentoASacar(t_lista_segmentos *numeroSegmento){
												return numeroSegmento->numeroSegmento==tabla_marcos[numeroMarcoASwapear].segmento;
												}
									bool pidASacar(t_lista_procesos *PIDEncontrado){
												return PIDEncontrado->pid==tabla_marcos[numeroMarcoASwapear].pid;
												}
									printf("Numero de marco a swapear: %i \n", numeroMarcoASwapear);
									t_lista_procesos *procesoDePaginaASacar;
									t_lista_paginas *paginaSacada;
									t_lista_segmentos *segmentoDePaginaASacar;
									procesoDePaginaASacar = list_find(listaProcesos,(void*) (*pidASacar));
									segmentoDePaginaASacar = list_find(procesoDePaginaASacar->lista_Segmentos, (void*) (*segmentoASacar));
									paginaSacada = list_find(segmentoDePaginaASacar->lista_Paginas, (void*) (*paginaASacar));
									printf("Pagina a sacar: %i\n", paginaSacada->numeroPagina);
									printf("Pagina en swap: %i\n", paginaSacada->swap);
									paginaSacada->swap=1;
									printf("Pagina en swap: %i\n", paginaSacada->swap);
									paginaSacada->marcoEnMemPpal=-1;
									hacerSwap(PID,direccion,pagina,segmento, numeroMarcoASwapear);
									pthread_mutex_lock(&mutex_log);
									log_info(logger,"Se hizo swap en el marco: %i.\n La pagina que salio de memoria principal fue: %i del segmento %i del PID %i \n La pagina que entro en memoria principal fue %i del segmento %i del PID %i \n",numeroMarcoASwapear, paginaSacada->numeroPagina,segmentoDePaginaASacar->numeroSegmento, procesoDePaginaASacar->pid,pagina->numeroPagina,segmento->numeroSegmento,PID);
									pthread_mutex_unlock(&mutex_log);}
							pthread_mutex_unlock(&mutex_tablaMarcos);
						}

							printf("Bit de Algoritmo antes: %i \n", tabla_marcos[pagina->marcoEnMemPpal].bitAlgoritmo);
							modificarBitAlgoritmo(pagina->marcoEnMemPpal);
							printf("Bit de Algoritmo despues: %i \n", tabla_marcos[pagina->marcoEnMemPpal].bitAlgoritmo);
							pthread_mutex_unlock(&mutex_swap);
							pthread_mutex_lock(&mutex_memoriaPpal);
							if((256-direccion.desplazamiento) <= tamanio){
									printf("Llego a perdir\n");
									uint32_t espacioLibre = 256-direccion.desplazamiento;
									void* buff = devolverInformacion(memoria_ppal+((pagina->marcoEnMemPpal)*256), direccion, espacioLibre);
									memcpy(respuesta+memoriaGuardada, buff, espacioLibre);
									direccion.pagina = direccion.pagina + 1;
									direccion.desplazamiento = 0;
									tamanio -= espacioLibre;
									memoriaGuardada += espacioLibre;
									memset(buff,0,tamanio);
								} else {
									void* buff = devolverInformacion(memoria_ppal+((pagina->marcoEnMemPpal)*256), direccion, tamanio);
									memcpy(respuesta+memoriaGuardada, buff, tamanio);
									tamanio = 0;
									memoriaGuardada += tamanio;
								}
							pthread_mutex_unlock(&mutex_memoriaPpal);
						} else {
							page_not_found_exception(direccion.pagina);
							return NULL;
						}
				}
				return respuesta;} else {segmentation_fault();}
		} else {
			segment_not_found_exception(direccion.segmento);
			return NULL;
		}
	free(segmento);} else {
	PID_not_found_exception(PID);
	return NULL;}
free(proceso);
free(buff);
return NULL;
}




