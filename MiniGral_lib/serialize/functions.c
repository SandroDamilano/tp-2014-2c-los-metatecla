/*
 * functions.c
 *
 *  Created on: 07/10/2014
 *      Author: utnso
 */

#include "functions.h"

// --------------------------------------
// to SEND

/************ AUXILIARES PARA SERIALIZAR ***********************/

t_header crearHeader(uint8_t tipoEstructura, uint16_t lengthDatos){
	t_header header;
	header.tipoEstructura = tipoEstructura;
	header.length = lengthDatos;
	return header;
}

char * crearDataConHeader(uint8_t tipoEstructura, int length){
	char * data = malloc(length);

	uint16_t lengthDatos = length - sizeof(t_header);

	t_header header = crearHeader(tipoEstructura, lengthDatos); //creo el header

	int tamanoTotal = 0, tamanoDato = 0;

	memcpy(data, &header.tipoEstructura, tamanoDato = sizeof(uint8_t)); //copio el tipoEstructura del header a data
	tamanoTotal = tamanoDato;
	memcpy(data + tamanoTotal, &header.length, tamanoDato = sizeof(uint16_t)); //copio el length del header a data

	return data;
}

/*********************** SERIALIZACIONES *****************************/

// SERIALIZE: SERIALIZA AUTOMATICAMENTE SEGUN TIPO DE ESTRUCTURA QUE PASEMOS POR PARAMETRO

t_stream * serialize(int tipoEstructura, void * estructuraOrigen){
	t_stream * paquete=NULL;

	switch (tipoEstructura){
			case D_STRUCT_SIGNAL:
				paquete = serializeStruct_signal((t_struct_signal *) estructuraOrigen);
				break;
			case D_STRUCT_MALC:
				paquete = serializeStruct_malc((t_struct_numero *) estructuraOrigen);
				break;
			case D_STRUCT_FREE:
				paquete = serializeStruct_free((t_struct_numero *) estructuraOrigen);
				break;
			case D_STRUCT_NUMERO:
				paquete = serializeStruct_numero((t_struct_numero *) estructuraOrigen);
				break;
			case D_STRUCT_DIRECCION:
				paquete = serializeStruct_direccion((t_struct_direccion *) estructuraOrigen);
				break;
			case D_STRUCT_CHAR:
				paquete = serializeStruct_char((t_struct_char *) estructuraOrigen);
				break;
			case D_STRUCT_STRING:
				paquete = serializeStruct_string((t_struct_string *) estructuraOrigen);
				break;
			case D_STRUCT_SOL_BYTES:
				paquete = serializeStruct_sol_bytes((t_struct_sol_bytes *) estructuraOrigen);
				break;
			case D_STRUCT_ENV_BYTES:
				paquete = serializeStruct_env_bytes((t_struct_env_bytes *) estructuraOrigen);
				break;
			case D_STRUCT_RESPUESTA_MSP:
				paquete = serializeStruct_respuestaMSP((t_struct_respuesta_msp *) estructuraOrigen);
				break;

		}

	return paquete;
}

