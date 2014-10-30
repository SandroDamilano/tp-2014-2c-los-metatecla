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

#endif /* LOADER_ESTRUCTURAS_H_ */
