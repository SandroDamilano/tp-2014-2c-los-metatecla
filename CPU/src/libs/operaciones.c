/*
 * operaciones.c
 *
 *  Created on: 07/09/2014
 *      Author: utnso
 */

#include "operaciones.h"
#include <estructuras_auxiliares.h>

void ejecutarLinea(int* bytecode){
	int bytecodeLetras = convertirAString(bytecode);
	t_list* parametros = list_create();
	char* aux;
	char* param;
	aux = malloc(sizeof(uint32_t));
	param = malloc(sizeof(int32_t)+2*sizeof(char)); //El maximo de parametros que me puede llegar es un numero y dos registros
	char reg1, reg2;
	int32_t numero;
	uint32_t direccion;
	int resultado;

	t_struct_sol_bytes* datos_solicitados = malloc(sizeof(t_struct_sol_bytes));
	t_struct_env_bytes* datos_enviados = malloc(sizeof(t_struct_env_bytes));
	void* datos_recibidos;
	void * structRecibido;
	t_tipoEstructura tipo_struct;

	incrementar_pc(4); //corro el PC 4 lugares por el bytecode

	//socket a MSP pidiendo parametros
	//socket de MSP recibiendolos

	switch(bytecodeLetras){
	case LOAD:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1 + 4; //registro + numero

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(sizeof(int32_t)+sizeof(char)); //registro + numero
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_num(datos_recibidos,1,&numero);
		list_add(parametros,&reg1);
		list_add(parametros, &numero);

		ejecucion_instruccion("LOAD",parametros);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] = numero;

		incrementar_pc(sizeof(char) + sizeof(int32_t)); //registro + numero

		list_clean(parametros);
		break;
	case GETM:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(2*sizeof(char)); //registro + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("GETM",parametros);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] = registros_cpu.registros_programacion[elegirRegistro(reg2)];

		incrementar_pc(2*sizeof(char)); //registro + registro

		list_clean(parametros);
		break;
	case SETM:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 4 + 1 + 1; //numero + registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(sizeof(int32_t) + 2*sizeof(char)); //numero + registro + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_numero(param,0,&numero,aux);
		obtener_registro(param,4,&reg1);
		obtener_registro(param,5,&reg2);
		list_add(parametros,&numero);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("SETM",parametros);

		memcpy(&registros_cpu.registros_programacion[elegirRegistro(reg1)],&registros_cpu.registros_programacion[elegirRegistro(reg2)],numero);

		incrementar_pc(sizeof(int32_t) + 2*sizeof(char));

		list_clean(parametros);
		break;
	case MOVR:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(2*sizeof(char)); //registro + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("MOVR",parametros);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] = registros_cpu.registros_programacion[elegirRegistro(reg2)];

		incrementar_pc(sizeof(char)*2);

		list_clean(parametros);
		break;
	case ADDR:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(2*sizeof(char)); //registro + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("ADDR",parametros);

		registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] + registros_cpu.registros_programacion[elegirRegistro(reg2)];

		incrementar_pc(sizeof(char)*2);

		list_clean(parametros);
		break;
	case SUBR:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(2*sizeof(char)); //registro + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("SUBR",parametros);

		registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] - registros_cpu.registros_programacion[elegirRegistro(reg2)];

		incrementar_pc(sizeof(char)*2);

		list_clean(parametros);
		break;
	case MULR:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(2*sizeof(char)); //registro + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("MULR",parametros);

		registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] * registros_cpu.registros_programacion[elegirRegistro(reg2)];

		incrementar_pc(sizeof(char)*2);

		list_clean(parametros);
		break;
	case MODR:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(2*sizeof(char)); //registro + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("MODR",parametros);

		registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] % registros_cpu.registros_programacion[elegirRegistro(reg2)];

		incrementar_pc(sizeof(char)*2);

		list_clean(parametros);
		break;
	case DIVR:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(2*sizeof(char)); //registro + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);

		ejecucion_instruccion("DIVR",parametros);

		if(registros_cpu.registros_programacion[elegirRegistro(reg2)] == 0){
			printf("division por cero");
			//TODO ABORTAR POR DIVISION POR CERO
		} else {
			registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] / registros_cpu.registros_programacion[elegirRegistro(reg2)];
		}

		incrementar_pc(sizeof(char)*2);

		list_clean(parametros);
		break;
	case INCR:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1; //registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(sizeof(char)); //registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		list_add(parametros,&reg1);
		ejecucion_instruccion("INCR",parametros);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] += 1;

		incrementar_pc(sizeof(char));

		list_clean(parametros);
		break;
	case DECR:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1; //registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(sizeof(char)); //registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		list_add(parametros,&reg1);
		ejecucion_instruccion("DECR",parametros);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] -= 1;

		incrementar_pc(sizeof(char));

		list_clean(parametros);
		break;
	case COMP:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(2*sizeof(char)); //registro + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);
		ejecucion_instruccion("COMP",parametros);

		registros_cpu.registros_programacion[0] = (registros_cpu.registros_programacion[elegirRegistro(reg1)] == registros_cpu.registros_programacion[elegirRegistro(reg2)] ? 1 : 0);

		incrementar_pc(sizeof(char)*2);

		list_clean(parametros);
		break;
	case CGEQ:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(2*sizeof(char)); //registro + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);
		ejecucion_instruccion("CGEQ",parametros);

		registros_cpu.registros_programacion[0] = (registros_cpu.registros_programacion[elegirRegistro(reg1)] >= registros_cpu.registros_programacion[elegirRegistro(reg2)] ? 1 : 0);

		incrementar_pc(sizeof(char)*2);

		list_clean(parametros);
		break;
	case CLEQ:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(2*sizeof(char)); //registro + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		obtener_registro(param,1,&reg2);
		list_add(parametros,&reg1);
		list_add(parametros, &reg2);
		ejecucion_instruccion("CLEQ",parametros);

		registros_cpu.registros_programacion[0] = (registros_cpu.registros_programacion[elegirRegistro(reg1)] <= registros_cpu.registros_programacion[elegirRegistro(reg2)] ? 1 : 0);

		incrementar_pc(sizeof(char)*2);

		list_clean(parametros);
		break;
	case GOTO:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 1; //registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(sizeof(char)); //registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_registro(param,0,&reg1);
		list_add(parametros,&reg1);
		ejecucion_instruccion("GOTO",parametros);

		registros_cpu.P = registros_cpu.registros_programacion[elegirRegistro(reg1)];

		list_clean(parametros);
		break;
	case JMPZ:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 4; //direccion

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(sizeof(uint32_t)); //direccion
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_direccion(param,0,&direccion,aux);
		list_add(parametros,&direccion);
		ejecucion_instruccion("JMPZ",parametros);

		if(registros_cpu.registros_programacion[0] == 0){
			registros_cpu.P = direccion;
		}

		list_clean(parametros);
		break;
	case JPNZ:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 4; //direccion

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(sizeof(uint32_t)); //direccion
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_direccion(param,0,&direccion,aux);
		list_add(parametros,&direccion);
		ejecucion_instruccion("JPNZ",parametros);

		if(registros_cpu.registros_programacion[0] != 0){
					registros_cpu.P = direccion;
		}

		list_clean(parametros);
		break;
	case INTE:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 4; //direccion

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(sizeof(uint32_t)); //direccion
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_direccion(param,0,&direccion,aux);
		list_add(parametros,&direccion);
		ejecucion_instruccion("INTE",parametros);

		//TODO INTE.  TENER EN CUENTA EL PROGRAM COUNTER

		list_clean(parametros);
		break;
	case SHIF:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 4 + 1; //numero + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(sizeof(int32_t) + sizeof(char)); //numero + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;


		obtener_numero(param,0,&numero,aux);
		obtener_registro(param,4,&reg1);
		list_add(parametros,&numero);
		list_add(parametros, &reg1);
		ejecucion_instruccion("SHIF",parametros);

		//TODO SHIF.

		incrementar_pc(5);

		list_clean(parametros);
		break;
	case NOPP:
		ejecucion_instruccion("NOPP",parametros);

		list_clean(parametros);
		break;
	case PUSH:

		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 4 + 1; //numero + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(sizeof(char) + sizeof(int32_t)); //numero + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_numero(param,0,&numero,aux);
		obtener_registro(param,4,&reg1);
		list_add(parametros,&numero);
		list_add(parametros, &reg1);
		ejecucion_instruccion("PUSH",parametros);

		int32_t auxiliar_copiar;
		memcpy(&auxiliar_copiar,&registros_cpu.registros_programacion[elegirRegistro(reg1)],numero);

		//socket a MSP enviando auxiliar_copiar con la direccion del cursor de stack
		datos_enviados->base = registros_cpu.X;
		datos_enviados->offset = registros_cpu.S;
		datos_enviados->buffer = &auxiliar_copiar;
		datos_enviados->tamanio = sizeof(int32_t);

		registros_cpu.S += numero;

		incrementar_pc(5);

		list_clean(parametros);
		break;
	case TAKE:

		//Pido el numero y el registro que sirven como parametros de TAKE
		datos_solicitados->base = registros_cpu.M;
		datos_solicitados->offset = registros_cpu.P;
		datos_solicitados->tamanio = 4 + 1; //numero + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(sizeof(char) + sizeof(int32_t)); //numero + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;
		free(datos_recibidos);
		free(structRecibido);

		obtener_numero(param,0,&numero,aux);
		obtener_registro(param,4,&reg1);
		list_add(parametros,&numero);
		list_add(parametros, &reg1);
		ejecucion_instruccion("TAKE",parametros);


		//Pido lo que hay en el stack del tamanio numero que recibi antes
		datos_solicitados->base = registros_cpu.X;
		datos_solicitados->offset = registros_cpu.S - numero;
		datos_solicitados->tamanio = numero;

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);


		datos_recibidos = malloc(sizeof(int32_t)); //numero
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_numero(param,0,&numero,aux);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] = numero;

		registros_cpu.S -= numero;
		incrementar_pc(5);

		list_clean(parametros);
		break;
	case XXXX:
		ejecucion_instruccion("XXXX",parametros);

		copiar_registros_a_tcb();

		list_clean(parametros);
		break;
	case MALC:
		ejecucion_instruccion("MALC",parametros);

		t_struct_numero* crear_segmento_struct = malloc(sizeof(t_struct_numero));
		crear_segmento_struct->numero = registros_cpu.registros_programacion['A'];

		resultado = socket_enviar(sockMSP, D_STRUCT_MALC, crear_segmento_struct);
		controlar_envio(resultado, D_STRUCT_MALC);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_DIRECCION);

		registros_cpu.registros_programacion['A'] = ((t_struct_direccion*) structRecibido)->numero;

		free(crear_segmento_struct);
		list_clean(parametros);
		break;
	case FREE:
		ejecucion_instruccion("FREE",parametros);

		t_struct_numero* liberar_segmento_struct = malloc(sizeof(t_struct_numero));
		liberar_segmento_struct->numero = registros_cpu.registros_programacion['A'];

		resultado = socket_enviar(sockMSP, D_STRUCT_FREE, liberar_segmento_struct);
		controlar_envio(resultado, D_STRUCT_FREE);

		free(liberar_segmento_struct);
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
	free(((t_struct_respuesta_msp*) structRecibido)->buffer);
	free(structRecibido); //FIXME En las operaciones que no reciben parametros, va a fallar
	free(datos_recibidos);
	free(datos_solicitados);
	free(datos_enviados);
	free(aux);
	free(param);
}

