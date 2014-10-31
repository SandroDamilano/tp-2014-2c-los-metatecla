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
	uint32_t direccionMSP;
	int resultado;

	t_struct_tcb* tcb_enviar = malloc(sizeof(t_struct_tcb));
	t_struct_numero* id_semaforo = malloc(sizeof(t_struct_numero));
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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;
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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;
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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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
			abortar();
		} else {
			registros_cpu.registros_programacion[0] = registros_cpu.registros_programacion[elegirRegistro(reg1)] / registros_cpu.registros_programacion[elegirRegistro(reg2)];
		}

		incrementar_pc(sizeof(char)*2);

		list_clean(parametros);
		break;
	case INCR:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		//Mando señal con la direccion
		t_struct_direccion* direccion_syscalls = malloc(sizeof(t_struct_direccion));
		direccion_syscalls->numero = direccion;

		resultado = socket_enviar(sockKernel, D_STRUCT_INTE, direccion_syscalls);
		controlar_envio(resultado, D_STRUCT_INTE);
		free(direccion_syscalls);

		//Mando tcb
		copiar_registros_a_tcb();

		t_struct_tcb* tcb_syscalls = malloc(sizeof(t_struct_tcb));
		copiar_tcb_a_structTcb(tcb, tcb_syscalls);
		resultado = socket_enviar(sockKernel, D_STRUCT_TCB, tcb_syscalls);
		controlar_envio(resultado, D_STRUCT_TCB);
		free(tcb_syscalls);

		incrementar_pc(4); //direccion

		list_clean(parametros);
		break;
	case SHIF:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		uint32_t cantidad_bits = abs(numero);

		if(numero > 0){
			registros_cpu.registros_programacion[elegirRegistro(reg1)] >>= cantidad_bits;
		} else {
			registros_cpu.registros_programacion[elegirRegistro(reg1)] <<= cantidad_bits;
		}

		incrementar_pc(5);

		list_clean(parametros);
		break;
	case NOPP:
		ejecucion_instruccion("NOPP",parametros);

		list_clean(parametros);
		break;
	case PUSH:

		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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

		direccionMSP = sumar_desplazamiento(registros_cpu.X, registros_cpu.S);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;
		datos_enviados->buffer = &auxiliar_copiar;
		datos_enviados->tamanio = sizeof(int32_t);

		int resultado = socket_enviar(sockMSP, D_STRUCT_ENV_BYTES, datos_enviados);
		controlar_envio(resultado, D_STRUCT_ENV_BYTES);

		registros_cpu.S += numero;

		incrementar_pc(5);

		list_clean(parametros);
		break;
	case TAKE:

		//Pido el numero y el registro que sirven como parametros de TAKE
		direccionMSP = sumar_desplazamiento(registros_cpu.M, registros_cpu.P);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;

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
		direccionMSP = sumar_desplazamiento(registros_cpu.X, registros_cpu.S - numero);

		datos_solicitados->base = direccionMSP;
		datos_solicitados->PID = tcb->pid;
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
	case XXXX://FIXME: COMO MANEJO LO DE ENVIAR TCB TERMINADO Y PEDIR OTRO?
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

		//Mando señal con el numero D_STRUCT_INNN
		t_struct_numero* INNN = malloc(sizeof(t_struct_numero));
		INNN->numero = D_STRUCT_INNN;

		resultado = socket_enviar(sockKernel, D_STRUCT_NUMERO, INNN);
		controlar_envio(resultado, D_STRUCT_NUMERO);
		free(INNN);

		//Recibo un numero que se ingreso por consola
		socket_recibir(sockKernel, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_INNN);

		registros_cpu.registros_programacion['A'] = ((t_struct_numero*) structRecibido)->numero;

		list_clean(parametros);
		break;
	case INNC:
		ejecucion_instruccion("INNC",parametros);

		//Mando señal con el numero D_STRUCT_INNC
		t_struct_numero* INNC = malloc(sizeof(t_struct_numero));
		INNC->numero = registros_cpu.registros_programacion['B'];

		resultado = socket_enviar(sockKernel, D_STRUCT_INNC, INNC);
		controlar_envio(resultado, D_STRUCT_INNC);
		free(INNC);

		//Recibo un string que se ingreso por consola
		socket_recibir(sockKernel, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_INNC);

		//Envio cadena recibida a memoria
		char* cadena = ((t_struct_string*) structRecibido)->string;

		datos_enviados->base = registros_cpu.registros_programacion['A'];
		datos_enviados->PID = tcb->pid;
		datos_enviados->buffer = cadena;
		datos_enviados->tamanio = strlen(cadena);

		resultado = socket_enviar(sockMSP, D_STRUCT_ENV_BYTES, datos_enviados);
		controlar_envio(resultado, D_STRUCT_ENV_BYTES);

		list_clean(parametros);
		break;
	case OUTN:
		ejecucion_instruccion("OUTN",parametros);

		t_struct_numero* OUTN = malloc(sizeof(t_struct_numero));
		OUTN->numero = registros_cpu.registros_programacion['A'];

		resultado = socket_enviar(sockKernel, D_STRUCT_OUTN, OUTN);
		controlar_envio(resultado, D_STRUCT_OUTN);
		free(OUTN);

		list_clean(parametros);
		break;
	case OUTC:
		ejecucion_instruccion("OUTC",parametros);

		//Pido la cadena apuntada por registro A en memoria
		datos_solicitados->base = registros_cpu.registros_programacion['A'];
		datos_solicitados->PID = tcb->pid;
		datos_solicitados->tamanio = registros_cpu.registros_programacion['B'];

		resultado = socket_enviar(sockMSP, D_STRUCT_SOL_BYTES, datos_solicitados);
		controlar_envio(resultado, D_STRUCT_SOL_BYTES);

		//Recibo la cadena de memoria
		socket_recibir(sockMSP, &tipo_struct, &structRecibido);
		controlar_struct_recibido(tipo_struct, D_STRUCT_RESPUESTA_MSP);

		datos_recibidos = malloc(registros_cpu.registros_programacion['B']); //tamaño determinado por reg B
		datos_recibidos = ((t_struct_respuesta_msp*) structRecibido)->buffer;

		//Envio cadena a kernel para ser mostrada por consola
		t_struct_string* cadena_consola = malloc(sizeof(t_struct_string));
		cadena_consola->string = (char*)datos_recibidos;

		resultado = socket_enviar(sockKernel, D_STRUCT_OUTC, cadena_consola);
		controlar_envio(resultado, D_STRUCT_OUTC);

		list_clean(parametros);
		break;
	case CREA: //FIXME: RECIBO EL ALGO DESPUES DE MANDAR ESTO?
		ejecucion_instruccion("CREA",parametros);

		//Mando TCB

		copiar_registros_a_tcb();
		copiar_tcb_a_structTcb(tcb, tcb_enviar);

		resultado = socket_enviar(sockKernel, D_STRUCT_TCB_CREA, tcb_enviar);
		controlar_envio(resultado, D_STRUCT_TCB_CREA);

		//Mando PC
		t_struct_numero* program_counter = malloc(sizeof(t_struct_numero));
		program_counter->numero = registros_cpu.registros_programacion['B'];
		resultado = socket_enviar(sockKernel, D_STRUCT_NUMERO, program_counter);
		controlar_envio(resultado, D_STRUCT_NUMERO);
		free(program_counter);

		list_clean(parametros);
		break;
	case JOIN:
		ejecucion_instruccion("JOIN",parametros);

		t_struct_join* join = malloc(sizeof(t_struct_join));
		join->tid_a_esperar = registros_cpu.registros_programacion['A'];
		join->tid_llamador = tcb->tid;

		resultado = socket_enviar(sockKernel, D_STRUCT_JOIN, join);
		controlar_envio(resultado, D_STRUCT_JOIN);

		free(join);

		list_clean(parametros);
		break;
	case BLOK:
		ejecucion_instruccion("BLOK",parametros);

		id_semaforo->numero = registros_cpu.registros_programacion['B'];

		resultado = socket_enviar(sockKernel, D_STRUCT_BLOCK, id_semaforo);
		controlar_envio(resultado, D_STRUCT_BLOCK);


		copiar_registros_a_tcb();
		copiar_tcb_a_structTcb(tcb, tcb_enviar);

		resultado = socket_enviar(sockKernel, D_STRUCT_TCB, tcb_enviar);
		controlar_envio(resultado, D_STRUCT_TCB);

		list_clean(parametros);
		break;
	case WAKE:
		ejecucion_instruccion("WAKE",parametros);

		id_semaforo->numero = registros_cpu.registros_programacion['B'];

		resultado = socket_enviar(sockKernel, D_STRUCT_BLOCK, id_semaforo);
		controlar_envio(resultado, D_STRUCT_BLOCK);

		list_clean(parametros);
		break;
	}

	list_destroy(parametros); //Que onda la destruccion de los elementos?
	free(id_semaforo);
	free(tcb_enviar);
	free(((t_struct_respuesta_msp*) structRecibido)->buffer);
	free(structRecibido); //FIXME En las operaciones que no reciben parametros, va a fallar
	free(datos_recibidos);
	free(datos_solicitados);
	free(datos_enviados);
	free(aux);
	free(param);
}

