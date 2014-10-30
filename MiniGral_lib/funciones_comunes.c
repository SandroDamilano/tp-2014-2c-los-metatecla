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

void copiar_structRecibido_a_tcb(t_hilo* tcb, void* structRecibido){
	tcb->base_stack = ((t_struct_tcb*) structRecibido)->base_stack;
	tcb->cola = ((t_struct_tcb*) structRecibido)->cola;
	tcb->cursor_stack = ((t_struct_tcb*) structRecibido)->cursor_stack;
	tcb->kernel_mode = ((t_struct_tcb*) structRecibido)->kernel_mode;
	tcb->pid = ((t_struct_tcb*) structRecibido)->pid;
	tcb->puntero_instruccion = ((t_struct_tcb*) structRecibido)->puntero_instruccion;
	tcb->registros[0] = ((t_struct_tcb*) structRecibido)->registros[0];
	tcb->registros[1] = ((t_struct_tcb*) structRecibido)->registros[1];
	tcb->registros[2] = ((t_struct_tcb*) structRecibido)->registros[2];
	tcb->registros[3] = ((t_struct_tcb*) structRecibido)->registros[3];
	tcb->registros[4] = ((t_struct_tcb*) structRecibido)->registros[4];
	tcb->segmento_codigo = ((t_struct_tcb*) structRecibido)->segmento_codigo;
	tcb->segmento_codigo_size = ((t_struct_tcb*) structRecibido)->segmento_codigo_size;
	tcb->tid = ((t_struct_tcb*) structRecibido)->tid;
}

void copiar_tcb_a_structTcb(t_hilo* tcb, t_struct_tcb* tcb_enviar){
	tcb_enviar->base_stack = tcb->base_stack;
	tcb_enviar->cola = tcb->cola;
	tcb_enviar->cursor_stack = tcb->cursor_stack;
	tcb_enviar->kernel_mode = tcb->kernel_mode;
	tcb_enviar->pid = tcb->pid;
	tcb_enviar->puntero_instruccion = tcb->puntero_instruccion;
	tcb_enviar->registros[0] = tcb->registros[0];
	tcb_enviar->registros[1] = tcb->registros[1];
	tcb_enviar->registros[2] = tcb->registros[2];
	tcb_enviar->registros[3] = tcb->registros[3];
	tcb_enviar->registros[4] = tcb->registros[4];
	tcb_enviar->segmento_codigo = tcb->segmento_codigo;
	tcb_enviar->segmento_codigo_size = tcb->segmento_codigo_size;
	tcb_enviar->tid = tcb->tid;
}
