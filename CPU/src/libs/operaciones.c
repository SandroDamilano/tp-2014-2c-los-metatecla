/*
 * operaciones.c
 *
 *  Created on: 07/09/2014
 *      Author: utnso
 */

#include "operaciones.h"
#include <estructuras_auxiliares.h>

int sockMSP;
int sockKernel;

void ejecutarLinea(int* bytecode){

	int bytecodeLetras = convertirAString(bytecode);
	t_list* parametros = list_create();
	char* aux;
	aux = malloc(sizeof(uint32_t));
	char reg1;
	char reg2;
	int32_t numero;
	int32_t numero_enviar;
	uint32_t direccion;
	uint32_t direccionMSP;
	int resultado;

	t_struct_tcb* tcb_enviar = malloc(sizeof(t_struct_tcb));
	t_struct_numero* id_semaforo = malloc(sizeof(t_struct_numero));
	t_struct_numero* tid_enviar = malloc(sizeof(t_struct_numero));
	t_struct_sol_bytes* datos_solicitados = malloc(sizeof(t_struct_sol_bytes));
	t_struct_env_bytes* datos_enviados = malloc(sizeof(t_struct_env_bytes));
	void* datos_recibidos;
	void * structRecibido;
	t_tipoEstructura tipo_struct;

	char param_reg1[2];
	char param_reg2[2];
	char* param_numero;


	switch(bytecodeLetras){
	case LOAD:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;
		datos_solicitados->tamanio = 1 + 4; //registro + numero

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
			return ;
		}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_num(datos_recibidos,1,&numero);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_numero = string_itoa(numero);
		list_add(parametros,param_reg1);
		list_add(parametros, param_numero);

		ejecucion_instruccion("LOAD",parametros);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] = numero;

		incrementar_pc(sizeof(char) + sizeof(int32_t)); //registro + numero

		free(datos_recibidos);
		free(structRecibido);
		free(param_numero);
		list_clean(parametros);
		break;
	case GETM:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;
		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
			return ;
		}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_reg(datos_recibidos,1,&reg2);
		free(datos_recibidos);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_reg2[0] = reg2;
		param_reg2[1] = '\0';
		list_add(parametros,param_reg1);
		list_add(parametros, param_reg2);

		ejecucion_instruccion("GETM",parametros);


		if(reg2 == 'S'){ //HORRIBLE
			numero_enviar = registros_cpu.S;
			datos_solicitados->base = sumar_desplazamiento(registros_cpu.X,numero_enviar);
		} else {
			numero_enviar = registros_cpu.registros_programacion[elegirRegistro(reg2)];
			datos_solicitados->base = numero_enviar;
		}


		if(registros_cpu.K == false){
			datos_enviados->PID = registros_cpu.I;
		} else {
			datos_enviados->PID = 0;
		}

		datos_solicitados->tamanio = 1; //FIXME MEDIO TURBIO PERO ANDA

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
			return ;
		}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		int8_t putooo = 0;

		if(reg1 == 'S'){
			memcpy(&putooo, datos_recibidos, 1);
			registros_cpu.S = putooo;

		} else {
			memcpy(&putooo, datos_recibidos, 1);

			registros_cpu.registros_programacion[elegirRegistro(reg1)] = putooo;
			printf("SACO DE MEMORIA : %d\n", putooo);
		}

		incrementar_pc(2*sizeof(char)); //registro + registro

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case SETM:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 4 + 1 + 1; //numero + registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
			return ;
		}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_num(datos_recibidos,0,&numero);
		obtener_reg(datos_recibidos,4,&reg1);
		obtener_reg(datos_recibidos,5,&reg2);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_reg2[0] = reg2;
		param_reg2[1] = '\0';
		param_numero = string_itoa(numero);
		list_add(parametros,param_numero);
		list_add(parametros,param_reg1);
		list_add(parametros, param_reg2);

		ejecucion_instruccion("SETM",parametros);

		//HORRIBLEEEE
		int dir;
		char* auxxx = malloc(numero);
		int forro = 0;

		if(reg2 == 'S'){
			printf("S TIENE %d\n", registros_cpu.S);
			//memcpy(auxxx, &registros_cpu.S, numero);
			memcpy(&forro, &registros_cpu.S,numero);


			dir = registros_cpu.registros_programacion[elegirRegistro(reg1)];

		} else {
			printf("REG TIENE %d\n", registros_cpu.registros_programacion[elegirRegistro(reg2)]);
			//memcpy(auxxx, &registros_cpu.registros_programacion[elegirRegistro(reg2)],numero);

			if(registros_cpu.registros_programacion[elegirRegistro(reg2)] <0){
				memcpy(&forro, &registros_cpu.registros_programacion[elegirRegistro(reg2)],numero);

				forro -= elevar(2, 8*numero);

				dir = registros_cpu.registros_programacion[elegirRegistro(reg1)];
			} else {
				memcpy(&forro, &registros_cpu.registros_programacion[elegirRegistro(reg2)],numero);
				dir = registros_cpu.registros_programacion[elegirRegistro(reg1)];
			}
		} //TURBIO :S

		printf("NUMERO ENVIAR SETM: %d\n", forro);

		if(registros_cpu.K == false){
			datos_enviados->PID = registros_cpu.I;
		} else {
			datos_enviados->PID = 0;
		}

		datos_enviados->buffer = malloc(numero);
		memcpy(datos_enviados->buffer, &forro, numero);
		datos_enviados->tamanio = numero;
		datos_enviados->base = dir;

		printf("DIR DE SETM: %d\n", dir);

		resultado = socket_enviar(sockMSP, D_STRUCT_ENV_BYTES, datos_enviados);
		controlar_envio(resultado, D_STRUCT_ENV_BYTES);

		resultado = socket_recibir(sockMSP, &tipo_struct, &structRecibido);

		if (((t_struct_numero*) structRecibido)->numero == -1){
			printf("No hay espacio suficiente en memoria\n");
			abortar();
		}

		incrementar_pc(sizeof(int32_t) + 2*sizeof(char));

		free(auxxx);

		free(datos_recibidos);
		free(structRecibido);
		free(param_numero);
		list_clean(parametros);
		break;
	case MOVR:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
			return ;
		}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_reg(datos_recibidos,1,&reg2);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_reg2[0] = reg2;
		param_reg2[1] = '\0';
		list_add(parametros,param_reg1);
		list_add(parametros,param_reg2);

		ejecucion_instruccion("MOVR",parametros);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] = registros_cpu.registros_programacion[elegirRegistro(reg2)];

		incrementar_pc(sizeof(char)*2);

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case ADDR:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
			return ;
		}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_reg(datos_recibidos,1,&reg2);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_reg2[0] = reg2;
		param_reg2[1] = '\0';
		list_add(parametros,param_reg1);
		list_add(parametros,param_reg2);

		ejecucion_instruccion("ADDR",parametros);

		//HORRIBLE ESTO
		if(reg1 == 'M'){
			registros_cpu.registros_programacion[0] = registros_cpu.M + registros_cpu.registros_programacion[elegirRegistro(reg2)];
		} else {
			if(reg2 == 'M'){
				registros_cpu.registros_programacion[0] = registros_cpu.M + registros_cpu.registros_programacion[elegirRegistro(reg1)];

			} else {
				if(reg2 != 'S'){
				registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] + registros_cpu.registros_programacion[elegirRegistro(reg2)];}
				else {
					registros_cpu.registros_programacion[0] = sumar_desplazamiento(registros_cpu.X, registros_cpu.registros_programacion[elegirRegistro(reg1)] + registros_cpu.S);
				}
			}
		}


		incrementar_pc(sizeof(char)*2);

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case SUBR:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_reg(datos_recibidos,1,&reg2);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_reg2[0] = reg2;
		param_reg2[1] = '\0';
		list_add(parametros,param_reg1);
		list_add(parametros, param_reg2);

		ejecucion_instruccion("SUBR",parametros);

		registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] - registros_cpu.registros_programacion[elegirRegistro(reg2)];

		incrementar_pc(sizeof(char)*2);

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case MULR:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_reg(datos_recibidos,1,&reg2);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_reg2[0] = reg2;
		param_reg2[1] = '\0';
		list_add(parametros,param_reg1);
		list_add(parametros,param_reg2);

		ejecucion_instruccion("MULR",parametros);

		registros_cpu.registros_programacion[0] = (registros_cpu.registros_programacion[elegirRegistro(reg1)] * registros_cpu.registros_programacion[elegirRegistro(reg2)]);

		incrementar_pc(sizeof(char)*2);

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case MODR:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_reg(datos_recibidos,1,&reg2);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_reg2[0] = reg2;
		param_reg2[1] = '\0';
		list_add(parametros,param_reg1);
		list_add(parametros,param_reg2);

		ejecucion_instruccion("MODR",parametros);

		registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] % registros_cpu.registros_programacion[elegirRegistro(reg2)];

		incrementar_pc(sizeof(char)*2);

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case DIVR:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_reg(datos_recibidos,1,&reg2);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_reg2[0] = reg2;
		param_reg2[1] = '\0';
		list_add(parametros,param_reg1);
		list_add(parametros,param_reg2);

		ejecucion_instruccion("DIVR",parametros);

		if(registros_cpu.registros_programacion[elegirRegistro(reg2)] == 0){
			printf("division por cero");
			abortar();
		} else {
			registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] / registros_cpu.registros_programacion[elegirRegistro(reg2)];
		}

		incrementar_pc(sizeof(char)*2);

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case INCR:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1; //registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		list_add(parametros,param_reg1);
		ejecucion_instruccion("INCR",parametros);

		if(reg1=='S'){
			registros_cpu.S += 1;
		}else{
			registros_cpu.registros_programacion[elegirRegistro(reg1)] += 1;
		}

		incrementar_pc(sizeof(char));

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case DECR:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1; //registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		list_add(parametros,param_reg1);
		ejecucion_instruccion("DECR",parametros);

		registros_cpu.registros_programacion[elegirRegistro(reg1)] -= 1;

		incrementar_pc(sizeof(char));

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case COMP:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_reg(datos_recibidos,1,&reg2);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_reg2[0] = reg2;
		param_reg2[1] = '\0';
		list_add(parametros,param_reg1);
		list_add(parametros,param_reg2);
		ejecucion_instruccion("COMP",parametros);

		if(reg1 == 'S' && reg2 == 'X'){ //HORRIBLE ESTO
			printf("REG1 = %d y REG2 = %d\n", sumar_desplazamiento(registros_cpu.X, registros_cpu.S), registros_cpu.X);
			registros_cpu.registros_programacion[0] = (sumar_desplazamiento(registros_cpu.X, registros_cpu.S) == registros_cpu.X ? 1 : 0);
		} else {
			printf("REG1 = %d y REG2 = %d\n", registros_cpu.registros_programacion[elegirRegistro(reg1)], registros_cpu.registros_programacion[elegirRegistro(reg2)]);
			registros_cpu.registros_programacion[0] = (registros_cpu.registros_programacion[elegirRegistro(reg1)] == registros_cpu.registros_programacion[elegirRegistro(reg2)] ? 1 : 0);
		}

		incrementar_pc(sizeof(char)*2);

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case CGEQ:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_reg(datos_recibidos,1,&reg2);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_reg2[0] = reg2;
		param_reg2[1] = '\0';
		list_add(parametros,param_reg1);
		list_add(parametros,param_reg2);
		ejecucion_instruccion("CGEQ",parametros);

		registros_cpu.registros_programacion[0] = (registros_cpu.registros_programacion[elegirRegistro(reg1)] >= registros_cpu.registros_programacion[elegirRegistro(reg2)] ? 1 : 0);

		incrementar_pc(sizeof(char)*2);

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case CLEQ:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1 + 1; //registro + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		obtener_reg(datos_recibidos,1,&reg2);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_reg2[0] = reg2;
		param_reg2[1] = '\0';
		list_add(parametros,param_reg1);
		list_add(parametros, param_reg2);
		ejecucion_instruccion("CLEQ",parametros);

		registros_cpu.registros_programacion[0] = (registros_cpu.registros_programacion[elegirRegistro(reg1)] <= registros_cpu.registros_programacion[elegirRegistro(reg2)] ? 1 : 0);

		incrementar_pc(sizeof(char)*2);

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case GOTO:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 1; //registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_reg(datos_recibidos,0,&reg1);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		list_add(parametros,param_reg1);
		ejecucion_instruccion("GOTO",parametros);

		registros_cpu.P = registros_cpu.registros_programacion[elegirRegistro(reg1)];

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case JMPZ:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 4; //direccion

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_direc(datos_recibidos,0,&direccion);
		param_numero = string_itoa(direccion);
		list_add(parametros,param_numero);
		ejecucion_instruccion("JMPZ",parametros);

		if(registros_cpu.registros_programacion[0] == 0){
			registros_cpu.P = direccion;
		} else {
			incrementar_pc(4);
		}

		free(datos_recibidos);
		free(structRecibido);
		free(param_numero);
		list_clean(parametros);
		break;
	case JPNZ:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 4; //direccion

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_direc(datos_recibidos,0,&direccion);
		param_numero = string_itoa(direccion);
		list_add(parametros,param_numero);
		ejecucion_instruccion("JPNZ",parametros);

		if(registros_cpu.registros_programacion[0] != 0){
					registros_cpu.P = direccion;
		} else {
			incrementar_pc(4);
		}

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case INTE: //TODO INTE (SOLO PARA UBICARME EN LA BARRA)

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 4; //direccion

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_direc(datos_recibidos,0,&direccion);
		param_numero = string_itoa(direccion);
		list_add(parametros,param_numero);
		ejecucion_instruccion("INTE",parametros);

		//Mando señal con la direccion
		t_struct_direccion* direccion_syscalls = malloc(sizeof(t_struct_direccion));
		direccion_syscalls->numero = direccion;

		resultado = socket_enviar(sockKernel, D_STRUCT_INTE, direccion_syscalls);
		controlar_envio(resultado, D_STRUCT_INTE);
		free(direccion_syscalls);

		incrementar_pc(4); //direccion

		//Mando tcb
		copiar_registros_a_tcb();

		copiar_tcb_a_structTcb(tcb, tcb_enviar);
		resultado = socket_enviar(sockKernel, D_STRUCT_TCB, tcb_enviar);
		controlar_envio(resultado, D_STRUCT_TCB);

		//Pido tcb del kernel
		t_struct_numero* pedir_tcb = malloc(sizeof(t_struct_numero));
		pedir_tcb->numero = D_STRUCT_PEDIR_TCB;
		resultado = socket_enviar(sockKernel, D_STRUCT_NUMERO, pedir_tcb);
		controlar_envio(resultado, D_STRUCT_NUMERO);

		printf("PEDI TCB (INTE)\n");

		resultado = socket_recibir(sockKernel, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_TCB) == EXIT_FAILURE){
			return;
		}

		copiar_structRecibido_a_tcb(tcb, structRecibido);
		copiar_tcb_a_registros();
		cantidad_lineas_ejecutadas = 0;

		comienzo_ejecucion(tcb, quantum);

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case SHIF:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 4 + 1; //numero + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;


		obtener_num(datos_recibidos,0,&numero);
		obtener_reg(datos_recibidos,4,&reg1);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_numero = string_itoa(numero);
		list_add(parametros,param_numero);
		list_add(parametros, param_reg1);
		ejecucion_instruccion("SHIF",parametros);

		uint32_t cantidad_bits = abs(numero);

		if(numero > 0){
			registros_cpu.registros_programacion[elegirRegistro(reg1)] >>= cantidad_bits;
		} else {
			registros_cpu.registros_programacion[elegirRegistro(reg1)] <<= cantidad_bits;
		}

		incrementar_pc(5);

		free(datos_recibidos);
		free(structRecibido);
		free(param_numero);
		list_clean(parametros);
		break;
	case NOPP:
		ejecucion_instruccion("NOPP",parametros);

		list_clean(parametros);
		break;
	case PUSH:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 4 + 1; //numero + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		obtener_num(datos_recibidos,0,&numero);
		obtener_reg(datos_recibidos,4,&reg1);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_numero = string_itoa(numero);
		list_add(parametros,param_numero);
		list_add(parametros, param_reg1);
		ejecucion_instruccion("PUSH",parametros);

		int32_t auxiliar_copiar;

		printf("EN PUSH, EL REGISTRO TIENE: %d\n", registros_cpu.registros_programacion[elegirRegistro(reg1)]);

		memcpy(&auxiliar_copiar,&registros_cpu.registros_programacion[elegirRegistro(reg1)],numero);

		//socket a MSP enviando auxiliar_copiar con la direccion del cursor de stack

		direccionMSP = sumar_desplazamiento(registros_cpu.X, registros_cpu.S);

		datos_enviados->base = direccionMSP;
		datos_enviados->PID = registros_cpu.I;
		datos_enviados->buffer = &auxiliar_copiar;
		printf("PUSHEO %d\n", auxiliar_copiar);
		datos_enviados->tamanio = numero;

		int resultado = socket_enviar(sockMSP, D_STRUCT_ENV_BYTES, datos_enviados);
		controlar_envio(resultado, D_STRUCT_ENV_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);


		if (((t_struct_numero*) structRecibido)->numero == -1){
			printf("No hay espacio suficiente en memoria\n");
			abortar();
		}

		registros_cpu.S += numero;

		incrementar_pc(5);

		free(datos_recibidos);
		free(structRecibido);
		list_clean(parametros);
		break;
	case TAKE:

		printf("PC EN TAKE %d\n", registros_cpu.P);

		//Pido el numero y el registro que sirven como parametros de TAKE
		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;

		datos_solicitados->tamanio = 4 + 1; //numero + registro

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		//datos_recibidos = malloc(sizeof(char) + sizeof(int32_t)); //numero + registro
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;
		free(structRecibido);

		obtener_num(datos_recibidos,0,&numero);
		printf("Llego numero %d\n", numero);
		obtener_reg(datos_recibidos,4,&reg1);
		param_reg1[0] = reg1;
		param_reg1[1] = '\0';
		param_numero = string_itoa(numero);
		list_add(parametros,param_numero);
		list_add(parametros, param_reg1);
		ejecucion_instruccion("TAKE",parametros);


		//Pido lo que hay en el stack del tamanio numero que recibi antes
		direccionMSP = sumar_desplazamiento(registros_cpu.X, registros_cpu.S - numero);
		printf("Direccion stack %d \n", direccionMSP);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = registros_cpu.I;
		datos_solicitados->tamanio = numero;

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}
		free(datos_recibidos);

		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		int aux;
		aux = *((int32_t*)datos_recibidos);


		registros_cpu.registros_programacion[elegirRegistro(reg1)] = aux;

		registros_cpu.S -= numero;
		incrementar_pc(5);

		free(datos_recibidos);
		free(structRecibido);
		free(param_numero);
		list_clean(parametros);
		break;
	case XXXX://PIDO OTRO Y LO RECIBO EN EL WHILE
		ejecucion_instruccion("XXXX",parametros);

		copiar_registros_a_tcb();
		copiar_tcb_a_structTcb(tcb, tcb_enviar);
		resultado = socket_enviar(sockKernel, D_STRUCT_TCB, tcb_enviar);
		controlar_envio(resultado, D_STRUCT_TCB);

		fin_ejecucion();

		terminoEjecucion = true;

		list_clean(parametros);
		break;
	case MALC:
		ejecucion_instruccion("MALC",parametros);

		t_struct_malloc* crear_segmento_struct = malloc(sizeof(t_struct_malloc));
		crear_segmento_struct->tamano_segmento = registros_cpu.registros_programacion[0];
		crear_segmento_struct->PID = tcb->pid;

		resultado = socket_enviar(sockMSP, D_STRUCT_MALC, crear_segmento_struct);
		controlar_envio(resultado, D_STRUCT_MALC);

		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_NUMERO)==EXIT_FAILURE){
			return;
		}

		if (((t_struct_numero*) structRecibido)->numero == -1){
			printf("No hay espacio suficiente en memoria\n");
			abortar();
		} else {
			registros_cpu.registros_programacion[0] = ((t_struct_direccion*) structRecibido)->numero;
		}

		free(crear_segmento_struct);
		free(structRecibido);
		list_clean(parametros);
		break;
	case FREE:
		ejecucion_instruccion("FREE",parametros);

		t_struct_free* liberar_segmento_struct = malloc(sizeof(t_struct_free));
		liberar_segmento_struct->direccion_base = registros_cpu.registros_programacion[0];
		liberar_segmento_struct->PID = tcb->pid;

		resultado = socket_enviar(sockMSP, D_STRUCT_FREE, liberar_segmento_struct);
		controlar_envio(resultado, D_STRUCT_FREE);

		free(liberar_segmento_struct);
		list_clean(parametros);
		break;
	case INNN:
		ejecucion_instruccion("INNN",parametros);

		//Mando señal con el numero D_STRUCT_INNN
		t_struct_numero* INNN = malloc(sizeof(t_struct_numero));
		INNN->numero = D_STRUCT_INNN;

		resultado = socket_enviar(sockKernel, D_STRUCT_NUMERO, INNN);
		controlar_envio(resultado, D_STRUCT_NUMERO);
		free(INNN);

		//Recibo un numero que se ingreso por consola
		socket_recibir(sockKernel, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_INNN)==EXIT_FAILURE){
					return;
				}

		registros_cpu.registros_programacion[0] = ((t_struct_numero*) structRecibido)->numero;

		printf("SE INGRESO POR CONSOLA %d\n", registros_cpu.registros_programacion[0]);

		list_clean(parametros);
		free(structRecibido);
		break;
	case INNC:
		ejecucion_instruccion("INNC",parametros);

		//Mando señal con el numero D_STRUCT_INNC
		t_struct_numero* INNC = malloc(sizeof(t_struct_numero));
		INNC->numero = registros_cpu.registros_programacion[1];

		resultado = socket_enviar(sockKernel, D_STRUCT_INNC, INNC);
		controlar_envio(resultado, D_STRUCT_INNC);
		free(INNC);

		//Recibo un string que se ingreso por consola
		socket_recibir(sockKernel, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_STRING)==EXIT_FAILURE){
					return;
				}

		//Envio cadena recibida a memoria
		char* cadena = ((t_struct_string*) structRecibido)->string;

		datos_enviados->base = registros_cpu.registros_programacion[0];
		datos_enviados->PID = tcb->pid;
		datos_enviados->buffer = cadena;
		datos_enviados->tamanio = strlen(cadena);

		resultado = socket_enviar(sockMSP, D_STRUCT_ENV_BYTES, datos_enviados);
		controlar_envio(resultado, D_STRUCT_ENV_BYTES);

		resultado = socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_NUMERO)==EXIT_FAILURE){
					return;
				}

		if (((t_struct_numero*) structRecibido)->numero == -1){
			printf("No hay espacio suficiente en memoria\n");
			abortar();
		}

		list_clean(parametros);
		free(structRecibido);
		break;
	case OUTN:
		ejecucion_instruccion("OUTN",parametros);

		t_struct_numero* OUTN = malloc(sizeof(t_struct_numero));
		OUTN->numero = registros_cpu.registros_programacion[0];

		resultado = socket_enviar(sockKernel, D_STRUCT_OUTN, OUTN);
		controlar_envio(resultado, D_STRUCT_OUTN);
		free(OUTN);

		list_clean(parametros);
		break;
	case OUTC:
		ejecucion_instruccion("OUTC",parametros);

		//Pido la cadena apuntada por registro A en memoria
		datos_solicitados->base = sumar_desplazamiento(registros_cpu.M,registros_cpu.registros_programacion[0]);
		datos_solicitados->PID = tcb->pid;
		datos_solicitados->tamanio = registros_cpu.registros_programacion[1];

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		//Recibo la cadena de memoria
		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		if(controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP) == EXIT_FAILURE){
					return ;
				}

		//datos_recibidos = malloc(registros_cpu.registros_programacion[1]); //tamaño determinado por reg B
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		//Envio cadena a kernel para ser mostrada por consola
		t_struct_string* cadena_consola = malloc(sizeof(t_struct_string));
		cadena_consola->string = (char*)datos_recibidos;//FIXME Está bien copiar el string así?

		resultado = socket_enviar(sockKernel, D_STRUCT_OUTC, cadena_consola);
		controlar_envio(resultado, D_STRUCT_OUTC);
		free(cadena_consola);

		t_struct_numero* tamanio = malloc(sizeof(t_struct_numero));
		tamanio->numero = registros_cpu.registros_programacion[1];
		resultado = socket_enviar(sockKernel, D_STRUCT_NUMERO, tamanio);
		controlar_envio(resultado, D_STRUCT_NUMERO);
		free(tamanio);

		free(structRecibido);
		list_clean(parametros);
		break;
	case CREA:
		ejecucion_instruccion("CREA",parametros);

		t_struct_numero* tid = malloc(sizeof(t_struct_numero));
		tid->numero = tcb->tid;

		printf("BASE DE STACK %d\n", tcb->base_stack);

		resultado = socket_enviar(sockKernel, D_STRUCT_TCB_CREA, tid);
		controlar_envio(resultado, D_STRUCT_TCB_CREA);

		//Mando PC
		t_struct_numero* program_counter = malloc(sizeof(t_struct_numero));
		program_counter->numero = registros_cpu.registros_programacion[1];
		resultado = socket_enviar(sockKernel, D_STRUCT_NUMERO, program_counter);
		controlar_envio(resultado, D_STRUCT_NUMERO);
		free(program_counter);

		//Recibo TID del hijo
		resultado = socket_recibir(sockKernel, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_NUMERO);

		registros_cpu.registros_programacion[0] = ((t_struct_numero*) structRecibido)->numero;

		printf("Tid del hijo %d\n", registros_cpu.registros_programacion[0]);

		free(tid);
		free(structRecibido);
		list_clean(parametros);
		break;
	case JOIN:
		ejecucion_instruccion("JOIN",parametros);

		t_struct_join* join = malloc(sizeof(t_struct_join));
		join->tid_a_esperar = registros_cpu.registros_programacion[0];
		join->tid_llamador = tcb->tid;


		resultado = socket_enviar(sockKernel, D_STRUCT_JOIN, join);
		controlar_envio(resultado, D_STRUCT_JOIN);

		free(join);


		list_clean(parametros);
		break;
	case BLOK:
		ejecucion_instruccion("BLOK",parametros);

		id_semaforo->numero = registros_cpu.registros_programacion[1];

		resultado = socket_enviar(sockKernel, D_STRUCT_BLOCK, id_semaforo);
		controlar_envio(resultado, D_STRUCT_BLOCK);

		tid_enviar->numero = tcb->tid;

		resultado = socket_enviar(sockKernel, D_STRUCT_NUMERO, tid_enviar);
		controlar_envio(resultado, D_STRUCT_NUMERO);


		list_clean(parametros);
		break;
	case WAKE:
		ejecucion_instruccion("WAKE",parametros);

		id_semaforo->numero = registros_cpu.registros_programacion[1];

		resultado = socket_enviar(sockKernel, D_STRUCT_WAKE, id_semaforo);
		controlar_envio(resultado, D_STRUCT_WAKE);


		list_clean(parametros);
		break;
	}

	printf("Registro A: %d\n", registros_cpu.registros_programacion[0]);
	printf("Registro B: %d\n", registros_cpu.registros_programacion[1]);
	printf("Registro D: %d\n", registros_cpu.registros_programacion[3]);

	list_destroy(parametros); //Que onda la destruccion de los elementos?
	free(id_semaforo);
	free(tcb_enviar);
	free(tid_enviar);
	free(datos_solicitados);
	free(datos_enviados);
	free(aux);
}

