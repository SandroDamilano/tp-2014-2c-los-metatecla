/*
 * auxiliares.c
 *
 *  Created on: 12/09/2014
 *      Author: utnso
 */

#include "auxiliares.h"

void copiar_tcb_a_registros(){
	registros_cpu.I = tcb->pid;
	registros_cpu.K = tcb->kernel_mode;
	registros_cpu.M = tcb->segmento_codigo;
	registros_cpu.P = tcb->puntero_instruccion;
	registros_cpu.S = tcb->cursor_stack;
	registros_cpu.X = tcb->base_stack;
	registros_cpu.registros_programacion[0] = tcb->registros[0];
	registros_cpu.registros_programacion[1] = tcb->registros[1];
	registros_cpu.registros_programacion[2] = tcb->registros[2];
	registros_cpu.registros_programacion[3] = tcb->registros[3];
	registros_cpu.registros_programacion[4] = tcb->registros[4];
	//Creo que falta el flag F que se olvidaron de ponerlo
}

void copiar_registros_a_tcb(){
	tcb->pid = registros_cpu.I;
	tcb->kernel_mode = registros_cpu.K;
	tcb->segmento_codigo = registros_cpu.M;
	tcb->puntero_instruccion = registros_cpu.P;
	tcb->cursor_stack = registros_cpu.S;
	tcb->base_stack = registros_cpu.X;
	tcb->registros[0] = registros_cpu.registros_programacion[0];
	tcb->registros[1] = registros_cpu.registros_programacion[1];
	tcb->registros[2] = registros_cpu.registros_programacion[2];
	tcb->registros[3] = registros_cpu.registros_programacion[3];
	tcb->registros[4] = registros_cpu.registros_programacion[4];
}

int32_t obtener_registro(int* parametros, int posicion){
	char* registro = string_substring((char*)parametros, posicion, posicion+1);
	return registro[0];
}

uint32_t obtener_numero(int* parametros, int posicion){
	char* registro = string_substring((char*)parametros, posicion, posicion+3);
	return atoi(registro);
}

uint32_t obtener_direccion(int* parametros, int posicion){
	return obtener_numero(parametros,posicion);
}

int convertirAString(int* byte){
	char* bytecode = byte;
	int bytecodeLetras;

	if(string_equals_ignore_case(bytecode,"LOAD")){
		bytecodeLetras = LOAD;
	}

	if(string_equals_ignore_case(bytecode,"GETM")){
			bytecodeLetras = GETM;
		}

	if(string_equals_ignore_case(bytecode,"SETM")){
			bytecodeLetras = SETM;
		}

	if(string_equals_ignore_case(bytecode,"MOVR")){
			bytecodeLetras = MOVR;
		}

	if(string_equals_ignore_case(bytecode,"ADDR")){
				bytecodeLetras = ADDR;
			}

	if(string_equals_ignore_case(bytecode,"SUBR")){
			bytecodeLetras = SUBR;
		}

	if(string_equals_ignore_case(bytecode,"MULR")){
			bytecodeLetras = MULR;
		}

	if(string_equals_ignore_case(bytecode,"MODR")){
			bytecodeLetras = MODR;
		}

	if(string_equals_ignore_case(bytecode,"DIVR")){
			bytecodeLetras = DIVR;
		}

	if(string_equals_ignore_case(bytecode,"INCR")){
			bytecodeLetras = INCR;
		}

	if(string_equals_ignore_case(bytecode,"DECR")){
			bytecodeLetras = DECR;
		}

	if(string_equals_ignore_case(bytecode,"COMP")){
			bytecodeLetras = COMP;
		}

	if(string_equals_ignore_case(bytecode,"CGEQ")){
			bytecodeLetras = CGEQ;
		}

	if(string_equals_ignore_case(bytecode,"CLEQ")){
			bytecodeLetras = CLEQ;
		}

	if(string_equals_ignore_case(bytecode,"GOTO")){
			bytecodeLetras = GOTO;
		}

	if(string_equals_ignore_case(bytecode,"JMPZ")){
			bytecodeLetras = JMPZ;
		}

	if(string_equals_ignore_case(bytecode,"JPNZ")){
			bytecodeLetras = JPNZ;
		}

	if(string_equals_ignore_case(bytecode,"INTE")){
			bytecodeLetras = INTE;
		}

	if(string_equals_ignore_case(bytecode,"FLCL")){
			bytecodeLetras = FLCL;
		}

	if(string_equals_ignore_case(bytecode,"SHIF")){
			bytecodeLetras = SHIF;
		}

	if(string_equals_ignore_case(bytecode,"NOPP")){
			bytecodeLetras = NOPP;
		}

	if(string_equals_ignore_case(bytecode,"PUSH")){
			bytecodeLetras = PUSH;
		}

	if(string_equals_ignore_case(bytecode,"TAKE")){
			bytecodeLetras = TAKE;
		}

	if(string_equals_ignore_case(bytecode,"XXXX")){
			bytecodeLetras = XXXX;
		}

	if(string_equals_ignore_case(bytecode,"MALC")){
			bytecodeLetras = MALC;
		}

	if(string_equals_ignore_case(bytecode,"FREE")){
			bytecodeLetras = FREE;
		}

	if(string_equals_ignore_case(bytecode,"INNN")){
			bytecodeLetras = INNN;
		}

	if(string_equals_ignore_case(bytecode,"INNC")){
			bytecodeLetras = INNC;
		}

	if(string_equals_ignore_case(bytecode,"OUTN")){
			bytecodeLetras = OUTN;
		}

	if(string_equals_ignore_case(bytecode,"OUTC")){
			bytecodeLetras = OUTC;
		}

	if(string_equals_ignore_case(bytecode,"CREA")){
			bytecodeLetras = CREA;
		}

	if(string_equals_ignore_case(bytecode,"JOIN")){
			bytecodeLetras = JOIN;
		}

	if(string_equals_ignore_case(bytecode,"BLOK")){
			bytecodeLetras = BLOK;
		}

	if(string_equals_ignore_case(bytecode,"WAKE")){
			bytecodeLetras = WAKE;
		}

	return bytecodeLetras;
}


