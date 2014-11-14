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
				paquete = serializeStruct_malc((t_struct_malloc *) estructuraOrigen);
				break;
			case D_STRUCT_FREE:
				paquete = serializeStruct_free((t_struct_free *) estructuraOrigen);
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
			case D_STRUCT_TCB:
				paquete = serializeStruct_tcb((t_struct_tcb *) estructuraOrigen);
				break;
			case D_STRUCT_TCB_QUANTUM:
				paquete = serializeStruct_tcbQuantum((t_struct_tcb *) estructuraOrigen);
				break;
			case D_STRUCT_INNN:
				paquete = serializeStruct_INNN((t_struct_numero *) estructuraOrigen);
				break;
			case D_STRUCT_INNC:
				paquete = serializeStruct_INNC((t_struct_string *) estructuraOrigen);
				break;
			case D_STRUCT_OUTN:
				paquete = serializeStruct_OUTN((t_struct_numero *) estructuraOrigen);
				break;
			case D_STRUCT_OUTC:
				paquete = serializeStruct_OUTC((t_struct_string *) estructuraOrigen);
				break;
			case D_STRUCT_TCB_CREA:
				paquete = serializeStruct_tcbCREA((t_struct_tcb *) estructuraOrigen);
				break;
			case D_STRUCT_JOIN:
				paquete = serializeStruct_join((t_struct_join *) estructuraOrigen);
				break;
			case D_STRUCT_INTE:
				paquete = serializeStruct_inte((t_struct_direccion *) estructuraOrigen);
				break;
			case D_STRUCT_BLOCK:
				paquete = serializeStruct_block((t_struct_numero *) estructuraOrigen);
				break;
			case D_STRUCT_WAKE:
				paquete = serializeStruct_wake((t_struct_numero *) estructuraOrigen);
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

t_stream * serializeStruct_block(t_struct_numero * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(int32_t);

	char * data = crearDataConHeader(D_STRUCT_BLOCK, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_numero));		//copio a data el numero.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_wake(t_struct_numero * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(int32_t);

	char * data = crearDataConHeader(D_STRUCT_WAKE, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_numero));		//copio a data el numero.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_malc(t_struct_malloc * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + 2*sizeof(uint32_t);

	char * data = crearDataConHeader(D_STRUCT_MALC, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_malloc));		//copio a data el numero.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_free(t_struct_free * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + 2*sizeof(uint32_t);

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

t_stream* serializeStruct_env_bytes(t_struct_env_bytes* estructuraOrigen){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(uint32_t) + sizeof(uint32_t) +sizeof(uint32_t) + estructuraOrigen->tamanio;

	char* data = crearDataConHeader(D_STRUCT_ENV_BYTES, paquete->length);

	int tamanoTotal = sizeof(t_header), tamanoDato = 0;

	memcpy(data + tamanoTotal, &estructuraOrigen->PID, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->base, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, &estructuraOrigen->tamanio, tamanoDato= sizeof(uint32_t));

	tamanoTotal+=tamanoDato;

	memcpy(data + tamanoTotal, estructuraOrigen->buffer, tamanoDato = (estructuraOrigen->tamanio));

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

t_stream* serializeStruct_tcb(t_struct_tcb* estructuraOrigen){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(t_struct_tcb);

	char* data = crearDataConHeader(D_STRUCT_TCB, paquete->length);

	int tamDato = 0;
	int tamTot = sizeof(t_header);
	memcpy(data + tamTot , &estructuraOrigen->base_stack, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->cola, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->cursor_stack, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->pid, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->puntero_instruccion, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->segmento_codigo, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->segmento_codigo_size, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->tid, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->kernel_mode, tamDato = sizeof(bool));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->registros[0], tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->registros[1], tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->registros[2], tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->registros[3], tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->registros[4], tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	paquete->data = data;

	return paquete;
}

t_stream* serializeStruct_tcbQuantum(t_struct_tcb* estructuraOrigen){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(t_struct_tcb);

	char* data = crearDataConHeader(D_STRUCT_TCB_QUANTUM, paquete->length);

	int tamDato = 0;
	int tamTot = sizeof(t_header);
	memcpy(data + tamTot , &estructuraOrigen->base_stack, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->cola, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->cursor_stack, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->pid, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->puntero_instruccion, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->segmento_codigo, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->segmento_codigo_size, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->tid, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->kernel_mode, tamDato = sizeof(bool));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->registros[0], tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->registros[1], tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->registros[2], tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->registros[3], tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(data + tamTot , &estructuraOrigen->registros[4], tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	paquete->data = data;

	return paquete;
}

t_stream* serializeStruct_INNN(t_struct_numero* estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(int32_t);

	char * data = crearDataConHeader(D_STRUCT_INNN, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_numero));		//copio a data el numero.

	paquete->data = data;

	return paquete;}

t_stream* serializeStruct_INNC(t_struct_string* estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + strlen(estructuraOrigen->string) + 1;

	char * data = crearDataConHeader(D_STRUCT_INNC, paquete->length); //creo el data

	int tamanoTotal = sizeof(t_header);

	memcpy(data + tamanoTotal, estructuraOrigen->string, strlen(estructuraOrigen->string)+1);		//copio a data el string.

	paquete->data = data;

	return paquete;
}

t_stream* serializeStruct_OUTN(t_struct_numero* estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(int32_t);

	char * data = crearDataConHeader(D_STRUCT_OUTN, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_numero));		//copio a data el numero.

	paquete->data = data;

	return paquete;
}

