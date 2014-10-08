/*
 * functions.h
 *
 *  Created on: 07/10/2014
 *      Author: utnso
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <sys/socket.h>
	#include <errno.h>

	#include "estructuras_auxiliares.h"

	stream_t* undo_struct_mensaje(char* datos);
	stream_t* deserialize_struct_mensaje(char *datos);

	stream_t *do_struct_mensaje(stream_t* datos, int* size);
	stream_t *serialize_struct_mensaje(stream_t *msg, int *tamanho);

#endif /* FUNCTIONS_H_ */
