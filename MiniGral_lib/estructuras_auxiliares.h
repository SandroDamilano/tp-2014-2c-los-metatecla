/*
 * estructuras_auxiliares.h
 *
 *  Created on: 14/09/2014
 *      Author: utnso
 *
 *	NOTA:
 *	ACA IRAN LAS ESTRUCTURAS NECESARIAS DE PROPOSITO GENERAL PARA TODOS LOS PROCESOS.
 *	AL QUERER ACCEDER A ESTAS DESDE CUALQUIER SOURCE SOLO HAY QUE HACER #include "estructuras_auxiliares.h"
 *	Y DESDE EL PROYECTO LINKEAR A ESTA SHARED LIBRARY
 */

#ifndef ESTRUCTURAS_AUXILIARES_H_
#define ESTRUCTURAS_AUXILIARES_H_

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sys/socket.h>
	#include <errno.h>
	#include <stdint.h>

	#include <commons/log.h>
	#include <commons/config.h>

	#define S_ERROR 99	// Utilizada en socket_recibirSeñal, para decir que lo que se recibio no era una señal

// Vars globales ( referenciada por todos los procesos)
	t_config* config_file;
	t_log* logger;

// Estructuras necesarias para serializar

	typedef uint8_t t_tipoEstructura;

	typedef struct Stream {
		int length;
		char* data;
	} t_stream;


	enum{
		D_STRUCT_SIGNAL=0,
		D_STRUCT_NUMERO=1,
		D_STRUCT_DIRECCION=2,
		D_STRUCT_CHAR=3,
		D_STRUCT_STRING=4,
		D_STRUCT_SOL_BYTES=5,
		D_STRUCT_RESPUESTA_MSP=6,
		D_STRUCT_ENV_BYTES=7,

		//comunicacion entre kernel y consola
		HANDSHAKE_SUCCESS = 100,// TODO: serializar segun estas señales
		HANDSHAKE_FAIL = 101,
		ENVIAR_IMPRIMIR_TEXTO = 102,
		FILE_LINE = 103,
		FILE_EOF = 104,
		FILE_RECV_SUCCESS = 105,
		FILE_RECV_FAIL = 106,
		END_PROGRAM = 107,
	} t_operaciones;

	// Header de stream
	typedef struct {
		uint8_t tipoEstructura;
		uint16_t length;
	} __attribute__ ((__packed__)) t_header;


// Estructuras segun tipo de datos a enviar por sockets

	typedef struct struct_numero {
		int32_t numero;
	} __attribute__ ((__packed__)) t_struct_numero;

	typedef struct struct_direccion {
		uint32_t numero;
	} __attribute__ ((__packed__)) t_struct_direccion;

	typedef struct struct_char {
		char letra;
	} __attribute__ ((__packed__)) t_struct_char;

	typedef struct struct_string {
		char * string;
	} __attribute__ ((__packed__)) t_struct_string;

	typedef struct struct_signal {
		uint32_t signal;
	} __attribute__ ((__packed__)) t_struct_signal;

	typedef struct struct_sol_bytes{
		uint32_t base;
		uint32_t offset;
		uint32_t tamanio;
	}__attribute__ ((__packed__)) t_struct_sol_bytes;

	typedef struct struct_env_bytes{
		uint32_t base;
		uint32_t offset;
		uint32_t tamanio;
		void* buffer;
	}__attribute__ ((__packed__)) t_struct_env_bytes;

	typedef struct struct_respuesta_msp{
		void* buffer;
		uint32_t tamano_buffer;
	}__attribute__((__packed__)) t_struct_respuesta_msp;

	char* leer_archivo(FILE* archivo, long int tamanio_archivo);
	long int calcular_tamanio_archivo(FILE* archivo);
	FILE* abrir_archivo(char* nombre_archivo, t_log* log, pthread_mutex_t *mutex);


#endif /* ESTRUCTURAS_AUXILIARES_H_ */