t_stream* serializeStruct_OUTC(t_struct_string* estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + strlen(estructuraOrigen->string) + 1;

	char * data = crearDataConHeader(D_STRUCT_OUTC, paquete->length); //creo el data

	int tamanoTotal = sizeof(t_header);

	memcpy(data + tamanoTotal, estructuraOrigen->string, strlen(estructuraOrigen->string)+1);		//copio a data el string.

	paquete->data = data;

	return paquete;
}

t_stream* serializeStruct_tcbCREA(t_struct_tcb* estructuraOrigen){

	t_stream* paquete = malloc(sizeof(t_stream));

	paquete->length = sizeof(t_header) + sizeof(t_struct_tcb);

	char* data = crearDataConHeader(D_STRUCT_TCB_CREA, paquete->length);

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_tcb));

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_join(t_struct_join * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + 2*sizeof(uint32_t);

	char * data = crearDataConHeader(D_STRUCT_JOIN, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_join));		//copio a data el numero.

	paquete->data = data;

	return paquete;
}

t_stream * serializeStruct_inte(t_struct_direccion * estructuraOrigen){

	t_stream * paquete = malloc(sizeof(t_stream));		//creo el paquete

	paquete->length = sizeof(t_header) + sizeof(uint32_t);

	char * data = crearDataConHeader(D_STRUCT_INTE, paquete->length); //creo el data

	memcpy(data + sizeof(t_header), estructuraOrigen, sizeof(t_struct_direccion));		//copio a data el numero.

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
			case D_STRUCT_INNN:
			case D_STRUCT_OUTN:
			case D_STRUCT_WAKE:
			case D_STRUCT_BLOCK:
			case D_STRUCT_NUMERO:
				estructuraDestino = deserializeStruct_numero(dataPaquete, length);
				break;
			case D_STRUCT_MALC:
				estructuraDestino = deserializeStruct_malloc(dataPaquete, length);
				break;
			case D_STRUCT_FREE:
				estructuraDestino = deserializeStruct_free(dataPaquete, length);
				break;
			case D_STRUCT_INTE:
			case D_STRUCT_DIRECCION:
				estructuraDestino = deserializeStruct_direccion(dataPaquete, length);
				break;
			case D_STRUCT_CHAR:
				estructuraDestino = deserializeStruct_char(dataPaquete,length);
				break;
			case D_STRUCT_INNC:
			case D_STRUCT_OUTC:
			case D_STRUCT_STRING:
				estructuraDestino = deserializeStruct_string(dataPaquete,length);
				break;
			case D_STRUCT_SOL_BYTES:
				estructuraDestino = deserializeStruct_sol_bytes(dataPaquete,length);
				break;
			case D_STRUCT_ENV_BYTES:
				estructuraDestino = deserializeStruct_env_bytes(dataPaquete,length);
				break;
			case D_STRUCT_RESPUESTA_MSP:
				estructuraDestino = deserializeStruct_respuestaMSP(dataPaquete,length);
				break;
			case D_STRUCT_TCB:
			case D_STRUCT_TCB_QUANTUM:
			case D_STRUCT_TCB_CREA:
				estructuraDestino = deserializeStruct_tcb(dataPaquete,length);
				break;
			case D_STRUCT_JOIN:
				estructuraDestino = deserializeStruct_join(dataPaquete, length);
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

t_struct_env_bytes* deserializeStruct_env_bytes(char* dataPaquete, uint16_t length){
	t_struct_env_bytes* estructuraDestino = malloc(sizeof(t_struct_env_bytes));

	int tamanoDato = 0, tamanoTotal = 0;

	memcpy(&estructuraDestino->PID,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->base,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	memcpy(&estructuraDestino->tamanio,dataPaquete+tamanoTotal,tamanoDato=sizeof(uint32_t));

	tamanoTotal+= tamanoDato;

	tamanoDato=estructuraDestino->tamanio;

	estructuraDestino->buffer= malloc(tamanoDato);
	memcpy(estructuraDestino->buffer, dataPaquete + tamanoTotal, tamanoDato);

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

t_struct_tcb* deserializeStruct_tcb(char* dataPaquete, uint16_t lenght){
	t_struct_tcb* estructuraDestino = malloc(sizeof(t_struct_tcb));

	int tamDato = 0;
	int tamTot = 0;
	memcpy(&estructuraDestino->base_stack, dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->cola, dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->cursor_stack, dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->pid, dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->puntero_instruccion, dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->segmento_codigo, dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->segmento_codigo_size, dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->tid, dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->kernel_mode, dataPaquete + tamTot, tamDato = sizeof(bool));
	tamTot += tamDato;

	memcpy(&estructuraDestino->registros[0], dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->registros[1], dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->registros[2], dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->registros[3], dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	memcpy(&estructuraDestino->registros[4], dataPaquete + tamTot, tamDato = sizeof(uint32_t));
	tamTot += tamDato;

	return estructuraDestino;
}

t_struct_join * deserializeStruct_join(char * dataPaquete, uint16_t length){
	t_struct_join * estructuraDestino = malloc(sizeof(t_struct_join));

	memcpy(estructuraDestino, dataPaquete, 2*sizeof(uint32_t)); //copio el data del paquete a la estructura.

	return estructuraDestino;
}

t_struct_malloc * deserializeStruct_malloc(char * dataPaquete, uint16_t length){
	t_struct_malloc * estructuraDestino = malloc(sizeof(t_struct_malloc));

	memcpy(estructuraDestino, dataPaquete, 2*sizeof(uint32_t)); //copio el data del paquete a la estructura.

	return estructuraDestino;
}

t_struct_free * deserializeStruct_free(char * dataPaquete, uint16_t length){
	t_struct_free * estructuraDestino = malloc(sizeof(t_struct_free));

	memcpy(estructuraDestino, dataPaquete, 2*sizeof(uint32_t)); //copio el data del paquete a la estructura.

	return estructuraDestino;
}

