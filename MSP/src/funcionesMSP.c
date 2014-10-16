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
		(*marco).memoria = memoria + i*256;
		list_add(lista_marcos, marco);
		(*marco).memoria = NULL;
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

	string_append(&file_name, path_swap);
	string_append(&file_name, "PID:");
	string_append(&file_name, string_itoa(pagina.PID));
	string_append(&file_name, "-");
	string_append(&file_name, "SEG:");
	string_append(&file_name, string_itoa(pagina.num_segmento));
	string_append(&file_name, "-");
	string_append(&file_name, "PAG:");
	string_append(&file_name, string_itoa(pagina.num_pag));

	FILE* arch_swap = NULL;
	arch_swap = txt_open_for_append(file_name);

	txt_write_in_file(arch_swap, "PID:");
	txt_write_in_file(arch_swap, string_itoa(pagina.PID));
	txt_write_in_file(arch_swap, "\n");
	txt_write_in_file(arch_swap, "PAG:");
	txt_write_in_file(arch_swap, string_itoa(pagina.num_pag));
	txt_write_in_file(arch_swap, "\n");
	txt_write_in_file(arch_swap, "SEG:");
	txt_write_in_file(arch_swap, string_itoa(pagina.num_segmento));

	free(file_name);

}

t_pagina leer_y_destruir_archivo_swap_in(int pid){
	struct dirent *dirent;
	DIR* dir;
	t_pagina pag;
	pag.PID = -1;
	pag.num_pag = -1;
	pag.num_segmento = -1;

	dir = opendir(path_swap);
	if(dir == NULL){
		printf("No se pudo abrir el directorio %s\n", path_swap);
		return pag;
	}

	while((dirent = readdir(dir)) != NULL){
		printf("%s\n", dirent->d_name);
		char** partes = string_split(dirent->d_name, "-");

		if(string_starts_with(partes[0], "P")){
		int i = 0;
		while(partes[i] != NULL){
			char** partes2 = string_split(partes[i], ":");

			if(string_starts_with(partes2[0],"PID")){
				int pid_arch = atoi(partes2[1]);
				if(pid_arch == pid){
					pag.PID = pid;

					char** partes3 = string_split(partes[i+1],":");
					pag.num_segmento = atoi(partes3[1]);

					partes3 = string_split(partes[i+2], ":");
					pag.num_pag = atoi(partes3[1]);

					int rdo = remove(dirent->d_name);
					if(rdo == 0){
						printf("Archivo se elimino correctamente\n");
						}else {
							printf("No se pudo eliminar el archivo\n");
						}
					}
				}
			i++;
			}
		}

	}
	closedir(dir);
	return pag;
}
