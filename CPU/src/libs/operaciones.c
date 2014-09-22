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
	char* aux;
	char* param;
	aux = malloc(sizeof(uint32_t));
	param = malloc(sizeof(uint32_t)+2*sizeof(char)); //El maximo de parametros que me puede llegar es un numero y dos registros
	char reg1, reg2;
	int32_t numero;
	uint32_t direccion;

	//siempre antes de hacer nada, tengo que:
	//mandar socket a MSP pidiendo los parametros
	//recibir socket de MSP con los parametros

	switch(bytecodeLetras){
	case LOAD:
		ejecucion_instruccion("LOAD",parametros);

		obtener_registro(param,0,&reg1);
		obtener_numero(param,1,&numero,aux);
		list_add(parametros,&reg1);
		list_add(parametros, &numero);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] = numero;
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case GETM:
		ejecucion_instruccion("GETM",parametros);

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		//TODO ACA VENDRIA ALGO COMO LO DE MOVR, PERO EN ESTE CASO TENGO UN PUNTERO

		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case SETM:
		ejecucion_instruccion("SETM",parametros);

		obtener_numero(param,0,&numero,aux);
		obtener_registro(param,4,&reg1);
		obtener_registro(param,5,&reg2);
		list_add(parametros,&numero);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		memcpy(&registros_cpu.registros_programacion[elegirRegistro(reg1)],&registros_cpu.registros_programacion[elegirRegistro(reg2)],numero);
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case MOVR:
		ejecucion_instruccion("MOVR",parametros);

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] = registros_cpu.registros_programacion[elegirRegistro(reg2)];
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case ADDR:
		ejecucion_instruccion("ADDR",parametros);

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] + registros_cpu.registros_programacion[elegirRegistro(reg2)];
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case SUBR:
		ejecucion_instruccion("SUBR",parametros);

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] - registros_cpu.registros_programacion[elegirRegistro(reg2)];
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case MULR:
		ejecucion_instruccion("MULR",parametros);

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] * registros_cpu.registros_programacion[elegirRegistro(reg2)];
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case MODR:
		ejecucion_instruccion("MODR",parametros);

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] % registros_cpu.registros_programacion[elegirRegistro(reg2)];
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case DIVR:
		ejecucion_instruccion("DIVR",parametros);

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		if(registros_cpu.registros_programacion[elegirRegistro(reg2)] == 0){
			printf("division por cero");
			//TODO ABORTAR POR DIVISION POR CERO
		} else {
			registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] / registros_cpu.registros_programacion[elegirRegistro(reg2)];
		}
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case INCR:
		ejecucion_instruccion("INCR",parametros);

		obtener_registro(param,0,&reg1);
		list_add(parametros,&reg1);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] += 1;
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case DECR:
		ejecucion_instruccion("DECR",parametros);

		obtener_registro(param,0,&reg1);
		list_add(parametros,&reg1);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] -= 1;
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case COMP:
		ejecucion_instruccion("COMP",parametros);

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		registros_cpu.registros_programacion[0] = (registros_cpu.registros_programacion[elegirRegistro(reg1)] == registros_cpu.registros_programacion[elegirRegistro(reg2)] ? 1 : 0);
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case CGEQ:
		ejecucion_instruccion("CGEQ",parametros);

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		registros_cpu.registros_programacion[0] = (registros_cpu.registros_programacion[elegirRegistro(reg1)] >= registros_cpu.registros_programacion[elegirRegistro(reg2)] ? 1 : 0);
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case CLEQ:
		ejecucion_instruccion("CLEQ",parametros);

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		registros_cpu.registros_programacion[0] = (registros_cpu.registros_programacion[elegirRegistro(reg1)] <= registros_cpu.registros_programacion[elegirRegistro(reg2)] ? 1 : 0);
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case GOTO:
		ejecucion_instruccion("GOTO",parametros);

		obtener_registro(param,0,&reg1);
		list_add(parametros,&reg1);

		registros_cpu.P = registros_cpu.registros_programacion[elegirRegistro(reg1)];

		list_clean(parametros);
		break;
	case JMPZ:
		ejecucion_instruccion("JMPZ",parametros);

		obtener_direccion(param,0,&direccion,aux);
		list_add(parametros,&direccion);

		if(registros_cpu.registros_programacion[0] == 0){
			registros_cpu.P = direccion;
		}

		list_clean(parametros);
		break;
	case JPNZ:
		ejecucion_instruccion("JPNZ",parametros);

		obtener_direccion(param,0,&direccion,aux);
		list_add(parametros,&direccion);

		if(registros_cpu.registros_programacion[0] != 0){
					registros_cpu.P = direccion;
		}

		list_clean(parametros);
		break;
	case INTE:
		ejecucion_instruccion("INTE",parametros);

		obtener_direccion(param,0,&direccion,aux);
		list_add(parametros,&direccion);

		//TODO INTE.  TENER EN CUENTA EL PROGRAM COUNTER

		list_clean(parametros);
		break;
	case SHIF:
		ejecucion_instruccion("SHIF",parametros);

		obtener_numero(param,0,&numero,aux);
		obtener_registro(param,4,&reg1);
		list_add(parametros,&numero);
		list_add(parametros, &reg1);

		//TODO SHIF.  TENER EN CUENTA EL PROGRAM COUNTER

		list_clean(parametros);
		break;
	case NOPP:
		ejecucion_instruccion("NOPP",parametros);

		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case PUSH:
		ejecucion_instruccion("PUSH",parametros);

		obtener_numero(param,0,&numero,aux);
		obtener_registro(param,4,&reg1);
		list_add(parametros,&numero);
		list_add(parametros, &reg1);

		int32_t auxiliar_copiar;
		memcpy(&auxiliar_copiar,&registros_cpu.registros_programacion[elegirRegistro(reg1)],numero);

		//socket a MSP enviando auxiliar_copiar con la direccion del cursor de stack

		registros_cpu.S += numero;
		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case TAKE:
		ejecucion_instruccion("TAKE",parametros);

		obtener_numero(param,0,&numero,aux);
		obtener_registro(param,4,&reg1);
		list_add(parametros,&numero);
		list_add(parametros, &reg1);


		//registros_cpu.registros_programacion[elegirRegistro(reg1)]=socket a MSP con el numero y la direccion del cursor de stack.

		registros_cpu.P += 1;

		list_clean(parametros);
		break;
	case XXXX:
		ejecucion_instruccion("XXXX",parametros);

		copiar_registros_a_tcb();
		tcb->cola = EXIT;
		free(bytecode);

		//socket a Kernel enviandole el tcb

		fin_ejecucion();

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
	free(aux);
	free(param);
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
		//a la funcion "terminar_ejecucion" que haga posteriormente, le tengo que pasar el bytecode como parametro y hacerle free()
		fin_ejecucion();
		break;
	} else{
		//socket a MSP con PC
		//socket de MSP con codigo
		return;
	}
	}
}