/********************* SERIALIZACIONES POR TIPO DE DATO **********************/
t_stream * serializeStruct_signal(t_struct_signal * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(t_struct_signal);

	char * data = crearDataConHeader(D_STRUCT_SIGNAL, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_signal));	//copio a data la estructura.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_numero(t_struct_numero * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(int32_t);

	char * data = crearDataConHeader(D_STRUCT_NUMERO, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_numero));		//copio a data el numero.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_malc(t_struct_numero * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(int32_t);

	char * data = crearDataConHeader(D_STRUCT_MALC, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_numero));		//copio a data el numero.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_free(t_struct_numero * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(int32_t);

	char * data = crearDataConHeader(D_STRUCT_FREE, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_numero));		//copio a data el numero.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_direccion(t_struct_direccion * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(uint32_t);

	char * data = crearDataConHeader(D_STRUCT_DIRECCION, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_direccion));		//copio a data el numero.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_char(t_struct_char * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(unsigned int);

	char * data = crearDataConHeader(D_STRUCT_CHAR, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), &estructuraOrigen->letra, sizeof(char));		//copio a data el char.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_string(t_struct_string * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + strlen(estructuraOrigen->string) + 1;

	char * data = crearDataConHeader(D_STRUCT_STRING, paquete->length); //creo el data

	int tamanoTotal = sizeof(t_header);

	memcpy(data + tamanoTotal, estructuraOrigen->string, strlen(estructuraOrigen->string)+1);		//copio a data el string.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_sol_bytes(t_struct_sol_bytes * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(t_struct_sol_bytes);

	char * dataNueva; //creo el data
	dataNueva = crearDataConHeader(D_STRUCT_SOL_BYTES, paquete->length);

	memcpy(dataNueva + sizeof(t_header), estructuraOrigen, sizeof(t_struct_sol_bytes));		//copio a data el numero.

	paquete->data = dataNueva;

	return paquete;

}

t_stream * serializeStruct_env_bytes(t_struct_env_bytes * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + (estructuraOrigen->tamanio) + sizeof(t_struct_env_bytes);

	void * data = crearDataConHeader(D_STRUCT_ENV_BYTES, paquete->length); //creo el data

	int tamanoTotal = sizeof(t_header);

	memcpy(data + tamanoTotal, estructuraOrigen,sizeof(t_struct_env_bytes));

	tamanoTotal += sizeof(t_struct_env_bytes);

	memcpy(data + tamanoTotal, estructuraOrigen->buffer, estructuraOrigen->tamanio);		//copio a data el nombre.

	tamanoTotal += estructuraOrigen->tamanio;


	paquete->data = data;

	return paquete;

}

t_stream * serializeStruct_respuestaMSP(t_struct_respuesta_msp * estructuraOrigen){
	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + (estructuraOrigen->tamano_buffer) + sizeof(t_struct_respuesta_msp);

	void * data = crearDataConHeader(D_STRUCT_RESPUESTA_MSP, paquete->length); //creo el data

	int tamanoTotal = sizeof(t_header);

	memcpy(data + tamanoTotal, estructuraOrigen,sizeof(t_struct_respuesta_msp));

	tamanoTotal += sizeof(t_struct_respuesta_msp);

	memcpy(data + tamanoTotal, estructuraOrigen->buffer, estructuraOrigen->tamano_buffer);		//copio a data el nombre.

	tamanoTotal += estructuraOrigen->tamano_buffer;

	paquete->data = data;

	return paquete;
}

// --------------------------------------
// to RECIEVE

/******************* AUXILIAR PARA DESERIALIZAR *************************************/
t_header despaquetizarHeader(char * header){
	t_header estructuraHeader;

	int tamanoTotal = 0, tamanoDato = 0;
	memcpy(&estructuraHeader.tipoEstructura, header + tamanoTotal, tamanoDato = sizeof(uint8_t));
	tamanoTotal = tamanoDato;
	memcpy(&estructuraHeader.length, header + tamanoTotal, tamanoDato = sizeof(uint16_t));

	return estructuraHeader;
}

/************************** DESERIALIZACIONES ***************************************/

