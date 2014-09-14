/*
 * operaciones.c
 *
 *  Created on: 07/09/2014
 *      Author: utnso
 */

#include "operaciones.h"

void ejecutarLinea(int* bytecode){
	int bytecodeLetras = convertirAString(bytecode);
	t_list* parametros = list_create();
	int32_t reg1, reg2;
	uint32_t numero, direccion;

	switch(bytecodeLetras){
	case LOAD:
		reg1= obtener_registro(param,0);
		numero = obtener_numero(param,1);
		list_add(parametros,&reg1);
		list_add(parametros, &numero);

		ejecucion_instruccion("LOAD",parametros);


		list_clean(parametros);
		break;
	case GETM:
		reg1= obtener_registro(param,0);
		reg2 = obtener_registro(param,1);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("GETM",parametros);


		list_clean(parametros);
		break;
	case SETM:
		numero = obtener_numero(param,4);
		reg1 = obtener_registro(param,6);
		reg2 = obtener_registro(param,7);
		list_add(parametros,&numero);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("SETM",parametros);


		list_clean(parametros);
		break;
	case MOVR:
		reg1 = obtener_registro(param,4);
		reg2 = obtener_registro(param,5);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("MOVR",parametros);


		list_clean(parametros);
		break;
	case ADDR:
		reg1 = obtener_registro(param,4);
		reg2 = obtener_registro(param,5);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("ADDR",parametros);


		list_clean(parametros);
		break;
	case SUBR:
		reg1 = obtener_registro(param,4);
		reg2 = obtener_registro(param,5);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("SUBR",parametros);


		list_clean(parametros);
		break;
	case MULR:
		reg1 = obtener_registro(param,4);
		reg2 = obtener_registro(param,5);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("MULR",parametros);


		list_clean(parametros);
		break;
	case MODR:
		reg1 = obtener_registro(param,4);
		reg2 = obtener_registro(param,5);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("MODR",parametros);


		list_clean(parametros);
		break;
	case DIVR:
		reg1 = obtener_registro(param,4);
		reg2 = obtener_registro(param,5);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("DIVR",parametros);


		list_clean(parametros);
		break;
	case INCR:
		reg1 = obtener_registro(param,4);
		list_add(parametros,&reg1);

		ejecucion_instruccion("INCR",parametros);


		list_clean(parametros);
		break;
	case DECR:
		reg1 = obtener_registro(param,4);
		list_add(parametros,&reg1);

		ejecucion_instruccion("DECR",parametros);


		list_clean(parametros);
		break;
	case COMP:
		reg1 = obtener_registro(param,4);
		reg2 = obtener_registro(param,5);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("COMP",parametros);


		list_clean(parametros);
		break;
	case CGEQ:
		reg1 = obtener_registro(param,4);
		reg2 = obtener_registro(param,5);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("CGEQ",parametros);


		list_clean(parametros);
		break;
	case CLEQ:
		reg1 = obtener_registro(param,4);
		reg2 = obtener_registro(param,5);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("CLEQ",parametros);


		list_clean(parametros);
		break;
	case GOTO:
		reg1 = obtener_registro(param,4);
		list_add(parametros,&reg1);

		ejecucion_instruccion("GOTO",parametros);


		list_clean(parametros);
		break;
	case JMPZ:
		direccion = obtener_direccion(param,4);
		list_add(parametros,&direccion);

		ejecucion_instruccion("JMPZ",parametros);


		list_clean(parametros);
		break;
	case JPNZ:
		direccion = obtener_direccion(param,4);
		list_add(parametros,&direccion);

		ejecucion_instruccion("JPNZ",parametros);


		list_clean(parametros);
		break;
	case INTE:
		direccion = obtener_direccion(param,4);
		list_add(parametros,&direccion);

		ejecucion_instruccion("INTE",parametros);


		list_clean(parametros);
		break;
	case FLCL:
		ejecucion_instruccion("JMPZ",parametros);


		list_clean(parametros);
		break;
	case SHIF:
		numero = obtener_numero(param,4);
		reg1= obtener_registro(param,5);
		list_add(parametros,&numero);
		list_add(parametros, &reg1);

		ejecucion_instruccion("SHIF",parametros);


		list_clean(parametros);
		break;
	case NOPP:
		ejecucion_instruccion("NOPP",parametros);


		list_clean(parametros);
		break;
	case PUSH:
		numero = obtener_numero(param,4);
		reg1= obtener_registro(param,5);
		list_add(parametros,&numero);
		list_add(parametros, &reg1);

		ejecucion_instruccion("PUSH",parametros);


		list_clean(parametros);
		break;
	case TAKE:
		numero = obtener_numero(param,4);
		reg1= obtener_registro(param,5);
		list_add(parametros,&numero);
		list_add(parametros, &reg1);

		ejecucion_instruccion("TAKE",parametros);


		list_clean(parametros);
		break;
	case XXXX:
		ejecucion_instruccion("XXXX",parametros);


		list_clean(parametros);
		break;
	case MALC:
		ejecucion_instruccion("MALC",parametros);


		list_clean(parametros);
		break;
	case FREE:
		ejecucion_instruccion("FREE",parametros);


		list_clean(parametros);
		break;
	case INNN:
		ejecucion_instruccion("INNN",parametros);


		list_clean(parametros);
		break;
	case INNC:
		ejecucion_instruccion("INNC",parametros);


		list_clean(parametros);
		break;
	case OUTN:
		ejecucion_instruccion("OUTN",parametros);


		list_clean(parametros);
		break;
	case OUTC:
		ejecucion_instruccion("OUTC",parametros);


		list_clean(parametros);
		break;
	case CREA:
		ejecucion_instruccion("CREA",parametros);


		list_clean(parametros);
		break;
	case JOIN:
		ejecucion_instruccion("JOIN",parametros);


		list_clean(parametros);
		break;
	case BLOK:
		ejecucion_instruccion("BLOK",parametros);


		list_clean(parametros);
		break;
	case WAKE:
		ejecucion_instruccion("WAKE",parametros);


		list_clean(parametros);
		break;
	}

	list_destroy(parametros); //Que onda la destruccion de los elementos?
}



void ejecutar(){
	//socket a MSP con PC de tcb
	//socket de MSP con bytecode
	int* bytecode = malloc(sizeof(uint32_t));
	cantidad_lineas_ejecutadas = 0;
	copiar_tcb_a_registros();
	comienzo_ejecucion(tcb,quantum);

	while(1){
	ejecutarLinea(bytecode);
	cantidad_lineas_ejecutadas++;

	if(cantidad_lineas_ejecutadas == quantum){
		//actualizo campo de tcb para informar que sale por quantum (habria que consultar a ayudante que se puede agregar eso)
		//socket a Kernel con el tcb
		fin_ejecucion();
		break;
	} else{
		//socket a MSP con PC
		//socket de MSP con codigo
		return;
	}
	}
}
