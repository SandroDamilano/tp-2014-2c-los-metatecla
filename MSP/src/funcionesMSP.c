/*
 * funcionesMSP.c
 *
 *  Created on: 24/09/2014
 *      Author: utnso
 */
#include "funcionesMSP.h"


int puertoMSP, tamanio_mem_ppal, cant_mem_swap;
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
		list_add(lista_marcos, marco);
		free((*marco).memoria);
		free(marco);
	}
return lista_marcos;
}


void crear_logger(t_log *logger){
	if ((logger = log_create("logMSP.log","MSP",false,LOG_LEVEL_DEBUG)) == NULL) {
		printf("No se pudo crear el logger\n");
	}
}

void crear_archivo_swap_out(t_pagina pagina){
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

	//Abro un nuevo archivo
	FILE* arch_swap = NULL;
	arch_swap = txt_open_for_append(file_name);

	//Pongo la informacion necesaria en el archivo
	txt_write_in_file(arch_swap, "PID:");
	txt_write_in_file(arch_swap, string_itoa(pagina.PID));
	txt_write_in_file(arch_swap, "\n");
	txt_write_in_file(arch_swap, "PAG:");
	txt_write_in_file(arch_swap, string_itoa(pagina.num_pag));
	txt_write_in_file(arch_swap, "\n");
	txt_write_in_file(arch_swap, "SEG:");
	txt_write_in_file(arch_swap, string_itoa(pagina.num_segmento)); //FIXME Lo que va adentro del archivo no es esto, si no la info que esta en memoria

	pthread_mutex_lock(&mutex_log);
	log_debug(logMSP,"Creado el archivo %s",file_name);
	pthread_mutex_unlock(&mutex_log);

	free(file_name);

}

t_pagina leer_y_destruir_archivo_swap_in(int pid){
	struct dirent *dirent;
	DIR* dir;
	t_pagina pag;
	pag.PID = -1;
	pag.num_pag = -1;
	pag.num_segmento = -1;

	dir = opendir(path_swap); //abro el directorio donde se encuentran los archivos
	if(dir == NULL){
		pthread_mutex_lock(&mutex_log);
		printf("No se pudo abrir el directorio %s\n", path_swap);
		log_error(logMSP,"No se pudo abrir el directorio %s\n", path_swap);
		pthread_mutex_unlock(&mutex_log);
		closedir(dir);
		return pag; //Devuelvo t_pagina con valores negativos => Error
	}

	while((dirent = readdir(dir)) != NULL){ //mientras que el directorio tenga archivos para buscar

		char** partes_nombre_archivo = string_split(dirent->d_name, "-"); //agarro el nombre del archivo y divido por cada "-" que tiene el nombre
														   	   	   	   	  //ejemplo: pid:3-pag:5-seg:6 => [pid:3, pag:5, seg:6]
		int i = 0;
		while(partes_nombre_archivo[i] != NULL){//mientras que no se me terminen las partes del nombre del archivo
			char** partes2 = string_split(partes_nombre_archivo[i], ":");//Divido cada parte en otras si esta el separador ":"

			if(string_starts_with(partes2[0],"PID")){
				if(atoi(partes2[1]) == pid){ //Si el pid del nombre del archivo que estoy mirando es el que busco

					char* nombre_archivo = string_new();//Concateno el path con el nombre del archivo seleccionado
					string_append(&nombre_archivo,path_swap);
					string_append(&nombre_archivo,dirent->d_name);

					FILE* arch_swap = fopen(nombre_archivo, "r");
					if(arch_swap == NULL){
						pthread_mutex_lock(&mutex_log);
						printf("Error al abrir el archivo %s\n", dirent->d_name);
						log_error(logMSP, "Error al abrir el archivo %s",dirent->d_name);
						pthread_mutex_unlock(&mutex_log);
						closedir(dir);
						free(nombre_archivo);
						return pag;
					}


					char buffer[100];//Numero arbitrario de caracteres maximo que puede tener el archivo

					char c;
					int j = 0;
					while((c = fgetc(arch_swap)) != EOF){ //copio todoo el archivo al buffer
						buffer[j]=c;
						j++;
					}
					buffer[j] = '\n';

					char** lineas = string_split(buffer,"\n");//dividio al buffer por lineas

					char** secciones = string_split(lineas[0], ":");//divido cada linea segun el separador ":"
					pag.PID = atoi(secciones[1]);

					secciones = string_split(lineas[1],":");
					pag.num_pag = atoi(secciones[1]);

					secciones = string_split(lineas[2],":");
					pag.num_segmento = atoi(secciones[1]);

					if(remove(nombre_archivo) != 0){
							pthread_mutex_lock(&mutex_log);
							printf("No se pudo eliminar el archivo %s\n", dirent->d_name);
							log_error(logMSP,"No se pudo eliminar el archivo %s\n", dirent->d_name);
							pthread_mutex_unlock(&mutex_log);
						} else {
							pthread_mutex_lock(&mutex_log);
							log_info(logMSP,"Eliminado archivo %s del directorio %s", dirent->d_name, path_swap);
							pthread_mutex_unlock(&mutex_log);
						}
					free(nombre_archivo);
					}
				}
			i++;
		}

	}
	closedir(dir);
	return pag;
}

uint32_t obtenerBaseDelSegmento(uint32_t numeroSegmento){
	// char* valorBinario = unaFuncion(numeroSegmento);
	//TODO preugntar que funcion es
	return 0;
}

t_direccion traducirDireccion(uint32_t unaDireccion){
	t_direccion direccionTraducida;
	char* direccionEnBinario = traducirABinario(unaDireccion); //FIXME definir funcion TraducirABinario
	int i;
	char* segmento, pagina, desplazamiento;
	for(i=0 ;i<12; i++){
		segmento[i]=direccionEnBinario[i];
	}
	for(i=12 ;i<23; i++){
			pagina[i]=direccionEnBinario[i];
		}
	for(i=23 ;i<32; i++){
				desplazamiento[i]=direccionEnBinario[i];
			}
	direccionTraducida.segmento=traducirADecimal(segmento);//FIXME definir funcion TraducirADecimal
	direccionTraducida.pagina=traducirADecimal(pagina);
	direccionTraducida.desplazamiento=traducirADecimal(desplazamiento);

	return direccionTraducida;
}
