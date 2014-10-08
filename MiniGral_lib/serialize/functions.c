/*
 * functions.c
 *
 *  Created on: 07/10/2014
 *      Author: utnso
 */

#include "functions.h"

// --------------------------------------
// to SEND

stream_t *serialize_struct_mensaje (stream_t *msg, int *tamanho) {
	*tamanho = 	strlen(msg->data) + 1;

	char *data = (char*)malloc(*tamanho);
	stream_t *stream = malloc(sizeof(stream_t));
	int offset = 0, tmp_size = 0;

//	memset(data, 0, *tamanho);

	offset += tmp_size;
	memcpy(data + offset, msg->data, tmp_size = strlen(msg->data) + 1);

	stream->data = data;

	return stream;
}

stream_t* do_struct_mensaje(stream_t* datos, int* size)
{
	stream_t *serializado = serialize_struct_mensaje(datos, size);

	return serializado;
}

// --------------------------------------
// to RECIEVE

stream_t *deserialize_struct_mensaje(char *datos) {
	stream_t * self= malloc(sizeof(stream_t));
	int offset = 0, tmp_size = 0;

	offset += tmp_size;
	for(tmp_size = 1; (datos + offset)[tmp_size-1] != '\0'; tmp_size++);
	self->data = malloc(tmp_size);
	memcpy(self->data, datos + offset, tmp_size);

	return self;
}

stream_t* undo_struct_mensaje(char* datos)
{
	stream_t *deserializado = deserialize_struct_mensaje(datos);

	return deserializado;
}
