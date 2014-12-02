/*
 * auxiliares.c
 *
 *  Created on: 12/09/2014
 *      Author: utnso
 */

#include "auxiliares.h"

int sockMSP;
int sockKernel;

void copiar_tcb_a_registros(){
	//registros_cpu.I = tcb->pid;
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
	//que onda con la t_cola? lo cambio a EXEC?
}

void copiar_registros_a_tcb(){
	//tcb->pid = registros_cpu.I;
	//tcb->kernel_mode = registros_cpu.K;
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

void obtener_registro(char* parametros, int posicion, char* registro){
	memcpy(registro, parametros + posicion, sizeof(char));
}

void obtener_reg(void* parametros, int posicion, char* registro){
	memcpy(registro, parametros + posicion, sizeof(char));
}

void obtener_numero(char* parametros, int posicion, int32_t* numero, char* aux){
	memcpy(aux,parametros,sizeof(int32_t));
	int num = atoi(aux);
	memcpy(numero,&num,sizeof(int32_t));
}

void obtener_num(void* parametros, int posicion, int32_t* numero){
	memcpy(numero,parametros + posicion,sizeof(int32_t));
}

void obtener_direccion(char* parametros, int posicion, uint32_t* direccion, char* aux){
	memcpy(aux,parametros,sizeof(uint32_t));
	int num = atoi(aux);
	memcpy(direccion,&num,sizeof(uint32_t));}

void obtener_direc(void* parametros, int posicion, uint32_t* direccion){
	memcpy(direccion, parametros + posicion, sizeof(uint32_t));
}

int elegirRegistro(char registro){
	int posicion_registros;
	switch(registro){
	case 'A':
		posicion_registros = 0;
		return posicion_registros;
		break;
	case 'B':
		posicion_registros = 1;
		return posicion_registros;
		break;
	case 'C':
		posicion_registros = 2;
		return posicion_registros;
		break;
	case 'D':
		posicion_registros = 3;
		return posicion_registros;
		break;
	case 'E':
		posicion_registros = 4;
		return posicion_registros;
		break;
	}
	}

void esperar_retardo(){
	int retardo = config_struct_cpu.retardo * 1000;
	usleep(retardo);
}

void incrementar_pc(int cant_bytes){
	registros_cpu.P += cant_bytes;
}

void abortar(){
	printf("\n\nABORTAR\n\n");
	t_tipoEstructura tipo_struct;
	void* structRecibido;

	t_struct_numero* abortar = malloc(sizeof(t_struct_numero));
	abortar->numero = D_STRUCT_ABORT;
	socket_enviar(sockKernel, D_STRUCT_NUMERO, abortar);
	free(abortar);

	t_struct_numero* pedir_tcb = malloc(sizeof(t_struct_numero));
		pedir_tcb->numero = D_STRUCT_PEDIR_TCB;
		int resultado = socket_enviar(sockKernel, D_STRUCT_NUMERO, pedir_tcb);
		if (resultado != 1) {
			printf("No se pudo pedir TCB\n");
		}
		free(pedir_tcb);
		//socket de Kernel con tcb
		socket_recibir(sockKernel, &tipo_struct, &structRecibido);
		copiar_structRecibido_a_tcb(tcb, structRecibido);
		free(structRecibido);
		cantidad_lineas_ejecutadas = 0;
		terminoEjecucion = false;
		comienzo_ejecucion(tcb, quantum);
		copiar_tcb_a_registros();
}

int controlar_struct_recibido(int struct_recibido, int struct_posta){

	if(struct_recibido == struct_posta){
		//printf("Se recibio correctamente el mensaje %d\n", struct_posta);
		return EXIT_SUCCESS;
	} else{
		if(struct_recibido == D_STRUCT_SEG_FAULT){
		printf("Hubo un segmentation fault\n");
		abortar();
		return EXIT_FAILURE;
		} else {
		printf("Se debia recibir %d, pero llego %d\n", struct_posta, struct_recibido);
		abortar();
		return EXIT_FAILURE;
		}
	}

}

void controlar_envio(int resultado, int tipo){
	if(resultado == 1){
		//printf("Se envio correctamente el mensaje de tipo %d\n", tipo);
	}
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


