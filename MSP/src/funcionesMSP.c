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

void leerConfiguracion(t_config *archConfigMSP, char *path){


	archConfigMSP= config_create(path);


	if(config_has_property(archConfigMSP, "CANTIDAD_MEMORIA")){
			tamanio_mem_ppal = config_get_int_value(archConfigMSP,"CANTIDAD_MEMORIA");}
	else {
				printf("No esta definida la cantidad de memoria principal en el archivo\n");
				exit(0);
			}
	if(config_has_property(archConfigMSP, "PUERTO")){
			puertoMSP = config_get_int_value(archConfigMSP, "PUERTO");}
	else {
			printf("No esta definido el puerto en el archivo\n");
			exit(0);
		}

	if(config_has_property(archConfigMSP, "CANTIDAD_SWAP")){
			cant_mem_swap = config_get_int_value(archConfigMSP,"CANTIDAD_SWAP");}
	else {
				printf("No esta definido la cantidad de memoria swap en el archivo\n");
				exit(0);
			}

	if(config_has_property(archConfigMSP, "SUST_PAGS")){
			alg_sustitucion = config_get_string_value(archConfigMSP,"SUST_PAGS");}
	else {
					printf("No esta definido el algoritmo de sustitucion de paginas en el archivo\n");
					exit(0);
			}

	//TODO Faltan logs y alguna que otra validacion


}


void *reservarBloquePpal(int tamanioMemoria){

     void *unaMemoria = malloc(tamanioMemoria);
     if(NULL == unaMemoria){
    	 //TODO log "no se pudo crear memoria principal"
         //TODO liberar recursos
    	 exit(0);
     }
     return unaMemoria;
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
		free((*marco).memoria);
		free(marco);
	}
return lista_marcos; //TODO Preguntar a ayudante
}


void crear_logger(t_log *logger){
	if ((logger = log_create("logMSP.log","MSP",false,LOG_LEVEL_DEBUG)) == NULL) {
		printf("No se pudo crear el logger\n");
	}
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
	log_debug(logMSP,"Creado el archivo %s",file_name);
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
		log_error(logMSP, "No se pudo eliminar el archivo %s\n",nombre_archivo);
		pthread_mutex_unlock(&mutex_log);
	} else {
		/*pthread_mutex_lock(&mutex_log);
		 log_info(logMSP,"Eliminado archivo %s del directorio %s", dirent->d_name, path_swap);
		 pthread_mutex_unlock(&mutex_log);*/
	}
	free(nombre_archivo);
}

DIR* abrir_directorio_swap(){
	DIR* dir;

	dir = opendir(path_swap); //abro el directorio donde se encuentran los archivos
		if(dir == NULL){
			pthread_mutex_lock(&mutex_log);
			printf("No se pudo abrir el directorio %s\n", path_swap);
			log_error(logMSP,"No se pudo abrir el directorio %s\n", path_swap);
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

					FILE* arch_swap = abrir_archivo(nombre_archivo, logMSP, &mutex_log);

					pag.archivo = arch_swap;

					}
				}
			}
			i++;
		}
	}
	return pag;
}

/************************************************************/

uint32_t obtenerBaseDelSegmento(uint32_t numeroSegmento){
	// char* valorBinario = unaFuncion(numeroSegmento);
	//TODO preugntar que funcion es
	return 0;
}

uint32_t *traducirABinario(uint32_t direccion) {

	uint32_t *binNumInv = malloc(sizeof(uint32_t)*32);
	uint32_t counter;
	uint32_t *binNum= malloc(sizeof(uint32_t)*32) ;
	uint32_t i;

	for ( counter = 0; counter < 32; counter++ ) {
	binNumInv[counter] = direccion % 2;
	direccion = direccion / 2;
	}
	for (i=0;i<32;i++){
		counter = counter -1;
		binNum[counter]=binNumInv[i];

	}
	return binNum;
	}

 t_direccion *traducirDireccion(uint32_t unaDireccion){
	t_direccion *direccionTraducida;
	uint32_t *direccionEnBinario = malloc(sizeof(uint32_t)*32);
	direccionEnBinario=traducirABinario(unaDireccion);
	int i;
	uint32_t *segmento, *pagina, *desplazamiento;
	for(i=0 ;i<12; i++){
		segmento[i]=direccionEnBinario[i];
	}
	for(i=12 ;i<23; i++){
			pagina[i]=direccionEnBinario[i];
		}
	for(i=23 ;i<32; i++){
				desplazamiento[i]=direccionEnBinario[i];
			}
	(*direccionTraducida).segmento=traducirADecimal(segmento);//FIXME definir funcion TraducirADecimal
	(*direccionTraducida).pagina=traducirADecimal(pagina);
	(*direccionTraducida).desplazamiento=traducirADecimal(desplazamiento);

	return direccionTraducida;
}



uint32_t* traducirADecimal(uint32_t *binario){
	return 1;
}
