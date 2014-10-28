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
    log_info(logger,"Parseo y Extraccion de valores de archivo de configuracion");

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
void *reservarBloquePpal(int tamanioMemoria){

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

t_list *dividirMemoriaEnMarcos(void *memoria, int tamanioMemoria){
	t_list *lista_marcos = list_create();
	int cant_marcos = (tamanioMemoria*1024)/256;
	int i ;
	for(i=0; i<cant_marcos; i++)
	{
		t_marco *marco = malloc(sizeof(t_marco));
		(*marco).memoria= malloc(256);
		(*marco).memoria = memoria + i*256;
		(*marco).numeroMarco = i;
		list_add(lista_marcos, marco);
		//free((*marco).memoria);
		//free(marco);
	}
return lista_marcos; //TODO Preguntar a ayudante
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

void swap_out(t_pagina pagina){
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
	arch_swap = fopen(file_name, "wb");

	//Pongo la informacion necesaria en el archivo
	fwrite(pagina.codigo, 1,pagina.tamanio_buffer, arch_swap);

	/*pthread_mutex_lock(&mutex_log);
	log_debug(logger,"Creado el archivo %s",file_name);
	pthread_mutex_unlock(&mutex_log);*/

	paginas_en_disco++;

	fclose(arch_swap);
	free(file_name);

}

t_pagina swap_in(int pid, int seg, int pagina){ //FIXME le paso como parametros el segmento y la pagina. En un futuro, si quieren, pasan la direccion y la traducen adentro de la funcion

	//struct dirent *dirent;
	DIR* dir;
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

	pag = buscar_archivo(pid, seg, pagina, dir);

	long int tamanio_archivo = calcular_tamanio_archivo(pag.archivo);

	char buffer[tamanio_archivo];//Copio el contenido del archivo al buffer

	char* code = leer_archivo(pag.archivo, tamanio_archivo);

	memcpy(buffer,code, tamanio_archivo);


	//Actualizo la estructura t_pagina con el codigo rescatado
	pag.tamanio_buffer = tamanio_archivo;

	pag.codigo = malloc(pag.tamanio_buffer);
	memcpy(pag.codigo, buffer, pag.tamanio_buffer);

	fclose(pag.archivo);

	destruir_archivo(pag.nombre_archivo);
	free(code);
	closedir(dir);
	return pag;
}

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

t_pagina buscar_archivo(int PID, int SEG, int PAG, DIR* dir){
	struct dirent* dirent;
	t_pagina pag;
	pag.PID = -1;
	pag.num_pag = -1;
	pag.num_segmento = -1;
	pag.codigo = NULL;
	pag.nombre_archivo = NULL;

	while((dirent = readdir(dir)) != NULL){ //mientras que el directorio tenga archivos para buscar
	char** partes_nombre_archivo = string_split(dirent->d_name, "-"); //agarro el nombre del archivo y divido por cada "-" que tiene el nombre
																		//ejemplo: pid:3-pag:5-seg:6 => [pid:3, pag:5, seg:6]
	int i = 0;
	while(partes_nombre_archivo[i] != NULL){//mientras que no se me terminen las partes del nombre del archivo
		char** partes_pid = string_split(partes_nombre_archivo[i], ":");//Divido cada parte en otras si esta el separador ":"

		if(string_starts_with(partes_pid[0],"PID")  && (atoi(partes_pid[1]) == PID)){
			char** partes_seg = string_split(partes_nombre_archivo[i+1], ":");

			if(string_starts_with(partes_seg[0], "SEG") && (atoi(partes_seg[1]) == SEG)){
				char** partes_pag = string_split(partes_nombre_archivo[i+2], ":");

				if(string_starts_with(partes_pag[0], "PAG") && (atoi(partes_pag[1]) == PAG)){

					//PID, pagina y segmento para completar la t_pagina
					pag.PID = atoi(partes_pid[1]);

					pag.num_segmento = atoi(partes_seg[1]);

					pag.num_pag = atoi(partes_pag[1]);

					char* nombre_archivo = string_new();//Concateno el path con el nombre del archivo seleccionado
					string_append(&nombre_archivo,path_swap);
					string_append(&nombre_archivo,dirent->d_name);

					pag.nombre_archivo = nombre_archivo;

					FILE* arch_swap = abrir_archivo(nombre_archivo, logger, &mutex_log);

					pag.archivo = arch_swap;

					}
				}
			}
			i++;
		}
	}
	return pag;
}

/*********************************************** DIRECCIONES *********************************************************/

uint32_t elevar(uint32_t numero, uint32_t elevado){
	int i;
	uint32_t resultado;
	resultado=numero;
	switch(elevado){
	case 0: resultado=1; break;
	case 1: resultado=numero; break;
	default: for(i=1;i<elevado;i++){resultado=resultado*numero;};

	}
return resultado;
}


char *traducirABinario(uint32_t direccion, int cantidad_bits) {

	uint32_t binNumInv[cantidad_bits];
	uint32_t counter;

	uint32_t i;
	uint32_t aux = direccion;


	char bina[cantidad_bits];
	char bina_inv[cantidad_bits];

	for ( counter = 0 ; counter < cantidad_bits; counter++ ) {
	binNumInv[counter] = aux % 2;


	aux = aux / 2;

	if(binNumInv[counter] == 0){

	bina[counter] = '0'; } else { bina[counter] = '1';}
	}

	for (i=0;i<cantidad_bits;i++){
		counter --;
		bina_inv[counter]=bina[i];

	}
	return bina_inv;
	}


uint32_t traducirADecimal(char *binario, int cantidad_bits){

	int i;
	int j;
	uint32_t decimal = 0;
	for(i=cantidad_bits-1; i >= 0; i--){
		j= cantidad_bits-1-i;
		switch(binario[i]){
		case '0':
			break;
		case '1':
			decimal = elevar(2,j)+decimal;
			break;
		default:
			break;
		}
	}
	return decimal;
}

uint32_t crearDireccion(uint32_t segmento, uint32_t pagina){ //FIXME: seria copado que reciba el desplazamiento por parametro
	uint32_t direccionCreada;
	char *binSegmento;
	char *binPagina;
	char *direccionCreadaBin = malloc(32);

	binSegmento=malloc(12);
	binPagina=malloc(13);
	memcpy(binSegmento, traducirABinario(segmento, 12), 12);
	memcpy(binPagina, traducirABinario(pagina, 12), 12);

	memcpy(direccionCreadaBin, binSegmento, 12);
	memcpy(direccionCreadaBin+12, binPagina, 12);
	memcpy(direccionCreadaBin+24, "00000000", 8);

	direccionCreada=traducirADecimal(direccionCreadaBin, 32);

	free(binSegmento);
	free(binPagina);
	free(direccionCreadaBin);
	return direccionCreada;
}

 t_direccion traducirDireccion(uint32_t unaDireccion){
	t_direccion direccionTraducida;
	char direccionEnBinario[32];
	memcpy(direccionEnBinario,traducirABinario(unaDireccion,32),32);

	char segmento[12];
	char pagina[12];
	char desplazamiento[8];

	memcpy(segmento, direccionEnBinario, 12);
	memcpy(pagina, direccionEnBinario + 12, 12);
	memcpy(desplazamiento, direccionEnBinario + 24, 8);

	(direccionTraducida).segmento=traducirADecimal(segmento, 12);
	(direccionTraducida).pagina=traducirADecimal(pagina, 12);
	(direccionTraducida).desplazamiento=traducirADecimal(desplazamiento, 8);

	return direccionTraducida;
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
				printf("La direccion del nuevo segmento es: %d\n", direccion_segmento);
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
				char* texto; //CAPAZ DEBERIA SER CHAR[ALGUN TAMANIO MAX]
				scanf("%s", texto);
				escribirMemoria(PID, direcVir, texto, tamanio_escritura);

	break;
	case '4': printf("El comando elegido fue: Leer memoria\n"
				"Ingrese un PID:\n");
				scanf("%d",&PID);
				printf("Ingrese una direccion virtual:\n");
				scanf("%d", &direcVir);
				printf("Ingrese un tamaño:\n");
				scanf("%d", &tamanio_escritura);
	break;
	case '5': printf("El comando elegido fue: Tabla de segmentos\n"); break;
	case '6': printf("El comando elegido fue: Tabla de paginas\n"
				"Ingrese un PID:\n");
				scanf("%d",&PID);
	break;
	case '7': printf("El comando elegido fue: Listar marcos\n"); break;
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

 }

 void tabla_paginas(){

 }

 void listar_marcos(){

 }

