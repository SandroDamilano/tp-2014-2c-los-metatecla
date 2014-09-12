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

typedef struct {
	uint32_t pid;
	uint32_t tid;
	bool kernel_mode;
	uint32_t M; //Direccion base del segmento de codigo
	uint32_t segmento_codigo_size;
	uint32_t P; //program counter
	uint32_t X; //Direccion base del segmento de stack
	uint32_t S; //Cursor de stack
	int32_t registros[5];
} t_tcb;

extern t_tcb* tcb;
int quantum;
int cantidad_lineas_ejecutadas;
int codigo[10];

void ejecutarLinea(int codigo[]);
int convertirAString(int bytecode);
void ejecutar(void);
int obtener_direccion(int codigo[], int posicion);
int obtener_numero(int codigo[], int posicion);
int obtener_registro(int codigo[], int posicion);
int obtener_bytecode(int codigo[]);

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



#endif /* OPERACIONES_H_ */
