/*
 * funcionesMSP.c
 *
 *  Created on: 24/09/2014
 *      Author: utnso
 */
#include "funcionesMSP.h"

uint32_t puertoMSP;
uint32_t tamanio_mem_ppal;
uint32_t cant_mem_swap;
char* alg_sustitucion;

/*********************************** INICIO *******************************************************************/
void crear_logger(){
	if ((logger = log_create("logger.log", "MSP", false, LOG_LEVEL_TRACE)) == NULL) {
		printf("No se pudo crear el logger. Proceso abortado.\n");
		exit(1);
	}
	log_info(logger,"Inicio de registro de actividades del Proceso MSP.");
}

void leer_config(char *config_path)	{

	config_file = config_create(config_path);
    log_info(logger,"Parseo y Extraccion de valores de archivo de configuracion.");

	if (config_keys_amount(config_file) != MAX_COUNT_OF_CONFIG_KEYS)
	{
		fprintf(stderr,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco.\n");
		log_error(logger,"El fichero 'config.cfg' contiene menos keys de las que deberia o no se encuentra al fichero en disco.");
		exit(1);
	}

	if(config_has_property(config_file, "CANTIDAD_MEMORIA"))	{
			tamanio_mem_ppal = config_get_int_value(config_file,"CANTIDAD_MEMORIA");
	    	sprintf(bufferLog,"CANTIDAD_MEMORIA = [%d]",tamanio_mem_ppal);
	    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'CANTIDAD_MEMORIA' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'CANTIDAD_MEMORIA' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	if(config_has_property(config_file, "PUERTO")){
			puertoMSP = config_get_int_value(config_file, "PUERTO");
	    	sprintf(bufferLog,"PUERTO = [%d]",puertoMSP);
	    	log_debug(logger,bufferLog);
	}
	 else {
			fprintf(stderr, "Falta key 'PUERTO' en archivo de configuracion. Chequear.\n");
			fprintf(stderr, "Programa abortado.\n");
			log_error(logger,"Falta key 'PUERTO' en archivo de configuracion. Programa abortado.");
			exit(1);
		}

	if(config_has_property(config_file, "CANTIDAD_SWAP")){
			cant_mem_swap = config_get_int_value(config_file,"CANTIDAD_SWAP");
	    	sprintf(bufferLog,"CANTIDAD_SWAP = [%d]",cant_mem_swap);
	    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'CANTIDAD_SWAP' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'CANTIDAD_SWAP' en archivo de configuracion. Programa abortado.");
		exit(1);
	}

	if(config_has_property(config_file, "SUST_PAGS"))	{
			alg_sustitucion = config_get_string_value(config_file,"SUST_PAGS");
	    	sprintf(bufferLog,"SUST_PAGS = [%s]",alg_sustitucion);
	    	log_debug(logger,bufferLog);
	} else {
		fprintf(stderr, "Falta key 'SUST_PAGS' en archivo de configuracion. Chequear.\n");
		fprintf(stderr, "Programa abortado.\n");
		log_error(logger,"Falta key 'SUST_PAGS' en archivo de configuracion. Programa abortado.");
		exit(1);
	}
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
/******************************************** MEMORIA PRINCIPAL ******************************************************/
void *reservarBloquePpal(uint32_t tamanioMemoria){

     void *bloquePrincipal = malloc(tamanioMemoria);
     if(NULL == bloquePrincipal){
    	 //TODO log "no se pudo crear memoria principal"
         //TODO liberar recursos
    	 exit(0);
     }
 	//Se puede inicializar la memoria, en este caso creo que conviene guardar un \0 en los bloques
 	memset(bloquePrincipal,0,tamanioMemoria);

 	return bloquePrincipal;
}

uint32_t calcularMarcos(uint32_t tamanioMemoria){
	uint32_t cantidadmarcos = (tamanioMemoria*1024)/256;
	return cantidadmarcos;
}

t_marco *crearTablaDeMarcos(){

	t_marco *nuevaTabla = malloc(sizeof(t_marco)*cant_marcos);
	int i ;
	for(i=0; i<cant_marcos; i++){
		nuevaTabla[i].marco_libre=1;
		nuevaTabla[i].numeroMarco=i;
		nuevaTabla[i].bitAlgoritmo=0;
	}
return nuevaTabla;
}

uint32_t buscarMarcoLibre(t_marco *unaTabla){//Devuelve -1 en caso no encontrar un marco vacio
	uint32_t i =0;
	uint32_t marcoEncontrado = -1;
	while((i<cant_marcos) && marcoEncontrado==-1){
		if(tabla_marcos[i].marco_libre==1){
			marcoEncontrado=tabla_marcos[i].numeroMarco;
		}
		i++;
	}
	return marcoEncontrado;
}
/************************************ EXCEPCIONES ************************************/
void PID_not_found_exception(uint32_t PID){
	printf("No se encontro el PID %d\n", PID);
	//TODO: AGREGAR LOGS Y NO SE SI ABORTAR
}

void segment_not_found_exception(uint32_t segmento){
	printf("No se encontro el segmento %d\n", segmento);
	//TODO: AGREGAR LOGS Y NO SE SI ABORTAR
}

void page_not_found_exception(uint32_t pagina){
	printf("No se encontro la pagina %d\n",pagina);
	//TODO: AGREGAR LOGS Y NO SE SI ABORTAR
}

void segmentation_fault(){
	printf("Se produjo segmentation fault\n");
	//TODO: AGREGAR LOGS Y NO SE SI ABORTAR
}
/*********************************** FUNCIONES DE SWAP *******************************/

void crearArchivoSwapEnDisco(t_pagina pagina){
	char* file_name = string_new();

	//Concateno el path donde se ubicaran los archivos, con el nombre del archivo en creacion
	string_append(&file_name, path_swap);
	string_append(&file_name, "PID:");
	string_append(&file_name, string_itoa(pagina.PID));
	string_append(&file_name, "-");
	string_append(&file_name, "SEG:");
	string_append(&file_name, string_itoa(pagina.num_segmento));
	string_append(&file_name, "-");
	string_append(&file_name, "PAG:");
	string_append(&file_name, string_itoa(pagina.num_pag));
	string_append(&file_name, ".bc");

	//Abro un nuevo archivo
	FILE* arch_swap = NULL;
	arch_swap = fopen(file_name, "w+"); //FIXME Aca el modo es w+ lo hablamos con el ayudante

	//Pongo la informacion necesaria en el archivo
	fwrite(pagina.codigo, 1,pagina.tamanio_buffer, arch_swap);


	/*pthread_mutex_lock(&mutex_log);
	log_debug(logger,"Creado el archivo %s",file_name);
	pthread_mutex_unlock(&mutex_log);*/

	paginas_en_disco++;

	fclose(arch_swap);
	free(file_name);

}

t_pagina extraerInfoDeArchSwap(uint32_t pid, uint32_t seg, uint32_t pagina){ //Abrimos directorio, abrimos archivo, sacamos la info, lo destruimos y la retornamos
	DIR *dir;
	t_pagina pag;
	pag.PID = -1;
	pag.num_pag = -1;
	pag.num_segmento = -1;
	pag.codigo = NULL;
	pag.archivo = NULL;

	dir = abrir_directorio_swap(); //abro el directorio donde se encuentran los archivos
	if(dir == NULL){
		return pag; //Devuelvo t_pagina con valores negativos => Error
	}
    printf("Abri el directorio, ahora abro archivo\n");
	pag = abrir_archivo_en_direcctorio(pid, seg, pagina);

	if(pag.PID == -1){
		printf("no se encontro archivo\n");
	}

	long int tamanio_archivo = calcular_tamanio_archivo(pag.archivo);

	char buffer[tamanio_archivo];//Copio el contenido del archivo al buffer

	if(tamanio_archivo != 0){
		char* code = leer_archivo(pag.archivo, tamanio_archivo);
		memcpy(buffer,code, tamanio_archivo);
		free(code);
	} else {
		buffer[0] = '\0';
	}



	//Actualizo la estructura t_pagina con el codigo rescatado
	pag.tamanio_buffer = tamanio_archivo;

	pag.codigo = malloc(pag.tamanio_buffer);
	memcpy(pag.codigo, buffer, pag.tamanio_buffer);

	fclose(pag.archivo);

	destruir_archivo(pag.nombre_archivo);
	closedir(dir);
	return pag;
}

/*FIXME Juli necesitamos que extraerinfodearchswap haga lo siguiente:
1- Abra el directorio
2- Abre el archivo ( NO LO TIENE QUE BUSCAR por que SIEMPRE va a existir y si no existe la funcion fopen en w+ lo crea)
3- Saca la informacion del archivo (si lo acaba de crear va a estar vacio)
4- Carga la info en el t_pagina y la retorna

Ponemos por que vos agregaste el swap_out en escribir y esto no tendria ser necesario ya que si esta funcion hace lo que pusimos no es necesario que se cree el archivo antes de abrirse
Nosotros tratamos de arreglar pero cuando lo corremos la consola se queda haciendo "nada" no tira error ni nada, no sabemos que puede ser, capaz vos te das cuenta.
Fijate que agregamos un par de prints y el error esta en abrir_archivo_en_direcctorio no sale de ahi
*/


/**************** AUXILIARES DE SWAPPING *****************/

void destruir_archivo(char* nombre_archivo) {
	if (remove(nombre_archivo) != 0) {
		paginas_en_disco--;
		pthread_mutex_lock(&mutex_log);
		printf("No se pudo eliminar el archivo %s\n", nombre_archivo);
		log_error(logger, "No se pudo eliminar el archivo %s\n",nombre_archivo);
		pthread_mutex_unlock(&mutex_log);
	} else {
		/*pthread_mutex_lock(&mutex_log);
		 log_info(logger,"Eliminado archivo %s del directorio %s", dirent->d_name, path_swap);
		 pthread_mutex_unlock(&mutex_log);*/
	}
	free(nombre_archivo);
}

void destruir_archivo_swap(int pid, uint32_t segmento, uint32_t pagina){
	char* file_name = string_new();

	string_append(&file_name, path_swap);
	string_append(&file_name, "PID:");
	string_append(&file_name, string_itoa(pid));
	string_append(&file_name, "-");
	string_append(&file_name, "SEG:");
	string_append(&file_name, string_itoa(segmento));
	string_append(&file_name, "-");
	string_append(&file_name, "PAG:");
	string_append(&file_name, string_itoa(pagina));
	string_append(&file_name, ".bc");

	destruir_archivo(file_name);

	free(file_name);
}

DIR* abrir_directorio_swap(){
	DIR* dir;

	dir = opendir(path_swap); //abro el directorio donde se encuentran los archivos
		if(dir == NULL){
			pthread_mutex_lock(&mutex_log);
			printf("No se pudo abrir el directorio %s\n", path_swap);
			log_error(logger,"No se pudo abrir el directorio %s\n", path_swap);
			pthread_mutex_unlock(&mutex_log);
			closedir(dir);
			return NULL;
		}

	return dir;
}

t_pagina abrir_archivo_en_direcctorio(uint32_t PID, uint32_t SEG, uint32_t PAG){
	t_pagina pag;
	char* file_name = string_new();

		string_append(&file_name, path_swap);
		string_append(&file_name, "PID:");
		string_append(&file_name, string_itoa(PID));
		string_append(&file_name, "-");
		string_append(&file_name, "SEG:");
		string_append(&file_name, string_itoa(SEG));
		string_append(&file_name, "-");
		string_append(&file_name, "PAG:");
		string_append(&file_name, string_itoa(PAG));
		string_append(&file_name, ".bc");
		FILE* arch_swap = fopen(file_name, "a+");

	//PID, pagina y segmento para completar la t_pagina
		pag.PID = PID;
		pag.num_segmento = SEG;
		pag.num_pag = PAG;
		pag.nombre_archivo = file_name;
		pag.archivo = arch_swap;
		return pag;
}

/********************************* MANEJO DE INFORMACION ******************************************/
void guardarInformacion(void* baseMarco,t_direccion direccion,char* bytes_escribir, uint32_t tamanio){

	//printf("baseMarco + direccion.desplazamiento + tamanio = %d\n",direccion.desplazamiento + tamanio);
	//printf("baseMarco+256 = %d\n",256);

	//FIXME: CAMBIAR EL 289 POR EL 256. LO QUE PASA ES QUE LAS SYSCALLS ENSAMBLADAS OCUPAN 256, O SEA QUE NO ENTRAN EN UNA PAGINA
	if((baseMarco + direccion.desplazamiento + tamanio) < (baseMarco+289)){ // Se fija que no se quiera escribir fuera de los limites del marco
			memcpy(baseMarco + direccion.desplazamiento, bytes_escribir, tamanio);
	}
	else {
			segmentation_fault();
	}
}

char* devolverInformacion(void* baseMarco, t_direccion direccion, uint32_t tamanio){
	void* buffer = malloc(tamanio); //FIXME: NO SE SI ES VOID* O CHAR*
	if((baseMarco + direccion.desplazamiento + tamanio) < (baseMarco+256)){
	memcpy(buffer, baseMarco + direccion.desplazamiento, tamanio+1);
	} else {
		segmentation_fault();}
	return buffer;
}

 /*********************************************** CONEXIONES ********************************************************/

void handler_conexiones(t_conexion_entrante* conexion){
	int sock = *conexion->socket;
	uint32_t senial;

	while(1){
	socket_recibirSignal(sock, &senial);

	switch (senial) {
	case ES_CPU:
		handler_cpu(conexion);
		break;
	case ES_KERNEL:
		handler_kernel(conexion);
		break;
	}
	}

}

void handler_kernel(t_conexion_entrante* conexion){
	int sock = *conexion->socket;
	t_tipoEstructura tipoRecibido;
	void* structRecibido;
	t_struct_numero* respuesta;
	int resultado;
	t_struct_env_bytes* escritura;

	resultado = socket_recibir(sock, &tipoRecibido,&structRecibido);
	if(resultado != 1){
		printf("No llego bien el paquete al handler kernel\n");
	}

	switch(tipoRecibido){
	case D_STRUCT_MALC:
		pthread_mutex_lock(&mutex_log);
		//TODO LOG diciendo que se va a crear un segmento con el tamaño recibido
		pthread_mutex_unlock(&mutex_log);

		resultado = crearSegmento(((t_struct_malloc* )structRecibido)->PID, ((t_struct_malloc* )structRecibido)->tamano_segmento);

		//Le comunico al Kernel si se pudo realizar operacion
		respuesta = malloc(sizeof(t_struct_numero));
		respuesta->numero = resultado;
		socket_enviar(sock, D_STRUCT_NUMERO, respuesta);

		pthread_mutex_lock(&mutex_log);
		//TODO LOG diciendo si se pudo crear correctamente (si es -1, poner error)
		pthread_mutex_unlock(&mutex_log);

		free(respuesta);
		free(structRecibido);
		break;

	case D_STRUCT_FREE:
		pthread_mutex_lock(&mutex_log);
		//TODO LOG diciendo que se va a liberar un segmento de direccion tal
		pthread_mutex_unlock(&mutex_log);

		destruirSegmento(((t_struct_free *) structRecibido)->PID, ((t_struct_free *) structRecibido)->direccion_base);

		/*Le comunico a CPU si se pudo destruir segmento?
		t_struct_numero* respuesta = malloc(sizeof(t_struct_numero));
		respuesta->numero = resultado;
		socket_enviar(sock, D_STRUCT_NUMERO, &respuesta);

		pthread_mutex_lock(mutex_log);
		//TODO LOG diciendo si se pudo borrar correctamente (si es -1, poner error)
		pthread_mutex_unlock(mutex_log);*/

		free(structRecibido);
		break;

	case D_STRUCT_ENV_BYTES:
		escritura = (t_struct_env_bytes*) structRecibido;

		pthread_mutex_lock(&mutex_log);
		//TODO LOG diciendo lo que se envio
		pthread_mutex_unlock(&mutex_log);

		resultado = escribirMemoria(escritura->PID, escritura->base, escritura->buffer, escritura->tamanio);
		respuesta = malloc(sizeof(t_struct_numero));
		respuesta->numero = resultado;
		socket_enviar(sock, D_STRUCT_NUMERO, respuesta);

		pthread_mutex_lock(&mutex_log);
		//TODO LOG diciendo si se pudo enviar correctamente
		pthread_mutex_unlock(&mutex_log);

		free(structRecibido);
		free(respuesta);
		break;
	}
}

 void handler_cpu(t_conexion_entrante* conexion){
	 	//pthread_detach(pthread_self());

	 	int sock = *conexion->socket;

	 	t_tipoEstructura tipoRecibido;
	 	void* structRecibido;
	 	t_struct_sol_bytes * solicitud;
	 	t_struct_env_bytes* escritura;

	 	t_struct_numero* respuesta;
	 	int resultado;

	 	while(1){
	 		socket_recibir(sock, &tipoRecibido,&structRecibido);
	 		switch(tipoRecibido){
	 			case D_STRUCT_SOL_BYTES:

	 				solicitud = (t_struct_sol_bytes*) structRecibido;

	 				pthread_mutex_lock(&mutex_log);
	 				//TODO LOG diciendo lo que se solicita
	 				pthread_mutex_unlock(&mutex_log);

	 				t_struct_respuesta_msp* buffer = malloc(sizeof(t_struct_respuesta_msp));
	 				//buffer->buffer = malloc(solicitud->tamanio);
	 				//memcpy(buffer->buffer,solicitar_memoria(solicitud->PID, solicitud->base, solicitud->tamanio), solicitud->tamanio);
	 				buffer->buffer = solicitar_memoria(solicitud->PID, solicitud->base, solicitud->tamanio);
	 				buffer->tamano_buffer = solicitud->tamanio;
	 				socket_enviar(sock, D_STRUCT_RESPUESTA_MSP, buffer);

	 				free(buffer->buffer);
	 				free(buffer);
	 				free(structRecibido);
	 				break;

	 			case D_STRUCT_ENV_BYTES:

	 				escritura = (t_struct_env_bytes*) structRecibido;

	 				pthread_mutex_lock(&mutex_log);
	 				//TODO LOG diciendo lo que se envio
	 				pthread_mutex_unlock(&mutex_log);

	 				resultado = escribirMemoria(escritura->PID, escritura->base, escritura->buffer, escritura->tamanio);
	 				respuesta = malloc(sizeof(t_struct_numero));
	 				respuesta->numero = resultado;
	 				socket_enviar(sock, D_STRUCT_NUMERO, respuesta);

	 				pthread_mutex_lock(&mutex_log);
	 				//TODO LOG diciendo si se pudo enviar correctamente
	 				pthread_mutex_unlock(&mutex_log);

	 				free(structRecibido);
	 				free(respuesta);
	 				break;

	 			case D_STRUCT_MALC:
	 				pthread_mutex_lock(&mutex_log);
	 				//TODO LOG diciendo que se va a crear un segmento con el tamaño recibido
	 				pthread_mutex_unlock(&mutex_log);

	 				resultado = crearSegmento(((t_struct_malloc* )structRecibido)->PID, ((t_struct_malloc* )structRecibido)->tamano_segmento);

	 				//Le comunico a la CPU si se pudo realizar operacion
	 				respuesta = malloc(sizeof(t_struct_numero));
	 				respuesta->numero = resultado;
	 				socket_enviar(sock, D_STRUCT_NUMERO, respuesta);

	 				pthread_mutex_lock(&mutex_log);
	 				//TODO LOG diciendo si se pudo crear correctamente (si es -1, poner error)
	 				pthread_mutex_unlock(&mutex_log);

	 				free(respuesta);
	 				free(structRecibido);
	 				break;

	 			case D_STRUCT_FREE:

	 				pthread_mutex_lock(&mutex_log);
	 				//TODO LOG diciendo que se va a liberar un segmento de direccion tal
	 				pthread_mutex_unlock(&mutex_log);

	 				destruirSegmento(((t_struct_free *) structRecibido)->PID, ((t_struct_free *) structRecibido)->direccion_base);

	 				/*Le comunico a CPU si se pudo destruir segmento?
	 				t_struct_numero* respuesta = malloc(sizeof(t_struct_numero));
	 				respuesta->numero = resultado;
	 				socket_enviar(sock, D_STRUCT_NUMERO, &respuesta);

	 				pthread_mutex_lock(mutex_log);
	 				//TODO LOG diciendo si se pudo borrar correctamente (si es -1, poner error)
	 				pthread_mutex_unlock(mutex_log);*/

	 				free(structRecibido);

	 				break;
	 		}
	 	}

	 	return;
 }

 /************************************************* CONSOLA *********************************************************/

 void *inciarConsola(void *param1){

	char command;
	uint32_t baseSeg, direcVir;
	int terminarConsola = 1;
	uint32_t PID;
	uint32_t tamanio_escritura;

	printf("\nConsola MSP inciada(exit para cerrar)\n");
	indicaciones_consola();
	while (terminarConsola){
	scanf("%c",&command);
	switch(command){
	case '1': printf("El comando elegido fue: Crear Segmento\n"
				"Ingrese un PID:\n");
				scanf("%d",&PID);
				printf("Ingrese Tamaño (en bytes):\n");
				uint32_t tam_segmento;
				scanf("%d",&tam_segmento);
				uint32_t direccion_segmento = crearSegmento(PID, tam_segmento);
				printf("La direccion base del nuevo segmento es: %d\n", direccion_segmento);
				indicaciones_consola();

	break;
	case '2': printf("El comando elegido fue: Destruir segmento\n"
				"Ingrese un PID:\n");
				scanf("%d",&PID);
				printf("Ingrese base del segmento:");
				scanf("%d",&baseSeg);
				destruirSegmento(PID,baseSeg);
				indicaciones_consola();
	break;
	case '3': printf("El comando elegido fue: Escribir Memoria\n"
				"Ingrese un PID:\n");
				scanf("%d",&PID);
				printf("Ingrese direccion virtual:\n");
				scanf("%d", &direcVir);
				printf("Ingrese un tamaño:\n");
				scanf("%d", &tamanio_escritura);
				printf("Ingrese un texto:\n");
				char texto[256]; //CAPAZ DEBERIA SER CHAR[ALGUN TAMANIO MAX]
				scanf("%s", texto);
				printf("Se ingreso %s\n", texto);
				escribirMemoria(PID, direcVir, texto, tamanio_escritura);
				indicaciones_consola();

	break;
	case '4': printf("El comando elegido fue: Leer memoria\n"
				"Ingrese un PID:\n");
				scanf("%d",&PID);
				printf("Ingrese una direccion virtual:\n");
				scanf("%d", &direcVir);
				printf("Ingrese un tamaño:\n");
				scanf("%d", &tamanio_escritura);
				char* bytesEnMemoria =solicitar_memoria(PID, direcVir, tamanio_escritura);
				printf("En la direccion: %d del PID: %d se encuentra guardado: %s", direcVir,PID,bytesEnMemoria);
				indicaciones_consola();

	break;
	case '5': printf("El comando elegido fue: Tabla de segmentos\n");
			  tabla_segmentos();
			  indicaciones_consola();
	break;
	case '6': printf("El comando elegido fue: Tabla de paginas\n"
				"Ingrese un PID:\n");
				scanf("%d",&PID);
				tabla_paginas(PID);
				indicaciones_consola();
	break;
	case '7': printf("El comando elegido fue: Listar marcos\n");
			  listar_marcos();
			  indicaciones_consola();
	break;
	case '8': terminarConsola=0; break;
	}
	}
pthread_exit((void*) "Termino el hilo Consola");	/* No se si esta bien implementado el pthread_exit */
// Falta ajustar cada case a lo que el comando pida: definir que funcion se llama de la MSP, definir tipos y liberar memoria
return NULL;
}

/******************* AUXILIARES CONSOLA *******************/
 void indicaciones_consola() {
 	printf(
 			"\nIntroduzca numero de comando elegido:\n1. Crear segmento\n2. Destruir Segmento\n3. Escribir memoria\n4. Leer memoria\n5. Tabla de segmentos\n6. Tabla de paginas\n7. Listar marcos\n8. Exit\n");
 }

 void tabla_segmentos(){
	void imprimirSegmento(t_lista_segmentos *segmento){
		uint32_t base = crearDireccion((*segmento).numeroSegmento,0, 0);
		printf("Numero segmento: %d,   Tamanio: %d,   Direccion Base: %d\n",(*segmento).numeroSegmento,(*segmento).tamanio, base);
	}
	void imprimirPID(t_lista_procesos *proceso){
	printf("El PID del proceso es: %d\n", (*proceso).pid);
	list_iterate((*proceso).lista_Segmentos, (void*) (*imprimirSegmento));
}
 list_iterate(listaProcesos, (void*) (*imprimirPID));
 }

 void tabla_paginas(uint32_t PID){
	 char* trueofalse(uint32_t unNumero){
			char* resultado;
			 if(unNumero==0){
				resultado = "En Memoria Principal";
			 } else {
				 resultado = "En Memoria Swap";
			 }
	 return resultado;}
	 t_lista_procesos *proceso = malloc(sizeof(t_lista_procesos));
	 void imprimirPagina(t_lista_paginas *pagina){
	 		printf("Numero pagina: %d,   Ubicacion: %s\n",(*pagina).numeroPagina,trueofalse((*pagina).swap));
	 	}
	 void imprimirSegmento(t_lista_segmentos *segmento){
	 		printf("Numero segmento: %d\n",(*segmento).numeroSegmento);
	 		list_iterate((*segmento).lista_Paginas, (void*) (*imprimirPagina));
	 	}
	 bool mismoPID(t_lista_procesos *PIDEncontrado){
	 			return PIDEncontrado->pid==PID;
	 		}
	 proceso=list_find(listaProcesos,(void*) (*mismoPID));
	 if(proceso!=NULL){
	 list_iterate((*proceso).lista_Segmentos, (void*) (*imprimirSegmento));
 }else {
	 printf("El PID ingresado: %d, no existe en el sistema", PID);}
 }

 void listar_marcos(){//FIXME ARREGLAR LISTA DE MARCOS!
	 char* trueofalse(uint32_t unNumero){
		char* resultado;
		 if(unNumero==0){
			resultado = "Ocupado";
		 } else {
			 resultado = "Libre";
		 }
	 return resultado;
	 }
	 uint32_t i = 0;
	 while(i<cant_marcos){
		 printf("Numero de marco: %d,  Marco: %s\n ",tabla_marcos[i].numeroMarco, trueofalse(tabla_marcos[i].marco_libre));
	  i++;
	 }
 }

