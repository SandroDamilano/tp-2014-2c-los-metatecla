/*
 * operaciones.c
 *
 *  Created on: 07/09/2014
 *      Author: utnso
 */

#include "operaciones.h"

int convertirAString(char* bytecode){
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

char* obtener_bytecode(int* codigo){
	return string_substring((char*)codigo,0,4);
}

int32_t obtener_registro(int* codigo, int posicion){
	char* registro = string_substring((char*)codigo, posicion, posicion+1);
	return registro[0];
}

uint32_t obtener_numero(int* codigo, int posicion){
	char* registro = string_substring((char*)codigo, posicion, posicion+3);
	return atoi(registro);
}

uint32_t obtener_direccion(int codigo[], int posicion){
	return obtener_numero(codigo,posicion);
}



void ejecutarLinea(int* codigo){
	char* bytecode = obtener_bytecode(codigo);
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
	codigo = malloc(7*sizeof(uint32_t));
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
		return;
	}
	}
}
