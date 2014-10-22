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
	string_append(&file_name, ".bc");

	//Abro un nuevo archivo
	FILE* arch_swap = NULL;
	arch_swap = fopen(file_name, "wb");

	//Pongo la informacion necesaria en el archivo
	fwrite(pagina.codigo, 1,pagina.tamanio_buffer, arch_swap);

	/*pthread_mutex_lock(&mutex_log);
	log_debug(logMSP,"Creado el archivo %s",file_name);
	pthread_mutex_unlock(&mutex_log);*/
	fclose(arch_swap);
	free(file_name);

}

t_pagina leer_y_destruir_archivo_swap_in(int pid, uint32_t direccionLogica){ //FIXME fijate antes de usar direccion hay que traducirla a fisica como lo hicimos hasta ahora en el destruirSegmento

	struct dirent *dirent;
	DIR* dir;
	t_pagina pag;
	pag.PID = -1;
	pag.num_pag = -1;
	pag.num_segmento = -1;
	pag.codigo = NULL;

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
																		//FIXME aca es donde deberia buscar el archivo que corresponde a la direccion que se pasa por parametro y destruir el archivo encontrado
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
						printf("no se pudo abrir el archivo\n");
					}

					if(arch_swap == NULL){
						pthread_mutex_lock(&mutex_log);
						printf("Error al abrir el archivo %s\n", dirent->d_name);
						log_error(logMSP, "Error al abrir el archivo %s",dirent->d_name);
						pthread_mutex_unlock(&mutex_log);
						closedir(dir);
						free(nombre_archivo);
						return pag;
					}

					//PID, pagina y segmento para completar la t_pagina
					pag.PID = atoi(partes2[1]);

					char** partes3 = string_split(partes_nombre_archivo[i+1], ":");
					pag.num_segmento = atoi(partes3[1]);

					partes3 = string_split(partes_nombre_archivo[i+2], ":");
					pag.num_pag = atoi(partes3[1]);

					//Recupero codigo
					fseek(arch_swap, 0L, SEEK_END); //Averiguo tamaño del archivo
					long tamanio_archivo = ftell(arch_swap);
					fseek(arch_swap, 0L, SEEK_SET);

					char buffer[tamanio_archivo+1];//Copio el contenido del archivo al buffer

					while(!feof(arch_swap)){
						fread(buffer, 1, tamanio_archivo, arch_swap);
					}
					buffer[tamanio_archivo]= '\0';

					pag.tamanio_buffer = tamanio_archivo;//Actualizo la estructura t_pagina
					pag.codigo = malloc(pag.tamanio_buffer);
					memcpy(pag.codigo, buffer, pag.tamanio_buffer);

					fclose(arch_swap);

					if(remove(nombre_archivo) != 0){
							pthread_mutex_lock(&mutex_log);
							printf("No se pudo eliminar el archivo %s\n", dirent->d_name);
							log_error(logMSP,"No se pudo eliminar el archivo %s\n", dirent->d_name);
							pthread_mutex_unlock(&mutex_log);
						} else {
							/*pthread_mutex_lock(&mutex_log);
							log_info(logMSP,"Eliminado archivo %s del directorio %s", dirent->d_name, path_swap);
							pthread_mutex_unlock(&mutex_log);*/
						}
					free(nombre_archivo);
					}
				}
			i++;
		}

	}
	closedir(dir);
	return pag;
} //FIXME la funcion deberia tomar por parametro una direccion tambien para que sepa correctamente que segmento y que pagina de dicho segmento es la que se quiere destruir.
  //FIXME necesitariamos que el leer este separado del destruir ya que para el destruir no necesitamos leer la pagina ni saber saber que tiene adentro
uint32_t obtenerBaseDelSegmento(uint32_t numeroSegmento){
	// char* valorBinario = unaFuncion(numeroSegmento);
	//TODO preugntar que funcion es
	return 0;
}

/* t_direccion *traducirDireccion(uint32_t unaDireccion){
	t_direccion *direccionTraducida;
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

char* traducirABinario(uint32_t unaDireccion){
	return "hola";
}

uint32_t* traducirADecimal(char* segmento){
	return 1;
} */
