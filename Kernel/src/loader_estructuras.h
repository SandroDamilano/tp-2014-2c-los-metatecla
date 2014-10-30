/*
 * loader_estructuras.h
 *
 *  Created on: 14/09/2014
 *      Author: utnso
 */

#ifndef LOADER_ESTRUCTURAS_H_
#define LOADER_ESTRUCTURAS_H_

#include <commons/string.h>

#include <sockets/enviar_recibir_paquete.h>
//#include <estructuras_auxiliares.h>

typedef struct __attribute__((__packed__)) PaqueteSolicitudMemoria {
	u_int32_t id_proceso;
	u_int32_t size;
	char* datos;
} paq_SolicitudMemoria;

typedef struct __attribute__((__packed__)) PaqueteRespuestaSolicitudMemoria {
	u_int32_t posicion;
} paq_res_SolicitudMemoria;

typedef enum Operaciones {  //TODO: ordenar y reorganizar numeros!!
	//CONSOLA
	HANDSHAKE_SUCCESS = 1,
	HANDSHAKE_FAIL,
	FILE_LINE,
	FILE_EOF,
	FILE_RECV_SUCCESS,
	FILE_RECV_FAIL,
	END_PROGRAM,
	//MSP
	HANDSHAKE_MSP_SUCCESS = 12,
	HANDSHAKE_MSP_FAIL = 13,
	SOLICITAR_MEMORIA_MSP = 14,
	MEMORIA_MSP_SUCCESS = 15,
	MEMORIA_MSP_FAIL = 16,
} t_operaciones;

char* bufferMSP = NULL;

stream_t* undo_struct_mensaje(char* datos);
stream_t* deserialize_struct_mensaje(char *datos);

stream_t *do_struct_mensaje(stream_t* datos, int* size);
stream_t *serialize_struct_mensaje(mensaje *msg, int *tamanho);

#endif /* LOADER_ESTRUCTURAS_H_ */
