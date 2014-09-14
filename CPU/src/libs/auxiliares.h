/*
 * auxiliares.h
 *
 *  Created on: 12/09/2014
 *      Author: utnso
 */

#ifndef AUXILIARES_H_
#define AUXILIARES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>
#include "ansisop-panel/cpu.h"

extern t_hilo* tcb;
t_registros_cpu registros_cpu;
int quantum;
int cantidad_lineas_ejecutadas;
extern int* param;

uint32_t obtener_direccion(int* parametros, int posicion);
uint32_t obtener_numero(int* parametros, int posicion);
int32_t obtener_registro(int* parametros, int posicion);
void copiar_tcb_a_registros();
void copiar_registros_a_tcb();

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
}bytecodes;



#endif /* AUXILIARES_H_ */