// DESERIALIZE: DESERIALIZAR AUTOMATICAMENTE SEGUN TIPO DE DATO QUE LE PASEMOS POR PARAMETRO
void * deserialize(uint8_t tipoEstructura, char * dataPaquete, uint16_t length){

	void * estructuraDestino;

	switch (tipoEstructura){
			case D_STRUCT_SIGNAL:
				estructuraDestino = deserializeStruct_signal(dataPaquete, length);
				break;
			case D_STRUCT_MALC:
			case D_STRUCT_FREE:
			case D_STRUCT_NUMERO:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_DIRECCION:
				estructuraDestino = deserializeStruct_direccion(dataPaquete, length);
				break;
			case D_STRUCT_CHAR:
				estructuraDestino = deserializeStruct_char(dataPaquete,length);
				break;
			case D_STRUCT_STRING:
				estructuraDestino = deserializeStruct_string(dataPaquete,length);
				break;
			case D_STRUCT_SOL_BYTES:
				estructuraDestino = deserializeStruct_sol_bytes(dataPaquete,length);
				break;
			case D_STRUCT_RESPUESTA_MSP:
				estructuraDestino = deserializeStruct_respuestaMSP(dataPaquete,length);
				break;
				}

	return estructuraDestino;
}

/************************ DESERIALIZACIONES POR TIPO DE DATO ***********************/
t_struct_signal * deserializeStruct_signal(char * dataPaquete, uint16_t length){
	t_struct_signal * estructuraDestino = malloc(sizeof(t_struct_signal));

	memcpy(estructuraDestino, dataPaquete, sizeof(t_struct_signal)); //copio el data del paquete a la estructura

	return estructuraDestino;
}

t_struct_numero * deserializeStruct_numero(char * dataPaquete, uint16_t length){
	t_struct_numero * estructuraDestino = malloc(sizeof(t_struct_numero));

	memcpy(estructuraDestino, dataPaquete, sizeof(int32_t)); //copio el data del paquete a la estructura.

	return estructuraDestino;
}

t_struct_direccion * deserializeStruct_direccion(char * dataPaquete, uint16_t length){
	t_struct_direccion * estructuraDestino = malloc(sizeof(t_struct_direccion));

	memcpy(estructuraDestino, dataPaquete, sizeof(uint32_t)); //copio el data del paquete a la estructura.

	return estructuraDestino;
}

t_struct_char * deserializeStruct_char(char * dataPaquete, uint16_t length){
	t_struct_char * estructuraDestino = malloc(sizeof(t_struct_char));

	memcpy(&estructuraDestino->letra, dataPaquete, sizeof(char)); //copio la letra a la estructura

	return estructuraDestino;
}

t_struct_string * deserializeStruct_string(char * dataPaquete, uint16_t length){
	t_struct_string * estructuraDestino = malloc(sizeof(t_struct_string));

	int tamanoTotal = 0, tamanoDato = 0;

	tamanoTotal = tamanoDato;

	for(tamanoDato = 1; (dataPaquete + tamanoTotal)[tamanoDato -1] != '\0';tamanoDato++); 	//incremento tamanoDato, hasta el tamaño del nombre.

	estructuraDestino->string = malloc(tamanoDato);
	memcpy(estructuraDestino->string, dataPaquete + tamanoTotal, tamanoDato); //copio el string a la estructura

	return estructuraDestino;
}

t_struct_sol_bytes * deserializeStruct_sol_bytes(char * dataPaquete,uint16_t length){
	t_struct_sol_bytes * estructuraDestino = malloc(sizeof(t_struct_sol_bytes));

	memcpy(estructuraDestino, dataPaquete, sizeof(t_struct_sol_bytes)); //copio el data del paquete a la estructura.

	return estructuraDestino;
}

t_struct_respuesta_msp * deserializeStruct_respuestaMSP(char * dataPaquete,uint16_t length){
	void * buffer = malloc(length - sizeof(t_struct_respuesta_msp));
	t_struct_respuesta_msp * estructuraDestino = malloc(sizeof(t_struct_respuesta_msp));

	memcpy(estructuraDestino, dataPaquete, sizeof(t_struct_respuesta_msp)); //copio el data del paquete a la estructura.
	memcpy(buffer, dataPaquete + sizeof(t_struct_respuesta_msp), length - sizeof(t_struct_respuesta_msp)); // copiamos los bytes que queriamos enviar

	estructuraDestino->buffer = buffer;

	return estructuraDestino;
}

