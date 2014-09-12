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

void ejecutar(int bytecode){
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
