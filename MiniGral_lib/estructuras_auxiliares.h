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

	#include <stdint.h>
	#include <commons/log.h>
	#include <commons/config.h>

	#include <sockets/enviar_recibir_paquete.h>

	typedef struct Stream {
		char* data;
	} stream_t;

	// Vars globales ( referenciada por todos los procesos)
	t_config* config_file;
	t_log* logger;

#endif /* ESTRUCTURAS_AUXILIARES_H_ */
