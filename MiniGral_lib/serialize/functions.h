/*
 * functions.h
 *
 *  Created on: 07/10/2014
 *      Author: utnso
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

	#include "../estructuras_auxiliares.h"

	t_header crearHeader(uint8_t tipoEstructura, uint16_t lengthDatos);
	char * crearDataConHeader(uint8_t tipoEstructura, int length);
	t_stream * serialize(int tipoEstructura, void * estructuraOrigen);

	t_stream * serializeStruct_signal(t_struct_signal * estructuraOrigen);
	t_stream * serializeStruct_numero(t_struct_numero * estructuraOrigen);
	t_stream * serializeStruct_malc(t_struct_numero * estructuraOrigen);
	t_stream * serializeStruct_free(t_struct_numero * estructuraOrigen);
	t_stream * serializeStruct_direccion(t_struct_direccion * estructuraOrigen);
	t_stream * serializeStruct_char(t_struct_char * estructuraOrigen);
	t_stream * serializeStruct_string(t_struct_string * estructuraOrigen);
	t_stream * serializeStruct_sol_bytes(t_struct_sol_bytes * estructuraOrigen);
	t_stream * serializeStruct_env_bytes(t_struct_env_bytes * estructuraOrigen);
	t_stream * serializeStruct_respuestaMSP(t_struct_respuesta_msp * estructuraOrigen);
	t_stream* serializeStruct_tcb(t_struct_tcb* estructuraOrigen);
	t_stream* serializeStruct_tcbQuantum(t_struct_tcb* estructuraOrigen);
	t_stream* serializeStruct_INNN(t_struct_string* estructuraOrigen);
	t_stream* serializeStruct_INNC(t_struct_string* estructuraOrigen);
	t_stream* serializeStruct_OUTN(t_struct_string* estructuraOrigen);
	t_stream* serializeStruct_OUTC(t_struct_string* estructuraOrigen);
	t_stream* serializeStruct_tcbCREA(t_struct_tcb* estructuraOrigen);

	t_header despaquetizarHeader(char * header);
	void * deserialize(uint8_t tipoEstructura, char * dataPaquete, uint16_t length);

	t_struct_signal * deserializeStruct_signal(char * dataPaquete, uint16_t length);
	t_struct_numero * deserializeStruct_numero(char * dataPaquete, uint16_t length);
	t_struct_direccion * deserializeStruct_direccion(char * dataPaquete, uint16_t length);
	t_struct_char * deserializeStruct_char(char * dataPaquete, uint16_t length);
	t_struct_string * deserializeStruct_string(char * dataPaquete, uint16_t length);
	t_struct_sol_bytes * deserializeStruct_sol_bytes(char * dataPaquete, uint16_t length);
	t_struct_respuesta_msp * deserializeStruct_respuestaMSP(char * dataPaquete,uint16_t length);
	t_struct_tcb* deserializeStruct_tcb(char* dataPaquete, uint16_t lenght);



#endif /* FUNCTIONS_H_ */
