/*
 * auxiliares.h
 *
 *  Created on: 12/09/2014
 *      Author: utnso
 */

#ifndef AUXILIARES_H_
#define AUXILIARES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <commons/string.h>
#include <string.h>
#include <ansisop-panel/cpu.h>
#include <unistd.h>
#include <sockets/enviar_recibir_paquete.h>

typedef struct{
	int puerto_kernel;
	int puerto_msp;
	char* ip_kernel;
	char* ip_msp;
	int retardo;
}t_config_cpu;

t_hilo* tcb;
t_registros_cpu registros_cpu;
int quantum;
int cantidad_lineas_ejecutadas;
t_config_cpu config_struct_cpu;
int sockMSP;
int sockKernel;
extern bool terminoEjecucion;

void obtener_direccion(char* parametros, int posicion, uint32_t* direccion, char* aux);
void obtener_numero(char* parametros, int posicion,int32_t* numero, char* aux);
void obtener_registro(char* parametros, int posicion, char* registro);

void obtener_reg(void* parametros, int posicion, char* registro);
void obtener_num(void* parametros, int posicion, int32_t* numero);
void obtener_direc(void* parametros, int posicion, uint32_t* direccion);

void copiar_tcb_a_registros();
void copiar_registros_a_tcb();
int elegirRegistro(char registro);
void esperar_retardo();
void incrementar_pc(int cant_bytes);
void controlar_struct_recibido(int struct_recibido, int struct_posta);
void controlar_envio(int resultado, int tipo);

void abortar();

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
}bytecodes;



#endif /* AUXILIARES_H_ */
