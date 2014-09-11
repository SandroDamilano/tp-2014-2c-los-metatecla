/*
 * operaciones.h
 *
 *  Created on: 07/09/2014
 *      Author: utnso
 */

#ifndef OPERACIONES_H_
#define OPERACIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>

//t_hilo tcb;

enum bytecodes{
	LOAD,
	GETM,
	SETM,
	MOVR,
	ADDR,
	SUBR,
	MULR,
	MODR,
	DIVR,
	INCR,
	DECR,
	COMP,
	CGEQ,
	CLEQ,
	GOTO,
	JMPZ,
	JPNZ,
	INTE,
	FLCL,
	SHIF,
	NOPP,
	PUSH,
	TAKE,
	XXXX,
}bytecodes;

enum {
	MALC,
	FREE,
	INNN,
	INNC,
	OUTN,
	OUTC,
	CREA,
	JOIN,
	BLOK,
	WAKE,
}intrucciones_protegidas;

/* EL TIPO t_hilo ESTA EN LA SHARED LIBRARY ANSISOP-PANEL QUE NO ME PUDE BAJAR*/

typedef struct {
	uint32_t pid;
	uint32_t tid;
	bool kernel_mode;
	uint32_t segmento_codigo;
	uint32_t segmento_codigo_size;
	uint32_t puntero_instruccion;
	uint32_t base_stack;
	uint32_t cursor_stack;
	int32_t registros[5];
	//t_cola cola;
} t_hilo;


#endif /* OPERACIONES_H_ */
