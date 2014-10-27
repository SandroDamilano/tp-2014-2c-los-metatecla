/*
 * funciones_comunes.c
 *
 *  Created on: 24/10/2014
 *      Author: utnso
 */

#include "estructuras_auxiliares.h"

char* leer_archivo(FILE* archivo, long int tamanio_archivo){
	char* codigo = malloc(tamanio_archivo);

	if(archivo == NULL){
		return codigo; //Error
	}

	fseek(archivo, 0L, SEEK_SET);

	char buffer[tamanio_archivo];//Copio el contenido del archivo al buffer

	while(!feof(archivo)){
		fread(buffer, 1, tamanio_archivo, archivo);
	}
	buffer[tamanio_archivo]= '\0';

	memcpy(codigo, buffer, tamanio_archivo);

	return codigo;
}

long int calcular_tamanio_archivo(FILE* archivo){
	fseek(archivo, 0L, SEEK_END); //Averiguo tamaño del archivo
	return ftell(archivo);
}

FILE* abrir_archivo(char* nombre_archivo, t_log* log, pthread_mutex_t *mutex){
	FILE* archivo = fopen(nombre_archivo, "r");

	if(archivo == NULL){
		//pthread_mutex_lock(mutex);
		printf("Error al abrir el archivo %s\n", nombre_archivo);
		//log_error(log, "Error al abrir el archivo %s",nombre_archivo);
		//pthread_mutex_unlock(mutex);
		free(nombre_archivo);
	}

	return archivo;
}
