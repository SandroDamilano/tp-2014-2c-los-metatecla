/*
 * operaciones.c
 *
 *  Created on: 07/09/2014
 *      Author: utnso
 */

#include "operaciones.h"

int convertirAString(int bytecode){
	char* string = string_itoa(bytecode);
	int bytecodeLetras;

	if(string_equals_ignore_case(string,"LOAD")){
		bytecodeLetras = LOAD;
	}

	if(string_equals_ignore_case(string,"GETM")){
			bytecodeLetras = GETM;
		}

	if(string_equals_ignore_case(string,"SETM")){
			bytecodeLetras = SETM;
		}

	if(string_equals_ignore_case(string,"MOVR")){
			bytecodeLetras = ADDR;
		}

	if(string_equals_ignore_case(string,"SUBR")){
			bytecodeLetras = SUBR;
		}

	if(string_equals_ignore_case(string,"MULR")){
			bytecodeLetras = MULR;
		}

	if(string_equals_ignore_case(string,"MODR")){
			bytecodeLetras = MODR;
		}

	if(string_equals_ignore_case(string,"DIVR")){
			bytecodeLetras = DIVR;
		}

	if(string_equals_ignore_case(string,"INCR")){
			bytecodeLetras = INCR;
		}

	if(string_equals_ignore_case(string,"DECR")){
			bytecodeLetras = DECR;
		}

	if(string_equals_ignore_case(string,"COMP")){
			bytecodeLetras = COMP;
		}

	if(string_equals_ignore_case(string,"CGEQ")){
			bytecodeLetras = CGEQ;
		}

	if(string_equals_ignore_case(string,"CLEQ")){
			bytecodeLetras = CLEQ;
		}

	if(string_equals_ignore_case(string,"GOTO")){
			bytecodeLetras = GOTO;
		}

	if(string_equals_ignore_case(string,"JMPZ")){
			bytecodeLetras = JMPZ;
		}

	if(string_equals_ignore_case(string,"JPNZ")){
			bytecodeLetras = JPNZ;
		}

	if(string_equals_ignore_case(string,"INTE")){
			bytecodeLetras = INTE;
		}

	if(string_equals_ignore_case(string,"FLCL")){
			bytecodeLetras = FLCL;
		}

	if(string_equals_ignore_case(string,"SHIF")){
			bytecodeLetras = SHIF;
		}

	if(string_equals_ignore_case(string,"NOPP")){
			bytecodeLetras = NOPP;
		}

	if(string_equals_ignore_case(string,"PUSH")){
			bytecodeLetras = PUSH;
		}

	if(string_equals_ignore_case(string,"TAKE")){
			bytecodeLetras = TAKE;
		}

	if(string_equals_ignore_case(string,"XXXX")){
			bytecodeLetras = XXXX;
		}

	if(string_equals_ignore_case(string,"MALC")){
			bytecodeLetras = MALC;
		}

	if(string_equals_ignore_case(string,"FREE")){
			bytecodeLetras = FREE;
		}

	if(string_equals_ignore_case(string,"INNN")){
			bytecodeLetras = INNN;
		}

	if(string_equals_ignore_case(string,"INNC")){
			bytecodeLetras = INNC;
		}

	if(string_equals_ignore_case(string,"OUTN")){
			bytecodeLetras = OUTN;
		}

	if(string_equals_ignore_case(string,"OUTC")){
			bytecodeLetras = OUTC;
		}

	if(string_equals_ignore_case(string,"CREA")){
			bytecodeLetras = CREA;
		}

	if(string_equals_ignore_case(string,"JOIN")){
			bytecodeLetras = JOIN;
		}

	if(string_equals_ignore_case(string,"BLOK")){
			bytecodeLetras = BLOK;
		}

	if(string_equals_ignore_case(string,"WAKE")){
			bytecodeLetras = WAKE;
		}

	return bytecodeLetras;
}

int obtener_bytecode(int codigo[]){
	//reutilizo obtener_direccion con posicion 0
	return 1;
}

int obtener_registro(int codigo[], int posicion){
	return 1;
}

int obtener_numero(int codigo[], int posicion){
	return 1;
}

int obtener_direccion(int codigo[], int posicion){
	return 1;
}

void ejecutarLinea(int codigo[]){
	int bytecode = obtener_bytecode(codigo);
	int bytecodeLetras = convertirAString(bytecode);

	switch(bytecodeLetras){
	case LOAD:

		break;
	/*case GETM:
		break;
	case SETM:
		break;
	case MOVR:
		break;
	case ADDR:
		break;
	case SUBR:
		break;
	case MULR:
		break;
	case MODR:
		break;
	case DIVR:
		break;
	case INCR:
		break;
	case DECR:
		break;
	case COMP:
		break;
	case CGEQ:
		break;
	case CLEQ:
		break;
	case GOTO:
		break;
	case JMPZ:
		break;
	case JPNZ:
		break;
	case INTE:
		break;
	case FLCL:
		break;
	case SHIF:
		break;
	case NOPP:
		break;
	case PUSH:
		break;
	case TAKE:
		break;
	case XXXX:
		break;
	case MALC:
		break;
	case FREE:
		break;
	case INNN:
		break;
	case INNC:
		break;
	case OUTN:
		break;
	case OUTC:
		break;
	case CREA:
		break;
	case JOIN:
		break;
	case BLOK:
		break;
	case WAKE:
		break;*/
	}
}

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

void ejecutar(){
	//socket a MSP con PC de tcb
	//socket de MSP con codigo
	cantidad_lineas_ejecutadas = 0;
	copiar_tcb_a_registros();

	while(1){
	ejecutarLinea(codigo);
	cantidad_lineas_ejecutadas++;

	if(cantidad_lineas_ejecutadas == quantum){
		//actualizo campo de tcb para informar que sale por quantum (habria que consultar a ayudante que se puede agregar eso)
		//socket a Kernel con el tcb
		break;
	} else{
		//socket a MSP con PC
		//socket de MSP con codigo
	}
	}
}
